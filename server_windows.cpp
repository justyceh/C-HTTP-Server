#include <iostream>
#include <http_tcpServer_windows.h>
using namespace std;
using namespace http;

int main(){
    // Creates a new tcpserver object using vexing parse syntax
    TcpServer server = TcpServer("127.0.0.1", 8080);
    server.startListen();
    return 0;
}