#include "pch.h"
#include "Network.h"




Network::Network() :
	mIsEscaped(false),
	mConnReady(false),
	mNextServer(false),
	mLastPause(false),
	mNumOfClient(0)
{
	mClientIndex.reserve(5);
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
	mUDPPort = mConfig->GetUDPPort();
	
	mNetTimer.SetFixedTimeStep(true);
	mNetTimer.SetTargetElapsedSeconds(1 / (240.f));
}

void Network::Connect()
{
	mConnReady = false;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) /* Load Winsock 2.2 DLL */
		std::cout << "WSAStartup() failed with error: " << std::endl;

	HostExist = InitUPDListener();

	if(!HostExist || mNextServer == true)
	{
		std::cout << "Hosting init " << std::endl;
		InitServer();
		std::thread thread_IPBoardcast(&Network::InitUDPBoardcaster, this);
		std::thread thread_sListen(&Network::ServerListen, this);
		//std::thread thread_sSend(&Network::ServerSend, this);
		thread_IPBoardcast.join();
		thread_sListen.join();
		mConnReady = false;
		//thread_sSend.join();
	}
	else
	{
		std::cout << "hosting exist " << std::endl;
		std::cout << "HOSTIP:" << HostIP << std::endl;

		if (!InitClient())
		{
			InitServer();
			std::thread thread_IPBoardcast(&Network::InitUDPBoardcaster, this);
			std::thread thread_sListen(&Network::ServerListen, this);
			
			thread_IPBoardcast.join();
			thread_sListen.join();
			mConnReady = false;
		}
		else
		{
			std::thread thread_cListen(&Network::ClientListen, this);				
			thread_cListen.join();		
			mConnReady = false;
		}
	}
		
	//Clean up Winsock
	WSACleanup();
	mConnReady = false;
}

void Network::Tick()
{
	std::thread thread_connect(&Network::Connect, this);
	while (true)
	{		
		if (mConfig->GetIsEscaped())
		{
			break;
		}
		mNetTimer.Tick([&]
		{
			if (!mConnReady)
			{
							
			}
			else
			{
				SendData();
			}
		});
	}
	thread_connect.join();
}

void Network::SendData()
{
	if(mLastPause != mConfig->GetIsPaused())
		SendIsPause();
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






bool Network::InitUPDListener()
{
	SOCKET hRecvSock;
	SOCKADDR_IN addr;

	int strLen;
	char buf[BUFSIZE];

	hRecvSock = socket(PF_INET, SOCK_DGRAM, 0);
	if (hRecvSock == INVALID_SOCKET)
		std::cout << "socket() failed with error: " << WSAGetLastError() << std::endl;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(mUDPPort);

	if (::bind(hRecvSock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		std::cout << "bind() error: " << WSAGetLastError() << std::endl;

	struct timeval tv;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(hRecvSock, &readfds);

	tv.tv_sec = 2;
	tv.tv_usec = 500000;


	while (true) //Waiting incoming IP Host address for set seconds	
	{
		printf("Listening to Boardcast Host IP\n");
		strLen = select(1, &readfds, NULL, NULL, &tv);
		//strLen = recvfrom(hRecvSock, buf, BUFSIZE - 1, 0, NULL, 0);

		if (strLen < 0)
		{
			continue;
		}
		else if (strLen == 0)
		{
			printf("Timeout occurred!  No IP received after 2 seconds.\n");
			break;
		}
		else {

			strLen = recvfrom(hRecvSock, buf, BUFSIZE - 1, 0, NULL, 0);
			if (buf[0] == 'O' && buf[1] == 'I') //recieve broadcast message
			{
				std::string msg = std::string(buf);
				std::stringstream ss;
				ss.str(msg.substr(2));
				int size;
				ss >> size >> HostIP;
				//HostIP = ss.str();
				if (HostIP.size() == size)
				{
					//Closing 
					closesocket(hRecvSock);
					//WSACleanup();
					return true;
				}
			}
		}
	}
	//Closing 
	closesocket(hRecvSock);
	//WSACleanup();
	return false;
}

void Network::InitUDPBoardcaster()
{
	if ((UDPSendSock = socket(PF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		std::cout << "socket() failed with error: " << WSAGetLastError() << std::endl;

	struct in_addr in;              // IN_ADDR in;
	struct sockaddr_in broadAddr;   // SOCKADDR_IN broadAddr;
	DWORD nMyIP, nMySubMask, nTemp1, nTemp2;
	char name[256] = "localhost";

	gethostname(name, 256);
	printf("Host Name : %s\n", name);
	char* address;

	struct hostent * pHostEnt = gethostbyname(name);
	in.s_addr = ((struct in_addr*)pHostEnt->h_addr)->s_addr;
	address = inet_ntoa(in);
	printf("IP Addr : %s\n", address);

	std::string IPmsg = address;

	nMyIP = inet_addr(address);
	nMySubMask = inet_addr("255.255.255.0");
	nTemp1 = nMyIP & nMySubMask;
	nTemp2 = nMySubMask ^ 0xFFFFFFFF;

	memset(&broadAddr, 0, sizeof(broadAddr));
	broadAddr.sin_family = AF_INET;
	broadAddr.sin_port = htons(mUDPPort);
	broadAddr.sin_addr.s_addr = nTemp1 + nTemp2;

	int so_broadcast = TRUE;
	int state = setsockopt(UDPSendSock, SOL_SOCKET, SO_BROADCAST, (char*)&so_broadcast, sizeof(so_broadcast));
	if (state == SOCKET_ERROR)
		std::cout << "setsockopt() error" << std::endl;

	IPmsg = "OI" + std::to_string(IPmsg.size()) + " " + IPmsg;

	while (!mConfig->GetIsEscaped())
	{
		sendto(UDPSendSock, IPmsg.c_str(), sizeof(IPmsg), 0, (SOCKADDR*)&broadAddr, sizeof(broadAddr));
		Sleep(500);
	}	
	closesocket(UDPSendSock);
}

void Network::InitServer()
{	
	hostSock = socket(AF_INET, SOCK_STREAM, 0);
	peerSock = socket(AF_INET, SOCK_STREAM, 0);

	//Setup socket options
	setsockopt(hostSock, SOL_SOCKET, SO_REUSEADDR, &OPTION_VALUE, sizeof(int)); //Make it possible to re-bind to a port that was used within the last 2 minutes
	setsockopt(hostSock, IPPROTO_TCP, TCP_NODELAY, &OPTION_VALUE, sizeof(int)); //Used for interactive programs

	hostAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(mTCPPort);

	::bind(hostSock, (SOCKADDR *)&hostAddr, sizeof(hostAddr));

	listen(hostSock, 5);

	std::cout << "HOST: LISTENING..." << std::endl;
}

void Network::ServerListen()
{
	//incoming = INVALID_SOCKET;
	hostSock = accept(hostSock, NULL, NULL);
	std::cout << "Attemp to Accept incoming socket" << std::endl;
	
	while (!mConfig->GetIsEscaped())
	{
		//Checking socket
		if (hostSock == INVALID_SOCKET)
		{
			std::cout << "Accept failed with " << WSAGetLastError() << std::endl;
			break;
		}
		if (hostSock == 0 || WSAGetLastError() == WSAECONNRESET)
		{
			std::cout << "Client disconnected" << std::endl;
			mConnReady = false;
			break;
		}

		//Recv msg		
		char headerbuf[2];

		int iResult = recv(hostSock, headerbuf, 2, 0);
		if (iResult != SOCKET_ERROR)
		{
			//Check Header
			if (headerbuf[0] == 'O' && headerbuf[1] == 'I')
			{
				//Get msg length
				std::string str = "";
				char msglengthbuf;
				while (true)
				{
					msglengthbuf = NULL;
					iResult = recv(hostSock, &msglengthbuf, 1, 0);
					if (iResult == SOCKET_ERROR)
					{
						break;
					}
					if (msglengthbuf == '|')
						break;
					str += msglengthbuf;
				}
				if (iResult == SOCKET_ERROR)
				{
					break;
				}
				//Convert str to int to obtain msg length
				int msgLen = std::stoi(str);
				if (msgLen <= 0)
					break;

				//ReadIn Actual msg
				char msgbuffer[BUFSIZE];
				iResult = recv(hostSock, msgbuffer, msgLen, 0);
				if (iResult == SOCKET_ERROR)
				{
					break;
				}
				//Extact msg
				if (msgbuffer[0] == 'P' && msgbuffer[1] == 'S') //Pause Command
				{
					std::cout << "Pause Command Received" << std::endl;
					recvPause(string(msgbuffer));
				}
				else if (msgbuffer[0] == 'T' && msgbuffer[1] == 'S') //TimeScale
				{
					std::cout << "TimeScale" << std::endl;
				}
				else if (msgbuffer[0] == 'G' && msgbuffer[1] == 'P') //Gw Position
				{
					//std::cout << "Gw Position." << std::endl;
					recvGwPos(string(msgbuffer));
				}
				else if (msgbuffer[0] == 'G' && msgbuffer[1] == 'F') //Gw Force
				{
					std::cout << "Gw Force" << std::endl;
				}
				else if (msgbuffer[0] == 'B' && msgbuffer[1] == 'P') //Ball Position
				{
					std::cout << "Ball Position" << std::endl;
				}
				else if (msgbuffer[0] == 'B' && msgbuffer[1] == 'R') //Ball Rotation
				{
					std::cout << "Ball Rotation" << std::endl;
				}
				else if (msgbuffer[0] == 'B' && msgbuffer[1] == 'O') //Ball Ownership Receive
				{
					std::cout << "Ball Ownership Receive" << std::endl;
				}
				else if (msgbuffer[0] == 'N' && msgbuffer[1] == 'C')
				{
					std::cout << "New Client Accepted" << std::endl; //New Client handling
					mNumOfClient++;
					std::string sent_message = "CI " + std::to_string(mNumOfClient);
					sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;
					if (send(hostSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
					{
						std::cout << "Fail to send ID back" << std::endl;
						mConnReady = false;
						break;
					}
					mConnReady = true;
				}
			}
		}
	}
	//Client DC Connection finish
	closesocket(hostSock);
}

bool Network::InitClient()
{
	hostSock = socket(AF_INET, SOCK_STREAM, 0);
	peerSock = socket(AF_INET, SOCK_STREAM, 0);

	peerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, HostIP.c_str(), &peerAddr.sin_addr);
	peerAddr.sin_port = htons(mTCPPort);

	int iResult = connect(peerSock, (SOCKADDR *)&peerAddr, sizeof(peerAddr));
	if (iResult == SOCKET_ERROR)
	{
		closesocket(peerSock);
		peerSock = INVALID_SOCKET;
		std::cout << "NO CONNECTION TO SERVER" << WSAGetLastError() << std::endl;
		std::cout << "Start Initialise as SERVER now" << WSAGetLastError() << std::endl;
		return false;
	}
	std::string sent_message = "NC";
	sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;
	if (send(peerSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
	{
		std::cout << "PEER: FAILED TO INFORM HOST AS A NC" << std::endl;
		return false;
	}
	mConnReady = true;
	return true;
}

void Network::ClientListen()
{
	//SOCKET incoming = INVALID_SOCKET;
	//incoming = peerSock;
	while (true)
	{
		//Checking socket
		if (peerSock == INVALID_SOCKET)
		{
			std::cout << "Accept failed with " << WSAGetLastError() << std::endl;
			break;
		}
		if (peerSock == 0 || WSAGetLastError() == WSAECONNRESET)
		{
			std::cout << "HOST disconnected" << std::endl;
			mConnReady = false;
			mNextServer = true;
			closesocket(peerSock);
			break;
		}

		//Recv msg		
		char headerbuf[2];

		int iResult = recv(peerSock, headerbuf, 2, 0);
		if (iResult != SOCKET_ERROR)
		{
			//Check Header
			if (headerbuf[0] == 'O' && headerbuf[1] == 'I')
			{
				//Get msg length
				std::string str = "";
				char msglengthbuf;
				while (true)
				{
					msglengthbuf = NULL;
					iResult = recv(peerSock, &msglengthbuf, 1, 0);
					if (iResult == SOCKET_ERROR)
					{
						break;
					}
					if (msglengthbuf == '|')
						break;
					str += msglengthbuf;
				}
				if (iResult == SOCKET_ERROR)
				{
					break;
				}
				//Convert str to int to obtain msg length
				int msgLen = std::stoi(str);
				if (msgLen <= 0)
					break;

				//ReadIn Actual msg
				char msgbuffer[BUFSIZE];
				iResult = recv(peerSock, msgbuffer, msgLen, 0);
				if (iResult == SOCKET_ERROR)
				{
					break;
				}
				//Extact msg
				if (msgbuffer[0] == 'P' && msgbuffer[1] == 'S') //Pause Command
				{
					std::cout << "Pause Command Received" << std::endl;					
					recvPause(string(msgbuffer));
				}
				else if (msgbuffer[0] == 'T' && msgbuffer[1] == 'S') //TimeScale
				{
					std::cout << "TimeScale" << std::endl;
				}
				else if (msgbuffer[0] == 'G' && msgbuffer[1] == 'P') //Gw Position
				{
					//std::cout << "Gw Position" << std::endl;
					recvGwPos(string(msgbuffer));
				}
				else if (msgbuffer[0] == 'G' && msgbuffer[1] == 'F') //Gw Force
				{
					std::cout << "Gw Force" << std::endl;
				}
				else if (msgbuffer[0] == 'B' && msgbuffer[1] == 'P') //Ball Position
				{
					std::cout << "Ball Position" << std::endl;
				}
				else if (msgbuffer[0] == 'B' && msgbuffer[1] == 'R') //Ball Rotation
				{
					std::cout << "Ball Rotation" << std::endl;
				}
				else if (msgbuffer[0] == 'B' && msgbuffer[1] == 'O') //Ball Ownership Receive
				{
					std::cout << "Ball Ownership Receive" << std::endl;
				}
				else if (msgbuffer[0] == 'C' && msgbuffer[1] == 'I') //New Client ID
				{
					std::string input = std::string(msgbuffer);
					std::stringstream ss;
					ss.str(input.substr(2));
					int peerID;
					ss >> peerID;
					std::cout << "Assigned Client ID received: " << peerID << std::endl;
					mLocalPeerID = peerID;
					mConfig->SetPeerID(mLocalPeerID);
					//mConfig->SetIsPaused(true);
					//mGwManager()
				}
			}
		}

	}
	//Client DC Connection finish
	//closesocket(incoming);
	closesocket(peerSock);
	//Regain all the ball and remove visual Gw
}

void Network::ServerSend()
{
	while (!mConfig->GetIsEscaped())
	{
		std::string sent_message;
		std::cout << ">";
		std::getline(std::cin, sent_message);

		sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;

		if (send(peerSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "HOST: FAILED TO SEND" << std::endl;
		}
	}
}

void Network::ClientSend()
{
	while (!mConfig->GetIsEscaped())
	{
		std::string sent_message;
		//std::cout << ">";
		//std::getline(std::cin, sent_message);
		sent_message = "PA";
		sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;

		if (send(hostSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "PEER: FAILED TO SEND" << std::endl;
		}
	}
}

void Network::SendIsPause()
{
	mLastPause ? mLastPause = false : mLastPause = true;
	ostringstream convert;
	convert << "PS"
		<< mLastPause;
	std::string sent_message = convert.str();
	sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;

	std::cout << "SENDING PS: " << std::to_string(mLastPause) << std::endl;

	if (!HostExist)
	{
		if (send(hostSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "HOST: FAILED TO SEND Pause" << std::endl;
		}
	}
	else
	{
		if (send(peerSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "PEER: FAILED TO SEND Pause" << std::endl;
		}
	}
}


void Network::SendGwPos()
{
	ostringstream convert;
	convert << "GP";
	SimpleMath::Vector3 GwPos = mGwManager->GwGetPos(mLocalPeerID);
	convert << mLocalPeerID << " "
		<< GwPos.x << " "
		<< GwPos.y << " "
		<< GwPos.z << " ";

	std::string sent_message = convert.str();
	sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;
	cout << "Sending: " << sent_message << endl;
	if (!HostExist)
	{
		if (send(hostSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "HOST: FAILED TO SENDGW POS" << std::endl;
		}
	}
	else
	{
		if (send(peerSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "PEER: FAILED TO SEND GW POS" << std::endl;
		}
	}
}

void Network::SendGwForce()
{
	ostringstream convert;
	convert << "GF";
	float GwForce = mGwManager->GwGetForce(mLocalPeerID);
	convert << mLocalPeerID << " "
			<< GwForce << endl;

	std::string sent_message = convert.str();
	sent_message = "OI" + std::to_string(sent_message.size()) + '|' + sent_message;
	cout << "Sending: " << sent_message << endl;
	if (!HostExist)
	{
		if (send(hostSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "HOST: FAILED TO SENDGW POS" << std::endl;
		}
	}
	else
	{
		if (send(peerSock, sent_message.c_str(), sent_message.size(), 0) == SOCKET_ERROR)
		{
			std::cout << "PEER: FAILED TO SEND GW POS" << std::endl;
		}
	}
}

void Network::recvPause(string input)
{
	stringstream ss;
	ss.str(input.substr(2));	
	bool InVal;
	ss >> InVal;
	mLastPause = InVal;
	mConfig->SetIsPaused(InVal);
}

void Network::recvTimeScale(string input)
{
	stringstream ss;
	ss.str(input.substr(2));
	float InTimeScale;
	ss >> InTimeScale;
	mConfig->SetTimeScale(InTimeScale);
}

void Network::recvGwPos(string input)
{
	
	stringstream ss;
	ss.str(input.substr(2));
	int InGwID;
	float x, y, z;
	ss >>InGwID >> x >> y >> z;
	SimpleMath::Vector3 InPos(x, y, z);
	std::cout << "Re:Gw Position " << InGwID << "|"
				<< x << "|"
				<< y << "|" 
				<< z <<	std::endl;
	mGwManager->GwSetPos(InGwID, InPos);
}

void Network::recvGwForce(string input)
{
	stringstream ss;
	ss.str(input.substr(2));
	int InGwID;
	float InGwForce;
	ss >> InGwID >> InGwForce;
	std::cout << "Re:Gw Force " << InGwID << "|"		
		<< InGwForce << std::endl;
	mGwManager->GwSetForce(InGwID, InGwForce);
}

void Network::recvBallPos(string input)
{
	stringstream ss;
	ss.str(input.substr(2));
	int InBallID;
	
}








