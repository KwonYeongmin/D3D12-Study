#pragma once
#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <D3D/DXContext.h>


#define FRAME_COUNT 2

class DXWindow
{
public:
	bool Init();
	void Update();
	void Present();
	void Shutdown();
	void Resize();

	inline bool ShouldClose() const { return m_shouldClose; }
	inline bool ShouldResize() const { return m_shouldResize; }

	inline UINT GetWidth() const { return m_width; }
	inline UINT GetHeight() const { return m_height; }

	static constexpr size_t GetFrameCount() { return FRAME_COUNT; }

private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	ATOM m_wndClass = 0;
	HWND m_window = nullptr;

	bool m_shouldClose = false;
	bool m_shouldResize = false;

	UINT m_width = 1280;
	UINT m_height = 720;

	ComPointer<IDXGISwapChain3> m_swapChain;
	UINT m_currentFrame = 0;

	ComPointer<ID3D12DescriptorHeap> m_rtvHeap;
	ComPointer<ID3D12Resource> m_renderTargets[FRAME_COUNT];
	UINT m_rtvDescriptorSize = 0;

public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle();
	ComPointer<ID3D12Resource> GetCurrentRenderTarget();
	DXWindow(const DXWindow&) = delete;
	DXWindow& operator=(const DXWindow&) = delete;

	// Singleton
	inline static DXWindow& Get()
	{
		static DXWindow instance;
		return instance;
	}
private:
	DXWindow() = default;
};