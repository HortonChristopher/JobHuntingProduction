﻿#include"GameFramework.h"

#include <wrl/client.h>
#include <d3d12.h>

// Entry point in Windows app (main function)
GameFramework* GameFramework::gameInstance = nullptr;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _Debug // Preprocessor directive checking if _Debug or not
	ID3D12Debug1* debugController; // Declaring pointer variable to debugging controller for DX12
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) { // IID_PPV_ARGS is used to pass address of debugController pointer to the function
		debugController->EnableDebugLayer(); // Enables Direct3D12 debug layer; layer provides additional runtime checks and validation for Direct3D12 operations
		debugController->SetEnableGPUBasedValidation(TRUE); // Enables GPU-based validation, a feature that performs additional validation checks on the GPU side to catch potential issues in Direct3D 12 operations
	}

	ID3D12InfoQueue* infoQueue; // Queue that stores debug messages and allows control over message filtering and debugging behaviors
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) { // IID_PPV_ARGS is used to pass address of infoQueue pointer to the function
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true); // If a corruption message is encountered during Direct3D12 operations, the program will break at the point where the message is generated
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true); // Instructs the program to break when an error message is encountered during Direct3D12 operations
		infoQueue->Release(); // Releases reference to infoQueu, freeing up system resources
	}
#endif // End of conditional compilation section

	// Returns current time as time_t value, which is used as seed for random number generator ensuring that each program execution starts with a different random seed
	srand(time(nullptr));

	// Declares unique pointer gameData to GameFramework class 
	// unique_ptr provides automatic memory management and ensures that object is properly destroyed when out of scope
	std::unique_ptr<GameFramework> gameData(GameFramework::GetInstance());

	// Initialization
	gameData->Initialize(); 

	// Main game loop is inside this function
	gameData->Run(); 

	// Cleanup
	gameData->End(); 

	// Returns exit code 0 to OS saying that program execution was successful
	return 0; 
}