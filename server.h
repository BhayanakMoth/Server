#ifndef SERVER_H
#define SERVER_H
#include "mainwindow.h"
#include <QApplication>
#include<WinSock2.h>
#include<iostream>
struct Client
{
    SOCKET socket;
};

class Server
{

 public:
   Server(std::string ip,int PORT);
   void Connect();
private:
   static void ClientConnectionThread();

  private:
    SOCKET sListen;
    Client Clients[10];
    SOCKADDR_IN addr_in;
    int sizeofaddr = sizeof(addr_in);
    int counter = 0;
    MainWindow window;
};
static Server * serverPtr;
#endif // SERVER_H
