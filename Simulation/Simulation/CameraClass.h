#pragma once
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&) = default;
	~CameraClass() = default;

	void SetLookAt();
	
	void SetPosition(SimpleMath::Vector3 &CameraPos);
	void SetRotation(SimpleMath::Vector3 &rotation);
	void ResetCamera();

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

	void ZoomIn();
	void ZoomOut();

private:
	SimpleMath::Vector3 mCamPos;
	SimpleMath::Vector3 mMove;
};

