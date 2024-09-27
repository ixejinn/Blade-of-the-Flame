#include "MeleeAttack.h"

#include "AEVec2.h"
#include "../../Event/Event.h"
#include "../../Manager/EventManager.h"

MeleeAttack::MeleeAttack(GameObject* owner) : BaseAttack(owner)
{
	dmg_ = 2;
	range_ = 100.f;
	tempdmg = dmg_;
	cooldown_ = 2000;
	dmgGrowthRate_ = 50.f;
	lifetime = 0;
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
<<<<<<< Updated upstream
	owner->GetComponent<AnimationComp>()->SetTerm(300);
	BoxCollider* col = owner_->GetComponent<BoxCollider>();
	col->SetType(Collider::OBB_TYPE);
	col->SetLayer(Collider::P_ATTACK);
=======
	owner->GetComponent<AnimationComp>()->SetTerm(100);
>>>>>>> Stashed changes
}

MeleeAttack::~MeleeAttack()
{
}

void MeleeAttack::Update()
{
	float dt = AEFrameRateControllerGetFrameRate();
	if (mode == set)
	{
		lifetime = 1000;
		owner_->GetComponent<Transform>()->SetScale({ 0, 0 });
		dmg_ = 0;
		AEInputInit();
		s32 x, y;
		AEInputGetCursorPosition(&x, &y);
		attackDir = { x - windowWidth / 2.f, windowHeight / 2.f - y };
		AEVec2 unitDir;
		AEVec2Normalize(&unitDir, &attackDir);

		attackDir = unitDir * range_;

		AEVec2 playerPos = player_->GetComponent<Transform>()->GetPosition();
		Transform* trans = owner_->GetComponent<Transform>();

		if (!AEInputCheckCurr(AEVK_LBUTTON))
		{
			trans->SetPosition(playerPos + attackDir / 2);
			trans->SetRotation(unitDir);
			dmg_ = tempdmg;
			mode = fire;
		}
	}
	if (mode == fire)
	{
		if (lifetime > 0)
		{
			lifetime -= dt;
			owner_->GetComponent<Transform>()->SetScale({ range_, range_ });
			owner_->GetComponent<Transform>()->SetPosition(player_->GetComponent<Transform>()->GetPosition() + attackDir / 2);
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

void MeleeAttack::LoadFromJson(const json&)
{
}

json MeleeAttack::SaveToJson()
{
	return json();
}

void MeleeAttack::LevelUp()
{
	tempdmg += int(tempdmg * dmgGrowthRate_ / 100);
}

void MeleeAttack::AttackObject()
{
}

ComponentSerializer* MeleeAttack::CreateComponent(GameObject* owner)
{
	return nullptr;
}
