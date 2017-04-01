#include "pch.h"
#include "CameraClass.h"

//https://www.gamedev.net/resources/_/technical/directx-and-xna/directx-11-c-game-camera-r2978

CameraClass::CameraClass():
	mCamPos(SimpleMath::Vector3::Zero),
	//mMove(SimpleMath::Vector3::Zero),
	mMovementGain(0.1f),
	mLookAt(SimpleMath::Vector3::Zero),
	mView(SimpleMath::Matrix::Identity)
{
	
}

void CameraClass::SetLookAt(SimpleMath::Vector3 newtarget)
{	
	return;
}

void CameraClass::SetPosition(SimpleMath::Vector3 & cameraPos)
{
	mCamPos = cameraPos;
	this->SetViewMatrix();
	return;
}

void CameraClass::SetMovementGain(float & movementGain)
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

void CameraClass::SetViewMatrix()
{
	mView = SimpleMath::Matrix::CreateLookAt(mCamPos, mLookAt, SimpleMath::Vector3::Up);
	return;
}

void CameraClass::Move(SimpleMath::Vector3 direction)
{
	SimpleMath::Vector3 move = direction*mMovementGain;
	mCamPos += move;
	mLookAt = move - mCamPos;
	this->SetViewMatrix();
	return;
}

void CameraClass::MoveForward()
{
	this->Move(SimpleMath::Vector3::Forward);
}
