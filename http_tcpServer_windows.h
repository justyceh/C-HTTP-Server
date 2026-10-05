#ifndef INCLUDED_HTTP_SERVER_WINDOWS
#define INCLUDED_HTTP_SERVER_WINDOWS
// Library includes and preprocesser directives ^
#include <stdio.h>
#include <winsock.h>
#include <string>
#include <stdlib.h>

// Here we define a namespace called http, this means we will have to add using namespace http;
// Or use the :: scope resolution operator anytime before we call one of the functions or classes
// in this namespace
namespace http{

    class TcpServer{
        public:
        // First one is class constructor, second is class destructor
        TcpServer(std::string ip_address, int port);
        ~TcpServer();
        void startListen();
        private:
        // Define all our variables for our sockets
        std::string m_ip_address;
        int m_port;
        SOCKET m_socket;
        SOCKET m_new_socket;
        long m_incomingMessage;
        struct sockaddr_in m_socketAddress;
        int m_socketAddress_len;
        std::string m_serverMessage;
        WSAData m_wsaData;

        int startServer();
        void closeServer();
        void acceptConnection(SOCKET&);
        void sendResponse();
        std::string buildResponse();
    };

} // namespace http
#endif