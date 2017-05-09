#pragma once
#include "pch.h"
#include "BallManagerClass.h"
#include "GravityWellManager.h"

struct client_type
{
	int id;
	SOCKET socket;
};

#define BUFSIZE 655536
const char OPTION_VALUE = 1;

class Network
{
public:
	Network();	
	~Network();

	void Initialise(shared_ptr<ConfigClass> InConfig);

	void Connect();
	void Tick();
	void SendData();

	void SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager);
	void SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager);

	
	

private:
	bool InitUPDListener();
	void InitUDPBoardcaster();

	void InitServer();
	void ServerListen();
	

	bool InitClient();
	void ClientListen();

	void ServerSend(std::string sent_message);
	void ClientSend(std::string sent_message);

	void ExtractMsg(string msgbuffer);

	void SendIsPause(string &str);
	void SendGwPos(string &str);
	void SendGwForce();
	void SendBallPos(string& str);

	void recvPause(string input);
	void recvTimeScale(string input);
	void recvGwPos(string input);
	void recvGwForce(string input);
	void recvBallPos(string input);
	void recvBallRotate(string input);
	void recvBallOwner(string input);
	

private:
	shared_ptr<ConfigClass> mConfig;
	shared_ptr<BallManagerClass> mBallManager;
	shared_ptr<GravityWellManager> mGwManager;

	DX::StepTimer mNetTimer;
	
	int		mLocalPeerID;

	bool mIsHost;
	bool mIsEscaped;
	bool mNextServer;
	bool mConnReady;	//Sockets ready to accept incoming connection
	bool mConnected;	//Connection established between peers
	int		mUDPPort;
	string mHostIP;


	WSADATA WSAData;
	SOCKADDR_IN hostAddr, peerAddr;
	SOCKET hostSock, peerSock;

	SOCKET UDPSendSock;
	std::string HostIP;
	int		mTCPPort = 9176;
	

	int mNumOfClient;
	std::vector<client_type> mClientIndex;

	bool mLastPause;

};

