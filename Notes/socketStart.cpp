#include <iostream>


int showIp(struct addrinfo *&servInfo){
    

    return 0;
}



int main(){
     //AF -> Address family , used in struct sockaddr_in
     //PF -> protocol family , used while creating sockets => both are so closely related that even there value are same , but both must be used at different places.

     //PF_INET -> in your socket() call.
     //AF_INET -> in your struct call.


     //for creating a socket , simply go through that getaddrinfo process and feed it directly to the socekt() call;
}