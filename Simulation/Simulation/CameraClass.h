#pragma once
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&) = default;
	~CameraClass() = default;

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

	void ZoomIn();
	void ZoomOut();

	void SetMovementGain(float &movementGain);


	void GetView(SimpleMath::Matrix &inView) const;

private:
	void SetViewMatrix();


private:
	SimpleMath::Vector3 mCamPos;
	//SimpleMath::Vector3 mMove;
	float mMovementGain;

	SimpleMath::Vector3 mLookAt;
	SimpleMath::Matrix mView;

};

