#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring> 
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;

#define BACKLOG 10

int main(int argc , char * argv[]){ //argc gives the number of arguments provided
    if(argc != 2){
        cerr<<"Usage : showIp www.hostname.com"<<endl;
    }   
    exit(1);

    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints , *res;
    
    memset(&hints , 0 , sizeof hints); //empties the hints struct completly
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //Fills the host ip automatically 

    int status; 

    if((status = getaddrinfo(argv[1] , "http" , &hints , &res)) != 0){
        cerr<<"Error while getting address , error: "<<gai_strerror(status); //gai_strerror takes the status and outputs the respective error binded with that specific status int
    }

    //if successfull , we will get the res linked list.

    //Make a socket
    int sockFd = socket(res->ai_family , res->ai_socktype , res->ai_protocol); //Now the socket has been created


    //in order to reuse the socket address , i.e to avoid the address already in use error
    int yes = 1;
    setsockopt(sockFd , SOL_SOCKET , SO_REUSEADDR, &yes ,sizeof yes); //

    //Bind the socket to port passed into the getaddrinfo , for server only !!!
    if((bind(sockFd , res->ai_addr , res->ai_addrlen) != 0)){
        cerr<<"failed to bind to the port specified";
    }; //return -1 on error

    if((listen(sockFd , BACKLOG)) != 0 ){
        cerr<<"Failed to listen"<<strerror(errno);
    }

    //Now accept incoming connections 
    addr_size = sizeof(their_addr);

    int newFd = accept(sockFd , (struct sockaddr *)&their_addr, &addr_size); //After this we will be ready to communicate on socket descriptor fd.

    //Now as the socket is successfully binded , we can connect it. 
    connect(sockFd , res->ai_addr , res->ai_addrlen); //return -1 on error.
}