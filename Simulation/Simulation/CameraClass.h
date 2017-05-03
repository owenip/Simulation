#pragma once
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&) = default;
	~CameraClass() = default;

	bool Initialize(const SimpleMath::Vector3 &gravityWellPos);

	void SetLookAt(SimpleMath::Vector3 newtarget);

	void SetPosition(SimpleMath::Vector3 &cameraPos);
	void SetRotation(SimpleMath::Vector3 &rotation);
	void ResetCamera();

//Camera Movement Controls for view matrix transformation
	void Move(SimpleMath::Vector3 direction);
	void MoveForward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();

	void RotateY();

	void Zooming(float &distance);
	void ZoomIn();
	void ZoomOut();

	void SetMovementGain(float &movementGain);

	void GetView(SimpleMath::Matrix &inView) const;
	void GetProj(SimpleMath::Matrix &inProj) const;

private:
	void UpdateViewMatrix();
	void CalCamPosHeight(float &newHeight);
	//Clamp Zooming distance
	float Clamp(float n, float lower, float upper) {
		return std::fmax(lower, std::fmin(n, upper));
	}

private:
	SimpleMath::Vector3 mCamPos;
	//SimpleMath::Vector3 mMove;
	float mMovementGain;
	
	SimpleMath::Vector3 mUp;
	SimpleMath::Vector3 mLookAt;

	SimpleMath::Matrix mView;
	SimpleMath::Matrix mProj;

	float mDsitanceFromTarget;
	const float mMinDistFromTarget = 1.0f;
	const float mMaxDistFromTarget = 50.0f;
	float mHeightfromTarget;
	
	float mRotationRad;

};

