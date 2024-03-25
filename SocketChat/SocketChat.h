#pragma once
#include <string>
#include <WinSock2.h>
#include <cstdio>
#include <thread>
using namespace std;

const u_short PORT = 12345;

class SocketChat {
private:
	string nickname;
	thread recieverThread;
	thread senderThread;
	string messages[10];
public:
	SocketChat();
	~SocketChat();
	void Shutdown();
	void SetSenderThread();
	void SetRecieverThread();
	void TopBar();
};
