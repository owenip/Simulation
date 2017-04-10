#pragma once

// Win32
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
#include <bemapiset.h>
#include <profileapi.h>

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
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

//AntTweakBar
#include <AntTweakBar.h>

//Global

//My Own Class
#include "ConfigClass.h"
#include "TimerClass.h"
#include "D3DClass.h"

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

