#include <poll.h>
#include <iostream>

using namespace std; //Program to wait for 2.5 sec before reading from the input stream;


int main(void){
    struct pollfd pfds[1];

    pfds[0].fd = 0;
    pfds[0].events = POLLIN; //Tell me when ready to read

    cout<<"Hit ENTER or wait for 2.5 sec to be ready to read"<<endl;

    int num_events = poll(pfds , 1 , 2500); //timeout is in miliseconds


    if(num_events == 0){
        cout<<"Poll Timed out"<<endl;
    }
    else{
        int poll_happened = pfds[0].revents & POLLIN;
        
        if(poll_happened){
            cout<<"File descriptor is ready to read \n";
        }
        else{
            cout<<"Some error occured \n"<<pfds[0].revents;
        }
    }

    return 0;
    
}