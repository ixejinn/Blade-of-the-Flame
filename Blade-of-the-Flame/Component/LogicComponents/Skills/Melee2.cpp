#include "Melee2.h"

#include "AEVec2.h"
#include "../../Event/Event.h"
#include "../../Manager/EventManager.h"

Melee2Attack::Melee2Attack(GameObject* owner) : BaseAttack(owner)
{
	dmg_ = 2;
	dmgGrowthRate_ = 50.f;

	range_ = 200.f;

	tempdmg = dmg_;
	cooldown_ = 2000;
	mode = set;
	/* SET COMPONENTS */
	owner_->AddComponent<BoxCollider>();
	owner_->AddComponent<Sprite>();
	owner->AddComponent<AnimationComp>();
	for (int i = 1; i < 5; i++)
	{
		owner->GetComponent<AnimationComp>()->AddDetail("Assets/meleeAnime/" + std::to_string(i) + ".png", "Attack");
	}
	owner->GetComponent<AnimationComp>()->ChangeAnimation("Attack");
	owner->GetComponent<AnimationComp>()->SetTerm(100);
	owner->GetComponent<Transform>()->SetScale({ 0, 0 });
}

Melee2Attack::~Melee2Attack()
{
}

void Melee2Attack::Update()
{
	float dt = AEFrameRateControllerGetFrameRate();
	owner_->GetComponent<Transform>()->SetPosition({ player_->GetComponent<Transform>()->GetPosition() });
	if (mode == set)
	{
		lifetime = 2500;
		elapsed = 0;
		dmg_ = 0;
		owner_->GetComponent<Transform>()->SetScale({ 0, 0 });
		if (!AEInputCheckCurr(AEVK_LBUTTON))
		{
			dmg_ = tempdmg;
			mode = fire;
		}
	}
	if (mode == fire)
	{
		if (lifetime > 0)
		{
			lifetime -= dt;
			elapsed += dt;
			if (500 < elapsed && elapsed < 1000 || 1500 < elapsed && elapsed < 2000)
			{
				owner_->GetComponent<Transform>()->SetScale({ 0, 0 });
				dmg_ = 0;
			}
			else
			{
				if (elapsed <= 500)
				{
					owner_->GetComponent<Transform>()->SetScale({ range_, range_ });
					dmg_ = tempdmg;
				}
				if (1000 <= elapsed && elapsed <= 1500)
				{
					owner_->GetComponent<Transform>()->SetScale({ range_ * 0.9f, range_ * 0.9f });
					dmg_ = tempdmg;
				}
				if (2000 <= elapsed && elapsed <= 2500)
				{
					owner_->GetComponent<Transform>()->SetScale({ range_ * 0.8f, range_ * 0.8f });
					dmg_ = tempdmg;
				}
			}
		}
		else
		{
			mode = set;
			owner_->GetComponent<Transform>()->SetScale({ 0, 0 });
			player_->GetComponent<Player>()->meleeCool = 0;
			owner_->active_ = false;
		}
	}
}

void Melee2Attack::LoadFromJson(const json&)
{
}

json Melee2Attack::SaveToJson()
{
	return json();
}

void Melee2Attack::LevelUp()
{
	//dmg_ += int(dmg_ * dmgGrowthRate_ / 100);
	tempdmg += int(tempdmg * dmgGrowthRate_ / 100);
}

void Melee2Attack::AttackObject()
{
}

ComponentSerializer* Melee2Attack::CreateComponent(GameObject* owner)
{
	return nullptr;
}
