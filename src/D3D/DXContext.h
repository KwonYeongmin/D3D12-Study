#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

class DXContext
{
public:
	bool Init();
	void Shutdown();

	void SignalAndWait();
	ID3D12GraphicsCommandList* InitCommandList();
	void ExecuteCommandList();

	inline void Flush(size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			SignalAndWait();
		}
	}

	inline ComPointer<IDXGIFactory4>& GetFactory() { return m_dxgiFactory; }
	inline ComPointer<ID3D12Device>& GetDevide() { return m_device; }
	inline ComPointer<ID3D12CommandQueue> GetCommandQueue() { return m_cmdQueue; }

public:
	ComPointer<ID3D12RootSignature> m_rootSignature;
	ComPointer<ID3D12PipelineState> m_pipelineState;

	ComPointer<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};

private:
	ComPointer<IDXGIFactory4> m_dxgiFactory;

	ComPointer<ID3D12Device> m_device;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;

	ComPointer<ID3D12CommandAllocator> m_cmdAllocator;
	ComPointer<ID3D12GraphicsCommandList> m_cmdList;

	ComPointer<ID3D12Fence1> m_fence;
	UINT64 m_fenceValue = 0;
	HANDLE m_fenceEvent = nullptr;

	// Singleton
public:
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