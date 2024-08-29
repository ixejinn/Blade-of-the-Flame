#pragma once
#include "AEMath.h"
#include "EngineComponent.h"
#include "../Manager/ComponentManager.h"
#include "../Utils/MathUtils.h"
#include "../Event/EventEntity.h"

class Collider : public EngineComponent
{
public:
	enum ColliderType
	{
		CIRCLE,	// circle
		AABB	// box
	};

protected:
	const ColliderType type_;

	AEVec2 center_;	// Transform�� position���κ��� �󸶳� �̵��ߴ����� ��Ÿ�� (�⺻ ��: 0.0f, 0.0f)
	AEVec2 scale_;	// Collider�� local scale

	AEVec2 collisionPoint_;

	Collider(GameObject* owner, ColliderType type);

public:
	void RemoveFromManager() override;

	void LoadFromJson(const json&) override;

	const ColliderType& GetColliderType() const { return type_; }
	const AEVec2& GetCenter() const { return center_; }
	const AEVec2& GetScale() const { return scale_; }
	const AEVec2& GetCollisionPoint() const { return collisionPoint_; }

	void SetScale(const AEVec2& scale) { scale_ = scale; }
	void SetCenter(const AEVec2& center) { center_ = center; }

	friend class CollisionManager;
};

/**
* @brief	GameObject�� Transform ���� �� ��� ����
*/
class BoxCollider : public Collider
{
private:
	Vec3 bottomLeft_;
	Vec3 topRight_;

	// Set bottomLeft, topRight
	void SetBounds();

	BoxCollider(GameObject* owner);

public:
	void Update() override;

	json SaveToJson() override;

	const AEVec2& GetBottomLeft() const { return AEVec2{ bottomLeft_.x, bottomLeft_.y }; }
	const AEVec2& GetTopRight() const { return AEVec2{ topRight_.x, topRight_.y }; }
	
	// for StateSerializer
	static constexpr const char* TypeName = "BoxCollider";
	static ComponentSerializer* CreateComponent(GameObject* owner);

	friend class ComponentManager<EngineComponent>;
	friend class CollisionManager;
};

/**
* @brief	GameObject�� Transform ���� �� ��� ����
*/
class CircleCollider : public Collider
{
private:
	float radius_;

	CircleCollider(GameObject* owner);

public:
	void Update() override;

	json SaveToJson() override;

	const float& GetRadius() const { return radius_; }

	// for StateSerializer
	static constexpr const char* TypeName = "CircleCollider";
	static ComponentSerializer* CreateComponent(GameObject* owner);

	friend class ComponentManager<EngineComponent>;
	friend class CollisionManager;
};