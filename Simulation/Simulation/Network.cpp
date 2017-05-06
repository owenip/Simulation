#include "Network.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")


Network::Network() :
	mIsEscaped(false)
{
	
}


Network::~Network()
{
	mConfig.reset();
	mBallManager.reset();
	mGwManager.reset();
}

void Network::Initialise(shared_ptr<ConfigClass> InConfig)
{
	mConfig = InConfig;
	mHostIP = mConfig->GetHostIP();
	mLocalPeerID = mConfig->GetPeerID();
}

void Network::Tick()
{
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	if(mConfig->GetInitServer())
	{
		//Init Server
		this->InitServer();
		std::thread thread_netowrk(&Network::ServerListen, this);

		while (true)
		{
			mNetTimer.Tick([&]
			{
				//Sending Data
				this->Run();
			});
			//Constantly Listening
			
			if (mConfig->GetIsEscaped())
			{
				thread_netowrk.join();
				//Close listening socket
				closesocket(hostSock);
				break;
			}
		}
	}
	else
	{
		//Init Client 	
		this->InitClient();
		std::thread thread_netowrk(&Network::ClientListen, this);
		while (true)
		{
			mNetTimer.Tick([&]
			{
				//Sending Data
				//this->Run();
			});
			//Constantly Listening
			

			if (mConfig->GetIsEscaped())
			{
				//Close listening socket				
				closesocket(peerSock);				
				break;
			}
		}
	}
	//Clean up Winsock
	WSACleanup();
}

void Network::Run()
{
	SendGwPos();
}

void Network::SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager)
{
	mBallManager = InBallManager;
	return;
}

void Network::SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager)
{
	mGwManager = InGwManager;
}

void Network::SendGwPos()
{
	std::string sent_message;
	ostringstream convert;
	convert << "|GP|";
	SimpleMath::Vector3 GwPos= mGwManager->GwGetPos(mLocalPeerID);
	convert << GwPos.x << "|" 
			<< GwPos.y << "|" 
			<< GwPos.z << "|";
		
	sent_message = convert.str();
	sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;

	if (mConfig->GetInitServer())
	{
		if (send(hostSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "HOST: FAILED TO SEND" << std::endl;
		}
	}
	else
	{
		if (send(peerSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "PEER: FAILED TO SEND" << std::endl;
		}
	}
	
}

void Network::InitServer()
{	
	mPort = mConfig->GetPortNum();
	hostSock = socket(AF_INET, SOCK_STREAM, 0);

	//Setup socket options
	setsockopt(hostSock, SOL_SOCKET, SO_REUSEADDR, &OPTION_VALUE, sizeof(int)); //Make it possible to re-bind to a port that was used within the last 2 minutes
	setsockopt(hostSock, IPPROTO_TCP, TCP_NODELAY, &OPTION_VALUE, sizeof(int)); //Used for interactive programs
		
	hostAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(mPort);

	::bind(hostSock, (SOCKADDR *)&hostAddr, sizeof(hostAddr));

	listen(hostSock, 5);

	std::cout << "HOST: LISTENING..." << std::endl;
	
}

void Network::InitClient()
{
	mPort = mConfig->GetPortNum();
	peerSock = socket(AF_INET, SOCK_STREAM, 0);
	peerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, mHostIP.c_str(), &peerAddr.sin_addr);
	
	peerAddr.sin_port = htons(mPort);

	int iResult = connect(peerSock, (SOCKADDR *)&peerAddr, sizeof(peerAddr));
	if (iResult == SOCKET_ERROR) 
	{
		closesocket(peerSock);
		peerSock = INVALID_SOCKET;
		return;
	}
}

void Network::ServerListen()
{
	while (true)
	{
		SOCKET incoming = INVALID_SOCKET;
		incoming = accept(hostSock, NULL, NULL);

		if (incoming == INVALID_SOCKET)
		{
			std::cout << "INVALID SOCKET: ERROR ACCEPTING" << std::endl;
			
			continue;

		}

		std::string Lenstr = "";
		char msgLengthBuffer = NULL;
		int iResult = 0;

		while(true)
		{
			//Checking msg header if not start "OI" ignore
			iResult = recv(incoming, &msgLengthBuffer, 2, 0);
			if (iResult == SOCKET_ERROR)
			{
				//std::cout << "HOST: FAILED TO RECIEVE0" << std::endl;
				break;
			}
			if (msgLengthBuffer == 'OI')
				break;
			else
			{
				//Get msg length
				Lenstr = "";
				msgLengthBuffer = NULL;
				while (true)
				{
					if (msgLengthBuffer == '|')
								break;

					iResult = recv(incoming, &msgLengthBuffer, 1, 0);
					if (iResult == SOCKET_ERROR)
					{
						std::cout << "HOST: FAILED TO RECIEVE1" << std::endl;
						break;
					}
					Lenstr += msgLengthBuffer;
				}
				if (iResult <= 0)
				{					
					break;
				}
				msgLengthBuffer = NULL;
				int msgLen = std::stoi(Lenstr);

				//ReadIn Actual msg	
				char msgbuffer[512];
				if (recv(incoming, msgbuffer, msgLen, 0) == SOCKET_ERROR)
				{
					std::cout << "HOST: FAILED TO RECIEVE2" << std::endl;
					break;
				}
				//Extract msg
				std::string msg = msgbuffer;
				msg = msg.substr(0, msgLen);
				std::cout << "RECIEVED: " << msg << std::endl;
			}
		}
		
	}
}

void Network::ClientListen()
{
	while (true)
	{
		SOCKET incoming = INVALID_SOCKET;
		incoming = accept(hostSock, NULL, NULL);

		if (incoming == INVALID_SOCKET)
		{
			std::cout << "INVALID SOCKET: ERROR ACCEPTING" << std::endl;

			continue;

		}

		std::string Lenstr = "";
		char msgLengthBuffer = NULL;
		int iResult = 0;

		while (true)
		{
			//Checking msg header if not start "OI" ignore
			iResult = recv(incoming, &msgLengthBuffer, 2, 0);
			if (iResult == SOCKET_ERROR)
			{
				//std::cout << "HOST: FAILED TO RECIEVE0" << std::endl;
				break;
			}
			if (msgLengthBuffer == 'OI')
				break;
			else
			{
				//Get msg length
				Lenstr = "";
				msgLengthBuffer = NULL;
				while (true)
				{
					if (msgLengthBuffer == '|')
						break;

					iResult = recv(incoming, &msgLengthBuffer, 1, 0);
					if (iResult == SOCKET_ERROR)
					{
						std::cout << "HOST: FAILED TO RECIEVE1" << std::endl;
						break;
					}
					Lenstr += msgLengthBuffer;
				}
				if (iResult <= 0)
				{
					break;
				}
				msgLengthBuffer = NULL;
				int msgLen = std::stoi(Lenstr);

				//ReadIn Actual msg	
				char msgbuffer[512];
				if (recv(incoming, msgbuffer, msgLen, 0) == SOCKET_ERROR)
				{
					std::cout << "HOST: FAILED TO RECIEVE2" << std::endl;
					break;
				}
				//Extract msg
				std::string msg = msgbuffer;
				msg = msg.substr(0, msgLen);
				std::cout << "RECIEVED: " << msg << std::endl;
			}
		}

	}
}

