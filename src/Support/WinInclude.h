#pragma once

#define NOMINMAX

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>	//�ֽ� ��� : �׻� �ֽ� ����� ����ϴ� ���� ����.

#ifdef _DEBUG	// ������� ��ȯ�ϸ� �ڵ����� �����
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif

#include <wincodec.h>