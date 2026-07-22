#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Info.hpp"
#include "udpserver.hpp"
#include <iostream>
#include <string>
#include <poll.h>

using namespace std;

#define PORT     4242
#define MAXLINE  1024

int main(int argc, char* argv[]) {

    int sockfd;
    char buffer[MAXLINE];
    const char *hello = "READY";
    struct sockaddr_in servaddr;
    struct Point sig;

    // Create UDP socket

    parse_arguments(argc, argv, &sig);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct pollfd pfd = {.fd = sockfd, .events = POLLIN, .revents = 0};

    memset(&servaddr, 0, sizeof(servaddr));

    // Fill server address info
    servaddr.sin_family = AF_INET;              // IPv4
    servaddr.sin_port   = htons(PORT);          // Server port
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP

    socklen_t len = sizeof(servaddr);

    // Send message to server
    if (sendto(sockfd, hello, strlen(hello), MSG_CONFIRM,
           (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("Error sending hello message");
            exit(EXIT_FAILURE);
        }
    std::cout << "Hello message sent" << std::endl;
    printf("Hello message sent.\n");


    // Receive reply from server

    Info curdata(sig.x, sig.y);

    int n = 1;
    int step = 0;
    int ret = 1;

    while (n > 0)
    {
        //std::cout << step << std::endl;
        while(not_finished(buffer) && n > 0 && ret > 0)
        {
        //std::cout << "enter" << std::endl;
            ret = poll(&pfd, 1, 1000);

            if (ret == 0)
                break;

            //std::cout << "ret " << ret << std::endl;
            n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL,
                            (struct sockaddr *)&servaddr, &len);
            //std::cout << n << std::endl;

            buffer[n] = '\0';   // Null terminate received data
            curdata.ana(std::string(buffer));
        }

        if (n <= 0 || success(buffer) || ret == 0)
            break;

        memset(buffer, 0, MAXLINE);

        curdata.compute(step);

        std::string rep = curdata.response();

        std::cout << "Sending To Server\n" << ">>" << rep.c_str() << "<<" << std::endl;

        if (sendto(sockfd, rep.c_str(), rep.size(), MSG_CONFIRM,
            (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
            {
                perror("Error writing data to server");
                exit(EXIT_FAILURE);
            }
        step = step + 1;
    }

    close(sockfd);
    //curdata.display();
    if (n <= 0) 
    {
        perror("Error receiving data fron server");
        exit(EXIT_FAILURE);
    }

    if (ret == 0)
    {
        if (step == 0)
            std::cout << "\nServer is not available." << std::endl;
        else
            std::cout << "\nSimulation ended, tracking error." << std::endl;
    }
    else
    {
        std::cout << "\nSimulation correctly tracked !" << std::endl;
        int output = system("python test.py");
        exit(output);
    }

    exit(EXIT_SUCCESS);

}

bool not_finished(char buf[])
{
    std::string buf_st(buf);
    if (buf_st.compare("MSG_END") == 0)
        return false;
    if (buf_st.compare("GOODBYE.") == 0)
        return false;
    return true;
}

bool success(char buf[])
{
    std::string buf_st(buf);

    if (buf_st.compare("GOODBYE.") == 0)
        return true;
    return false;
}

void parse_arguments(int argc, char *argv[], struct Point *sig)
{
    if (argc >= 4)
    {
        std::cout << "./kalman : Too many arguments" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (argc == 1)
    {
        sig->x = 0.0001;
        sig->y = 0.1;
        return;
    }

    try
    {
        if (argc >= 2)
        {
            sig->x = stod(std::string(argv[1]));
            sig->y = 0.1;
        }

        if (argc == 2)
            return;

        if (argc == 3)
            sig->y = stod(std::string(argv[2]));
    }
    catch (...)
    {
        std::cout << "./kalman : Parsing error" << std::endl;
        exit(EXIT_FAILURE);
    }
}