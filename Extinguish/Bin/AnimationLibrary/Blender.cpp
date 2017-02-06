#include "Blender.h"
#include <map>

using namespace DirectX;

Blender::Blender()
{
	nextInterpolator = new Interpolator();
	curInterpolator = new Interpolator();

	//hashString = HashString::GetSingleton();
	//resourceManager = ResourceManager::GetSingleton();
	curBlendTime = 0;
}

Blender::~Blender()
{
	delete curInterpolator;
	delete nextInterpolator;
}

void Blender::Init(std::string curAnimName, std::string nextAnimName)
{
	//set interpolators values
	curInterpolator->SetAnimation(animationSet->GetAnimation(curAnimName));
	//curInterpolator->SetIsTimeBased(timeBased);

	nextInterpolator->SetAnimation(animationSet->GetAnimation(nextAnimName));
	//nextInterpolator->SetIsTimeBased(timeBased);
}

void Blender::Update(float time, unsigned int frameIndex) // i just use frameIndex for bear, so if its 0 and time isn't 0, don't update
{
	std::vector<Bone>* bones;
	//Bone* bones;

	if (!nextInterpolator->HasAnimation())
	{
		if (!time)
		{
			curInterpolator->SetCurFrame(frameIndex % animationSet->GetAnimation(0)->GetNumKeyFrames());
		}

		curInterpolator->Update(time);

		bones = curInterpolator->GetBones();
	}
	else
	{
		//update curBlendTime	
		curBlendTime += time;

		//nextInterpolator->SetAnimation(animationSet->GetAnimation(nextAnimationIndex));
		
		//update interpolators
		curInterpolator->Update(time);
		nextInterpolator->Update(time);

		//interpolate to get keyframe between two animations
		if (curBlendTime < blendInfo.totalBlendTime)
		{
			blendedKeyFrame.ClearBones();
			Interpolate(curInterpolator->GetBetweenKeyFrame(), nextInterpolator->GetBetweenKeyFrame(), curBlendTime / blendInfo.totalBlendTime);
		}
		else
		{
			curBlendTime = 0;

			//make cur animation = next animation
			curInterpolator->SetAnimation(nextInterpolator->GetAnimation()); 
			curInterpolator->SetPrevFrame(nextInterpolator->GetPrevFrame());
			curInterpolator->SetNextFrame(nextInterpolator->GetNextFrame());
			curInterpolator->SetSpeed(nextInterpolator->GetSpeed());
			curInterpolator->SetIsLoop(nextInterpolator->IsLoop());

			nextInterpolator->SetAnimation(nullptr); //set next animation to null
		}
		
		bones = &blendedKeyFrame.GetBones();
	}

	boneOffsets.clear();
	bonesWorlds.clear();

	for (unsigned int i = 0; i < (*bones).size(); ++i)
	{
		DirectX::XMFLOAT4X4 boneOffset;
		DirectX::XMMATRIX boneWorld;
		DirectX::XMFLOAT4X4 boneWorldFloat;
		DirectX::XMMATRIX inverseBindPose;

		boneWorld = DirectX::XMLoadFloat4x4(&(*bones)[i].GetWorld());

		inverseBindPose = DirectX::XMLoadFloat4x4(&animationSet->GetSkeleton()->GetInverseBindPose(i));

		DirectX::XMStoreFloat4x4(&boneOffset, DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(inverseBindPose, boneWorld)));
		DirectX::XMStoreFloat4x4(&boneWorldFloat, boneWorld);

		boneOffsets.push_back(boneOffset);
		bonesWorlds.push_back(boneWorldFloat);
	}
}

//setters//
void Blender::SetCurAnimation(std::string animName)
{ 
	//curAnimationIndex = curIndex;

	curInterpolator->SetAnimation(animationSet->GetAnimation(animName));
}

void Blender::SetCurAnimation(int animIndex)
{
	curInterpolator->SetAnimation(animationSet->GetAnimation(animIndex));
}

void Blender::SetNextAnimation(std::string animName)
{
	//nextAnimationIndex = nextIndex;

	if (!nextInterpolator->HasAnimation())
	{
		nextInterpolator->SetAnimation(animationSet->GetAnimation(animName));
		//nextInterpolator->SetIsTimeBased(true);
	}
}

void Blender::SetNextAnimation(int animIndex)
{
	if (!nextInterpolator->HasAnimation())
	{
		nextInterpolator->SetAnimation(animationSet->GetAnimation(animIndex));
		//nextInterpolator->SetIsTimeBased(true);
	}
}

//getters//
Interpolator* Blender::GetCurInterpolator()
{
	return curInterpolator;
}

Interpolator* Blender::GetNextInterpolator()
{
	return nextInterpolator;
}

AnimationSet* Blender::GetAnimationSet()
{
	return animationSet;
}

//private helper functions
void Blender::Interpolate(KeyFrame* previous, KeyFrame* next, float ratio)
{
	for (int i = 0; i < previous->GetBones().size(); ++i)
	{
		XMFLOAT4X4 newWorld;
		Bone newBone;

		XMVECTOR quarternion = XMQuaternionSlerp(XMQuaternionRotationMatrix(XMLoadFloat4x4(&previous->GetBone(i)->GetWorld())), XMQuaternionRotationMatrix(XMLoadFloat4x4(&next->GetBone(i)->GetWorld())), ratio);

		XMFLOAT4X4 prevBone = previous->GetBone(i)->GetWorld();
		XMVECTOR prevTranslation = XMVectorSet(prevBone._41, prevBone._42, prevBone._43, prevBone._44);

		XMFLOAT4X4 nextBone = next->GetBone(i)->GetWorld();
		XMVECTOR nextTranslation = XMVectorSet(nextBone._41, nextBone._42, nextBone._43, nextBone._44);

		XMVECTOR newTranslation = XMVectorLerp(prevTranslation, nextTranslation, ratio);

		XMMATRIX resultMatrix = XMMatrixAffineTransformation({ 1, 1, 1, 1 }, { 0, 0, 0, 0 }, quarternion, newTranslation);

		newBone = *previous->GetBone(i);
		XMStoreFloat4x4(&newWorld, resultMatrix);
		newBone.SetWorld(newWorld);

		blendedKeyFrame.InsertBone(newBone);
	}

}

//void Blender::MakeCurAnimation(bool timeBased)
//{
//
//}
//
//void Blender::MakeNextAnimation(bool timeBased)
//{
//	if (!nextInterpolator)
//	{
//
//	}
//}