#pragma once
#include "AEEngine.h"
#include "../../LogicComponent.h"
#include "../../AnimationComp.h"
#include "../../../Manager/ComponentManager.h"

class doubleFlameL : public BaseAttack, public EventEntity
{
	enum
	{
		set,
		fire
	};
	doubleFlameL(GameObject* owner);
	float lifetime;
	int mode;
	float tempdmg;
	AEVec2 dir;
	~doubleFlameL();
public:

	void SetRange(float other) { range_ = other; }

	void Update() override;

	void On() override { owner_->active_ = true; }

	void LevelUp() override;

	void AttackObject() override;

	void LoadFromJson(const json&) override;
	json SaveToJson() override;

	// for StateSerializer
	static constexpr const char* TypeName = "doubleFlameLComp";
	static ComponentSerializer* CreateComponent(GameObject* owner);

	void OnEvent(BaseEvent*) override;
	void OnCollision(CollisionEvent*) override;

public:
	friend class ComponentManager<LogicComponent>;
};