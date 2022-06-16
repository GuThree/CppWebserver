
#include "../include/server.h"

using namespace std;

const int port = 10022;

int main(int argc, char *argv[])
{
    HttpServer httpServer(port);
    httpServer.run(16, 5000);
    return 0;
}