#include "pch.h"
#include "D3DClass.h"

D3DClass::D3DClass():
	mConfig(nullptr),
	mSwapChain(nullptr),
	mDevice(nullptr),
	mDeviceContext(nullptr), 
	mRenderTargetView(nullptr),
	blendFactor{ 0.f,0.f,0.f,0.3f }
{
	
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(const HWND hwnd, const ConfigClass * Config)
{
	
	HRESULT result;
	mConfig = make_shared<ConfigClass>(Config);
	DX::ThrowIfFailed(mScreenWidth = mConfig->GetScreenWidth());
	DX::ThrowIfFailed(mScreenHeight = mConfig->GetScreenHeight());

	D3D_FEATURE_LEVEL featureLevel;

	result = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr, 0,			///Default Feature Levels
		D3D11_SDK_VERSION,
		&mDevice,
		&featureLevel,
		&mDeviceContext);

	if (FAILED(result))
	{
		MessageBox(NULL, L"D3D11CreateDevice Failed.", NULL, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(NULL, L"Direct3D Feature Level 11 unsupported.", NULL, 0);
		return false;
	}

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	DXGI_SWAP_CHAIN_DESC sd;

	// Set the width and height of the back buffer.
	sd.BufferDesc.Width = mScreenWidth;
	sd.BufferDesc.Height = mScreenHeight;

	// Set the refresh rate of the back buffer.
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the scan line ordering and scaling to unspecified.
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Set the usage of the back buffer.
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Set to a single back buffer.
	sd.BufferCount = 1;
	// Set the handle for the window to render to.
	sd.OutputWindow = hwnd;
	// Discard the back buffer contents after presenting.
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	// Don't set the advanced flags.
	sd.Flags = 0;
	// Turn multisampling off.
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	// Set to full screen or windowed mode.
	if (mConfig->CheckFullScreen())
	{
		sd.Windowed = false;
	}
	else
	{
		sd.Windowed = true;
	}

	//Obtain the Factory that was used to create the device for creating the swap chain correctly
	IDXGIDevice *dxgiDevice = nullptr;
	result = mDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	if(FAILED(result))
	{
		MessageBox(NULL, L"D3D11Device QueryInterface Failed.", NULL, 0);
		return false;
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	result = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
	if (FAILED(result))
	{
		MessageBox(NULL, L"IDXGIDevice GetParent Failed.", NULL, 0);
		return false;
	}

	IDXGIFactory* dxgiFactory = nullptr;
	result = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
	if (FAILED(result))
	{
		MessageBox(NULL, L"dxgiAdapter GetParent Failed.", NULL, 0);
		return false;
	}

	//Create SwapChain
	result =  dxgiFactory->CreateSwapChain(mDevice, &sd, &mSwapChain);
	if (FAILED(result))
	{
		MessageBox(NULL, L"CreateSwapChain Failed.", NULL, 0);
		return false;
	}
		
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	//Create Render Target View
	ID3D11Texture2D* backBuffer;
	DX::ThrowIfFailed(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	DX::ThrowIfFailed(mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.Width = mConfig->GetScreenWidth();
	depthStencilDesc.Height = mConfig->GetScreenHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	DX::ThrowIfFailed(mDevice->CreateTexture2D(&depthStencilDesc, nullptr, &mDepthStencilBuffer));
	DX::ThrowIfFailed(mDevice->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &mDepthStencilView));

	//Bind the render target view and depth/stencil view to the pipeline
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);



	D3D11_RASTERIZER_DESC rasterDesc;
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = mDevice->CreateRasterizerState(&rasterDesc, &m_NormalrasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	mDeviceContext->RSSetState(m_NormalrasterState);

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	// Create the rasterizer state from the description we just filled out.
	result = mDevice->CreateRasterizerState(&rasterDesc, &m_WireFramerasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Set the viewport transform.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mConfig->GetScreenWidth());
	mScreenViewport.Height = static_cast<float>(mConfig->GetScreenHeight());
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mDeviceContext->RSSetViewports(1, &mScreenViewport);

	// Setup the projection matrix.
	//mProj = XMMatrixPerspectiveFovLH(0.25*XM_PI, AspectRatio(), 1.0f, 1000.f);
	mProj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		this->AspectRatio(), 1.f, 1000.f);

	// Initialize the world matrix to the identity matrix.
	mWorld = SimpleMath::Matrix::Identity;

	// Create an orthographic projection matrix for 2D rendering.
	mOrth = XMMatrixOrthographicLH(static_cast<float>(mScreenWidth), static_cast<float>(mScreenHeight), 1.0f, 1000.f);
	
	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	result = mDevice->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = mDevice->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		return false;
	}
	

	return true;
}

void D3DClass::Shutdown()
{
	mConfig.reset();

	if (mSwapChain)
	{
		mSwapChain->SetFullscreenState(false, nullptr);
	}
	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = nullptr;
	}

	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = nullptr;
	}
	if (m_NormalrasterState)
	{
		m_NormalrasterState->Release();
		m_NormalrasterState = nullptr;
	}
	if (m_WireFramerasterState)
	{
		m_WireFramerasterState->Release();
		m_WireFramerasterState = nullptr;
	}
	if (mDepthStencilView)
	{
		mDepthStencilView->Release();
		mDepthStencilView = nullptr;
	}

	if (mDepthStencilBuffer)
	{
		mDepthStencilBuffer->Release();
		mDepthStencilBuffer = nullptr;
	}

	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = nullptr;
	}

	if (mDeviceContext)
	{
		mDeviceContext->Release();
		mDeviceContext = nullptr;
	}

	if (mDevice)
	{
		mDevice->Release();
		mDevice = nullptr;
	}

	if (mSwapChain)
	{
		mSwapChain->Release();
		mSwapChain = nullptr;
	}

}

void D3DClass::BeginScene(DirectX::SimpleMath::Vector4 ClearColour)
{
	FLOAT clearColour[4];
	// Setup the color to clear the buffer to.
	clearColour[0] = ClearColour.x;
	clearColour[1] = ClearColour.y;
	clearColour[2] = ClearColour.z;
	clearColour[3] = ClearColour.w;
	
	// Clear the back buffer.
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColour);
	// Clear the depth buffer.
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		mSwapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		mSwapChain->Present(0, 0);
	}
}

float D3DClass::AspectRatio() const
{
	return static_cast<float>(mScreenWidth) / mScreenHeight;
}

ID3D11Device* D3DClass::GetDevice() const 
{
	return mDevice;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext() const 
{
	return mDeviceContext;
}

void D3DClass::GetProj(SimpleMath::Matrix & projMatrix) const
{
	projMatrix = mProj;
	return;
}

void D3DClass::GetWorld(SimpleMath::Matrix & worldMatrix) const
{
	worldMatrix = mWorld;
	return;
}

void D3DClass::GetOrthoMatrix(SimpleMath::Matrix & orthoMatrix) const
{
	orthoMatrix = mOrth;
	return;
}

void D3DClass::TurnOnAlphaBlending() const
{
	// Turn on the alpha blending.
	mDeviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
	return;
}

void D3DClass::TurnOffAlphaBlending()
{
	// Turn off the alpha blending.
	mDeviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
	return;
}

void D3DClass::TurnOnWireFrame() const
{
	mDeviceContext->RSSetState(m_WireFramerasterState);
	return;
}

void D3DClass::TurnOffWireFrame() const
{
	mDeviceContext->RSSetState(m_NormalrasterState);
	return;
}
