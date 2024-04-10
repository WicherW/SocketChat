#define WIN32_LEAN_AND_MEAN
#include "SocketChat.h"
#include <WinSock2.h>
#include <iostream>
#include <mutex>
#include <WS2tcpip.h>
#include <vector>
#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) //przez inet_addr ktore jest przestarzale
#define ADDRES_MULTICAST "224.123.123.123"


using namespace std;

SocketChat::SocketChat(){

    SocketChat::TopBar();
    cout << "Podaj nick: ";
    cin >> nickname;
    system("cls");
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup nie dziala ;c %d\n", iResult);
        exit(0);
    }

    recieverThread = thread(&SocketChat::SetRecieverThread, this);
    senderThread = thread(&SocketChat::SetSenderThread, this);

    recieverThread.join();
    senderThread.join();
}

void SocketChat::TopBar(){
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Socket Multicast chat :) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}

void SocketChat::SendMessageToBuffer(string message) {

    lock_guard<mutex> lock(semafor);

    if(messages.size() == MESSAGE_MAX){
        messages.erase(messages.begin());
    }
    messages.push_back(message);

    system("cls");
    for(int i = 0; i < messages.size(); i++){
        cout << messages[i] << endl;
    } 
}

void SocketChat::SetSenderThread(){
 
    string buffer = "";
    string message = "";
    sockaddr_in destAddr;
    SOCKET senderSocket;
    
    senderSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(senderSocket == INVALID_SOCKET){
        cout << "sender: nie udalo sie zrobic socketu ;c" << endl;
        WSACleanup();
    }

    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT);
    destAddr.sin_addr.s_addr = inet_addr(ADDRES_MULTICAST);


    while (true) {
        getline(cin, buffer);
        message = nickname + ": " + buffer;

        int bytesFeedback = sendto(senderSocket, message.c_str(), message.length(), 0, (sockaddr*)&destAddr, sizeof(destAddr));
        if (bytesFeedback < 0) {
            cout << "sender: nie udalo sie wyslac ;c" << endl;
            break;
        }
    }

}

void SocketChat::SetRecieverThread(){
   
    SOCKET recevierSocket;
    sockaddr_in bindAddr;
    sockaddr_in fromAddr;
    ip_mreq mreq;
    int fromAddrLen;
    int reuse = 1;
    char buffer[1024];
    string message;
    
   
    recevierSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (recevierSocket == SOCKET_ERROR) {
        cout << "recevier: socket nie dziala ;c" << endl;
    }

    if ((setsockopt(recevierSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse))) < 0){
        cout << "recevier: setsockopt SOL_SOCKET, SO_REUSEADDR nie dziala ;c" << endl;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(ADDRES_MULTICAST);
    mreq.imr_interface.s_addr = INADDR_ANY; 

    if (setsockopt(recevierSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0) {
        cout << "recevier: blad ustawienia multicastu ;c" << endl;
        closesocket(recevierSocket);
        WSACleanup();
        return;
    }

  
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(PORT);
    bindAddr.sin_addr.s_addr = INADDR_ANY;


    if (bind(recevierSocket, (sockaddr*)&bindAddr, sizeof(bindAddr)) < 0) {
        cout << "recevier: bindowanie socketu nie dziala ;c" << endl;
        closesocket(recevierSocket);
        WSACleanup();
        return;
    }

    TopBar();
    
    while(true){

        fromAddrLen = sizeof(fromAddr);

        int recvBytes = recvfrom(recevierSocket, buffer, sizeof(buffer), 0, (sockaddr*)&fromAddr, &fromAddrLen);
        if (recvBytes < 0) {
            cout << "recevier: nie udalo sie odebrac ;c" << endl;
            break;
        }
        
        message = string(buffer, recvBytes);
        SendMessageToBuffer(message);
    }

}
