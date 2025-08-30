#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

using namespace std;

#define MAXSIZE 100

void *get_ip_address(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &((struct sockaddr_in *)sa)->sin_addr;
    }
    return &((struct sockaddr_in6 *)sa)->sin6_addr;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "usage: listener PORT" << endl;
        exit(1);
    }

    struct addrinfo hints, *servInfo, *p;
    int status, sockFd;
    char buff[MAXSIZE];
    struct sockaddr_storage their_addr;
    socklen_t addr_len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM;  // UDP
    hints.ai_flags = AI_PASSIVE;     // use my IP

    if ((status = getaddrinfo(NULL, argv[1], &hints, &servInfo)) != 0)
    {
        cerr << "getaddrinfo error: " << gai_strerror(status) << endl;
        exit(1);
    }

    // Loop through all results and bind to the first possible one
    for (p = servInfo; p != nullptr; p = p->ai_next)
    {
        if ((sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("socket");
            continue;
        }

        if (bind(sockFd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("bind");
            close(sockFd);
            continue;
        }
        break;
    }

    if (p == nullptr)
    {
        cerr << "Failed to bind to port " << argv[1] << endl;
        exit(1);
    }

    freeaddrinfo(servInfo);

    cout << "Listening on port " << argv[1] << " ..." << endl;

    // Receive message
    addr_len = sizeof their_addr;
    int numbytes;
    if ((numbytes = recvfrom(sockFd, buff, MAXSIZE - 1, 0,
                             (struct sockaddr *)&their_addr, &addr_len)) == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    buff[numbytes] = '\0';

    char senderIP[INET6_ADDRSTRLEN];
    inet_ntop(their_addr.ss_family,
              get_ip_address((struct sockaddr *)&their_addr),
              senderIP, sizeof senderIP);

    cout << "Message received from " << senderIP << ": " << buff << endl;

    close(sockFd);
    return 0;
}
