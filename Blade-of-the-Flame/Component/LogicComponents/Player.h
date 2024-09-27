#pragma once
#include "../LogicComponent.h"
#include "../../Manager/ComponentManager.h"
#include "../../Event/EventEntity.h"
#include "../../Manager/ParticleSystem.h"
#include "../../Utils/Utils.h"

class Text;
class BaseAttack;
class Audio;
class Transform;
class PlayerController;
class AnimationComp;
class MeleeAttack;
class Sprite;

class Player : public LogicComponent, public EventEntity
{
private:
	enum State
	{
		IDLE,
		RUN,
		ATTACK,
		HURT
	};
	static int count;

	int level_;
	int hp_ = 100;
	int exp_ = 0;

	int maxLevel_ = 10;
	int maxHp_ = 100;
	int maxExp_ = 50;

	float moveSpeed_ = 5.f;
	float attractionRadius_ = 40.f;

	float expRequirement_ = 30.f;
	float hpGrowthRate_ = 20.f;

	//Basic_Attack
	GameObject* melee_Attack = nullptr;
	GameObject* melee_Attack2 = nullptr;
	GameObject* gtae_Attack = nullptr;
	GameObject* double_flame_Attack = nullptr;
	GameObject* penetrable_double_flame_Attack = nullptr;

	//Special_Attack
	GameObject* boomerang_Attack = nullptr;
	GameObject* fire_bubble_Attack = nullptr;

	Transform* trans_;
	Audio* audio_;
	AnimationComp* ani_;
	PlayerController* pCtrl_;
	Sprite* sp_;

	Direction dir_ = RIGHT;
	void SetAnimation();

	Player(GameObject* owner);

public:
	//Timer for Skills
	double meleeCool = 0;
	double flameCool = 0;
	double doubleflameCool = 0;
	double pendoubleflameCool = 0;

	bool getCompass_ = false;
	bool findAltar_ = false;

	int SkillGage = 0;

	void RemoveFromManager() override;

	void Update() override;

	void LoadFromJson(const json&) override;
	json SaveToJson() override;

	void OnEvent(BaseEvent* event) override;
	void OnCollision(CollisionEvent*) override;
	
	BaseAttack* curAttack_ = nullptr;			// Current basic attack
	GameObject* meteor = nullptr;
	GameObject* shield_Attack = nullptr;

	const int& GetLevel() const { return level_; }
	const int& GetHp() const { return hp_; }
	const int& GetExp() const { return exp_; }
	const int& GetMaxHp() const { return maxHp_; }
	const int& GetMaxExp() const { return maxExp_; }
	const float& GetMoveSpeed() const { return moveSpeed_; }

	void LevelUp();
	void AddHp(int hp);
	void AddExp(int exp);

	// for StateSerializer
	static constexpr const char* TypeName = "Player";
	static ComponentSerializer* CreateComponent(GameObject* owner);

	friend class ComponentManager<LogicComponent>;
};