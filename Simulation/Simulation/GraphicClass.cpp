#include "pch.h"
#include "GraphicClass.h"



GraphicClass::GraphicClass():
	mConfig(nullptr),
	mDirect3D(nullptr),
	mATBar(nullptr),
	mTimer(nullptr),
	mouseX(0),
	mouseY(0)
{

}


GraphicClass::~GraphicClass()
{
}

bool GraphicClass::Initialize(const HWND hwnd, const ConfigClass * mConfig, TimerClass *SysTimer)
{
	bool result = false;
	DX::ThrowIfFailed(mScreenWidth = mConfig->GetScreenWidth());
	DX::ThrowIfFailed(mScreenHeight = mConfig->GetScreenHeight());

	//DirectX Class initialisation
	mDirect3D = new D3DClass;
	if (!mDirect3D)
	{
		return false;
	}
	result = mDirect3D->Initialize(hwnd, mConfig);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	//AntTweakBar initialse
	InitAntTweak(hwnd);

	//Timer link
	mTimer = SysTimer;

	//Mouse & keyboard initialisation
	m_keyboard = std::make_unique<Keyboard>();
	tracker.Update(m_keyboard->GetState());

	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(hwnd);
	//m_mouse->SetMode(Mouse::MODE_RELATIVE);
	
	
	//Camera Initialisation
	mCamera = new CameraClass;
	DX::ThrowIfFailed(mCamera->Initialize(SimpleMath::Vector3::Zero));

	//mCamera->SetPosition(SimpleMath::Vector3(0.f, 2.f, 5.f));

	//Models Initialisation
	mGravityWellPos = SimpleMath::Vector3::Zero;
	mGWMovementGain = 0.001f;

	mSphere = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext());
	return true;
}

void GraphicClass::Shutdown()
{
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();

	mSphere.reset();

	if (mCamera)
	{
		delete mCamera;
		mCamera = nullptr;
	}

	TwTerminate();

	if (mDirect3D)
	{
		mDirect3D->Shutdown();
		delete mDirect3D;
		mDirect3D = nullptr;
	}
	if (mConfig)
	{
		delete mConfig;
		mConfig = nullptr;
	}	
}

void GraphicClass::OnPause()
{
	tracker.Reset();
	return;
}

void GraphicClass::OnResume()
{
	
}

bool GraphicClass::Update()
{
	bool result;

	this->CheckInput();

	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}

bool GraphicClass::Render()
{
	// Clear the buffers to begin the scene.
	mDirect3D->BeginScene(SimpleMath::Vector4(Colors::CornflowerBlue));

	
	SimpleMath::Matrix mWorld;
	SimpleMath::Matrix mProj = SimpleMath::Matrix::Identity;

	mDirect3D->GetWorld(mWorld);
		
	mCamera->GetView(m_view);
	mDirect3D->GetProj(mProj);

	mWorld.Translation(mGravityWellPos);
	mSphere->Draw(mWorld, m_view, mProj, Colors::Black);

	//Second sphere
	mDirect3D->GetWorld(mWorld);
	mWorld.Translation(SimpleMath::Vector3(0.f, 0.f, 2.f));
	mSphere->Draw(mWorld, m_view, mProj, Colors::Wheat);

	//Draw AntTweakBar
	TwDraw();
	//Present Scene
	mDirect3D->EndScene();
	return true;
}

bool GraphicClass::InitAntTweak(const HWND hwnd)
{
	//Initialize the AntTweakBar
	if (!TwInit(TW_DIRECT3D11, mDirect3D->GetDevice()))
	{
		MessageBox(hwnd, L"Could not initialize AntTweakBar.", L"Error", MB_OK);
		return false;
	};
	TwWindowSize(mScreenWidth, mScreenHeight);

	mATBar = TwNewBar("AntTweakBar");
	int barSize[2] = { mScreenWidth / 4, mScreenHeight /4};
	TwSetParam(mATBar, nullptr, "size", TW_PARAM_INT32, 2, barSize);
	//Non-Changable variables
	TwAddVarRW(mATBar, "No. of balls owned by this peer", TW_TYPE_INT32,&mNumberOfBalls,"min=0 max=64000 keyincr=+ keydecr=-");
	TwAddVarRW(mATBar, "No. of balls currently contended", TW_TYPE_INT32, nullptr, "");
	TwAddVarRW(mATBar, "Total number of balls", TW_TYPE_INT32, nullptr, "");
	TwAddVarRW(mATBar, "Magnitude of the applying force ", TW_TYPE_FLOAT, nullptr, "");
	TwAddVarRW(mATBar, "Time step of each simulation loop per second", TW_TYPE_INT32, nullptr, "");
	//Changable variables
	TwAddVarRW(mATBar, "The time scale", TW_TYPE_FLOAT, nullptr, "");
	TwAddVarRW(mATBar, "Magnitude of elasticity", TW_TYPE_FLOAT, nullptr, "");
	TwAddVarRW(mATBar, "Magnitude of frictional forces", TW_TYPE_FLOAT, nullptr, "");
	TwAddVarRW(mATBar, "Target frequency of the physics (in Hz)", TW_TYPE_FLOAT, nullptr, "");
	TwAddVarRW(mATBar, "Target frequency of the graphics (in Hz),", TW_TYPE_FLOAT, nullptr, "");
	TwAddVarRW(mATBar, "Target frequency of the networking (in Hz),", TW_TYPE_FLOAT, nullptr, "");
	TwAddVarRW(mATBar, "Mouse X", TW_TYPE_INT32, &mouseX, "");
	TwAddVarRW(mATBar, "Mouse Y", TW_TYPE_INT32, &mouseY, "");
	return true;
}



void GraphicClass::CheckInput()
{
	//source for trigger for holding buttons issue
	//https://github.com/Microsoft/DirectXTK/wiki/Keyboard#keyboard-state-tracker

	auto kbState = m_keyboard->GetState();
	tracker.Update(kbState);

	if (tracker.pressed.Escape)
		PostQuitMessage(0);
	else if (tracker.pressed.R)
	{		
		//Reset		
	}
	else if (tracker.pressed.P)
	{
		//Pause the simulation loop globally (globally)
	}
	else if (tracker.pressed.O)
	{
		//Increase frequency of physics
	}
	else if (tracker.pressed.L)
	{
		//Decrease frequency of physics
	}
	else if (tracker.pressed.I)
	{
		//Increase frequency of graphics
	}
	else if (tracker.pressed.K)
	{
		//Decrease frequency of graphics
	}
	else if (tracker.pressed.U)
	{
		//Increase frequency of networking
	}
	else if (tracker.pressed.J)
	{
		//Decrease frequency of networking
	}
	else if (tracker.pressed.Y)
	{
		//Increase frequency of Time scale (globally)
		//Maximum 1.0
	}
	else if (tracker.pressed.H)
	{
		//Decrease frequency of Time scale (globally)
		//Minimum 0.001
	}
	else if (tracker.pressed.M)
	{
		//Increase the height of the gravity well above the surface
	}
	else if (tracker.pressed.M)
	{
		//Decrease the height of the gravity well above the surface
	}

	//Camera Control
	if (kbState.W)
	{
		//Camera move forward
		this->GwMoveForward();
	}
	else if (kbState.S)
	{
		//Camera move Backward
		this->GwMoveBackward();
	}
	else if (kbState.A)
	{
		//Camera move left
		this->GwMoveLeft();
	}
	else if (kbState.D)
	{
		//Camera move right
		this->GwMoveRight();
	}
	else if (kbState.Up)
{	
		//Camera Zoom in
		mCamera->ZoomIn();
	}
	else if (kbState.Down)
	{
		//Camera zoom out
		mCamera->ZoomOut();
	}
	
		
	auto mouse = m_mouse->GetState();
	if (mouse.positionMode == Mouse::MODE_RELATIVE)
	{
		mouseX = mouse.x;
		mouseY = mouse.y;

		mGravityWellPos += SimpleMath::Vector3(float(mouse.x),0.f, float(mouse.y)) * mGWMovementGain;
		mCamera->SetLookAt(mGravityWellPos);
	}
	if (mouse.leftButton && mouse.rightButton)
	{
		//Cancel force
		//Force = 0
	}
	else if (mouse.leftButton)
	{
		//1.Clear repelling force
		//2.Apply attractor force
	}
	else if (mouse.rightButton)
	{
		//Clear Attrating force
		//Apply repellor force
	}
	
	{
		//Handle move position to move the position of peer paralled to the surface
	}
	m_mouse->SetMode(mouse.middleButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
}

void GraphicClass::GwMove(SimpleMath::Vector3 direction)
{
	SimpleMath::Vector3 move = direction*mGWMovementGain;
	mGravityWellPos += move;
	mCamera->SetLookAt(mGravityWellPos);
	return;
}

void GraphicClass::GwMoveForward()
{
	this->GwMove(SimpleMath::Vector3::Forward);
	return;
}

void GraphicClass::GwMoveBackward()
{
	this->GwMove(SimpleMath::Vector3::Backward);
	return;
}

void GraphicClass::GwMoveLeft()
{
	this->GwMove(SimpleMath::Vector3::Left);
	return;
}

void GraphicClass::GwMoveRight()
{
	this->GwMove(SimpleMath::Vector3::Right);
	return;
}
