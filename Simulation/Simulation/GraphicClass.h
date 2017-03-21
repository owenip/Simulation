#pragma once
class GraphicClass
{
public:
	GraphicClass();
	GraphicClass& operator=(const GraphicClass&) = default;
	~GraphicClass();

	bool Initialize(const HWND hwnd, const ConfigClass *mConfig);
	void Shutdown();

private:


private:
	//Config Class
	ConfigClass *mConfig;
};

