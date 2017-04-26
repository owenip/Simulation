#include "GraphicClass.h"



GraphicClass::GraphicClass():
	mDirect3D(std::make_shared<D3DClass>()),
	mConfig(nullptr),
	mATBar(nullptr),
	mTimer(nullptr),
	mScreenWidth(0),
	mScreenHeight(0),
	mLastKeyPressed(0),
	mNumberOfBalls(0),
	mOwnerID(0),
	mGWMovementGain(0), mGwForce(0),
	mGravityWellPos(SimpleMath::Vector3::Zero)
{
}


GraphicClass::~GraphicClass()
{
	this->Shutdown();
}

bool GraphicClass::Initialize(const HWND hwnd, shared_ptr<ConfigClass> Config, shared_ptr<TimerClass> SysTimer)
{
	auto result = false;

	mConfig = Config;
	DX::ThrowIfFailed(mScreenWidth = mConfig->GetScreenWidth());
	DX::ThrowIfFailed(mScreenHeight = mConfig->GetScreenHeight());
	DX::ThrowIfFailed(mNumberOfBalls = mConfig->GetNumberOfBalls());
	DX::ThrowIfFailed(mOwnerID = mConfig->GetOwnerID());

	//DirectX Class initialisation
	//mDirect3D = std::make_shared<D3DClass>();
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
	mCamera = std::make_unique<CameraClass>();
	DX::ThrowIfFailed(mCamera->Initialize(SimpleMath::Vector3::Zero));

	//Effect Initialisation
	m_states = std::make_unique<DirectX::CommonStates>(mDirect3D->GetDevice());
	m_effect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	m_effect->SetTextureEnabled(true);
	m_effect->SetAmbientLightColor(SimpleMath::Vector3(.5f, .5f, .5f));
	m_effect->SetAlpha(.3f);

	m_fxFactory = std::make_unique<EffectFactory>(mDirect3D->GetDevice());

	//Testing Cup
	mCupfxFactory = std::make_unique<EffectFactory>(mDirect3D->GetDevice());
	mCupfxFactory->SetDirectory(L".\\Resources\\");
	mCup = Model::CreateFromCMO(mDirect3D->GetDevice(), L".\\Resources\\cup.cmo", *mCupfxFactory);
	mCup->UpdateEffects([](IEffect* effect)
	{
		auto alpha = dynamic_cast<BasicEffect*>(effect);
		if(alpha)
		{
			alpha->SetAlpha(.3f);
		}
	});

	//Texture
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\earth.bmp", nullptr,
			m_texture.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\roomtexture.dds",
			nullptr, mSurfaceTex.ReleaseAndGetAddressOf()));

	m_effect->SetTexture(m_texture.Get());

	
	//
	//Models Initialisation
	//

	//Surface
	mSurface = GeometricPrimitive::CreateCylinder(mDirect3D->GetDeviceContext(), 0.05f, mConfig->GetSurfaceRadius()* 2);
	//mSurface = Model::CreateFromCMO(mDirect3D->GetDevice(), L".\\Resources\\surface.cmo", *m_fxFactory, true, true);

	//Wall
	/*mWall = GeometricPrimitive::CreateCylinder(mDirect3D->GetDeviceContext(), 25.f, 50.f);
	mWall->CreateInputLayout(m_effect.get(), mWallInputLayout.ReleaseAndGetAddressOf());*/
	mWallfxFactory = std::make_unique<EffectFactory>(mDirect3D->GetDevice());
	mWallfxFactory->SetDirectory(L".\\Resources\\");
	mWall = Model::CreateFromCMO(mDirect3D->GetDevice(), L".\\Resources\\wall.cmo", *m_fxFactory, true,true);
	mWall->UpdateEffects([](IEffect* effect)
	{
		auto alpha = dynamic_cast<BasicEffect*>(effect);
		if (alpha)
		{
			alpha->SetAlpha(.5f);
		}
	});

	//GravityWell
	//mGravityWellPos = SimpleMath::Vector3::Zero;
	mGWMovementGain = 0.01f;
	//mGravityWell = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(),1.f);
	

	return true;
}

void GraphicClass::Shutdown()
{
	mGwManager.reset();
	mBallManager.reset();
	mSimulation.reset();


	mCup.reset();
	mCupfxFactory.reset();

	m_fxFactory.reset();
	mWall.reset();

	m_texture.Reset();
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();

	mGravityWell.reset();

	mBallManager.reset();	

	mCamera.reset();	

	TwTerminate();

	mDirect3D.reset();
	mConfig.reset();
	mTimer.reset();
}

void GraphicClass::OnPause()
{
	tracker.Reset();
	return;
}

void GraphicClass::OnResume()
{
	
}

bool GraphicClass::Update(float dt)
{
	bool result;

	this->CheckInput();
	
	mGravityWellPos = mGwManager->GwGetPos(mOwnerID);
	mGwForce = mGwManager->GwGetForce(mOwnerID);

	result = Render();
	if (!result)
	{
		return false;
	}
	return true;
}

void GraphicClass::SetSimulationPtr(shared_ptr<Simulation> InSimulation)
{
	mSimulation = InSimulation;
}

void GraphicClass::SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager)
{
	mBallManager = InBallManager;
	//Balls Manager Visual Initialisation	
	mBallManager->Initialise(mDirect3D);
}

void GraphicClass::SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager)
{
	mGwManager = InGwManager;
	mGwManager->InitialiseGraphic(mDirect3D);
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
	mSurface->Draw(m_world, m_view, m_proj, Colors::Silver);
	/*mSurface->Draw(mDirect3D->GetDeviceContext(), *m_states, m_world, m_view, m_proj, true, [=]
	{
		mDirect3D->GetDeviceContext()->RSSetState(m_states->CullClockwise());		
	});*/

	//Balls
	mBallManager->Render(m_view);

	//GravityWell
	/*mDirect3D->GetWorld(m_world);
	m_world.Translation(mGravityWellPos);
	m_effect->SetWorld(m_world);
	m_effect->SetColorAndAlpha(SimpleMath::Color(0.f, 0.f, 0.f, 0.3f));
	mGravityWell->Draw(m_effect.get(), mGwInputLayout.Get(), true, false, [=]
	{
		mDirect3D->GetDeviceContext()->RSSetState(m_states->CullNone());
	});*/
	mGwManager->Render(m_view);

	//Wall	
	mDirect3D->GetWorld(m_world);
	mWall->Draw(mDirect3D->GetDeviceContext(), *m_states, m_world, m_view, m_proj, false, [=]
	{
		mDirect3D->GetDeviceContext()->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
		mDirect3D->GetDeviceContext()->RSSetState(m_states->CullNone());
	});

	//Testing Cup
	/*mCup->Draw(mDirect3D->GetDeviceContext(), *m_states, m_world, m_view, m_proj, false, [=]
	{
		mDirect3D->GetDeviceContext()->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
	});*/


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
	TwAddVarRW(mATBar, "Total number of balls", TW_TYPE_INT32, &mNumberOfBalls, "");
	TwAddVarRW(mATBar, "Magnitude of the applying force ", TW_TYPE_FLOAT, &mGwForce, "");
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
		if (mConfig->GetIsPaused())
			mConfig->SetIsPaused(false);
		else
			mConfig->SetIsPaused(true);
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
	//else if (tracker.pressed.Right)
	//{
	//	mCamera->RotateY();
	//}
			
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
		mGwManager->ClearForce(mOwnerID);
	}
	else if (mouse.leftButton)
	{
		//1.Clear repelling force
		//2.Apply attractor force
		mGwManager->ClearForce(mOwnerID);
		mGwManager->GwAddAttractF(mOwnerID);
	}
	else if (mouse.rightButton)
	{
		//Clear Attrating force
		//Apply repellor force
		mGwManager->ClearForce(mOwnerID);
		mGwManager->GwAddRepellF(mOwnerID);
	}
	m_mouse->SetMode(mouse.middleButton ?  Mouse::MODE_ABSOLUTE: Mouse::MODE_RELATIVE);
}

void GraphicClass::GwMove(SimpleMath::Vector3 direction)
{
	SimpleMath::Vector3 move = direction*mGWMovementGain;		
	mGwManager->GwAddMove(mOwnerID, move);
	mGravityWellPos = mGwManager->GwGetPos(mOwnerID);
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

void GraphicClass::GwSetForce(float InForce)
{
	
}
