#ifndef FILETRANSFERDATA_H
#define FILETRANSFERDATA_H
#include<string.h>
#include<fstream>
struct FileTransferData
{
static const int buffersize = 8192;
std::string fileName = "";
int fileSize = 0;
int bytesWritten = 0;
int fileOffset = 0;
int remainingBytes = 0;
std::ifstream inFileStream;
std::ofstream outFileStream;
char buffer[buffersize];
};
#endif // FILETRANSFERDATA_H
