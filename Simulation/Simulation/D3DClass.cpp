#include "pch.h"
#include "D3DClass.h"



D3DClass::D3DClass():
	mConfig(nullptr),
	mSwapChain(nullptr),
	mDevice(nullptr),
	mDeviceContext(nullptr)
	
{
	//Create Device and device context

	
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(const HWND hwnd, const ConfigClass * mConfig)
{
	HRESULT result;
	

	D3D_FEATURE_LEVEL featureLevel;

	result = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr, 0,		///Default Feature Levels
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
	sd.BufferDesc.Width = mConfig->GetScreenWidth();
	sd.BufferDesc.Height = mConfig->GetScreenHeight();

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
	DX::ThrowIfFailed(mDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_depthStencilBuffer));
	DX::ThrowIfFailed(mDevice->CreateDepthStencilView(m_depthStencilBuffer, nullptr, &mDepthStencilView));

	//Bind the render target view and depth/stencil view to the pipeline
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Set the viewport transform.

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mConfig->GetScreenWidth());
	mScreenViewport.Height = static_cast<float>(mConfig->GetScreenHeight());
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mDeviceContext->RSSetViewports(1, &mScreenViewport);

	return true;
}

void D3DClass::Shutdown()
{
	if (mConfig)
	{
		delete mConfig;
		mConfig = nullptr;
	}
}


