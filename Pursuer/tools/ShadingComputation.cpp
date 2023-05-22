#include "ShadingComputation.h"
#include "WrapperComputation.h"

ComPtr<ID3D12RootSignature> ShadingComputation::rootsignature = {};
ComPtr<ID3D12PipelineState> ShadingComputation::pipeline = {};

ID3D12Device* ShadingComputation::device = nullptr;
ID3D12GraphicsCommandList* ShadingComputation::cmdList = nullptr;

ShadingComputation::ShadingComputation(const int numElement) : maxElement(numElement)
{
	CreateInputResource();

	CreateOutputResource();

	CreateDescHeap();
}

void ShadingComputation::StaticInitialize()
{
	device = DirectXCommon::GetInstance()->GetDevice();

	cmdList = WrapperComputation::GetInstance()->GetCmdList();

	CreatePipeline();
}

OutputData* ShadingComputation::GetOutputData()
{
	return (OutputData*)outputData;
}

void ShadingComputation::Update(void* sendInputData, void* sendInputData2, const int element)
{
	if (sendInputData != nullptr)
	{
		// memcpy function is then used to copy the contents of sendInputData to the inputData1 array.
		memcpy(inputData1, sendInputData, sizeof(ParticleParameters) * element);
	}

	if (sendInputData2 != nullptr)
	{
		// memcpy function is used to copy the contents of sendInputData2 to the inputData2 array. 
		memcpy(inputData2, sendInputData2, sizeof(OutputData) * element);
	}

	// Assigns the value of the element variable to the member variable element
	this->element = element;
}

void ShadingComputation::Dispatch()
{
	if (element <= 0)
	{
		return;
	}

	cmdList->SetComputeRootSignature(rootsignature.Get());
	cmdList->SetPipelineState(pipeline.Get());

	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	// Set Descriptor Heap
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE handle;

	handle = descHeap->GetGPUDescriptorHandleForHeapStart();
	cmdList->SetComputeRootDescriptorTable(0, handle);

	handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cmdList->SetComputeRootDescriptorTable(1, handle);

	handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	cmdList->SetComputeRootDescriptorTable(2, handle);

	if (element == 0)
	{
		return;
	}

	// The Dispatch method is called on the cmdList object to execute the compute shader.
	// The parameters provided are element (indicating the number of thread groups to dispatch in the x-axis), 1 (indicating the y-axis thread group count), and 1 (indicating the z-axis thread group count)
	cmdList->Dispatch(element, 1, 1);

	element = 0;
}

void ShadingComputation::CreateInputResource()
{
	HRESULT result;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(ParticleParameters) * maxElement);
	D3D12_RESOURCE_DESC desc2 = CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData) * maxElement);

	// Specifies the type of heap to use for the resources, indicating that they will be used for uploading data from the CPU to the GPU.
	D3D12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// Creates a committed resource in the upload heap with the specified properties (prop), description (desc), initial state (D3D12_RESOURCE_STATE_GENERIC_READ), no optimized clear value (nullptr), and the resulting resource is stored in the inputResource1 pointer
	result = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&inputResource1));
	assert(SUCCEEDED(result));

	// Map the structured buffer to a virtual address space accessible from the CPU
	// Do not do this while this instance is alive to reduce the overhead of mapping and unmapping
	CD3DX12_RANGE readRange(0, 0);
	// Map method is called on inputResource1 to map the resource to a virtual address space accessible from the CPU.
	inputResource1.Get()->Map(0, &readRange, reinterpret_cast<void**>(&inputData1));

	// Resource (inputResource2) is created in a similar manner as inputResource1. The only differences are the use of desc2 for the description and inputResource2 for the resource pointer
	result = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&inputResource2));
	assert(SUCCEEDED(result));

	// Mapping structured buffers to a virtual address space accessible from the CPU
	// To reduce the overhead of mapping and unmapping, this is not done while this instance is alive
	CD3DX12_RANGE readRange(0, 0);
	inputResource2.Get()->Map(0, &readRange, reinterpret_cast<void**>(&inputData2));
}

void ShadingComputation::CreateOutputResource()
{
	HRESULT result;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData) * maxElement);
	// This flag indicates that the resource can be used as an unordered access view, allowing read and write operations by the GPU
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES prop{};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // CPU writes to the resource are written back to GPU memory
	prop.CreationNodeMask = 1; // The node(s) on which the resource is created and is visible
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // Preference for placing the resource in the level-0 memory pool
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;

	// Creates a committed resource in a custom heap with the specified properties (prop), description (desc), initial state (D3D12_RESOURCE_STATE_UNORDERED_ACCESS), no optimized clear value (nullptr), and the resulting resource is stored in the outputResource pointer
	result = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&outputResource));
	assert(SUCCEEDED(result));

	// Mapping structured buffers to a virtual address space accessible from the CPU
	// Not done while this instance is alive to reduce map and unmap overhead
	CD3DX12_RANGE readRange(0, 0);
	outputResource.Get()->Map(0, &readRange, reinterpret_cast<void**>(&outputData));
}

void ShadingComputation::CreateDescHeap()
{
	// Creation of DescriptorHeap
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // Shader visible
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; // Type
	heapDesc.NumDescriptors = 3;

	auto result = device->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(descHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));

	auto cpuHandle = descHeap->GetCPUDescriptorHandleForHeapStart();
	auto gpuHandle = descHeap->GetGPUDescriptorHandleForHeapStart();

	// Creating SRVs
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxElement;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleParameters);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(
		inputResource1.Get(),
		&srvDesc,
		cpuHandle);
	cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Creating SRVs
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxElement;
	srvDesc.Buffer.StructureByteStride = sizeof(OutputData);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	device->CreateShaderResourceView(
		inputResource2.Get(),
		&srvDesc,
		cpuHandle);
	cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Creation of UAVs
	D3D12_UNORDERED_ACCESS_VIEW_DESC heapDesc2 = {};
	heapDesc2.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	heapDesc2.Format = DXGI_FORMAT_UNKNOWN;
	heapDesc2.Buffer.NumElements = maxElement;
	heapDesc2.Buffer.StructureByteStride = sizeof(OutputData);

	device->CreateUnorderedAccessView(
		outputResource.Get(), nullptr,
		&heapDesc2,
		cpuHandle);
	cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ShadingComputation::CreatePipeline()
{
	HRESULT result;

	ComPtr < ID3DBlob> csBlob = nullptr; // Compute shader object
	ComPtr < ID3DBlob> errorBlob = nullptr; // Error object

	// Load and compile pixel shaders
	result = D3DCompileFromFile(
		L"Resources/shaders/ShadingComputation.hlsl",	// Shader file name
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Include-enable
		"CSmain", "cs_5_0",	// Entry point name, shader model designation
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // Setup for debugging
		0,
		&csBlob, &errorBlob);

	if (FAILED(result)) {
		// Copy error content from errorBlob to string type
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// Error contents are displayed in the output window
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
	CD3DX12_ROOT_PARAMETER1 rootParameters[3];

	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
	rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);
	rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);
	rootParameters[2].InitAsDescriptorTable(1, &ranges[2]);

	// Root signature flags are set to deny access to the vertex, hull, domain, geometry, and pixel shaders
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	// Describes the versioned root signature using the initialized root parameters and flags.
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
	assert(SUCCEEDED(result));

	// Root Signature Generation
	result = device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));

	// Pipeline setup for compute shaders
	D3D12_COMPUTE_PIPELINE_STATE_DESC cpipeline{};
	cpipeline.CS = CD3DX12_SHADER_BYTECODE(csBlob.Get());
	cpipeline.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	cpipeline.NodeMask = 0;
	// Set root signature in pipeline
	cpipeline.pRootSignature = rootsignature.Get();

	// device->CreateComputePipelineState method is called to create the compute pipeline state using the cpipeline description. The resulting pipeline state is stored in the pipeline ComPtr
	result = device->CreateComputePipelineState(&cpipeline, IID_PPV_ARGS(&pipeline));
	assert(SUCCEEDED(result));
}