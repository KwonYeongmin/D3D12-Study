#pragma once

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>

///  @breif DirectX 12 디버그 레이어는 코드 작성 및 문제 해결을 돕는 레이어
class DXContext
{
	public:
		bool Init();
		void Shutdown();
	
	private:
	#ifdef _DEBUG
		ComPointer<ID3D12Debug6> m_d3d12Debug;
		ComPointer<IDXGIDebug1> m_dxgiDebug;
	#endif

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