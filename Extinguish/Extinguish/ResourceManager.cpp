//#include "Animation.h"
//#include "AnimationSet.h"
//#include "BindPose.h"
#include "Includes.h"
#include <fstream>
//#include <Windows.h>
#include "ForFBX.h"
#include "ResourceManager.h"
#include "Skeleton.h"
#include "EDMesh.h"

ResourceManager* ResourceManager::singleton = 0;

ResourceManager::ResourceManager()
{
	//animationSetsTable = HashString::GetSingleton();
	animationSetIndex = 0;
}

ResourceManager::~ResourceManager()
{
	//animationSetsTable->CleanUp();


}

//basic//
void ResourceManager::Init(DeviceResources const * devResources)
{

	device = devResources->GetDevice();
	devContext = devResources->GetDeviceContext();
	
	DoFBXExporting();

	//load in shaders
	LoadAndCreateShaders();
	
	//load in animation sets
	LoadInAnimationSetsAndMeshes();

	//load in textures
	LoadInTextures();

	DXGI_SWAP_CHAIN_DESC tempDesc;
	devResources->GetSwapChain()->GetDesc(&tempDesc);
}

void ResourceManager::Shutdown()
{
	delete singleton;
	singleton = nullptr;
}

//private helper functions//
void ResourceManager::LoadInAnimationSetsAndMeshes()
{
	WIN32_FIND_DATA fileData, folderData;
	Skeleton skeleton;
	Animation animation;
	HANDLE hFolderFind = 0, hFileFind = 0;
	std::string resourcesFolderPath;

	resourcesFolderPath = resourcesPath;
	resourcesFolderPath += "*";

	hFolderFind = ::FindFirstFile(resourcesFolderPath.c_str(), &folderData);

	if (hFolderFind != INVALID_HANDLE_VALUE)
	{
		do // for every folder in resources folder
		{
			//every folder is one animation set
			AnimationSet animationSet;

			if (folderData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::string filePath = resourcesPath;
				filePath += folderData.cFileName;
				filePath += "/*.skel";

				//load in the  one skeleton
				hFileFind = ::FindFirstFile(filePath.c_str(), &fileData);

				if (hFileFind != INVALID_HANDLE_VALUE)
				{
						//set file path to the skel file
						filePath = resourcesPath;
						filePath += folderData.cFileName;
						filePath += "/";
						filePath += fileData.cFileName;

						skeleton = LoadInSkeleton(filePath);
						animationSet.SetSkeleton(skeleton);
				}

				//load in mesh
				filePath = resourcesPath;
				filePath += folderData.cFileName;
				filePath += "/*.mesh";

				hFileFind = ::FindFirstFile(filePath.c_str(), &fileData);

				if (hFileFind != INVALID_HANDLE_VALUE)
				{
					//set file path to the mesh file
					filePath = resourcesPath;
					filePath += folderData.cFileName;
					filePath += "/";
					filePath += fileData.cFileName;
					
					LoadInMesh(filePath);

					vertexBuffersTable.Insert(folderData.cFileName);
					indexBuffersTable.Insert(folderData.cFileName);
				}

				//load in basic mesh
				filePath = resourcesPath;
				filePath += folderData.cFileName;
				filePath += "/*.bmesh";

				hFileFind = ::FindFirstFile(filePath.c_str(), &fileData);

				if (hFileFind != INVALID_HANDLE_VALUE)
				{
					//set file path to the mesh file
					filePath = resourcesPath;
					filePath += folderData.cFileName;
					filePath += "/";
					filePath += fileData.cFileName;

					string file = fileData.cFileName;
					size_t found = 0;
					found = file.find("COLLMESH");
					if (found != file.npos)
					{
						ED2Mesh* mesh = new ED2Mesh;
						LoadInBasicMesh(filePath, mesh);
						collisionMeshes.push_back(mesh);
					}
					else
					{
						LoadInBasicMesh(filePath, nullptr);
						vertexBuffersTable.Insert(folderData.cFileName);
						indexBuffersTable.Insert(folderData.cFileName);
					}
				}

				//load in every animation
				filePath = resourcesPath;
				filePath += folderData.cFileName;
				filePath += "/*.anim";

				hFileFind = ::FindFirstFile(filePath.c_str(), &fileData);

				if (hFileFind != INVALID_HANDLE_VALUE)
				{
					do //for every file in folder
					{
						//set file path to the anim file
						filePath = resourcesPath;
						filePath += folderData.cFileName;
						filePath += "/";
						filePath += fileData.cFileName;

						//Load in multiple animations
						animation = LoadInAnimation(filePath);

						//get animation name by itself
						string animName = fileData.cFileName;

						animName.erase(0, animName.find('_') + 1);
						animName.erase(animName.find(".anim"), 5);

						//give animation name
						animation.SetAnimationName(animName);

						//initialize animation set
						animationSet.AddAnimation(animation, animName);
					} while (::FindNextFile(hFileFind, &fileData));

					FindClose(hFileFind);
				}
			}

			if (hFileFind != INVALID_HANDLE_VALUE) //only add animation set if a file was loaded in
			{
				//add set to animation set and increment inedx
				animationSets[animationSetIndex++] = animationSet;

				//add to hash string
				//size_t length = WideCharToMultiByte(CP_ACP, 0, &folderData.cFileName[0], -1, NULL, 0, NULL, NULL);
				//std::string hashValue(length, 0);
				//WideCharToMultiByte(CP_UTF8, 0, &folderData.cFileName[0], 260, &hashValue[0], (int)length, NULL, NULL);

				animationSetsTable.Insert(folderData.cFileName);
			}

		} while (::FindNextFile(hFolderFind, &folderData));

		FindClose(hFolderFind);
	}
}

void ResourceManager::LoadInMesh(std::string path)
{
	std::ifstream bin;
	std::vector<Vertex> verts;
	std::vector<unsigned int> tempIndices;
	unsigned int numVerts;
	unsigned int tempNumIndices;
	unsigned int sizeOfVertex;

	bin.open(path, std::ios::binary);

	if (bin.is_open())
	{
		//Read Header
		bin.read((char*)&numVerts, sizeof(unsigned int));
		bin.read((char*)&tempNumIndices, sizeof(unsigned int));
		bin.read((char*)&sizeOfVertex, sizeof(unsigned int));

		//resize based off of header
		verts.resize(numVerts);
		tempIndices.resize(tempNumIndices);

		//read in verts
		bin.read((char*)verts.data(), sizeOfVertex * numVerts);

		//read in names
		bin.read((char*)tempIndices.data(), sizeof(unsigned int) * tempNumIndices);

		bin.close();
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	//create index buffer
	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = tempIndices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * (unsigned int)tempIndices.size(), D3D11_BIND_INDEX_BUFFER);
	device->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());

	indexBuffers.push_back(indexBuffer);

	//create vertex buffer
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	vertexBufferData.pSysMem = verts.data();

	CD3D11_BUFFER_DESC vertexBufferDesc(sizeOfVertex * (unsigned int)verts.size(), D3D11_BIND_VERTEX_BUFFER);
	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());

	vertexBuffers.push_back(vertexBuffer);

	//push back vertex stride
	vertexStrides.push_back(sizeof(Vertex));

	//push back num of vertices
	numVertices.push_back(numVerts);

	//push back num of indices
	numIndices.push_back(tempNumIndices);
}

void ResourceManager::LoadInBasicMesh(std::string path, ED2Mesh* collmesh)
{
	std::ifstream bin;
	std::vector<VS_BasicInput> verts;
	std::vector<unsigned int> tempIndices;
	unsigned int numVerts;
	unsigned int tempNumIndices;
	unsigned int sizeOfVertex;

	bin.open(path, std::ios::binary);

	if (bin.is_open())
	{
		//Read Header
		bin.read((char*)&numVerts, sizeof(unsigned int));
		bin.read((char*)&tempNumIndices, sizeof(unsigned int));
		bin.read((char*)&sizeOfVertex, sizeof(unsigned int));

		//resize based off of header
		verts.resize(numVerts);
		tempIndices.resize(tempNumIndices);

		//read in verts
		bin.read((char*)verts.data(), sizeOfVertex * numVerts);

		//read in names
		bin.read((char*)tempIndices.data(), sizeof(unsigned int) * tempNumIndices);

		bin.close();
	}
	if (collmesh)
	{
		ED2Mesh::DoEDMesh(verts, tempIndices, collmesh);
		return;
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	//create index buffer
	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = tempIndices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * (unsigned int)tempIndices.size(), D3D11_BIND_INDEX_BUFFER);
	device->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());

	indexBuffers.push_back(indexBuffer);

	//create vertex buffer
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	vertexBufferData.pSysMem = verts.data();

	CD3D11_BUFFER_DESC vertexBufferDesc(sizeOfVertex * (unsigned int)verts.size(), D3D11_BIND_VERTEX_BUFFER);
	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());

	vertexBuffers.push_back(vertexBuffer);

	//push back vertex stride
	vertexStrides.push_back(sizeof(VS_BasicInput));

	//push back num of vertices
	numVertices.push_back(numVerts);

	//push back num of indices
	numIndices.push_back(tempNumIndices);
}

//getters//

ResourceManager* ResourceManager::GetSingleton()
{
	if (!singleton)
	{
		singleton = new ResourceManager();
	}

	return singleton;
}

ID3D11Buffer* ResourceManager::GetVertexBuffer(std::string name)
{
	ID3D11Buffer* result = nullptr;
	
	int index = vertexBuffersTable.GetKey(name);

	if (index != -1)
	{
		result = vertexBuffers[index].Get();
	}

	return result;
}

ID3D11Buffer* ResourceManager::GetIndexBuffer(std::string name)
{
	ID3D11Buffer* result = nullptr;

	int index = indexBuffersTable.GetKey(name);

	if (index != -1)
	{
		result = indexBuffers[index].Get();
	}

	return result;
}

ID3D11InputLayout* ResourceManager::GetInputLayout(std::string name)
{
	ID3D11InputLayout* result = nullptr;

	int index = inputLayoutsTable.GetKey(name);

	if (index != -1)
	{
		result = inputLayouts[index].Get();
	}

	return result;
}

AnimationSet* ResourceManager::GetAnimationSet(std::string animation)
{
	AnimationSet* result = nullptr;

	int index = animationSetsTable.GetKey(animation);

	if (index != -1)
	{
		result = &animationSets[index];
	}

	return result;
}

ID3D11PixelShader* ResourceManager::GetPixelShader(std::string name)
{
	ID3D11PixelShader* result = nullptr;

	int index = pixelShadersTable.GetKey(name);

	if (index != -1)
	{
		result = pixelShaders[index].Get();
	}

	return result;
}

ID3D11VertexShader* ResourceManager::GetVertexShader(std::string name)
{
	ID3D11VertexShader* result = nullptr;

	int index = vertexShadersTable.GetKey(name);

	if (index != -1)
	{
		result = vertexShaders[index].Get();
	}

	return result;
}

ID3D11ComputeShader* ResourceManager::GetComputeShader(std::string name)
{
	ID3D11ComputeShader* result = nullptr;

	int index = computeShadersTable.GetKey(name);

	if (index != -1)
	{
		result = computeShaders[index].Get();
	}

	return result;
}

ID3D11ShaderResourceView* ResourceManager::GetShaderResourceView(std::string name)
{
	ID3D11ShaderResourceView* result = nullptr;

	int index = shaderResourceViewsTable.GetKey(name);

	if (index != -1)
	{
		result = shaderResourceViews[index].Get();
	}

	return result;
}

int ResourceManager::GetVertexStride(std::string name)
{
	int result = -1;

	int index = vertexBuffersTable.GetKey(name);

	if (index != -1)
	{
		result = vertexStrides[index];
	}

	return result;
}

int ResourceManager::GetNumIndices(std::string name)
{
	int result = -1;

	int index = vertexBuffersTable.GetKey(name);

	if (index != -1)
	{
		result = numIndices[index];
	}

	return result;
}

int ResourceManager::GetNumVertices(std::string name)
{
	int result = -1;

	int index = vertexBuffersTable.GetKey(name);

	if (index != -1)
	{
		result = numVertices[index];
	}

	return result;
}
//setters//



//private helper functions//

void ResourceManager::LoadAndCreateShaders()
{
	WIN32_FIND_DATA fileData;
	HANDLE hFileFind;

	std::string filePath;
#if _DEBUG
	filePath = debugPath;
#else
		filePath = releasePath;
#endif
	filePath += "*.cso";

	//find the first shader
	hFileFind = ::FindFirstFile(filePath.c_str(), &fileData);

	//if file location isn't valid, do  
	if (hFileFind != INVALID_HANDLE_VALUE)
	{
		do //for every other .cso
		{
			FILE* file = nullptr;
			std::vector<uint8_t> shaderData;

			//get string fileName from wchar cFileName
			//size_t length = WideCharToMultiByte(CP_ACP, 0, &fileData.cFileName[0], -1, NULL, 0, NULL, NULL);
			//std::string curFileName(length, 0);
			//WideCharToMultiByte(CP_UTF8, 0, &fileData.cFileName[0], 260, &curFileName[0], (int)length, NULL, NULL);
			std::string curFileName = fileData.cFileName;

			//make full file path
			std::string filePath;
			#if _DEBUG
						filePath = debugPath;
			#else
						filePath = releasePath;
			#endif
			filePath += curFileName; //this already includes extension

			//open shader
			fopen_s(&file, filePath.c_str(), "rb"); //rb = read binary

			//if shader was successfully opened
			if (file)
			{
				long fileSize;

				//go to end of file and then use this to get size of file
				fseek(file, 0, SEEK_END);
				fileSize = ftell(file);
				shaderData.resize(fileSize);

				//set to begining of file and read in the whole shader in one read
				fseek(file, 0, SEEK_SET);
				fread(&shaderData[0], sizeof(uint8_t), fileSize, file);

				//close the shader
				fclose(file);

				//remove .cso from fileName for hash string table
				curFileName.erase(curFileName.find(".cso"), 4);

				//create a shader depending on type
				if (curFileName.find("VS") != -1)
				{
					Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
					HRESULT vsResult = device->CreateVertexShader(&shaderData[0], shaderData.size(), NULL, vs.GetAddressOf());
					vertexShaders.push_back(vs);

					//remove prefix and then insert
					curFileName.erase(0, 3);
					vertexShadersTable.Insert(curFileName);

					//create input layout for this vertex
					if (inputLayoutsTable.GetKey("Bind") == -1 && curFileName == "Bind")
					{
						Microsoft::WRL::ComPtr<ID3D11InputLayout> bindInput;

						D3D11_INPUT_ELEMENT_DESC bindInputElementDescs[] =
						{
							{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						};

						HRESULT bindInputResult = device->CreateInputLayout(bindInputElementDescs, ARRAYSIZE(bindInputElementDescs), &shaderData[0], shaderData.size(), bindInput.GetAddressOf());

						inputLayouts.push_back(bindInput);
						inputLayoutsTable.Insert(curFileName);
					}
					else if (inputLayoutsTable.GetKey("Trail") == -1 && curFileName == "Trail")
					{
						Microsoft::WRL::ComPtr<ID3D11InputLayout> staticInput;

						D3D11_INPUT_ELEMENT_DESC staticInputElementDescs[] =
						{
							{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						};

						HRESULT inputResult = device->CreateInputLayout(staticInputElementDescs, ARRAYSIZE(staticInputElementDescs), &shaderData[0], shaderData.size(), staticInput.GetAddressOf());

						inputLayouts.push_back(staticInput);
						inputLayoutsTable.Insert(curFileName);
					}
					else if (inputLayoutsTable.GetKey("Static") == -1 && curFileName == "Static")
					{
						Microsoft::WRL::ComPtr<ID3D11InputLayout> staticInput;

						D3D11_INPUT_ELEMENT_DESC staticInputElementDescs[] =
						{
							{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						};

						HRESULT inputResult = device->CreateInputLayout(staticInputElementDescs, ARRAYSIZE(staticInputElementDescs), &shaderData[0], shaderData.size(), staticInput.GetAddressOf());

						inputLayouts.push_back(staticInput);
						inputLayoutsTable.Insert(curFileName);
					}
					else if (inputLayoutsTable.GetKey("TempStatic") == -1 && curFileName == "TempStatic")
					{
						Microsoft::WRL::ComPtr<ID3D11InputLayout> staticInput;

						D3D11_INPUT_ELEMENT_DESC staticInputElementDescs[] =
						{
							{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
						};

						HRESULT inputResult = device->CreateInputLayout(staticInputElementDescs, ARRAYSIZE(staticInputElementDescs), &shaderData[0], shaderData.size(), staticInput.GetAddressOf());

						inputLayouts.push_back(staticInput);
						inputLayoutsTable.Insert(curFileName);
					}
					else if (inputLayoutsTable.GetKey("InstancedStatic") == -1 && curFileName == "InstancedStatic")
					{
						Microsoft::WRL::ComPtr<ID3D11InputLayout> InststaticInput;

						D3D11_INPUT_ELEMENT_DESC staticInputElementDescs[] =
						{
							{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
							{ "INSTANCECOLOR", 0, DXGI_FORMAT_R32_UINT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },//set last value to 2 for half the cost but half the accuracy
						};

						HRESULT inputResult = device->CreateInputLayout(staticInputElementDescs, ARRAYSIZE(staticInputElementDescs), &shaderData[0], shaderData.size(), InststaticInput.GetAddressOf());

						inputLayouts.push_back(InststaticInput);
						inputLayoutsTable.Insert(curFileName);
					}
				}
				else if (curFileName.find("PS") != -1)
				{
					Microsoft::WRL::ComPtr<ID3D11PixelShader> ps;
					HRESULT psResult = device->CreatePixelShader(&shaderData[0], shaderData.size(), NULL, ps.GetAddressOf());
					pixelShaders.push_back(ps);

					//remove prefix and then insert
					curFileName.erase(0, 3);
					pixelShadersTable.Insert(curFileName);
				}
				else if (curFileName.find("CS") != -1)
				{
					Microsoft::WRL::ComPtr<ID3D11ComputeShader> cs;
					HRESULT csResult = device->CreateComputeShader(&shaderData[0], shaderData.size(), NULL, cs.GetAddressOf());
					computeShaders.push_back(cs);
				
					//remove prefix and then insert
					curFileName.erase(0, 3);
					computeShadersTable.Insert(curFileName);
				}
				else if (curFileName.find("GS") != -1)
				{
					HRESULT csResult = device->CreateGeometryShader(&shaderData[0], shaderData.size(), NULL, &geometryShader);
				}
			}
		} while (::FindNextFile(hFileFind, &fileData));

		FindClose(hFileFind);
	}
}

Skeleton ResourceManager::LoadInSkeleton(std::string path)
{
	std::ifstream bin;
	FriendlyIOSkeleton skeleton;
	unsigned int sizeOfNames;
	unsigned int numOfBones;
	//unsigned int nameSize;

	bin.open(path, std::ios::binary);

	if (bin.is_open())
	{
		//Read Header
		bin.read((char*)&numOfBones, sizeof(unsigned int));
		bin.read((char*)&sizeOfNames, sizeof(unsigned int));

		//resize based off of header
		skeleton.transforms.resize(numOfBones);
		skeleton.inverseBindPoses.resize(numOfBones);
		skeleton.names.resize(sizeOfNames);
	
		//read in skeleton bones
		bin.read((char*)skeleton.transforms.data(), sizeof(FriendlyIOTransformNode) * numOfBones);

		//read in skeleton names
		bin.read((char*)skeleton.names.data(), sizeOfNames);

		//read in inverse bind poses
		bin.read((char*)skeleton.inverseBindPoses.data(), sizeof(DirectX::XMFLOAT4X4) * numOfBones);

		bin.close();
	}

	Skeleton nonFriendlySkeleton;

	nonFriendlySkeleton.Init(skeleton.transforms, skeleton.names, skeleton.inverseBindPoses);

	return nonFriendlySkeleton;
}

Animation ResourceManager::LoadInAnimation(std::string path)
{
	std::ifstream bin;
	Animation animation;
	std::vector<KeyFrame> keyFrames;
	std::vector<Bone> bones;
	AnimType animType;
	float time;
	unsigned int numOfKeyFrames;
	unsigned int numOfBones;

	bin.open(path, std::ios::binary);

	if (bin.is_open())
	{
		//read header
		bin.read((char*)&numOfKeyFrames, sizeof(unsigned int));
		bin.read((char*)&numOfBones, sizeof(unsigned int));

		//read keyframes
		keyFrames.resize(numOfKeyFrames);
		bones.resize(numOfBones);

		for (unsigned int i = 0; i < numOfKeyFrames; ++i)
		{
			float keyFrameTime;
			float tweenTime;

			bin.read((char*)bones.data(), sizeof(Bone) * numOfBones);
			bin.read((char*)&keyFrameTime, sizeof(float));
			bin.read((char*)&tweenTime, sizeof(float));

			keyFrames[i].SetTime(keyFrameTime);
			keyFrames[i].SetTweenTime(tweenTime);
			keyFrames[i].SetBones(bones);
		}

		//write out animtype
		bin.read((char*)&animType, sizeof(AnimType));

		//read in time
		bin.read((char*)&time, sizeof(float));

		animation.Init(animType, time, keyFrames);

		//close fstream
		bin.close();
	}

	return animation;
}

void ResourceManager::DoFBXExporting()
{
#if 1
	// load in box animations and rig
	//FBXLoader::Functions::FBXLoadExportFileBind("..\\Assets\\Box\\Box_Idle.fbx", "Box", "Box_Idle");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Box\\Box_Attack.fbx", "Box", "Box_Attack");

	//////load in teddy animation and rig
	//FBXLoader::Functions::FBXLoadExportFileBind("..\\Assets\\Teddy\\Teddy_Idle.fbx", "Teddy", "Teddy_Idle");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Teddy\\Teddy_Attack1.fbx", "Teddy", "Teddy_Attack1");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Teddy\\Teddy_Attack2.fbx", "Teddy", "Teddy_Attack2");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Teddy\\Teddy_Run.fbx", "Teddy", "Teddy_Run");

	//////load in sphere
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Sphere.fbx", "Sphere");

	//////load in mage with rig and animation
	//FBXLoader::Functions::FBXLoadExportFileBind("..\\Assets\\Mage\\Idle.fbx", "Mage", "Mage_Idle");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Mage\\Run.fbx", "Mage", "Mage_Run");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Mage\\Walk.fbx", "Mage", "Mage_Walk");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Mage\\StumbleBackwards.fbx", "Mage", "Mage_StumbleBackwards");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Mage\\StumbleBackwards2.fbx", "Mage", "Mage_StumbleBackwards2");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Mage\\StumbleBackwards3.fbx", "Mage", "Mage_StumbleBackwards3");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Mage\\Jump.fbx", "Mage", "Mage_Jump");
	//FBXLoader::Functions::FBXLoadExportAnimation("..\\Assets\\Mage\\Land.fbx", "Mage", "Mage_Land");

	//load in plane
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Plane\\Plane.fbx", "Plane");

	//load in crosse
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Crosse\\Crosse.fbx", "Crosse");

	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\MeterBox\\MeterBox.fbx", "MeterBox");

	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Ball\\Ball.fbx", "Ball");

	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Hexagon_COLLMESH\\Hexagon_COLLMESH.fbx", "Hexagon_COLLMESH");

	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Axis\\Axis.fbx", "Axis");

	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Goal\\Goal.fbx", "Goal");

	//powerups
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Super Jump\\Super Jump.fbx", "Super Jump");
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Magnet\\Magnet.fbx", "Magnet");
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Shield\\Shield.fbx", "Shield");
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\ShieldBubble\\ShieldBubble.fbx", "ShieldBubble");

	//test models
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\TestPlayer\\TestPlayer.fbx", "TestPlayer");
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\HighDetalBall\\HighDetalBall.fbx", "HighDetalBall");
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\WallGoal\\WallGoal.fbx", "WallGoal");
	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\Titan\\Titan.fbx", "Titan");
	//FBXLoader::Functions::FBXLoadExportFileBind("..\\Assets\\Titan\\TestAnim.fbx", "Titan", "Titan_Test");

	//FBXLoader::Functions::FBXLoadExportFileBasic("..\\Assets\\PowerUp\\Icon test.fbx", "PowerUp");

	//FBXLoader::Functions::FBXLoadExportMesh("..\\Assets\\ArenaGoal\\ArenaGoal.fbx", "ArenaGoal");
	//FBXLoader::Functions::FBXLoadExportMesh("..\\Assets\\ArenaGoalWall\\ArenaGoalWall.fbx", "ArenaGoalWall");
	//FBXLoader::Functions::FBXLoadExportMesh("..\\Assets\\ArenaWall\\ArenaWall.fbx", "ArenaWall");
	//FBXLoader::Functions::FBXLoadExportMesh("..\\Assets\\ArenaPillar\\ArenaPillar.fbx", "ArenaPillar");

#endif
}

void ResourceManager::CreateInputLayouts()
{
	//Microsoft::WRL::ComPtr<ID3D11InputLayout> basicInput;

	//D3D11_INPUT_ELEMENT_DESC basicInputElementDescs[] =
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//};

	//HRESULT inputResult = device->CreateInputLayout(basicInputElementDescs, ARRAYSIZE(basicInputElementDescs), basicVSBuffer->GetBufferPointer(), basicVSBuffer->GetBufferSize(), basicInput.GetAddressOf());


	//inputLayouts.push_back(basicInput);
}

void ResourceManager::LoadInTextures()
{
	WIN32_FIND_DATA fileData;
	HANDLE hFileFind;

	std::string filePath = ddsPath;
	filePath += "*.dds";

	//find the first shader
	hFileFind = ::FindFirstFile(filePath.c_str(), &fileData);

	//if file location isn't valid, do  
	if (hFileFind != INVALID_HANDLE_VALUE)
	{
		do //for every other .dds
		{
			std::string fileName = ddsPath;
			fileName += fileData.cFileName;

			//if (fileName.find("NM") == std::string::npos || fileName.find("Spec") == std::string::npos)
			//{

			//}

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

			wstring wideNormalPath = wstring(fileName.begin(), fileName.end());
			HRESULT baseNormalResult = CreateDDSTextureFromFile(device, wideNormalPath.c_str(), nullptr, srv.GetAddressOf());

			fileName = fileData.cFileName;
			fileName.erase(fileName.find(".dds"), 4);

			shaderResourceViews.push_back(srv);
			shaderResourceViewsTable.Insert(fileName);

		} while (::FindNextFile(hFileFind, &fileData));

		FindClose(hFileFind);
	}
}
