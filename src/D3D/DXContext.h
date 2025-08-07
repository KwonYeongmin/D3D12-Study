#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

///  @breif DirectX 12 디버그 레이어는 코드 작성 및 문제 해결을 돕는 레이어
class DXContext
{
public:
	bool Init();
	void Shutdown();
	
	void SignalAndWait();
	ID3D12GraphicsCommandList7* InitCommandList();
	void ExecuteCommandList();
	
	// inline ComPointer<IDXGIFactory7>& GetDXGIFactory() { return m_dxgiFactory; }

	inline ComPointer<ID3D12Device10>& GetDevide() { return m_device; }
	
	inline ComPointer<ID3D12CommandQueue> GetCommandQueue() { return m_cmdQueue; }
	
private:
	// ComPointer<IDXGIFactory7> m_dxgiFactory; 
	ComPointer<ID3D12Device10> m_device;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;

	ComPointer<ID3D12CommandAllocator> m_cmdAllocator;
	ComPointer<ID3D12GraphicsCommandList7> m_cmdList;

	ComPointer<ID3D12Fence1> m_fence;
	UINT64 m_fenceValue = 0;
	HANDLE m_fenceEvent = nullptr;

// Singleton
	public :
		DXContext(const DXContext&) = delete;
		DXContext& operator=(const DXContext&) = delete;

		inline static DXContext& Get()
		{
			static DXContext instance;
			return instance;
		}
	private:
		DXContext() = default;
};