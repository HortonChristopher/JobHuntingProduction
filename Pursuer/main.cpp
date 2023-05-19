#include"GameFramework.h"

#include <wrl/client.h>
#include <d3d12.h>

// Entry point in Windows app (main function)
GameFramework* GameFramework::gameInstance = nullptr;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _Debug // Preprocessor directive checking if _Debug or not
	ID3D12Debug1* debugController; // Declaring pointer variable to debugging controller for DX12
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}

	ID3D12InfoQueue* infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->Release();
	}
#endif

	srand(time(nullptr));

	std::unique_ptr<GameFramework> gameData(GameFramework::GetInstance());

	gameData->Initialize();

	gameData->Run();

	gameData->End();

	return 0;
}