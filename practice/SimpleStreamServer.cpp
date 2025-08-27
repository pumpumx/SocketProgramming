#include <iostream> 
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

#define PORT "3490"
#define BACKLOG 10

void sigchld_handler(int s){
    (void)s; //quiet unused variable warning
    
    int saved_errno = errno;
    //waitpid might ovewrite errno , so we wait and restore it.
    //waitpid blocks or hangs until some child process exists.

    while(waitpid(-1,NULL,WNOHANG) > 0) //this term WNOHANG  , 
                                        //in the server blocking in signal handler basically = death   ,
                                        // it means , Don't block if no child has exited yet , just return 0.

    errno = saved_errno;
}

void *getIpVersion(struct sockaddr *sa){ //crazy concept of generic pointer , dosen't matter whether it is ipv4 or ipv6 -> returns a generic pointer.
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &((struct sockaddr_in6 *)sa)->sin6_addr;
}

int main(int argc , char* argv[]){
    if(argc != 3){
        cerr<<"usage: telnet remotehostname portname"<<endl;
        exit(1);
    }
    int sockFd  , newFd;
    struct addrinfo hints , *servInfo , *p;
    memset(&hints , 0 , sizeof hints);

    struct sockaddr_storage their_addr;
    socklen_t their_size;
    struct  sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //use my ip

    int status;
    if((status = getaddrinfo(NULL , PORT , &hints , &servInfo)) != 0){
        cerr<<"Failed to get machine address info"<<gai_strerror(status);
        return 1;
    }

    for(p=servInfo;p != nullptr ; p = p->ai_next){
        if((sockFd = socket(p->ai_family , p->ai_socktype , p->ai_protocol)) == -1){
            cerr<<"server: "<<sockFd<<endl;
            continue;
        }

        if(setsockopt(sockFd , SOL_SOCKET ,SO_REUSEADDR , &yes , sizeof(int)) == -1){
            cerr<<"Error while setting socket options<<endl";
            continue;
        }

        if(bind(sockFd , p->ai_addr , p->ai_addrlen) == -1){
            cerr<<"Error while binding socket: "<<endl;
            continue;
        }
        break;
    }

    freeaddrinfo(servInfo);

    if(p == NULL){
        cerr<<"Server failed to bind";
        exit(1);
    }

    if(listen(sockFd , BACKLOG) == -1){
        cout<<"Error while listening on port: "<<PORT;
        exit(1);
    }

    //Reap all dead processes , dk htf this happens , let's studyyyyy
    sa.sa_handler = sigchld_handler; //Crazy signal handling , alright it's used to avoid zombies
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;


    if((sigaction(SIGCHLD , &sa , NULL)) == -1){
        cerr<<"Error while performing sigaction"<<endl;
    }

    while(1){ //Main accept loop
        their_size = sizeof their_addr;
        newFd = accept(sockFd , (struct sockaddr *)&their_addr,&their_size);

        if(newFd == -1){
            cerr<<"Failed to accept client request";
            continue;
        }

        inet_ntop(their_addr.ss_family , getIpVersion((struct sockaddr *)&their_addr),s,sizeof s);
        cout<<"Server got connection from: "<<s<<endl;

        if(!fork()){ //This is the child process
            close(sockFd);  //child process dosen't need listening socket 
            if(send(newFd , "Hello My friend" , 16 , 0) == -1){ //Sends the client the hello world message
                cerr<<"Failed to send the message to the client: "<<s<<endl;
            }
            close(newFd);
            exit(0);
        }
        close(newFd);
    }

    return 0;


}