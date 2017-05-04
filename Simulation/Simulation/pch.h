#pragma once
// Win32
#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <Winuser.h>
#include <mmsystem.h>
#include <wrl.h>
#include <exception>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "winmm.lib")

// STL
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <bemapiset.h>
#include <profileapi.h>
#include <thread>
#include<mutex>

// DirectX
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

//DXTK
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "SimpleMath.h"
#include "WICTextureLoader.h"

//AntTweakBar
#include <AntTweakBar.h>

//Global Class
#include "ConfigClass.h"
#include "D3DClass.h"
#include "StepTimer.h"

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
			static char s_str[64] = { 0 };
			sprintf_s(s_str, "Failure with HRESULT of %08X", result);
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}

// Convenience macro for releasing COM objects
#define ReleaseCOM(x) if (x != NULL) { x->Release(); x = NULL; }
