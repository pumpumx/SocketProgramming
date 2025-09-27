#include <iostream>
#include <fcntl.h>
#include <wait.h>
#include <sys/socket.h>
using namespace std;


void reapZombie(int s){
    (void) s; //explixity ignore the signaler to avoid compiler warnings

    int savedErrno = errno;
    while((waitpid(-1,NULL, WNOHANG) > 0)); //WNOHANG -> don't block when any child process exit

    errno = savedErrno;    

}

int main(int argc , char * argv[]){ 
    //If you don't want a socket to be blocking you have to use fnctl 

    int sockFd = socket(PF_INET6 , SOCK_DGRAM , 0 ); // 0 -> typically means tcp

    fcntl(sockFd , F_SETFL , O_NONBLOCK); //This set the socket to non blocking
                                            

    //Poll is horibbly slow when it comes to handling large number of connections , use libevent library instead


    
}