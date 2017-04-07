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
	m_mouse->SetMode(Mouse::MODE_RELATIVE);
	
	
	//Camera Initialisation
	mCamera = new CameraClass;
	DX::ThrowIfFailed(mCamera->Initialize(SimpleMath::Vector3::Zero));

	//Effect Initialisation
	m_states = std::make_unique<DirectX::CommonStates>(mDirect3D->GetDevice());
	m_effect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	m_effect->SetTextureEnabled(false);
	m_effect->SetAmbientLightColor(SimpleMath::Vector3(.5f, .5f, .5f));
	m_effect->SetAlpha(.5f);

	m_fxFactory = std::make_unique<EffectFactory>(mDirect3D->GetDevice());

	//Texture
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\earth.bmp", nullptr,
			m_texture.ReleaseAndGetAddressOf()));

	//mSurface->CreateInputLayout(m_effect.get(),
	//	m_inputLayout.ReleaseAndGetAddressOf());

	//
	//Models Initialisation
	//Surface
	mSurface = GeometricPrimitive::CreateCylinder(mDirect3D->GetDeviceContext(), 0.05f, 50.f);
	
	//Wall
	mWall = GeometricPrimitive::CreateCylinder(mDirect3D->GetDeviceContext(), 25.f, 50.f);
	mWall->CreateInputLayout(m_effect.get(), mWallInputLayout.ReleaseAndGetAddressOf());

	m_model = Model::CreateFromCMO(mDirect3D->GetDevice(), L".\\Resources\\wall.cmo", *m_fxFactory);
	
	//GravityWell
	mGravityWellPos = SimpleMath::Vector3::Zero;
	mGWMovementGain = 0.001f;
	mGravityWell = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(),1.f);
	mGravityWell->CreateInputLayout(m_effect.get(), mGwInputLayout.ReleaseAndGetAddressOf());

	//m_effect->SetTexture(m_texture.Get());

	return true;
}

void GraphicClass::Shutdown()
{
	m_fxFactory.reset();
	m_model.reset();

	m_texture.Reset();
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();

	mGravityWell.reset();

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
	
	mDirect3D->GetWorld(m_world);
	mCamera->GetView(m_view);
	mDirect3D->GetProj(m_proj);
		
	
	m_effect->SetView(m_view);
	m_effect->SetProjection(m_proj);
	m_effect->SetWorld(m_world);

	//Draw Model
	//Surface
	m_world.Translation(SimpleMath::Vector3(0.f,-5.f,0.f));
	mSurface->Draw(m_world, m_view, m_proj, Colors::Silver);

	//GravityWell
	mDirect3D->GetWorld(m_world);
	m_world.Translation(mGravityWellPos);
	m_effect->SetWorld(m_world);
	m_effect->SetColorAndAlpha(SimpleMath::Color(0.f, 0.f, 0.f, 0.3));
	mGravityWell->Draw(m_effect.get(),mGwInputLayout.Get(), true, false, [=]
	{
		mDirect3D->GetDeviceContext()->RSSetState(m_states->CullNone());
	});
	//Wall
	
	mDirect3D->GetWorld(m_world);
	m_effect->SetWorld(m_world);
	m_effect->SetColorAndAlpha(SimpleMath::Color(1.f, 1.f, 1.f, 0.5));


	//mWall->Draw(m_effect.get(), mWallInputLayout.Get(), true, false, [=]
	//{
	//	mDirect3D->GetDeviceContext()->RSSetState(m_states->CullNone());
	//});
	
	
	m_model->UpdateEffects([&](IEffect* effect)
	{
		auto TranEffect = dynamic_cast<BasicEffect*>(effect);
		if (TranEffect)
		{
			m_effect->SetColorAndAlpha(SimpleMath::Color(0.f, 0.f, 0.f, 0.3));
		}
	});

	m_world.CreateScale(2.f);

	m_model->Draw(mDirect3D->GetDeviceContext(), *m_states, m_world, m_view, m_proj,false, [=]
	{
		mDirect3D->GetDeviceContext()->RSSetState(m_states->CullNone());
	});

	

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
	else if (kbState.M)
	{
		//Increase the height of the gravity well above the surface
		this->GwMoveUp();
	}
	else if (kbState.N)
	{
		//Decrease the height of the gravity well above the surface
		this->GwMoveDown();
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
		float mouseX = static_cast<float>(mouse.x);
		float mouseY = static_cast<float>(mouse.y);
		this->GwMoveByMouse(mouseX, mouseY);
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
	//m_mouse->SetMode(mouse.middleButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
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

void GraphicClass::GwMoveByMouse(float & mouseX, float & mouseY)
{
	SimpleMath::Vector3 move = SimpleMath::Vector3(mouseX, 0.f, mouseY);
	this->GwMove(move);
	return;
}

void GraphicClass::GwMoveUp()
{
	this->GwMove(SimpleMath::Vector3::Up);
}

void GraphicClass::GwMoveDown()
{
	this->GwMove(SimpleMath::Vector3::Down);
}
