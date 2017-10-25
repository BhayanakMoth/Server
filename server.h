#ifndef SERVER_H
#define SERVER_H
#include<WinSock2.h>
#include<iostream>
#include "filetransferdata.h"
struct Client
{
    SOCKET socket;
    std::string userName = " ";
    FileTransferData file;
};

class Server
{
  private:
   enum Packet
    {
        P_ChatMessage,
        P_Ping,
        P_FileTransferRequestFile,
        P_FileTransferEndOfFile,
        P_FileTransferByteBuffer,
        P_FileTransferRequestNextBuffer
    };
 public:
   Server(std::string ip,int PORT);
   void Connect();
 private:
   bool sendInt(int32_t _int32_t,int ID);
   bool sendPacket(Packet _packetType,int ID);
   bool sendAll(char* data,int sizeofdata,int ID);
   bool recvAll(char* data, int sizeofdata, int ID);
   bool recvInt(int32_t & _int32_t, int ID);
   bool recvPacket(Packet & packetType, int ID);
   bool recvString(std::string & str,int ID);
   bool sendString(std::string str, int ID);
   bool ProcessPacket(Packet packetType, int ID);
   bool HandleFileSend(int ID);
   static void ClientHandlerThread(int ID);

  private:

    Client Clients[10];
    SOCKADDR_IN addr_in;
    int sizeofaddr = sizeof(addr_in);
    int counter = 0;
    int vacantID[10]={0,1,2,3,4,5,6,7,8,9};
    int vacancy = 10;
    int lastVacantIndex = 10;
    int firstVacantID = 0;
};
static Server * serverPtr;
#endif // SERVER_H
