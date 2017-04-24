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
protected:	
	int mGwID;
	SimpleMath::Vector3	mGravityWellPos;
	SimpleMath::Color	mGwColor;
	float mForce;
};

