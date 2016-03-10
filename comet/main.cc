#include "server.h"

int main()
{
    comet::Server server;
    server.Start("0.0.0.0", 12345, 5);
}
