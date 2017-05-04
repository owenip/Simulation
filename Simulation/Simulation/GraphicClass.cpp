#include "GraphicClass.h"



GraphicClass::GraphicClass():
	timescale(1.f),
	mActualFPS(0)
{
	
}


GraphicClass::~GraphicClass()
{
	
}

bool GraphicClass::Initialize(const HWND hwnd, shared_ptr<ConfigClass> Config)
{
	auto result = false;

	mConfig = Config;
	DX::ThrowIfFailed(mScreenWidth = mConfig->GetScreenWidth());
	DX::ThrowIfFailed(mScreenHeight = mConfig->GetScreenHeight());
	DX::ThrowIfFailed(mPeerID = mConfig->GetPeerID());
	DX::ThrowIfFailed(mNumberOfBalls = mConfig->GetNumberOfBalls());

	//DirectX Class initialisation
	mDirect3D = std::make_shared<D3DClass>();
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

	//Timer initialse
	mTarGraphicFreq = mConfig->GetTarGraphicFreq();
	mTarPhysicsFreq = mConfig->GetTarPhyFreq();
	mTarNetowrkFreq = mConfig->GetTarNetworkFreq();
	mGraphicTimer.SetFixedTimeStep(true);
	mGraphicTimer.SetTargetElapsedSeconds(1/(mTarGraphicFreq));
	
	//Changable force
	mFriction = mConfig->GetFriction();
	mElasticity = mConfig->GetElasticForce();

	//AntTweakBar initialse
	InitAntTweak(hwnd);

	//Mouse & keyboard initialisation
	m_keyboard = std::make_unique<Keyboard>();
	tracker.Update(m_keyboard->GetState());

	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(hwnd);
	m_mouse->SetMode(Mouse::MODE_RELATIVE);
	mousetracker.Update(m_mouse->GetState());


	//Camera Initialisation
	mCamera = std::make_unique<CameraClass>();
	DX::ThrowIfFailed(mCamera->Initialize(SimpleMath::Vector3::Zero));

	//Effect Initialisation
	m_states = std::make_unique<DirectX::CommonStates>(mDirect3D->GetDevice());
	m_effect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	//m_effect->SetTextureEnabled(true);
	//m_effect->SetAmbientLightColor(SimpleMath::Vector3(.5f, .5f, .5f));
	//m_effect->SetAlpha(.3f);
	SimpleMath::Color WallColor = Colors::Black;	
	//Change to Transparent effect
	WallColor.w = 0.3f;
	m_effect->SetColorAndAlpha(WallColor);

	m_fxFactory = std::make_unique<EffectFactory>(mDirect3D->GetDevice());

	//
	//Models Initialisation
	//

	//Surface
	mSurface = GeometricPrimitive::CreateCylinder(mDirect3D->GetDeviceContext(), 0.05f, mConfig->GetSurfaceRadius() * 2);
	//mSurface = Model::CreateFromCMO(mDirect3D->GetDevice(), L".\\Resources\\surface.cmo", *m_fxFactory, true, true);

	//Wall
	mWall = GeometricPrimitive::CreateCylinder(mDirect3D->GetDeviceContext(), 50.f, mConfig->GetSurfaceRadius() * 2);
	mWall->CreateInputLayout(m_effect.get(), mWallInputLayout.ReleaseAndGetAddressOf());
	//mWallfxFactory = std::make_unique<EffectFactory>(mDirect3D->GetDevice());
	//mWallfxFactory->SetDirectory(L".\\Resources\\");
	//mWall = Model::CreateFromCMO(mDirect3D->GetDevice(), L".\\Resources\\wall.cmo", *m_fxFactory, true, true);
	/*mWall->UpdateEffects([](IEffect* effect)
	{
		auto alpha = dynamic_cast<BasicEffect*>(effect);
		if (alpha)
		{
			alpha->SetAlpha(.5f);
		}
	});*/

	return true;
}

void GraphicClass::Shutdown()
{	
	//mWallEffect.reset();
	mWallfxFactory.reset();	
	mWall.reset();

	mSurface.reset();
	mSurfaceInputLayout.Reset();
	mSurfaceTex.Reset();

	m_fxFactory.reset();
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();

	mCamera.reset();

	TwTerminate();
	mBallManager.reset();
	mDirect3D.reset();
	mConfig.reset();	
}

void GraphicClass::OnPause()
{
	tracker.Reset();
	mousetracker.Reset();
	return;
}

void GraphicClass::OnResume()
{

}

void GraphicClass::Tick()
{
	mIsEscaped = false;
	//while (true)
	{
		Update(mGraphicTimer);
		mGraphicTimer.Tick([&]()
		{			
			Render();
		});
		mActualFPS = mGraphicTimer.GetFramesPerSecond();
		mTarGraphicFreq = mConfig->GetTarGraphicFreq();
		mTarPhysicsFreq = mConfig->GetTarPhyFreq();
		mTarNetowrkFreq = mConfig->GetTarNetworkFreq();
		mGwForce = mGwManager->GwGetForce(mPeerID);
		/*if (mIsEscaped)
			break;*/
	}
}

bool GraphicClass::Update(DX::StepTimer const& timer)
{
	CheckInput();
	//float dt = float(mGraphicTimer.GetElapsedSeconds());
	mCamera->SetLookAt(mGwManager->GwGetPos(mPeerID));
	//mBallManager->Integrate(dt);
	//CheckInput();
	
	

	return true;
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
	//m_effect->SetWorld(m_world);

	//Draw Model
	//Surface	
	mSurface->Draw(m_world, m_view, m_proj, Colors::Silver);
	//Balls
	mBallManager->Render(m_view);
	//GravityWell
	mGwManager->Render(m_view);
	//Wall	
	mDirect3D->GetWorld(m_world);
	m_world = m_world.CreateTranslation(SimpleMath::Vector3(0.f, 25.f, 0.f));
	m_effect->SetWorld(m_world);
	mWall->Draw(m_effect.get(), mWallInputLayout.Get(), true, false, [=]
	{
		mDirect3D->GetDeviceContext()->RSSetState(m_states->CullClockwise());
	});
	//mDirect3D->GetWorld(m_world);
	/*mWall->Draw(mDirect3D->GetDeviceContext(), *m_states, m_world, m_view, m_proj, false, [=]
	{
		mDirect3D->GetDeviceContext()->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xFFFFFFFF);
		mDirect3D->GetDeviceContext()->RSSetState(m_states->CullNone());
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
	int barSize[2] = { mScreenWidth / 4, mScreenHeight / 4 };
	TwSetParam(mATBar, nullptr, "size", TW_PARAM_INT32, 2, barSize);
	//Non-Changable variables
	TwAddVarRW(mATBar, "Actual FPS", TW_TYPE_DOUBLE, &mActualFPS, "");
	TwAddVarRW(mATBar, "No. of balls owned by this peer", TW_TYPE_INT32, &mNumberOfBalls, "min=0 max=64000");
	TwAddVarRW(mATBar, "No. of balls currently contended", TW_TYPE_INT32, nullptr, "");
	TwAddVarRW(mATBar, "Total number of balls", TW_TYPE_INT32, &mNumberOfBalls, "");
	TwAddVarRW(mATBar, "Magnitude of the applying force ", TW_TYPE_FLOAT, &mGwForce, "");
	TwAddVarRW(mATBar, "Time step of each simulation loop per second", TW_TYPE_INT32, nullptr, "");

	//Changable variables
	TwAddVarRW(mATBar, "The time scale", TW_TYPE_FLOAT, &timescale, "");
	TwAddVarRW(mATBar, "1 - Elasticity", TW_TYPE_FLOAT, &mElasticity, "");
	TwAddVarRW(mATBar, "1 - Frictional", TW_TYPE_FLOAT, &mFriction, "");
	TwAddVarRW(mATBar, "Tar Physics Freq(in Hz)", TW_TYPE_FLOAT, &mTarPhysicsFreq, "");
	TwAddVarRW(mATBar, "Tar graphics freq(in Hz)", TW_TYPE_FLOAT, &mTarGraphicFreq, "");
	TwAddVarRW(mATBar, "Tar networking freq(in Hz)", TW_TYPE_FLOAT, &mTarNetowrkFreq, "");

	return true;
}

void GraphicClass::CheckInput()
{
	auto kbState = m_keyboard->GetState();
	tracker.Update(kbState);

	if (tracker.pressed.Escape)
	{
		mIsEscaped = true;
		mConfig->SetIsEscaped(true);
		PostQuitMessage(0);
	}		
	else if (tracker.pressed.R)
	{
		//Reset		
		//mConfig->Reset();
		mBallManager->ReSetBallPosition();		
	}
	else if (tracker.pressed.P)
	{
		//Pause the simulation loop globally (globally)
		if (mConfig->GetIsPaused())
			mConfig->SetIsPaused(false);
		else
			mConfig->SetIsPaused(true);

		mGraphicTimer.ResetElapsedTime();
	}
	else if (tracker.pressed.O)
	{
		//Increase frequency of physics
		mTarPhysicsFreq+=10;
		mConfig->SetTarPhyFreq(mTarPhysicsFreq);
	}
	else if (tracker.pressed.L)
	{
		//Decrease frequency of physics
		(mTarPhysicsFreq < 1) ? mTarPhysicsFreq = 1 : mTarPhysicsFreq-=10;
		mConfig->SetTarPhyFreq(mTarPhysicsFreq);
	}
	else if (tracker.pressed.I)
	{
		//Increase frequency of graphics
		mTarGraphicFreq+=10;
		mConfig->SetTarGraphicFreq(mTarGraphicFreq);
		mGraphicTimer.SetTargetElapsedSeconds(1 / (mTarGraphicFreq));
	}
	else if (tracker.pressed.K)
	{
		//Decrease frequency of graphics
		(mTarGraphicFreq < 1) ? mTarGraphicFreq = 1 : mTarGraphicFreq-=10;
		mConfig->SetTarGraphicFreq(mTarGraphicFreq);
		mGraphicTimer.SetTargetElapsedSeconds(1 / (mTarGraphicFreq));
	}
	else if (tracker.pressed.U)
	{
		//Increase frequency of networking
		mTarNetowrkFreq+=10;
		mConfig->SetTarNetworkFreq(mTarNetowrkFreq);
	}
	else if (tracker.pressed.J)
	{
		//Decrease frequency of networking
		(mTarNetowrkFreq < 1) ? mTarNetowrkFreq = 1 : mTarNetowrkFreq-=10;
		mConfig->SetTarNetworkFreq(mTarNetowrkFreq);
	}
	else if (tracker.pressed.Y)
	{
		//Increase frequency of Time scale (globally)
		//Maximum 1.0 Step:0.01
		timescale += 0.05f;
		if (timescale > 1.f)
			timescale = 1.f;
		mConfig->SetTimeScale(timescale);
	}
	else if (tracker.pressed.H)
	{
		//Decrease frequency of Time scale (globally)
		//Minimum 0.001
		timescale -= 0.05f;
		if (timescale < 0.001f)
			timescale = 0.001f;
		mConfig->SetTimeScale(timescale);
	}
	else if (kbState.M)
	{
		//Increase the height of the gravity well above the surface
		mGwManager->GwMoveUp();
	}
	else if (kbState.N)
	{
		//Decrease the height of the gravity well above the surface
		mGwManager->GwMoveDown();
	}
	else if (tracker.pressed.F1)
	{
		//Display All Owned Ball
		mConfig->GetDisplayAll() ? mConfig->SetDisplayAll(false) : mConfig->SetDisplayAll(true);
	}
	else if(tracker.pressed.F9)
	{
		mElasticity -= 0.1;
		if (mElasticity < 0)
			mElasticity = 0.f;		
		mConfig->SetElasticForce(mElasticity);
	}
	else if (tracker.pressed.F10)
	{
		mElasticity += 0.1;
		if(mElasticity > 2) 
			mElasticity = 2.f;
		mConfig->SetElasticForce(mElasticity);
	}
	else if (tracker.pressed.F7)
	{
		mFriction -= 0.1;
		if (mFriction < 0)
			mFriction = 0.f;
		mConfig->SetFriction(mFriction);
	}
	else if (tracker.pressed.F8)
	{
		mFriction += 0.1;
		if (mFriction > 1)
			mFriction = 1.f;
		mConfig->SetFriction(mFriction);
	}

	//Camera Control
	if (kbState.W)
	{
		//Camera move forward
		mGwManager->GwMoveForward();
	}
	else if (kbState.S)
	{
		//Camera move Backward
		mGwManager->GwMoveBackward();
	}
	else if (kbState.A)
	{
		//Camera move left
		mGwManager->GwMoveLeft();
	}
	else if (kbState.D)
	{
		//Camera move right
		mGwManager->GwMoveRight();
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
		mGwManager->GwMoveByMouse(mouseX * 100 , mouseY* 100);
	}
	if (mouse.leftButton && mouse.rightButton)
	{
		//Cancel force
		//Force = 0		
		mGwManager->ClearForce();
	}
	else if (mouse.leftButton)
	{
		//1.Clear repelling force
		//2.Apply attractor force
		if(mGwForce < 0)
			mGwManager->ClearForce();
		mGwManager->GwAddAttractF();
	}
	else if (mouse.rightButton)
	{
		//Clear Attrating force
		//Apply repellor force
		if (mGwForce > 0)
			mGwManager->ClearForce();
		mGwManager->GwAddRepellF();
	}

	m_mouse->SetMode(mouse.middleButton ? Mouse::MODE_ABSOLUTE : Mouse::MODE_RELATIVE);
}

