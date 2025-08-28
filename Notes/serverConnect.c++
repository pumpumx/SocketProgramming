#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

using namespace std;


int connectToServer(struct addrinfo *&servInfo){ 
        struct addrinfo hints;
        int status;
        memset(&hints , 0 , sizeof hints); //Good practice.
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if((status = getaddrinfo("www.google.com" , "3490" , &hints , &servInfo)) != 0){
                cout<<"Error while connecting to the client"<<gai_strerror(status);
                exit(1); //exit the process
        }

        //if successful then the servInfo pointer points to a linked list which contains the available socket address.

        return 0;
}

int main()
{

        //inet_pton return -1 on error , and 0 if i messed up 
        //Use getaddrinfo()

        int status ;
        struct addrinfo hints; 
        struct addrinfo *servinfo; //This is basically the pointer which will point to the linked list provided by the hints struct 

        /*memset(&hints , 0 , sizeof hints); //it makes sure that the struct is empty , memset copies a byte from memory to another location , in this secnario , the memory of hints is being copied to 0 i.e making sure that the struct is empty


        hints.ai_family = AF_UNSPEC;//Dosen't care if it's ipv4 or ipv6
        hints.ai_socktype = SOCK_STREAM; //tcp stream sockets
        hints.ai_flags = AI_PASSIVE; //Fills in the ip for me i.e i am a server choose my localaddress , good as you don't have to hardcode your ip address
        
        if((status = getaddrinfo(NULL , "3490" , &hints , &servinfo)) != 0){ //getaddrinfo() returns a linked list!! 
                gai_strerror(status);
                exit(1);
        }*/

        connectToServer(servinfo);
                
        freeaddrinfo(servinfo); //Always free the allocated memory 
                
}



