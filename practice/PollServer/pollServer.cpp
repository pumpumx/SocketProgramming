#include <iostream>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string_view>
#include <cstring>
#include <fstream>
#include <unistd.h>
using namespace std;

constexpr char* port = "3304";


// const char *inet_ntop2(void* addr , char* buf , size_t size){
//    struct sockaddr_storage *sas = addr;
//    struct 
    

// }


int getListenerSockets(void){
    int listener;
    int rv;
    int yes = 1;
    struct addrinfo hints, *ai , *p;

    memset(&hints ,0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //Take's ip of our own local machine 

    if((rv = getaddrinfo(NULL , port ,&hints ,&ai )) != 0){
        cout<<"Error while getting addressInfo"<<gai_strerror(rv);
        exit(1);
    }

    for(p = ai;p!=nullptr;p=p->ai_next){
        listener = socket(p->ai_family, p->ai_socktype,p->ai_protocol );
        if(listener < 0) continue;

        //To lose the "address already in use error"
        setsockopt(listener , SOL_SOCKET , SO_REUSEADDR , &yes , sizeof(int)); //Used like tuning the socket's  , TCP_NODELAY option disables nagle's algorithm 

        if(bind(listener , p->ai_addr , p->ai_addrlen) < 0){
            close(listener);
            continue;
        }




    }
    
}

int main(void){
    int listener;
    int fd_size = 5; //For five clients
    int fd_count = 5; 

    struct pollfd *pfds = new pollfd[fd_size]; 
    
    listener = getListenerSockets();

}