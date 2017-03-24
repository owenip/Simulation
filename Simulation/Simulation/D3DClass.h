#pragma once
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&) = default;
	D3DClass& operator=(const D3DClass&) = default;
	~D3DClass();

	bool Initialize(const HWND hwnd, ConfigClass *mConfig);
	void Shutdown();

private:
	//Config Class
	ConfigClass *mConfig;

	//DirectX
	IDXGISwapChain* mSwapChain;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;


};

