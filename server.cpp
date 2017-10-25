#include "server.h"

Server::Server(std::string ip,int Port)
{
    WSAData wsad;
    WORD ver = MAKEWORD(2,2);
    WSAStartup(ver,&wsad);
    addr_in.sin_addr.s_addr = inet_addr(ip.c_str());

    addr_in.sin_port = htons(Port);
    addr_in.sin_family = AF_INET;
    serverPtr = this;
}

void Server::Connect()
{
    SOCKET sListen = socket(AF_INET,SOCK_STREAM,NULL);
    bind(sListen,(sockaddr*)&addr_in,sizeofaddr);
    listen(sListen,SOMAXCONN);
    SOCKET dock;
   while(true)
  {
     dock = accept(sListen,(SOCKADDR*)&addr_in,&sizeofaddr);

     if(dock!=INVALID_SOCKET)
      {

         int len = 0;
         recv(dock,(char*)&len,4,0);
         char * _buffer= new char[len+1];
         _buffer[len] = '\0';
         recv(dock,_buffer,len,NULL);
         if(firstVacantID == lastVacantIndex)
             continue;
         int ID  = vacantID[firstVacantID];
         firstVacantID++;
         Clients[ID].socket = dock;
         Clients[ID].userName = _buffer;
         std::cout<<Clients[ID].userName<<"Has connected.";
         delete[] _buffer;
         dock=INVALID_SOCKET;


        std::string buffer = "MIC CHECK! MIC CHECK! MIC CHECK!";
        Packet p = P_ChatMessage;
        int bufferLength = buffer.length();
        sendPacket(p,ID);
        sendString(buffer,ID);
        CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ClientHandlerThread,(LPVOID)ID,NULL,NULL);

      }
    else continue;
}
}
bool Server::ProcessPacket(Packet packetType,int ID)
{
  switch(packetType)
   {
     case P_ChatMessage:
     {
      int bufferLength=0;
      recvInt(bufferLength,ID);
      char * buffer = new char[bufferLength];
      recv(Clients[ID].socket,buffer,bufferLength,NULL);

      std::string message =Clients[ID].userName+": "+buffer;
      std::cout<<std::endl<<Clients[ID].userName<<"Has sent :"<<message;
      bufferLength = message.length();
      for(int i = 0; i<10;++i)
       {
          if(i==ID)
              continue;

          sendPacket(packetType,i);
          sendInt(bufferLength,i);
          send(Clients[i].socket,message.c_str(),bufferLength,NULL);

       }
       break;
      }
      case P_FileTransferRequestFile:
       {
         int bufferLength=0;
         recvInt(bufferLength,ID);
         char * buffer = new char[bufferLength];
         recv(Clients[ID].socket,buffer,bufferLength,NULL);
         std::string fileName = buffer;
         Clients[ID].file.inFileStream.open(fileName,std::ios::binary|std::ios::ate);
         if(!Clients[ID].file.inFileStream.is_open())
         {
             std::cout<<"Client: "<<ID<<" requested an unknown file.";
             std::string Err = "The file "+fileName+" was not found.";
             Packet p = P_ChatMessage;
             sendPacket(p,ID);
             sendString(Err,ID);
             return true;
         }
         Clients[ID].file.fileName = fileName;
         Clients[ID].file.fileOffset = 0;
         Clients[ID].file.fileSize = Clients[ID].file.inFileStream.tellg();
         Clients[ID].file.inFileStream.seekg(0);
         if(!HandleFileSend(ID))
             return false;
         break;
       }
     case P_FileTransferByteBuffer:
      {
         if(!HandleFileSend(ID))
             return false;
      break;
      }
      default:
      {
         std::cout<<"UNKNOWN PACKET RECVD FROM CLIENT: "<<ID;
         break;
      }
    }
 }
bool Server::HandleFileSend(int ID)
{
    if(Clients[ID].file.fileOffset >=Clients[ID].file.fileSize)
   {
        if(!sendPacket(P_FileTransferEndOfFile,ID))
        {
            return false;
        }
        return true;
    }

    Clients[ID].file.remainingBytes = Clients[ID].file.fileSize - Clients[ID].file.fileOffset;


    if(Clients[ID].file.remainingBytes > Clients[ID].file.buffersize)
    {
        Clients[ID].file.inFileStream.read(Clients[ID].file.buffer,Clients[ID].file.buffersize);
        if(!sendPacket(P_FileTransferByteBuffer,ID))
            return false;
        sendInt(Clients[ID].file.buffersize,ID);
        sendAll(Clients[ID].file.buffer,Clients[ID].file.buffersize,ID);
        Clients[ID].file.fileOffset += Clients[ID].file.buffersize;
        std::cout<<"File Byte Buffer Sent: "<<Clients[ID].file.buffersize<<std::endl;

    }
    else
    {
        Clients[ID].file.inFileStream.read(Clients[ID].file.buffer,Clients[ID].file.remainingBytes);
        if(!sendPacket(P_FileTransferByteBuffer,ID))
            return false;
        sendInt(Clients[ID].file.remainingBytes,ID);
        send(Clients[ID].socket,Clients[ID].file.buffer,Clients[ID].file.remainingBytes,NULL);
        Clients[ID].file.fileOffset +=Clients[ID].file.remainingBytes;
        std::cout<<"File Byte Buffer Sent: "<<Clients[ID].file.remainingBytes<<std::endl;
    }

        std::cout<<std::endl<<"File Sent: "<<Clients[ID].file.fileName<<std::endl;
    std::cout<<std::endl<<"File Size(byte)"<<Clients[ID].file.fileSize<<std::endl<<std::endl;
    return true;
}

void Server::ClientHandlerThread(int ID)
{
    int retnCheck = 0;
    while(true)
    {
       Packet packetType;
       retnCheck = recv(serverPtr->Clients[ID].socket,(char*)&packetType,sizeof(Packet),0);
       if(retnCheck!=0&&retnCheck!=SOCKET_ERROR)
           serverPtr->ProcessPacket(packetType,ID);
       else if(retnCheck==SOCKET_ERROR)
       {
           serverPtr->Clients[ID].socket = INVALID_SOCKET;
           serverPtr->firstVacantID--;
           serverPtr->vacantID[serverPtr->firstVacantID] = ID;
           break;
      }
    }
}
