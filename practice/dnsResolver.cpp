#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring> 
#include <sys/types.h>



int main(int argc , char ** argv[]){
    if(argc != 2){
        cerr<<"Usage : showIp www.hostname.com"<<endl;
    }
    exit(1);
}