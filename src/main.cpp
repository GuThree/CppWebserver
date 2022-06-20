#include "../include/server.h"
#include "../include/utility.h"
#include <signal.h>

const int port = 10022;

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGTERM, signal_handler);
    HttpServer httpServer(port);
    httpServer.run(8, 5000);
    return 0;
}