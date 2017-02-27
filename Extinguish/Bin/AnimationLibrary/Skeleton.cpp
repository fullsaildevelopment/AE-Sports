#include "Skeleton.h"

using namespace std;

void Skeleton::Init(std::vector<FriendlyIOTransformNode> tempBones, std::string boneNames, std::vector<DirectX::XMFLOAT4X4> invBindPoses)
{
	bones = tempBones;
	inverseBindPoses = invBindPoses;

	//extract names from stringtable
	int size = (int)boneNames.find('\0');
	int index = 0;

	while (true)
	{
		string tempName;

		size = (int)boneNames.find('\0', index + 1);

		if (size == string::npos)
		{
			break;
		}

		tempName.resize(size - index);

		strcpy(&tempName[0], boneNames.c_str() + index);

		index = (int)boneNames.find('\0', index + 1) + 1;

		names.push_back(tempName);
	}
}

FriendlyIOTransformNode* Skeleton::GetBone(std::string name)
{
	FriendlyIOTransformNode* result = nullptr;
	
	for (int i = 0; i < names.size(); ++i)
	{
		if (names[i] == name)
		{
			result = &bones[i];
			break;
		}
	}

	return result;
}