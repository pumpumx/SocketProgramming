#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <sys/wait.h>

using namespace std;


#define MAXMESSAGELENGTH 100

void *get_ip_address(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &((struct sockaddr_in *)sa)->sin_addr;
    }
    return &((struct sockaddr_in6 *)sa)->sin6_addr;
}


int main(int argc , char *argv[]){
    if(argc != 4){
        cerr<<"Usage talker remotehostname port message"<<endl;
        exit(1);
    }

    int status , sockFd , newFd;
    struct addrinfo hostInfo , *servInfo , *p;
    char s[INET6_ADDRSTRLEN];


    if( strlen(argv[3]) >= MAXMESSAGELENGTH){
        cerr<<"Message length is too long: Message should be 100 chars long"<<endl;
        exit(1);
    }
    
    memset(&hostInfo , 0  , sizeof hostInfo);
    hostInfo.ai_family = AF_UNSPEC;
    hostInfo.ai_socktype = SOCK_DGRAM;

    if((status = getaddrinfo(argv[1] , argv[2] , &hostInfo , &servInfo)) != 0){
        cerr<<"Error while fetching host Information"<<gai_strerror(status);
        exit(1);
    }

    for(p=servInfo ; p != nullptr ; p = p->ai_next){
        if((sockFd = socket(p->ai_family , p->ai_socktype , p->ai_protocol)) == -1){
            perror("socket");
            continue;
        }


        break;
    }

    if(p == NULL){
        cerr<<"Failed to connect with the listener";
        exit(1);
    }

    freeaddrinfo(servInfo);
    //Now the thing is i need ro send the message;


    int numbytes;   
    if((numbytes = sendto(sockFd , argv[3] ,  strlen(argv[3]) , 0 ,p->ai_addr , p->ai_addrlen )) == -1){
        perror("sendto");
        exit(1);
    }

    cout<<"Message sent successfully"<<endl;

    close(sockFd);

    exit(0);
}