#pragma once

#define NOMINMAX

#include <Windows.h>

// D3D Header
#include <d3d12.h>
#include <dxgi1_6.h>		//최신 헤더 : 항상 최신 헤더를 사용하는 것이 좋다.

// Debugging
#ifdef _DEBUG				// 릴리즈로 전환하면 자동으로 사라짐
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif