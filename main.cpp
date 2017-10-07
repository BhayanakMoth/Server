#include "server.h"
int main(int argc, char *argv[])
{
    std::string ip = "127.0.0.1";
    QApplication a(argc, argv);
    Server server(ip,1111);
    server.Connect();
    return a.exec();
}
