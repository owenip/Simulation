#pragma once


class D3DClass
{

public:
	D3DClass();
	D3DClass(const D3DClass&) = default;
	D3DClass& operator=(const D3DClass&) = default;
	~D3DClass();

	bool Initialize(const HWND hwnd, const ConfigClass *mConfig);
	void Shutdown();

	//Clear buffer for next frame
	void BeginScene(DirectX::SimpleMath::Vector4 ClearColour);
	//Present drawn scene to the screen
	void EndScene();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	
	void GetProj(SimpleMath::Matrix &projMatrix) const;
	void GetWorld(SimpleMath::Matrix &worldMatrix) const;
	void GetOrthoMatrix(SimpleMath::Matrix &orthoMatrix) const;

	void TurnOnAlphaBlending() const;
	void TurnOffAlphaBlending();

	void TurnOnWireFrame() const;
	void TurnOffWireFrame() const;


	float AspectRatio() const;

private:
	//Config Class
	ConfigClass *mConfig;

	//DirectX
	IDXGISwapChain *mSwapChain;
	ID3D11Device *mDevice;
	ID3D11DeviceContext *mDeviceContext;
	ID3D11RenderTargetView* mRenderTargetView;

	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11Texture2D* mDepthStencilBuffer;

	ID3D11RasterizerState* m_NormalrasterState;
	ID3D11RasterizerState* m_WireFramerasterState;


	D3D11_VIEWPORT mScreenViewport;

	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

	DirectX::SimpleMath::Matrix mWorld;
	DirectX::SimpleMath::Matrix mProj;
	DirectX::SimpleMath::Matrix mOrth;
		
	bool m_vsync_enabled;
	int mScreenWidth;
	int mScreenHeight;
	float blendFactor[4];



};

