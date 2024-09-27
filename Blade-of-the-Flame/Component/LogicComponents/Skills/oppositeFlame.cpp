#include "oppositeFlame.h"

#include "../../../Event/Event.h"
#include "../Monster.h"
#include "../../../Utils/Utils.h"


oFlame::oFlame(GameObject* owner) : BaseAttack(owner)
{
	mode = set;
	lifetime = 12000;
	dmg_ = 10;
	tempdmg = dmg_;
	cooldown_ = 2000;
	dmgGrowthRate_ = 5.f;
	owner_->AddComponent<Transform>();
	owner_->AddComponent<RigidBody>();
	owner_->AddComponent<Sprite>();
	owner->AddComponent<AnimationComp>();
	owner->GetComponent<AnimationComp>()->AddDetail("Assets/FlameAnime/1.png", "Attack");
	owner->GetComponent<AnimationComp>()->AddDetail("Assets/FlameAnime/2.png", "Attack");
	owner->GetComponent<AnimationComp>()->AddDetail("Assets/FlameAnime/3.png", "Attack");
	owner->GetComponent<AnimationComp>()->AddDetail("Assets/FlameAnime/4.png", "Attack");
	owner->GetComponent<AnimationComp>()->ChangeAnimation("Attack");
	owner->GetComponent<AnimationComp>()->SetTerm(200);
	owner->GetComponent<Transform>()->SetScale({ 0, 0 });

	owner_->AddComponent<BoxCollider>();
	BoxCollider* col = owner_->GetComponent<BoxCollider>();
	col->SetLayer(Collider::P_ATTACK);
	col->SetType(Collider::AABB_TYPE);
	col->SetHandler(static_cast<EventEntity*>(this));
}

oFlame::~oFlame()
{
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Transform>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<RigidBody>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Sprite>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<AnimationComp>())));
	GameObjectManager::GetInstance().RemoveObject(owner_->GetName());
}

void oFlame::Update()
{
	if (mode == set)
	{
		dmg_ = 0;
		owner_->GetComponent<Transform>()->SetPosition(player_->GetComponent<Transform>()->GetPosition());
		AEInputInit();
		s32 x, y;
		AEInputGetCursorPosition(&x, &y);
		AEVec2 mousePosF({ static_cast<float>(x), static_cast<float>(y) });
		if (!AEInputCheckCurr(AEVK_LBUTTON))
		{
			AEVec2 attackDir{ convert(mousePosF) - owner_->GetComponent<Transform>()->GetPosition() };
			AEVec2Normalize(&dir, &attackDir);
			owner_->GetComponent<Transform>()->SetRotation(dir * -1);
			mode = fire;
			dmg_ = tempdmg;
			owner_->GetComponent<Transform>()->SetScale({ 80, 50 });
		}
	}
	if (mode == fire)
	{
		if (lifetime > 0)
		{
			float dt = AEFrameRateControllerGetFrameRate();
			lifetime -= dt;
			owner_->GetComponent<RigidBody>()->AddVelocity(dir * -1 * 300);
		}
		else
		{
			owner_->active_ = false;
			owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<oFlame>())));
		}
	}
}

void oFlame::LevelUp()
{
	tempdmg += int(tempdmg * dmgGrowthRate_ / 100);
}

void oFlame::AttackObject()
{
}

ComponentSerializer* oFlame::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<oFlame>())
		std::cout << "oFlame::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<oFlame>();
}

void oFlame::OnEvent(BaseEvent*)
{
}

void oFlame::OnCollision(CollisionEvent* event)
{
	Monster* monster = event->from_->GetComponent<Monster>();
	if (monster)
	{
		player_->GetComponent<Player>()->SkillGage += 1;
		monster->ReserveDmg(dmg_);
		owner_->active_ = false;
		owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<oFlame>())));
	}

	Boss1* boss = event->from_->GetComponent<Boss1>();
	if (boss)
	{
		player_->GetComponent<Player>()->SkillGage += 1;
		boss->ReserveDmg(dmg_);
	}
}

void oFlame::LoadFromJson(const json&)
{
}

json oFlame::SaveToJson()
{
	return json();
}
