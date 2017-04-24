#pragma once
#include "GravityWellClass.h"

class GravityWellManager
{
public:
	GravityWellManager();
	~GravityWellManager();

	bool	Initialise();
	bool	InitialiseGraphic(shared_ptr<D3DClass> InDirect3D);

	void	Render(SimpleMath::Matrix InView);
	void	Shutdown();

	void	SetGwRadius(int InGwRadius);
	float	GetGwRadius() const;

	void	AddGw(int GwID, SimpleMath::Vector3	InGravityWellPos, SimpleMath::Color InGwColor);
	void	RemoveGw(int GwID);

	void GwSetPos(int GwID, SimpleMath::Vector3 InGravityWellPos);

	//Increase Gravity Well Attracting Force (Will naturalise repelling force) 
	void	GwAddAttractF(int GwID);
	//Increase Gravity Well Repelling force (Will naturalise Attracting force)
	void	GwAddRepellF(int GwID);

private:
	float mGwRadius;

	shared_ptr<D3DClass> mDirect3D;

	std::unique_ptr<DirectX::CommonStates> mStates;
	std::unique_ptr<DirectX::GeometricPrimitive> mGwPrimitive;
	std::unique_ptr<DirectX::GeometricPrimitive> mGwCenter;
	std::unique_ptr<DirectX::BasicEffect> mGwEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mGwInputLayout;

	std::vector<GravityWellClass*> mGwIndex;

	float ForceGain;
};

