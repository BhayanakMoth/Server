#include <QCoreApplication>
#include <QFile>
#include "server.h"
int main()
{
    QCoreApplication a();
    std::string ip = "127.0.0.1";
    int port = 1111;
    Server chatter(ip,port);
    chatter.Connect();
    return 0;
}


/*

*/
