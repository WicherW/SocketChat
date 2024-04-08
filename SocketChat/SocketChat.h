#pragma once
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <cstdio>
#include <mutex>
#include <thread>
#include <vector>
using namespace std;

const u_short PORT = 12345;
const int MESSAGE_MAX = 10;

class SocketChat {
private:
	string nickname;
	thread recieverThread;
	thread senderThread;
public:
	vector<string> messages;
	mutex semafor;
	SocketChat();
	~SocketChat();
	void Shutdown();
	void SetSenderThread();
	void SetRecieverThread();
	void SendMessageToBuffer(string message);
	void TopBar();
};
