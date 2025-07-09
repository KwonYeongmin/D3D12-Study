#include <iostream>

#include <Support/WinInclude.h>
#include <Support/Compointer.h>
#include <Debug/DXDebugLayer.h>

using namespace std;

int main()
{
	// Debug Layer 초기화
	DXDebugLayer::Get().Init();

	ComPointer<ID3D12Device10> device;
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));


	// 
	DXDebugLayer::Get().Shutdown();
}


