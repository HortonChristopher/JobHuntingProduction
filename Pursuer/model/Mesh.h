#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <vector>
#include <unordered_map>
#include <d3dcompiler.h>
#include <cassert>

#include "Material.h"
#include "Textures.h"
#include "Object3d.h"
#include "DirectXCommon.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

// Shape data
class Mesh
{
private: // Alias
	// Using Microsoft::WRL::
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// Using DirectX::
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

public: // Subclass
	// Vertex data structure (with texture)
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz coordinates
		XMFLOAT3 normal; // Normal vector
		XMFLOAT2 uv;  // uv coordinates
	};

public: // Member function
	inline const std::string& GetName() { return name; }
	void SetName(const std::string& name) { this->name = name; }
	Material* GetMaterial() { return material; }
	void SetMaterial(Material* material) { this->material = material; }
	void AddVertex(const VertexPosNormalUv& vertex) { vertices.emplace_back(vertex); }
	void AddIndex(unsigned short index) { indices.emplace_back(index); }
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }
	inline size_t GetVertexCount() { return vertices.size(); }
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
	{
		smoothData[indexPosition].emplace_back(indexVertex);
	}

	void CreateBuffers();

	void Draw();

	void CalculateSmoothedVertexNormals();

	// Get vertex array
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }

	// Get index array
	inline const std::vector<unsigned short>& GetIndices() { return indices; }

private: // Member function
	std::string name;
	ComPtr<ID3D12Resource> vertBuff;
	ComPtr<ID3D12Resource> indexBuff;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	std::vector<VertexPosNormalUv> vertices;
	std::vector<unsigned short> indices;
	Material* material = nullptr;
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;
};

