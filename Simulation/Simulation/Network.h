#pragma once
#include "pch.h"
#include "BallManagerClass.h"
#include "GravityWellManager.h"

struct client_type
{
	int id;
	SOCKET socket;
};

class Network
{
public:
	Network();	
	~Network();

	void Initialise(shared_ptr<ConfigClass> InConfig);

	void Tick();
	void Run();

	void SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager);
	void SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager);


	void SendGwPos();


private:
	void InitServer();
	void InitClient();

	void ServerListen();
	void ClientListen();

private:
	shared_ptr<ConfigClass> mConfig;
	shared_ptr<BallManagerClass> mBallManager;
	shared_ptr<GravityWellManager> mGwManager;

	DX::StepTimer mNetTimer;

	std::mutex mClientIndexMutex;
	std::mutex mIOmutex;

	int		mLocalPeerID;

	bool mIsEscaped;
	int		mPort;
	string mHostIP;


	const char OPTION_VALUE = 1;
	WSADATA WSAData;
	SOCKADDR_IN hostAddr, peerAddr;
	SOCKET hostSock, peerSock;

	std::vector<client_type> mClientIndex;

	union bytefloat
	{
		char bytes[sizeof(float)];
		float fval;
	};

	inline void thing()
	{
		std::string fs = "1912.09";
		float f = 1912.09f;
		bytefloat bytes;
		bytes.fval = f;
		bytes.bytes[0];
		bytes.bytes[1];
		bytes.bytes[2];
		bytes.bytes[3];
	}

};

