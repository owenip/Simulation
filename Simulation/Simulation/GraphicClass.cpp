#include "pch.h"
#include "GraphicClass.h"



GraphicClass::GraphicClass():
	mConfig(nullptr)
{

}


GraphicClass::~GraphicClass()
{
}

bool GraphicClass::Initialize(const HWND hwnd, const ConfigClass * mConfig)
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

	//Mouse & keyboard initialisation
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(hwnd);
	tracker.Update(m_keyboard->GetState());

	return true;
}

void GraphicClass::Shutdown()
{

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
}

void GraphicClass::OnResume()
{
	tracker.Reset();
	return;
}

bool GraphicClass::Update()
{
	bool result;

	CheckInput();

	result = Render();
	if (!result)
	{
		return false;
	}
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
	TwAddVarRW(mATBar, "No. of balls owned by this peer", TW_TYPE_INT32,&mNumberOfBalls,"");
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

	return true;
}

bool GraphicClass::Render()
{
	// Clear the buffers to begin the scene.
	mDirect3D->BeginScene(DirectX::SimpleMath::Vector4(Colors::Gray));

	//Draw AntTweakBar
	TwDraw();

	mDirect3D->EndScene();
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
	else if (tracker.pressed.W)
	{
		//Camera move Up
	}
	else if (tracker.pressed.S)
	{
		//Camera move down
	}
	else if (tracker.pressed.A)
	{
		//Camera move left
	}
	else if (tracker.pressed.D)
	{
		//Camera move right
	}
	else if (tracker.pressed.Up)
	{
		//Camera Zoom in
	}
	else if (tracker.pressed.Down)
	{
		//Camera zoom out
	}
		
	auto mouse = m_mouse->GetState();
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
	
}
