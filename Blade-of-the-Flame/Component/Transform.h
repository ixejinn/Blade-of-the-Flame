#pragma once
#include "AEMath.h"
#include "EngineComponent.h"
#include "../Manager/ComponentManager.h"
#include "../Utils/Utils.h"

class Transform : public EngineComponent
{
private:
	AEVec2 position_;
	float rotation_;
	AEVec2 scale_;
	AEVec2 localScale_ = { 1.f, 1.f };



	AEMtx33 transformMatrix_;

	bool useLimit_ = false;
	AEVec2 upperLimit_ = { 4 * windowWidth, 4 * windowHeight };
	AEVec2 lowerLimit_ = { -4 * windowWidth, -4 * windowHeight };

	bool isRotationChanged_ = false;

	Transform(GameObject* owner);

	void UpdateMatrix();

public:
	void RemoveFromManager() override;

	void Update() override;

	void LoadFromJson(const json&) override;
	json SaveToJson() override;

	const AEVec2& GetPosition() const { return position_; }
	const float& GetRotation() const { return rotation_; }
	const AEVec2& GetScale() const { return scale_; }
	const AEVec2& GetLocalScale() const { return localScale_; }
	const AEMtx33& GetMatrix() const { return transformMatrix_; }

	void SetPosition(const AEVec2& pos);
	void SetPosition(const float& x, const float& y); 
	void SetScale(const AEVec2& scl);
	void SetLocalScale(const AEVec2& scl);
	void SetRotation(const AEVec2& pos);
	void SetRotation(const float& rot);
	void SetLimit(const AEVec2& limit);
	void SetLimit(const float& x, const float& y);
	void SetUpperLimit(const AEVec2& limit);
	void SetLowerLimit(const AEVec2& limit);
	void SetFlip() { scale_.x *= -1; }

	bool IsRotationChanged();

	void PrintMatrix() const;

	// for StateSerializer
	static constexpr const char* TypeName = "Transform";
	static ComponentSerializer* CreateComponent(GameObject* owner);

	friend class ComponentManager<EngineComponent>;
};