#include <iostream> 
#include <arpa/inet.h> //inet_pton inet_ntop
#include <netdb.h> //struct addrinfo , freeadrinfo , getnameinfo() gai_strerror
#include <sys/types.h> //gives types pid_t , size_t
#include <sys/socket.h> //core socket api , bind listen
#include <cstring> //memset

using namespace std;

int main(int argc , char *argv[]){
    int status;
    char ipstr[INET6_ADDRSTRLEN]; //this will store the ip , even if its ipv6;


    struct addrinfo hints , *res , *p;

    memset(&hints , 0 , sizeof hints);

    if(argc != 2){
        cerr<<"Usage scriptName hostname\n";
    }

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((status = getaddrinfo(argv[1] , NULL , &hints , &res)) != 0){
            cerr<<"Error while geting addrInfo"<<gai_strerror(status)<<"\n";
            exit(1);
    }

    //if successfull we will get the address linked list 

    cout<<"Ip address for: "<<argv[1]<<"\n";
    int val = 0;

    for(auto p = res ; p != nullptr ; p = p->ai_next){
        void *addr;
        string ipver;

        struct sockaddr_in *ipv4;
        struct sockaddr_in6 *ipv6;

        if(p->ai_family == AF_INET){ //if the ipaddress is v4
            ipv4 = (struct sockaddr_in*)p->ai_addr; //type casting it to sockaddr_in;
            addr = &(ipv4->sin_addr);
            ipver = "IPV4";
        } 
        else if(p->ai_family == AF_INET6){ //if the ipaddress is v6;
            ipv6  = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPV6";
        }

        inet_ntop(p->ai_family, addr , ipstr , sizeof ipstr); //network to printable -> convert the ip adress to readable string.
        cout<<ipver<<" "<<ipstr<<"\n";
        if(val++ == 4) break; //amount of ip address you want to display
    }

    freeaddrinfo(res);

    return 0;
    
}