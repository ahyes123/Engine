#include "GraphicsEngine.pch.h"
#include "ModelInstance.h"
#include "Model.h"

#define LERP(a, b, t) (a + (b - a) * t)

void ModelInstance::Init(std::shared_ptr<Model> aModel)
{
	myModel = aModel;
	myTimer = 0;
	myName = myModel->GetName();
	myPath = aModel->GetPath();
	if (myModel->GetSkeleton()->GetRoot() != nullptr)
	{
		myCurrentAnimation = myModel->GetSkeleton()->myAnimations[myModel->GetSkeleton()->myName];
		myCurrentAnimation.myState = Animation::AnimationState::Playing;
		isLooping = true;
	}
	else
	{
		myCurrentAnimation.myState = Animation::AnimationState::Finished;
	}
}

void ModelInstance::Update(float aDeltaTime)
{
	if (myCurrentAnimation.myFrames.size() == 0)
		return;
	if(myCurrentAnimation.myState == Animation::AnimationState::Playing)
	{
		myTimer += aDeltaTime;

		if (myTimer >= myCurrentAnimation.myDuration)
		{
			if (isLooping)
			{
				myTimer -= myCurrentAnimation.myDuration;
			}
			else
			{
				myTimer = myCurrentAnimation.myDuration;
				myCurrentAnimation.myState = Animation::AnimationState::Finished;
			}
		}

		float frameTime = 1 / myCurrentAnimation.myFPS;
		const float result = myTimer / frameTime;
		size_t frame = static_cast<size_t>(std::floor(result));
		const float alpha = result - static_cast<float>(frame);

		Matrix4x4f identity;

		UpdateAnimationHierarchy(frame, 0, &myCurrentAnimation, identity, &myBoneTransforms[0], alpha);
	}
}

void ModelInstance::UpdateAnimationHierarchy(size_t aCurrentFrame, unsigned aBoneIDx, const Animation* anAnimation,
	Matrix4x4f& aParentTransform, Matrix4x4f* outboneTransforms, const float& anAlpha)
{
	size_t nextFrame = aCurrentFrame + 1;

	if (myCurrentAnimation.myState == Animation::AnimationState::Finished)
	{
		nextFrame = aCurrentFrame;
	}
	else if (nextFrame >= myCurrentAnimation.myFrames.size() - 1)
	{
		nextFrame = 1;
	}
	
	const Skeleton::Bone& bone = myModel->GetSkeleton()->myBones[aBoneIDx];

	if (aCurrentFrame >= anAnimation->myFrames.size())
	{
		aCurrentFrame = 0;
	}
	
	const Transform& currentTransform = anAnimation->myFrames[aCurrentFrame].LocalTransform[aBoneIDx];

	Matrix4x4f matrix = currentTransform.GetMatrix();

	if (true)
	{
		const Transform& nextTransform = anAnimation->myFrames[nextFrame].LocalTransform[aBoneIDx];

		const Vector3f T = Vector3f::Lerp(currentTransform.GetPosition(), nextTransform.GetPosition(), anAlpha);
		const Quatf R = Quatf::Slerp(currentTransform.GetQuaternion(), nextTransform.GetQuaternion(), anAlpha);
		const Vector3f S = Vector3f::Lerp(currentTransform.GetScale(), nextTransform.GetScale(), anAlpha);

		matrix = Matrix4x4f::CreateScaleMatrix(S) * R.GetRotationMatrix4x4f() * Matrix4x4f::CreateTranslationMatrix(T);
	}

	Matrix4x4f boneTransform = aParentTransform * Matrix4x4f::Transpose(matrix);
	Matrix4x4f result;
	result *= boneTransform;
	result *= bone.BindPoseInverse;

	outboneTransforms[aBoneIDx] = result;

	for(size_t children = 0; children < bone.Children.size(); children++)
	{
		UpdateAnimationHierarchy(aCurrentFrame, bone.Children[children], anAnimation, boneTransform, outboneTransforms, anAlpha);
	}
}

void ModelInstance::SetAnimation(const std::wstring& aName, bool aLoop)
{
	myCurrentAnimation = myModel->GetSkeleton()->myAnimations[aName];
	myCurrentAnimation.myState = Animation::AnimationState::Playing;
	myTimer = 0;
	isLooping = aLoop;
}