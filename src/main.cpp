
#include "../include/server.h"

using namespace std;

const int port = 9999;

int main(int argc, char *argv[])
{
    int threadNum = 5;
    HttpServer httpServer(port);
    httpServer.run(threadNum);
    return 0;
}