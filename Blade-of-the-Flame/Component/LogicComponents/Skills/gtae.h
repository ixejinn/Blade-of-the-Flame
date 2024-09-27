#pragma once
#include "AEEngine.h"
#include "../../LogicComponent.h"
#include "../../Component/AnimationComp.h"
#include "../../Manager/ComponentManager.h"

class Gtae : public BaseAttack, public EventEntity
{
	enum State
	{
		df,
		set,
		fire
	};

	Gtae(GameObject* owner);
	~Gtae();
	AEVec2 convertPos;
	AEVec2 halfPos;
	AEVec2 attackDir;
	AEVec2 dir;
	float delay;
	float scaleA, scaleB;
	int mode;
	float tempdmg;
	float spin;
	float speed;
	float lifetime;
public:

	void Update() override;

	void On() override { owner_->active_ = true; }

	void LevelUp() override;

	void AttackObject() override;

	void LoadFromJson(const json&) override;
	json SaveToJson() override;

	// for StateSerializer
	static constexpr const char* TypeName = "GtaeComp";
	static ComponentSerializer* CreateComponent(GameObject* owner);

	void OnEvent(BaseEvent*) override;
	void OnCollision(CollisionEvent*) override;

public:
	friend class ComponentManager<LogicComponent>;
};