//#include "Animation.h"
//#include "AnimationSet.h"
//#include "BindPose.h"
#include <fstream>
#include <Windows.h>
#include "ForFBX.h"
#include "ResourceManager.h"

//ResourceManager* ResourceManager::singleton = 0;

ResourceManager::ResourceManager()
{
	//animationSetsTable = HashString::GetSingleton();
	animationSetIndex = 0;
}

ResourceManager::~ResourceManager()
{
	//animationSetsTable->CleanUp();
}

//void ResourceManager::CleanUp()
//{
//	delete singleton;
//	singleton = nullptr;
//}

void ResourceManager::Init(DeviceResources const * devResources)
{
	device = devResources->GetDevice();
	devContext = devResources->GetDeviceContext();

	//load in shaders
	LoadAndCreateShaders();
	
	//load in animation sets
	LoadInAnimationSetsAndMeshes();
}

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

						//initialize animation set
						animationSet.AddAnimation(animation);
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
	std::vector<unsigned int> indices;
	unsigned int numVerts;
	unsigned int numIndices;

	bin.open(path, std::ios::binary);

	if (bin.is_open())
	{
		//Read Header
		bin.read((char*)&numVerts, sizeof(unsigned int));
		bin.read((char*)&numIndices, sizeof(unsigned int));

		//resize based off of header
		verts.resize(numVerts);
		indices.resize(numIndices);

		//read in verts
		bin.read((char*)verts.data(), sizeof(Vertex) * numVerts);

		//read in names
		bin.read((char*)indices.data(), sizeof(unsigned int) * numIndices);

		bin.close();
	}

	vertices.push_back(verts.data());
}

//getters//

AnimationSet* ResourceManager::GetAnimationSet(std::string animation)
{
	return &animationSets[animationSetsTable.GetKey(animation)];
}

ID3D11PixelShader* ResourceManager::GetPixelShader(unsigned int index)
{
	return pixelShaders[index].Get();
}

ID3D11VertexShader* ResourceManager::GetVertexShader(unsigned int index)
{
	return vertexShaders[index].Get();
}

ID3D11ComputeShader* ResourceManager::GetComputeShader(unsigned int index)
{
	return computeShaders[index].Get();
}

//setters//



//private helper functions//

void ResourceManager::LoadAndCreateShaders()
{
	WIN32_FIND_DATA fileData;
	HANDLE hFileFind;

	std::string filePath = debugPath;
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
			std::string filePath = debugPath;
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