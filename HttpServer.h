/**
 *@author: HaiZiChe
 *@date: 2022-5-24
 *@description: Simple micro http server
 *@contact：17690575336@163.com
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fstream>
#include <pthread.h>

#define  SERVER_PORT 9999
#define BUF_LEN  512
#define LISTEN_NUM 20
using namespace std;
void Server();
void *doHttpRequest(void *pClientSock);
void GETRequest(int clientSock, char *url);
void POSTRequest(int clientSock, char *url);
void SendImage(char *url, int clientSock);
void NotFound(int clientSock,char  *url);//404
void NotImplemented(int clientSock);//501
int getLine(int sock, char *buf, int size);

#endif //HTTPSERVER_H
