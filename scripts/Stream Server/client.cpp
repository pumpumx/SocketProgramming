#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;



#define PORT "3490" //This is the port on which the client will connect to
#define MAXDATASIZE 100 //maximum amount of data we can send


void *get_in_addr(struct sockaddr* sa ){ //generic pointer

    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &((struct sockaddr_in6 *)sa)->sin6_addr;
}


int main(int argc , char *argv[]){
    if(argc != 2){
        cerr<<"Usage: client remotehostname"<<endl;
        exit(1);
    }

    int sockFd , numbytes;
    char buff[MAXDATASIZE];
    int status;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints ,*servInfo, *p;
    memset(&hints , 0 , sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((status = getaddrinfo(argv[1],PORT,&hints,&servInfo)) != 0){
        cerr<<"Error while getting address information"<<gai_strerror(status);
    };

    //Looping through all results

    for(p=servInfo ; p != nullptr ; p = p->ai_next){
        if((sockFd = socket(p->ai_family , p->ai_socktype , p->ai_protocol)) == -1){
            cerr<<"Error connecting socket with socket name"<<p->ai_addr<<endl;
            continue;
        }

       inet_ntop(p->ai_family , get_in_addr((struct sockaddr *)p->ai_addr) , s, sizeof s   ); 
        cout<<"Attempting connection to client"<<s<<endl;

        if((connect(sockFd ,  p->ai_addr,p->ai_addrlen)) == -1){
            cerr<<"error while establishing connection "<<s<<endl;
            close(sockFd);
            continue;
        }
        break;
    }

    if(p == NULL){
        cerr<<"Client failed to connect"<<endl;
        exit(1);
    }
    inet_ntop(p->ai_family , get_in_addr((struct sockaddr *)p->ai_addr) , s, sizeof s   );
    
    cout<<"connection established successfully with server: "<<s<<endl;
    freeaddrinfo(servInfo);


    if((numbytes = recv(sockFd,buff,MAXDATASIZE - 1,0)) <= 0){
        if(numbytes == 0){
            cout<<"client: server conenction closed"<<endl;
        }
        else{
            perror("recv");
            exit(1);
        }
    }
    buff[numbytes] = '\0';

    cout<<"Message recieved: "<<buff<<endl;
    close(sockFd);
    
    exit(0);
}