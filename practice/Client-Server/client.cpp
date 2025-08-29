#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

using namespace std;

#define PORT "3490"
#define MAXBUFFSIZE 20
void *get_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &((struct sockaddr_in*)sa)->sin_addr;
    }
    return &((struct sockaddr_in6 *)sa)->sin6_addr;
}

int main(int argc , char * argv[]){
    if(argc != 2){
        cerr<<"Usage client remotehostname"<<endl;
        exit(1);
    }

    int sockFd , status;
    struct addrinfo hints , *servInfo , *p;

    char s[INET6_ADDRSTRLEN];

    memset(&hints , 0 , sizeof hints);

    hints.ai_family = AF_UNSPEC; //unspecified
    hints.ai_socktype = SOCK_STREAM;
    
    //let's get server addressinfo 
    if((status = getaddrinfo(argv[1] , PORT , &hints , &servInfo )) != 0){
        cerr<<"Unable to reach the server";
        exit(1);
    }
    
    //lets iterate over the available ip

    for(p = servInfo ; p!=nullptr ; p = p->ai_next){
        if((sockFd = socket(p->ai_family , p->ai_socktype , p->ai_protocol)) == -1){
            perror("socket");
            continue;
        }

        //let's get the ip address
        inet_ntop(p->ai_family , get_addr((struct sockaddr *)p->ai_addr), s , sizeof s); //Convert;s it into presentable ip

        if((connect(sockFd , p->ai_addr , p->ai_addrlen )) == -1){
            close(sockFd); //comes from unistd
            continue;
        }
        break;
    }

    if(p==NULL){
        perror("connect");
        exit(1);
    }

    freeaddrinfo(servInfo); //Free the memory allocated to the linked list

    //Reciveing Message part 

    int rv;
    char buff[MAXBUFFSIZE];

    if((rv = recv(sockFd , buff , MAXBUFFSIZE -1 , 0)) <= 0){
        if(rv == 0){
            cerr<<"Connection closed from server side";
            exit(0);
        }
        else{
            perror("recv");
            exit(1);
        }
    }

    buff[rv] = '\0';
    cout<<"Message Recieved: "<<buff<<endl;
    close(sockFd);

    exit(0);

}