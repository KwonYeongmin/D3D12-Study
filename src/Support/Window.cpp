#include "Window.h"

bool DXWindow::Init()
{
	// Window class
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = &DXWindow::OnWindowMessage;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"D3D12ExWndCls";
	wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
	m_wndClass = RegisterClassExW(&wcex);

	if (m_wndClass == 0)
	{
		return false;
	}

	// Place window on current screen
	POINT pos{ 0,0 };
	GetCursorPos(&pos);
	HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfoW(monitor, &monitorInfo);

	//Window
	m_window = CreateWindowExW(
		WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
		(LPCWSTR)m_wndClass, L"D3D12GraphicsStudy",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		monitorInfo.rcWork.left + 100,
		monitorInfo.rcWork.top + 100,
		m_width, m_height,
		nullptr, nullptr, wcex.hInstance, nullptr);

	if (m_window == nullptr)
	{
		return false;
	}

	// Desc swap chain
	DXGI_SWAP_CHAIN_DESC1 swd{};
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC sfd{};

	swd.Width = m_width;
	swd.Height = m_height;
	swd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swd.Stereo = false;
	swd.SampleDesc.Count = 1; // Multi Sample Anti-Aliasing 1 pixel per pixel 
	swd.SampleDesc.Quality = 0; // No Multi Sample Anti-Aliasing
	swd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swd.BufferCount = 2;
	swd.Scaling = DXGI_SCALING_STRETCH;
	swd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	sfd.Windowed = true;

	// SwapChain
	auto& factory = DXContext::Get().GetFactory();
	ComPointer<IDXGISwapChain1> sc1;
	factory->CreateSwapChainForHwnd(DXContext::Get().GetCommandQueue(), m_window, &swd, &sfd, nullptr, &sc1);
	if (!sc1.QueryInterface(m_swapChain))
	{
		return false;
	}

	// Create RTV
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = GetFrameCount();
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DXContext::Get().GetDevide()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

		m_rtvDescriptorSize = DXContext::Get().GetDevide()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT n = 0; n < GetFrameCount(); n++)
		{
			m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			DXContext::Get().GetDevide()->CreateRenderTargetView(m_renderTargets[n], nullptr, rtvHandle);
			rtvHandle.ptr += m_rtvDescriptorSize;
		}
	}

	m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();

	return true;
}

void DXWindow::Update()
{
	MSG msg;
	while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

}

void DXWindow::Present()
{
	m_swapChain->Present(1, 0);
	m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();
}

void DXWindow::Shutdown()
{
	for (auto& rt : m_renderTargets)
	{
		rt.Release();
	}
	m_rtvHeap.Release();
	m_swapChain.Release();

	if (m_window)
	{
		DestroyWindow(m_window);
	}

	if (m_wndClass)
	{
		UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
	}
}

void DXWindow::Resize()
{
	RECT cr;
	if (GetClientRect(m_window, &cr))
	{
		m_width = cr.right - cr.left;
		m_height = cr.bottom - cr.top;

		for (auto& rt : m_renderTargets)
		{
			rt.Release();
		}

		m_swapChain->ResizeBuffers(GetFrameCount(), m_width, m_height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
		
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < GetFrameCount(); n++)
		{
			m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n]));
			DXContext::Get().GetDevide()->CreateRenderTargetView(m_renderTargets[n], nullptr, rtvHandle);
			rtvHandle.ptr += m_rtvDescriptorSize;
		}

		m_currentFrame = m_swapChain->GetCurrentBackBufferIndex();
		m_shouldResize = false;
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE DXWindow::GetCurrentRTVHandle()
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_currentFrame * m_rtvDescriptorSize;
	return handle;
}

ComPointer<ID3D12Resource> DXWindow::GetCurrentRenderTarget()
{
	return m_renderTargets[m_currentFrame].Get();
}

LRESULT DXWindow::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE: 
		{
			if (lParam && (HIWORD(lParam) != Get().m_height || LOWORD(lParam) != Get().m_width))
			{
				Get().m_shouldResize = true;
			}
		}
		break;

		case WM_CLOSE:
		{
			Get().m_shouldClose = true;
			return 0;
		}
	}
	return DefWindowProcW(wnd, msg, wParam, lParam);
}