#include "Transform.h"

#include <iostream>
#include <cmath>
#include "../GameObject/GameObject.h"
#include "../Manager/Camera.h"

Transform::Transform(GameObject* owner) : EngineComponent(owner), position_(), scale_(), rotation_(0), transformMatrix_()
{
	position_.x = 0;
	position_.y = 0;

	scale_.x = 1;
	scale_.y = 1;

	UpdateMatrix();
}

void Transform::UpdateMatrix()
{
	AEMtx33 tranMtx;
	AEMtx33Trans(&tranMtx, position_.x, position_.y);

	AEMtx33 rotMtx;
	AEMtx33Rot(&rotMtx, rotation_);

	AEMtx33 sclMtx;
	//AEMtx33Scale(&sclMtx, scale_.x, scale_.y);
	AEMtx33Scale(&sclMtx, localScale_.x * scale_.x, localScale_.y * scale_.y);

	// Concatenate trnasform, rotation, scaling matrix
	AEMtx33Concat(&transformMatrix_, &rotMtx, &sclMtx);
	AEMtx33Concat(&transformMatrix_, &tranMtx, &transformMatrix_);

	//AEMtx33Concat(&transformMatrix_, &Camera::GetInstance().GetMatrix(), &transformMatrix_);
}

void Transform::RemoveFromManager()
{
	ComponentManager<EngineComponent>::GetInstance().DeleteComponent(static_cast<EngineComponent*>(this));
}

void Transform::Update()
{
	UpdateMatrix();

	float x = position_.x;
	float y = position_.y;

	if (useLimit_)
	{
		x = AEClamp(x, lowerLimit_.x, upperLimit_.x);
		y = AEClamp(y, lowerLimit_.y, upperLimit_.y);
	}

	SetPosition({ x, y });
}

void Transform::LoadFromJson(const json& data)
{
	auto compData = data.find("compData");
	if (compData != data.end())
	{
		auto p = compData->find("position");
		position_.x = p->begin().value();
		position_.y = (p->begin() + 1).value();

		auto s = compData->find("scale");
		scale_.x = s->begin().value();
		scale_.y = (s->begin() + 1).value();

		auto r = compData->find("rotation");
		rotation_ = r.value();
	}

	UpdateMatrix();
}

json Transform::SaveToJson()
{
	json data, compData;
	data["type"] = TypeName;

	compData["position"] = { position_.x, position_.y };
	compData["scale"] = { scale_.x, scale_.y };
	compData["rotation"] = rotation_;

	data["compData"] = compData;
	return data;
}

void Transform::SetPosition(const AEVec2& pos)
{
	position_ = pos;

	UpdateMatrix();

	//if (owner_->GetComponent<Player>())
	//	AEGfxSetCamPosition(pos.x, pos.y);
}

void Transform::SetPosition(const float& x, const float& y)
{
	position_.x = x;
	position_.y = y;

	UpdateMatrix();
}

void Transform::SetScale(const AEVec2& scl)
{
	scale_ = scl;
	UpdateMatrix();
}

void Transform::SetLocalScale(const AEVec2& scl)
{
	localScale_ = scl;
}

void Transform::SetRotation(const AEVec2& pos)
{
	SetRotation(std::atan2(pos.y, pos.x));
}

void Transform::SetRotation(const float& rot)
{
	isRotationChanged_ = true;

	rotation_ = rot;
	UpdateMatrix();
}

void Transform::SetLimit(const AEVec2& limit)
{
	upperLimit_ = limit;

	lowerLimit_ = limit * -1;
}

void Transform::SetLimit(const float& x, const float& y)
{
	upperLimit_.x = x;
	upperLimit_.y = y;

	lowerLimit_.x = -x;
	lowerLimit_.y = -y;
}

void Transform::SetUpperLimit(const AEVec2& limit)
{
	upperLimit_ = limit;
}

void Transform::SetLowerLimit(const AEVec2& limit)
{
	lowerLimit_ = limit;
}

bool Transform::IsRotationChanged()
{
	if (isRotationChanged_)
	{
		isRotationChanged_ = false;
		return true;
	}
	else
		return false;
}

void Transform::PrintMatrix() const
{
	std::cout << "Printing Transform component. With this values: " << std::endl;
	std::cout << "Translate : " << position_.x << " " << position_.y << std::endl;
	std::cout << " Rotation : " << rotation_ << std::endl;
	std::cout << "    Scale : " << scale_.x << " " << scale_.y << std::endl;

	std::cout << "-----------------------------------\n";
	for (int i = 0; i < 3; i++)
	{
		std::cout << "|";
		for (int j = 0; j < 3; j++)
			std::cout << std::setw(4) << transformMatrix_.m[i][j];
		std::cout << "|\n";
	}
	std::cout << "-----------------------------------\n";
}

ComponentSerializer* Transform::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<Transform>())
		std::cout << "Transform::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<Transform>();
}