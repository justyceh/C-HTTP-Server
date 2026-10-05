#include <http_tcpServer_windows.h>
#include <iostream>
#include <sstream>


namespace {
    const int BUFFER_SIZE = 30720;
    void log(const std::string &message){
        std::cout << message << std::endl;
    }
    void exitWithError(const std::string &errorMessage){
        std::cout << WSAGetLastError() << std::endl;
        log("ERROR" + errorMessage);
        exit(1);
    }
}

namespace http{
    // TCPServer constructor, we use member intialization so the variables are intialized before the constructor runs
    TcpServer::TcpServer(std::string ip_address, int port) : m_ip_address(ip_address), m_port(port), m_socket(),
    m_new_socket(), m_incomingMessage(), m_socketAddress(), m_socketAddress_len(sizeof(m_socketAddress)), m_serverMessage(buildResponse()), m_wsaData(){
        // Define the socket address characteristics, family, port, address
        m_socketAddress.sin_family = AF_INET;
        m_socketAddress.sin_port = htons(m_port);
        m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());
        
        if(startServer() != 0){
            std::ostringstream ss;
            ss << "Failed to start server with port: " << ntohs(m_socketAddress.sin_port);
            log(ss.str()); 
        }
        
    }
    TcpServer::~TcpServer(){
        closeServer();
    }
    int TcpServer::startServer(){
        // Here we use the WSAstartup function to enable window sockets, using the makeword
        // to specify a version 2.0 and &m_wsaData is a reference to the variable
        // which holds the data related to it which will belong to our class
        if(WSAStartup(MAKEWORD(2, 0), &m_wsaData) != 0){
            exitWithError("WSA startup failed!");
        }
        // Here we make a socket using a domain, type, and protocol
        // Domain represents the communication protocol family the socket will belong to
        // For tcp/ip we use the af_inet which supports the Ipv4 internet protocols
        // Type specifies the structure that the communication socket will allow for
        // We use sock_stream to allow for full duplex reliable byte streams
        // With these two settings we use the default 0 protocol to support the two to make them work
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket < 0){
            exitWithError("Cannot create socket");
            return 1;
        }
        // Bind ties a socket address to a given socket
        if(bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0){
            exitWithError("Cannot connect to socket address");
            return 1;
        }
        return 0;
    }
    void TcpServer::closeServer(){
        closesocket(m_socket);
        closesocket(m_new_socket);
        WSACleanup();
        exit(0);
    }
    void TcpServer::startListen()
    {
        // Listens for connections on a queue with max size of 20, prevents new connections
        // once 20 are already made
        if (listen(m_socket, 20) < 0)
        {
            exitWithError("Socket listen failed");
        }

        std::ostringstream ss;
        ss << "\n*** Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << " PORT: " << ntohs(m_socketAddress.sin_port) << " ***\n\n";
        log(ss.str());

        int bytesReceived;

        while (true)
        {
            log("====== Waiting for a new connection ======\n\n\n");
            acceptConnection(m_new_socket);

            char buffer[BUFFER_SIZE] = {0};
            bytesReceived = recv(m_new_socket, buffer, BUFFER_SIZE, 0);
            if (bytesReceived < 0)
            {
                exitWithError("Failed to receive bytes from client socket connection");
            }

            std::ostringstream ss;
            ss << "------ Received Request from client ------\n\n";
            log(ss.str());

            sendResponse();

            closesocket(m_new_socket);
        }
    }
    void TcpServer::acceptConnection(SOCKET &new_socket){
        // This function here creates a new socket that makes a connection thread between the client and server
        new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
        if(new_socket < 0){
            std::ostringstream ss;
            ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr) << "; PORT: " << ntohs(m_socketAddress.sin_port);
            exitWithError(ss.str());
        }
    }
    void TcpServer::sendResponse()
    {
        int bytesSent;
        long totalBytesSent = 0;

        while (totalBytesSent < m_serverMessage.size())
        {
            bytesSent = send(m_new_socket, m_serverMessage.c_str(), m_serverMessage.size(), 0);
            if (bytesSent < 0)
            {
                break;
            }
            totalBytesSent += bytesSent;
        }

        if (totalBytesSent == m_serverMessage.size())
        {
            log("------ Server Response sent to client ------\n\n");
        }
        else
        {
            log("Error sending response to client.");
        }
    }
    std::string TcpServer::buildResponse()
    {
        std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
           << htmlFile;

        return ss.str();
    }


} // namespace http