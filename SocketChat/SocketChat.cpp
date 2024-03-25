#define WIN32_LEAN_AND_MEAN
#include "SocketChat.h"
#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) //przez inet_addr ktore jest przestarzale

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
        SocketChat::Shutdown();
        exit(0);
    }

    recieverThread = thread(&SocketChat::SetRecieverThread, this);
    senderThread = thread(&SocketChat::SetSenderThread, this);

    recieverThread.join();
    senderThread.join();
}

SocketChat::~SocketChat() {
    SocketChat::Shutdown();
}

void SocketChat::TopBar(){
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Socket Multicast chat :) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}

void SocketChat::Shutdown(){
    cout << "xd";
}

void SocketChat::SetSenderThread(){
 
    //int ttl = 0;
    string buffer = "";
    string message = "";
    sockaddr_in destAddr;
    SOCKET senderSocket;
    

    senderSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(senderSocket == INVALID_SOCKET){
        cout << "sender: nie udalo sie zrobic socketu ;c" << endl;
        WSACleanup();
    }

    /*if (setsockopt(senderSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl)) < 0) {
        cout << "sender: blad ustawienia multicast ttl ;c" << endl;
        closesocket(senderSocket);
        WSACleanup();
        return;
    }*/

    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(PORT);
    destAddr.sin_addr.s_addr = inet_addr("239.255.255.255");

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
 
    mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.255"); 
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


    // bind wrzuca dane ze struktury ktore zawieraja dane konfiguracji
    if (bind(recevierSocket, (sockaddr*)&bindAddr, sizeof(bindAddr)) < 0) {
        cout << "recevier: bindowanie socketu nie dziala ;c" << endl;
        closesocket(recevierSocket);
        WSACleanup();
        return;
    }

    SocketChat::TopBar();
    
    while(true){

        fromAddrLen = sizeof(fromAddr);

        int recvBytes = recvfrom(recevierSocket, buffer, sizeof(buffer), 0, (sockaddr*)&fromAddr, &fromAddrLen);
        if (recvBytes < 0) {
            cout << "recevier: nie udalo sie odebrac ;c" << endl;
            break;
        }
        
        message = string(buffer, recvBytes);
        cout << message << endl;
    }

}
