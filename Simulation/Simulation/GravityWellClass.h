#pragma once
#include "pch.h"

class GravityWellClass
{
public:
	GravityWellClass();
	~GravityWellClass();

	void Initialize(int InID, SimpleMath::Vector3 InPosition, SimpleMath::Color InColor);

	void UpdatePos(SimpleMath::Vector3 InPosition);

	void	SetGwIDID(int InID);
	int		GetGwID() const;

	SimpleMath::Color GetColor() const;

	void	SetForce(float InForce);
	float	GetForce() const;

	void	AddForce(float InForce);
	void	ReduceForce(float InForce);

	void	SetPos(SimpleMath::Vector3 InPos);
	SimpleMath::Vector3 GetPos() const;
	void	AddMove(SimpleMath::Vector3 move);

	void SetApplyingForce(bool inVal);
	bool GetApplyingForce() const;


protected:
	int mGwID;
	SimpleMath::Vector3	mGravityWellPos;
	SimpleMath::Color	mGwColor;
	float mForce;
	bool mApplyingForce;
};

