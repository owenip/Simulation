#include "pch.h"
#include "CameraClass.h"

//https://www.gamedev.net/resources/_/technical/directx-and-xna/directx-11-c-game-camera-r2978

CameraClass::CameraClass():
	mCamPos(SimpleMath::Vector3::Zero),
	mMovementGain(0.f),
	mLookAt(SimpleMath::Vector3::Zero),
	mView(SimpleMath::Matrix::Identity),
	mUp(SimpleMath::Vector3::Up),
	mDsitanceFromTarget(0.f),
	mHeightfromTarget(0.f),
	mRotationRad(0.f)
{
	
}

bool CameraClass::Initialize(const SimpleMath::Vector3 &gravityWellPos)
{
	mMovementGain = 0.0001f;
	mDsitanceFromTarget = 30.f;
	this->CalCamPosHeight(mHeightfromTarget);

	//Setup ViewMatrix and parameters
	this->SetLookAt(gravityWellPos);
	return true;
}

void CameraClass::SetLookAt(SimpleMath::Vector3 newtarget)
{	
	mLookAt = newtarget;
	this->SetPosition(mLookAt + SimpleMath::Vector3(0.f, mHeightfromTarget, mDsitanceFromTarget));
	return;
}

void CameraClass::SetPosition(SimpleMath::Vector3 & cameraPos)
{
	mCamPos = cameraPos;
	this->UpdateViewMatrix();
	return;
}

void CameraClass::SetMovementGain(float &movementGain)
{
	mMovementGain = movementGain;
	return;
}

void CameraClass::GetView(SimpleMath::Matrix & inView) const
{
	if (inView != mView)
		inView = mView;
	return;
}

void CameraClass::GetProj(SimpleMath::Matrix & inProj) const
{
	if (inProj != mProj)
		inProj = mProj;
}

void CameraClass::UpdateViewMatrix()
{	
	mView = SimpleMath::Matrix::CreateLookAt(mCamPos, mLookAt, SimpleMath::Vector3::Up);
	return;
}

void CameraClass::CalCamPosHeight(float & newHeight)
{
	newHeight = mDsitanceFromTarget * tanf(XM_PI / 6.f);
}

void CameraClass::Move(SimpleMath::Vector3 direction)
{
	SimpleMath::Vector3 move = direction*mMovementGain;
	mCamPos += move;
	mLookAt += move;
	mUp += move;

	this->UpdateViewMatrix();
	return;
}

void CameraClass::MoveForward()
{
	this->Move(SimpleMath::Vector3::Forward);
}

void CameraClass::MoveBackward()
{
	this->Move(SimpleMath::Vector3::Backward);
}

void CameraClass::MoveLeft()
{
	this->Move(SimpleMath::Vector3::Left);
}

void CameraClass::MoveRight()
{
	this->Move(SimpleMath::Vector3::Right);
}

void CameraClass::RotateY()
{		
	mRotationRad += XM_PI * 0.05;
	
	SimpleMath::Vector3 newPos = mCamPos.Transform(mCamPos, SimpleMath::Matrix::CreateRotationY(mRotationRad));
	this->SetPosition(newPos);
	return;	
}


void CameraClass::Zooming(float &distance)
{
	distance = Clamp(distance, mMinDistFromTarget, mMaxDistFromTarget);
	CalCamPosHeight(mHeightfromTarget);
	SimpleMath::Vector3 newPos = mLookAt + SimpleMath::Vector3(0.f, mHeightfromTarget, mDsitanceFromTarget);

	this->SetPosition(newPos);
}

void CameraClass::ZoomIn()
{
	mDsitanceFromTarget -= mMovementGain;
	this->Zooming(mDsitanceFromTarget);
}

void CameraClass::ZoomOut()
{
	mDsitanceFromTarget += mMovementGain;
	this->Zooming(mDsitanceFromTarget);
}

