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
    window.show();
    QString debug = "Server constructed.\n";
    window.WriteToServerArea(debug);

}

void Server::Connect()
{
    sListen = socket(AF_INET,SOCK_STREAM,NULL);
    bind(sListen,(sockaddr*)&addr_in,sizeofaddr);
    listen(sListen,SOMAXCONN);

    QString debug = "Listener socket has been binded to SockAddr and it is listening.\n";
    window.WriteToServerArea(debug);
    CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ClientConnectionThread,NULL,NULL,NULL);
}
void Server::ClientConnectionThread()
{
    SOCKET dock;
      int counter = 0;
    QString debug = "Entering While Loop.\n";
    serverPtr->window.WriteToServerArea(debug);
    while(true)
    {
      dock = accept(serverPtr->sListen,(SOCKADDR*)&serverPtr->addr_in,&serverPtr->sizeofaddr);

     if(dock!=INVALID_SOCKET)
      {
       if(serverPtr->firstVacantID == serverPtr->lastVacantIndex)
           continue;
       int ID  = serverPtr->vacantID[serverPtr->firstVacantID];
       serverPtr->firstVacantID++;
       serverPtr->Clients[ID].socket = dock;
       dock = INVALID_SOCKET;
       std::string buffer = "MIC CHECK! MIC CHECK! MIC CHECK!";
       int bufferLength = buffer.length();
       send(serverPtr->Clients[ID].socket,(char*)&bufferLength,sizeof(int),NULL);
       send(serverPtr->Clients[ID].socket,buffer.c_str(),bufferLength,NULL);
       serverPtr->window.WriteToServerArea("MOTD sent to Client #: "+QVariant(ID).toString()+"\n");
       CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)ClientHandlerThread,(LPVOID)ID,NULL,NULL);
      }
   }
}
void Server::ClientHandlerThread(int ID)
{
	int len = 0;
    while(true)
    {
        int retnCheck = recv(serverPtr->Clients[ID].socket,(char*)&len,4,NULL);
        if(retnCheck == 0)
        {
            continue;
        }
        if(retnCheck == SOCKET_ERROR)
        {
            serverPtr->window.WriteToServerArea("Connection Error with Client ID: #"+QVariant(ID).toString());
            break;
        }
		ProcessMessage();
    }
    serverPtr->firstVacantID--;
    serverPtr->vacantID[serverPtr->firstVacantID] = ID;
}
void Server::ProcessMessage(int len, int ID)
{
   char * message = new char[len];
   recv(Client[ID].socket,message,len,NULL);
   for(int i = 0; i<10; i++)
   {
	  if(i == ID)
		  continue;
	  send(Client[i].socket,(char*)&len,4,NULL);
	  send(Client[i].socket,buffer,len,NULL);
   }
}