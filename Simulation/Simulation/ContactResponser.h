#pragma once
#include "pch.h"

class ContactResponser
{
public:
	ContactResponser();
	~ContactResponser();

	void Resolve(float duration);

private:
	void ResolveVelocity(float duration);
	void ResolveInterPenetration(float duration);
};

