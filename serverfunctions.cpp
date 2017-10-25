#include "server.h"
bool Server::recvInt(int & _int32_t,int ID)
{
   int retnCheck = recv(Clients[ID].socket,(char*)&_int32_t,sizeof(int32_t),NULL);
   //if(!recvAll((char*)&_int32_t,sizeof(int32_t),ID))
   //{
     // return false;
    //}
   _int32_t = ntohl(_int32_t);
    return true;
}
bool Server::sendInt(int32_t _int32_t,int ID)
{
    _int32_t = htonl(_int32_t);
    if(!sendAll((char*)&_int32_t,sizeof(int32_t),ID))
     {
        return false;
     }
    return true;
}
bool Server::sendPacket(Packet _packetType,int ID)
{
  if(!sendAll((char*)&_packetType,sizeof(Packet),ID))
    return false;
  return true;
}
bool Server::sendString(std::string str,int ID)
{
    int bufferLength = str.length();
    if(!sendInt(bufferLength,ID))
        return false;
    int retnCheck = send(Clients[ID].socket,str.c_str(),bufferLength,NULL);
    if(retnCheck == SOCKET_ERROR)
     {
        return false;
     }
    return true;
}
bool Server::recvPacket(Packet &_packetType,int ID)
{

// int retnCheck = recv(Clients[ID].socket,(char*)&_packetType,sizeof(Packet),NULL);
 if(!recvAll((char*)&_packetType,sizeof(Packet),ID))
     return false;
 return true;
}
bool Server::recvString(std::string & str,int ID)
{
    int bufferLength = 0;
    if(!recvInt(bufferLength,ID))
        return false;
    char * buffer = new char[bufferLength];
    int retnCheck = recv(Clients[ID].socket,buffer,bufferLength,NULL);
    if(retnCheck == SOCKET_ERROR)
     {
        return false;
     }
    std::cout<<"Recieved Message!";
    str = buffer;
    return true;
}
bool Server::sendAll(char* data, int totalBytes, int ID )
{
    int bytesSent = 0;
    while(bytesSent<totalBytes)
    {
        int retnCheck = send(Clients[ID].socket,data+bytesSent,totalBytes-bytesSent,NULL);
        if(retnCheck==SOCKET_ERROR)
            return false;
        bytesSent+= retnCheck;
    }
    return true;
}
bool Server::recvAll(char* data, int totalBytes, int ID)
{
    int bytesRecvd = 0;
    while(bytesRecvd<totalBytes)
    {
        int retnCheck = send(Clients[ID].socket,data,totalBytes-bytesRecvd,NULL);
        if(retnCheck==SOCKET_ERROR||retnCheck==0)
            return false;
        bytesRecvd+= retnCheck;
    }
    return true;
}
