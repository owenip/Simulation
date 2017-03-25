#pragma once
class D3DClass
{
#define ReleaseCOM(x) if (x != NULL) { x->Release(); x = NULL; }
public:
	D3DClass();
	D3DClass(const D3DClass&) = default;
	D3DClass& operator=(const D3DClass&) = default;
	~D3DClass();

	bool Initialize(const HWND hwnd, const ConfigClass *mConfig);
	void Shutdown();

private:
	//Config Class
	ConfigClass *mConfig;

	//DirectX
	IDXGISwapChain *mSwapChain;
	ID3D11Device *mDevice;
	ID3D11DeviceContext *mDeviceContext;
	ID3D11RenderTargetView* mRenderTargetView;

	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11Texture2D* m_depthStencilBuffer;

	D3D11_VIEWPORT mScreenViewport;

	bool m_vsync_enabled;
	
};

