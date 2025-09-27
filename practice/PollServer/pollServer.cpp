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
#include <array>
using namespace std; // poll is simply like in order to handle multiple connection and checking there state all at a time.

constexpr char *port = "3304";

const char *inet_ntop2(void *addr, char *buf, size_t size)
{
    struct sockaddr_storage *sas = (struct sockaddr_storage *)addr;

    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;

    void *src;

    switch (sas->ss_family)
    {
    case AF_INET:
        sa4 = (struct sockaddr_in *)addr;
        src = &(sa4->sin_addr);
        break;
    case AF_INET6:
        sa6 = (struct sockaddr_in6 *)addr;
        src = &(sa6->sin6_addr);
        break;
    default:
        return NULL;
    }
    return inet_ntop(sas->ss_family , src , buf , size);
}

int getListenerSockets(void)
{ // Program to get the listener socket , socket() bind() listen()!!
    int listener;
    int rv;
    int yes = 1;
    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // Take's ip of our own local machine

    if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0)
    {
        cout << "Error while getting addressInfo" << gai_strerror(rv);
        exit(1);
    }

    for (p = ai; p != nullptr; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol); // Getting a socket descriptor
        if (listener < 0)
            continue;

        // To lose the "address already in use error"
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // Used like tuning the socket's  , TCP_NODELAY option disables nagle's algorithm

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        { // Binding to the port
          
            close(listener);
            continue;
        }
          cout<<"Binded to the port successfully"<<port<<endl;
        break;
    }

    if (p == NULL)
    {
        cerr << "Failed to get any socket";
        return -1;
    }

    freeaddrinfo(ai);

    if (listen(listener, 10) == -1)
    {
        return -1;
    }

    return listener;
}
void addToPfds(struct pollfd **pfds, int *fd_count, int *fd_size, int newFd)
{ // Adding a new file descriptor to the set

    if (*fd_count == *fd_size)
    {
        *fd_size = 2 * *fd_size;
        *pfds = (struct pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size)); // While using reallow always check for nullptr , i.e memory leaks
    }

    (*pfds)[*fd_count].events = POLLIN;
    (*pfds)[*fd_count].fd = newFd;
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

void deleteFromPfds(struct pollfd pfds[], int *fd_count, int i)
{
    pfds[i] = (pfds)[*fd_count - 1];

    (*fd_count)--;
}

void handleClientData(int listener, int *fd_count, struct pollfd *pfds, int &clientIndex)
{
    char buf[256];
    int nBytes = recv(pfds[clientIndex].fd , buf , sizeof buf , 0);

    int sender_fd = pfds[clientIndex].fd;
    if(nBytes <=0 ){
        if(nBytes == 0 ) cerr<<"No data sent by the user"<<endl;
        else{
            perror("recv");
        }
        close(pfds[clientIndex].fd);

        deleteFromPfds(pfds , fd_count , clientIndex);

        (clientIndex)--;
    } //Got error or connection closed by the user;

    else{ // We got some good data from the client
        cout<<"We got a message from fd: "<<sender_fd<<nBytes<<buf;

        //Let's broadcast it to everyone , except the server and the client

        for( int i = 0; i<*fd_count ; i++){
            int dest_fd = pfds[i].fd;
            if(dest_fd != listener && dest_fd != sender_fd){
                
                if(send(dest_fd , buf ,nBytes , 0) == -1){
                    cerr<<"Failed to broadcast the message to user with fd: "<<dest_fd;
                    perror("send");
                }
                
                cout<<"Message broadcasted successfully"<<endl;
            }
        }
    }

}
void handleNewConnection(int listener, int *fd_count, int *fd_size, struct pollfd **pfds)
{
    // Wen you get a new connection you basically add the client address

    struct sockaddr_storage remoteAddr;
    socklen_t addrlen;
    int newFd;
    array<char, INET6_ADDRSTRLEN> buf;
    addrlen = sizeof remoteAddr;

    newFd = accept(listener, (struct sockaddr *)&remoteAddr, &addrlen);

    if (newFd == -1)
    {
        cerr << "Error accepting client request\n";
    }
    else
    {
        addToPfds(pfds, fd_count, fd_size, newFd);
        cout << "New Connection got from client: " << inet_ntop2(&remoteAddr, buf.data(), buf.size()) << endl;
    }
}
void process_connections(int listener, int *fd_count, int *fd_size, struct pollfd **pfds)
{

    for (int i = 0; i < *fd_count; i++)
    {

        if ((*pfds)[i].revents & (POLLIN | POLLOUT))
        {

            // We got a connection

            if ((*pfds)[i].fd == listener)
            { // It means that we are the listener
                handleNewConnection(listener, fd_count, fd_size, pfds);
            }
            else
            {
                handleClientData(listener, fd_count, *pfds, i);
            }
        }
    }
}

int main(void)
{
    int listener;
    int fd_size = 5; // For five clients
    int fd_count = 0;

    struct pollfd *pfds = new pollfd[fd_size];

    listener = getListenerSockets();

    if (listener == -1)
    {
        cerr << "Unable to utilize a port to listen \n";
        exit(1);
    }
    // Add the listener to set , Not the main roll of poll() system call will come into action.
    // Report ready to read on incoming connections

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1;

    puts("PollServer: Waiting for connection");

    // Main loop

    for (;;)
    {
        int pollCount = poll(pfds, fd_count, -1);

        if (pollCount == -1)
        {
            cerr << "Unable to poll";
            exit(1);
        }

        // Run through connection looking for data to read ;
        process_connections(listener, &fd_count, &fd_size, &pfds);
    }
    delete[] pfds;
}