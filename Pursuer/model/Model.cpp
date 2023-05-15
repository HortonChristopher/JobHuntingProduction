#include "Model.h"

// The entity of a static member variable
const std::string Model::baseDirectory = "Resources/3DObj/";

Model* Model::CreateModel(const std::string& name, bool smooth)
{
	Model* instance = new Model;

	instance->Initialize(name, smooth);

	return instance;
}

Model::~Model()
{
	for (auto m : meshes)
	{
		delete m;
	}
	meshes.clear();

	for (auto m : materials)
	{
		delete m.second;
	}
	materials.clear();
}

void Model::Initialize(const std::string& name, bool smooth)
{
	std::ifstream file;

	file.open(baseDirectory + name);

	if (file.fail())
	{
		assert(0);
	}

	this->name = name;

	Mesh* mesh = new Mesh;
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	vector<XMFLOAT3> positions; // Vertex coordinates
	vector<XMFLOAT3> normals; // Normal vector
	vector<XMFLOAT2> texcoords; // Texture UV

	// Read one line at a time
	string line;

	while (getline(file, line))
	{
		// Convert a single line of string into a stream for easier parsing
		std::istringstream line_stream(line);

		// Get the first character of a line separated by a single space
		string key;

		getline(line_stream, key, ' ');

		// Material
		if (key == "mtllib")
		{
			string filename;

			line_stream >> filename;

			LoadMaterial(filename);
		}

		// If the first string is g, start group
		if (key == "g")
		{
			if (mesh->GetName().size() > 0)
			{
				// Smoothing
				if (smooth)
				{
					mesh->CalculateSmoothedVertexNormals();
				}

				meshes.emplace_back(mesh);

				mesh = new Mesh;

				indexCountTex = 0;
			}

			string groupName;

			line_stream >> groupName;

			// Set name to mesh
			mesh->SetName(groupName);
		}

		// If the first string is v, vertex coordinates
		if (key == "v")
		{
			XMFLOAT3 position{};

			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			positions.emplace_back(position);
		}

		// If the first string is vt, then texture
		if (key == "vt")
		{
			XMFLOAT2 texcoord{};

			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			// V-direction reversal
			texcoord.y = 1.0f - texcoord.y;

			// Add to texture coordinate data
			texcoords.emplace_back(texcoord);
		}

		// Normal vector if the first string is vn
		if (key == "vn")
		{
			XMFLOAT3 normal{};

			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			normals.emplace_back(normal);
		}

		// Assign a material if the first string is usemtl
		if (key == "usemtl")
		{
			if (mesh->GetMaterial() == nullptr)
			{
				// Materials
				string materialName;

				line_stream >> materialName;

				auto itr = materials.find(materialName);

				if (itr != materials.end())
				{
					mesh->SetMaterial(itr->second);
				}
			}
		}

		// Polygon (triangle) if the first string is f
		if (key == "f")
		{
			int faceIndexCount = 0;

			// Read the rest of the line, separated by a single space.
			string index_string;

			while (getline(line_stream, index_string, ' '))
			{
				// Convert a string of one vertex index into a stream for easier parsing
				std::istringstream index_stream(index_string);

				unsigned short indexPosition, indexNormal, indexTexcoord;

				// Vertex number
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();

				index_stream.seekg(1, ios_base::cur);

				if (material && material->textureFileName.size() > 0)
				{
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);
					index_stream >> indexNormal;

					// Adding vertex data
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];

					mesh->AddVertex(vertex);

					// Add data for edge smoothing
					if (smooth)
					{
						// Register a set of v-key (coordinate data) numbers and indexes of all composited vertices
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
				}
				else
				{
					char c;

					index_stream >> c;

					// In the case of two consecutive slashes, only the vertex number
					if (c == '/')
					{
						// Adding vertex data
						Mesh::VertexPosNormalUv vertex{};

						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0,0,1 };
						vertex.uv = { 0,0 };

						mesh->AddVertex(vertex);

						// Add data for edge smoothing
						if (smooth)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
					else
					{
						index_stream.seekg(-1, ios_base::cur);
						index_stream >> indexTexcoord;

						index_stream.seekg(1, ios_base::cur);
						index_stream >> indexNormal;

						// Adding vertex data
						Mesh::VertexPosNormalUv vertex{};

						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0,0 };

						mesh->AddVertex(vertex);

						// Add data for edge smoothing
						if (smooth)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
				}

				if (faceIndexCount >= 3)
				{
					mesh->AddIndex(indexCountTex - 1);
					mesh->AddIndex(indexCountTex);
					mesh->AddIndex(indexCountTex - 3);
				}
				else
				{
					mesh->AddIndex(indexCountTex);
				}

				indexCountTex++;
				faceIndexCount++;
			}
		}
	}

	file.close();

	if (smooth)
	{
		mesh->CalculateSmoothedVertexNormals();
	}

	// Register for Container
	meshes.emplace_back(mesh);

	// Mesh material check
	for (auto& m : meshes)
	{
		if (m->GetMaterial() == nullptr)
		{
			if (defaultMaterial == nullptr)
			{
				// Generate default material
				defaultMaterial = Material::Create();
				defaultMaterial->name = "no material";
				materials.emplace(defaultMaterial->name, defaultMaterial);
			}

			// Set default material
			m->SetMaterial(defaultMaterial);
		}
	}

	// Mesh buffer generation
	for (auto& m : meshes)
	{
		m->CreateBuffers();
	}

	// Material values are reflected in the constant buffer
	for (auto& m : materials)
	{
		m.second->Update();
	}

	// Descriptor Heap Generation
	CreateDescriptorHeap();

	// Loading textures
	LoadTextures();
}

void Model::Draw()
{
	for (auto& mesh : meshes)
	{
		mesh->Draw();
	}
}

void Model::LoadMaterial(const std::string& name)
{
	// Filestream
	std::ifstream file;

	// Open material file
	file.open(baseDirectory + name);

	// Check for file open failures
	if (file.fail())
	{
		assert(0);
	}

	Material* material = nullptr;

	// Read one line at a time
	string line;

	while (getline(file, line))
	{
		std::istringstream line_stream(line);

		string key;

		getline(line_stream, key, ' ');

		// Ignore leading tab characters.
		if (key[0] == '\t')
		{
			key.erase(key.begin());
		}

		if (key == "newmtl")
		{
			if (material)
			{
				AddMaterial(material);
			}

			// Generate new material
			material = Material::Create();

			// Material name loading
			line_stream >> material->name;
		}

		// Ambient color if the first string is Ka
		if (key == "Ka") {
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;
		}
		// Diffuse color if first string is Kd
		if (key == "Kd") {
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;
		}

		// Specular color if the first string is Ks
		if (key == "Ks") {
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;
		}

		// Texture file name if the first string is map_Kd
		if (key == "map_Kd") {
			// Read texture file name
			std::vector<std::string> vstr;

			while (line_stream >> material->textureFileName)
			{
				vstr.push_back(material->textureFileName);
			}

			if (vstr.size())
			{
				material->textureFileName = vstr[vstr.size() - 1];
			}

			// Extract the file name from the full path
			size_t pos1;

			pos1 = material->textureFileName.rfind('\\');

			if (pos1 != string::npos) {
				material->textureFileName = material->textureFileName.substr(pos1 + 1, material->textureFileName.size() - pos1 - 1);
			}

			pos1 = material->textureFileName.rfind('/');

			if (pos1 != string::npos) {
				material->textureFileName = material->textureFileName.substr(pos1 + 1, material->textureFileName.size() - pos1 - 1);
			}
		}
	}

	// Close a file
	file.close();

	if (material) {
		// Register Material
		AddMaterial(material);
	}
}

void Model::AddMaterial(Material* material)
{
	materials.emplace(material->name, material);
}

void Model::CreateDescriptorHeap()
{
	HRESULT result = S_FALSE;

	// Number of materials
	size_t count = materials.size();

	// Generate descriptor heap	
	if (count > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};

		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
		descHeapDesc.NumDescriptors = (UINT)count + 1; // シェーダーリソースビューの数

		result = DirectXCommon::GetInstance()->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap)); // Generation

		if (FAILED(result)) {
			assert(0);
		}
	}
}

void Model::LoadTextures()
{
	for (auto& m : materials)
	{
		Material* material = m.second;
		material->LoadTexture();
	}
}