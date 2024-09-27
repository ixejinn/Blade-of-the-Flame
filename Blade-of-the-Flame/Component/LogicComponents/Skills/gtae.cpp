#include "gtae.h"

#include <iostream>
#include "../../Event/Event.h"
#include "../Monster.h"
#include "../../Utils/Utils.h"
#include <cmath>

Gtae::Gtae(GameObject* owner) : BaseAttack(owner)
{
	mode = df;
	range_ = 200;
	dmgGrowthRate_ = 5.f;
	dmg_ = 1;
	tempdmg = dmg_;
	cooldown_ = 3000;
	owner_->AddComponent<Transform>();
	owner_->AddComponent<RigidBody>();
	owner_->AddComponent<Sprite>();
	owner->GetComponent<Sprite>()->SetColor({ 255, 255, 255 });
	owner->GetComponent<RigidBody>()->SetDragCoefficient(0);
	//owner->AddComponent<AnimationComp>();
	//for (int i = 0; i < 16; i++)
	//{
	//	owner->GetComponent<AnimationComp>()->AddDetail("Assets/gtae__Anime/" + std::to_string(i) + ".png", "Attack");
	//}
	//owner->GetComponent<AnimationComp>()->ChangeAnimation("Attack");
	//owner->GetComponent<AnimationComp>()->SetTerm(100);
	owner->GetComponent<Transform>()->SetScale({ 0, 0 });

	owner_->AddComponent<BoxCollider>();
	BoxCollider* col = owner_->GetComponent<BoxCollider>();
	col->SetLayer(Collider::P_ATTACK);
	col->SetType(Collider::AABB_TYPE);
	col->SetHandler(static_cast<EventEntity*>(this));
}

Gtae::~Gtae()
{
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Transform>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<RigidBody>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Sprite>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<AnimationComp>())));
	GameObjectManager::GetInstance().RemoveObject(owner_->GetName());
}

void Gtae::Update()
{
	float dt = AEFrameRateControllerGetFrameTime();
	if (mode == df)
	{			
		delay = 0;
		//에니메이션
		owner_->GetComponent<Sprite>()->SetColor({ 0, 255, 0 });
		dmg_ = 0;
		scaleA = 5;
		scaleB = 5;
		lifetime = 0.7;
		owner_->GetComponent<Transform>()->SetPosition(player_->GetComponent<Transform>()->GetPosition());
		owner_->GetComponent<Transform>()->SetScale({ 0, 0 });
		AEInputInit();
		s32 x, y;
		AEInputGetCursorPosition(&x, &y);
		AEVec2 mousePosF({ static_cast<float>(x), static_cast<float>(y) });
		convertPos = convert(mousePosF);
		if (!AEInputCheckCurr(AEVK_LBUTTON))
		{
			attackDir = { convertPos - owner_->GetComponent<Transform>()->GetPosition() };
			AEVec2Normalize(&dir, &attackDir);
			player_->GetComponent<Player>()->meleeCool = 0;
			speed = sqrt(attackDir.x * attackDir.x + attackDir.y * attackDir.y) / lifetime;
			mode = set;
		}
	}
	/*************************************************************************/
	if (mode == set)
	{
		if (lifetime > 0)
		{
			if (lifetime > 0.35)
			{
				owner_->GetComponent<Transform>()->SetScale({ owner_->GetComponent<Transform>()->GetScale().x + scaleA,
					owner_->GetComponent<Transform>()->GetScale().y + scaleA });
				scaleA *= 0.98;
			}
			else
			{
				owner_->GetComponent<Transform>()->SetScale({ owner_->GetComponent<Transform>()->GetScale().x - scaleB,
					owner_->GetComponent<Transform>()->GetScale().y - scaleB });
				scaleB *= 0.98;
			}

			lifetime -= dt;

			owner_->GetComponent<Transform>()->SetRotation(spin / 10);
			spin += AEFrameRateControllerGetFrameRate();

			AEVec2 unitMoveDir;

			AEVec2Normalize(&unitMoveDir, &dir);

			owner_->GetComponent<RigidBody>()->SetVelocity(unitMoveDir * speed);
		}
		else
		{
			owner_->GetComponent<RigidBody>()->ClearVelocity();
			mode = fire;
			lifetime = 3;
		}
	}
	/*************************************************************************/
	if (mode == fire)
	{
		if(lifetime <= 0)
		{
			mode = df;
			owner_->GetComponent<Transform>()->SetScale({ 0, 0 });
			owner_->active_ = false;
		}
		else
		{
			delay += AEFrameRateControllerGetFrameRate();
			lifetime -= dt;
			owner_->GetComponent<Transform>()->SetScale({ range_, range_ });
			//에니메이션
			owner_->GetComponent<Sprite>()->SetColor({ 255, 0, 0 });
			dmg_ = tempdmg;
		}
	}
}

void Gtae::LevelUp()
{
	tempdmg += int(tempdmg * dmgGrowthRate_ / 100);
}

void Gtae::AttackObject()
{
}

ComponentSerializer* Gtae::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<Gtae>())
		std::cout << "Gtae::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<Gtae>();
}

void Gtae::OnEvent(BaseEvent*)
{
}

void Gtae::OnCollision(CollisionEvent* event)
{
	Monster* monster = event->from_->GetComponent<Monster>();
	if (monster && delay >= 100)
	{
		delay = 0;
		player_->GetComponent<Player>()->SkillGage += 0.1;
		monster->ReserveDmg(dmg_);
	}

	Boss1* boss = event->from_->GetComponent<Boss1>();
	if (boss && delay >= 100)
	{
		delay = 0;
		player_->GetComponent<Player>()->SkillGage += 0.1;
		boss->ReserveDmg(dmg_);
	}
}

void Gtae::LoadFromJson(const json&)
{
}

json Gtae::SaveToJson()
{
	return json();
}
