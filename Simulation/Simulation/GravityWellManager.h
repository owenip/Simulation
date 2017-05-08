#pragma once
#include "GravityWellClass.h"

class GravityWellManager
{
public:
	GravityWellManager();
	~GravityWellManager();

	bool	Initialise(shared_ptr<ConfigClass> Config);
	bool	Initialise(int OwnerID);
	bool	InitialiseGraphic(shared_ptr<D3DClass> InDirect3D);

	void	Render(SimpleMath::Matrix InView);
	void	Shutdown();

	void	SetGwRadius(float InGwRadius);
	float	GetGwRadius() const;

	void	AddGw(int GwID, SimpleMath::Vector3	InGravityWellPos, SimpleMath::Color InGwColor);
	void	RemoveGw(int GwID);

	void	SetLocalID(int GwID);

	void	GwSetPos(int GwID, SimpleMath::Vector3 InGravityWellPos);
	SimpleMath::Vector3	GwGetPos(int GwID);

	void	GwAddMove(SimpleMath::Vector3 InMove);
	void	GwMoveForward();
	void	GwMoveBackward();
	void	GwMoveLeft();
	void	GwMoveRight();
	void	GwMoveByMouse(float mouseX, float mouseY);
	void	GwMoveUp();
	void	GwMoveDown();

	//Gw Force
	void	GwSetForce(int GwID, float Inforce);
	float	GwGetForce(int GwID);
	//Increase Gravity Well Attracting Force (Will naturalise repelling force) 
	void	GwAddAttractF();
	//Increase Gravity Well Repelling force (Will naturalise Attracting force)
	void	GwAddRepellF();
	//Clear Gw force
	void	ClearForce();

	

	std::vector<GravityWellClass*>	GetGwIndex() const;
	int	GetNumofGw() const;
	

private:
	std::mutex mustex_gw;

	float	mGwRadius;
	int		mLocalPeerID;

	shared_ptr<D3DClass> mDirect3D;
	shared_ptr<ConfigClass> mConfig;
	std::unique_ptr<DirectX::CommonStates> mStates;
	std::unique_ptr<DirectX::GeometricPrimitive> mGwPrimitive;
	std::unique_ptr<DirectX::GeometricPrimitive> mGwCenter;
	std::unique_ptr<DirectX::BasicEffect> mGwEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mGwInputLayout;

	std::vector<GravityWellClass*> mGwIndex;
		
	float mGWMovementGain;
	float mGwForceGain;
};

