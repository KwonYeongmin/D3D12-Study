#include <iostream>

#include <Support/WinInclude.h>
#include <Support/ComPointer.h>
#include <Support/Window.h>

#include <Debug/DXDebugLayer.h>
#include <D3D/DXContext.h>


int main()
{
	DXDebugLayer::Get().Init();

	bool temp1 = DXContext::Get().Init();
	bool temp2 = DXWindow::Get().Init();
	if (temp1 && temp2)
	{
		while (DXWindow::Get().ShouldClose() == false)
		{
			// process pending window message
			DXWindow::Get().Update();

			// handle resizing
			if (DXWindow::Get().ShouldResize())
			{
				DXContext::Get().Flush(DXWindow::GetFrameCount());
				DXWindow::Get().Resize();
			}
			auto* cmdList = DXContext::Get().InitCommandList();

			// Indicate that the back buffer will be used as a render target.
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = DXWindow::Get().GetCurrentRenderTarget();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			cmdList->ResourceBarrier(1, &barrier);

			D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(DXWindow::Get().GetWidth()), static_cast<float>(DXWindow::Get().GetHeight()), 0.0f, 1.0f };
			D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(DXWindow::Get().GetWidth()), static_cast<LONG>(DXWindow::Get().GetHeight()) };
			cmdList->RSSetViewports(1, &viewport);
			cmdList->RSSetScissorRects(1, &scissorRect);

			cmdList->SetGraphicsRootSignature(DXContext::Get().m_rootSignature.Get());
			cmdList->SetPipelineState(DXContext::Get().m_pipelineState.Get());

			// Record drawing commands.
			const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
			auto rtvHandle = DXWindow::Get().GetCurrentRTVHandle();
			cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
			cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmdList->IASetVertexBuffers(0, 1, &DXContext::Get().m_vertexBufferView);
			cmdList->DrawInstanced(3, 1, 0, 0);

			// Indicate that the back buffer will now be used to present.
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			cmdList->ResourceBarrier(1, &barrier);

			DXContext::Get().ExecuteCommandList();
			// Show me the stuff
			DXWindow::Get().Present();
		}

		// Flushing
		DXContext::Get().Flush(DXWindow::GetFrameCount());

		DXWindow::Get().Shutdown();
		DXContext::Get().Shutdown();
	}

	DXDebugLayer::Get().Shutdown();
}