#include "Monster.h"

#include <iomanip>	// std::setw, std::setfill
#include <sstream>	// std::stringstream
#include "../../Event/Event.h"
#include "../../Manager/EventManager.h"
#include "../../Manager/GameObjectManager.h"
#include "../../Manager/MonsterManager.h"
#include "../../Manager/ExpItemManager.h"
#include "../../Utils/Utils.h"
#include "../AnimationComp.h"
#include "../LogicComponents/Skills/shield.h"
<<<<<<< Updated upstream
=======
#include "Skills/Melee2.h"
#include "Skills/grab.h"
>>>>>>> Stashed changes

#include <iostream>

void Monster::SetAnimation()
{
	ani_->AddAnimation("Walk");
	for (int i = 0; i <= 12; i++)
	{
		std::stringstream ss;
		ss << "Assets/Monster_anime/tile" << std::setw(3) << std::setfill('0') << i << ".png";
		std::string name = ss.str();
		ani_->AddDetail(name, "Walk");
	}
	ani_->SetTerm(200);

	ani_->ChangeAnimation("Walk");
}

Monster::Monster(GameObject* owner) : LogicComponent(owner), timeStart_()
{
	hp_ = 20;
	maxHp_ = 20;
	exp_ = 5;
	dmg_ = 5;
	moveSpeed_ = 10.f;
	knockback_ = 10.f;
	cooldown_ = 1.0f;
	timeStart_ = std::chrono::system_clock::now();

	/* Set Monster component */
	owner_->AddComponent<BoxCollider>();
	owner_->AddComponent<Sprite>();
	owner_->AddComponent<AnimationComp>();

	owner_->GetComponent<Transform>()->SetScale({ 30, 100 });

	BoxCollider* col = owner_->GetComponent<BoxCollider>();
	col->SetLayer(Collider::E_BODY);
	col->SetHandler(static_cast<EventEntity*>(this));
	col->SetScale({ 0.8f, 0.7f });
	col->SetCenter({ -0.1f, -0.1f });

	ani_ = owner_->GetComponent<AnimationComp>();
	SetAnimation();
	
	/* Set pointer */
	playerTrans_ = GameObjectManager::GetInstance().GetObjectA("player")->GetComponent<Transform>();
	trans_ = owner_->GetComponent<Transform>();
	rb_ = owner_->GetComponent<RigidBody>();
	sp_ = owner_->GetComponent<Sprite>();
}

void Monster::RemoveFromManager()
{
	ComponentManager<LogicComponent>::GetInstance().DeleteComponent(static_cast<LogicComponent*>(this));
}

void Monster::Update()
{
	std::cout << hp_ << std::endl;
	AEVec2 playerPos = playerTrans_->GetPosition();
	AEVec2 pos = trans_->GetPosition();
	AEVec2 moveDir = playerPos - pos, unitMoveDir;
	f32 squareDist = AEVec2SquareLength(&moveDir);

	bool death = false;
	if (hp_ <= 0)
	{
		ExpItem* expGem = ExpItemManager::GetInstance().Spawn(pos);
		if (expGem)
			expGem->SetExp(exp_);
		death = true;

		MonsterManager::GetInstance().AddCapturedCount();
	}
	else if (squareDist > 4 * windowHeight * windowHeight)
		death = true;

	if (death)
	{
		hp_ = maxHp_;
		MonsterManager::GetInstance().Release(owner_);
		return;
	}

	AEVec2 velocity = rb_->GetVelocity();
	f32 dotProduct = moveDir.x * velocity.x + moveDir.y * velocity.y;
	if (dotProduct < 0)
		rb_->ClearVelocity();

	AEVec2Normalize(&unitMoveDir, &moveDir);
	rb_->AddVelocity(unitMoveDir * moveSpeed_);

	Direction curDir = unitMoveDir.x < 0 ? LEFT : RIGHT;
	if (dir_ != curDir)
		trans_->SetFlip();
	dir_ = curDir;

	// Change monster color to red briefly when attacked
	if (state_ == HURT)
		sp_->SetColor({ 255, 0, 0 });
	else
		sp_->SetColor({ 0, 0, 0 });
	state_ = MOVE;
}

void Monster::LoadFromJson(const json&)
{
}

json Monster::SaveToJson()
{
	return json();
}

void Monster::OnEvent(BaseEvent* event)
{
	std::type_index eventType = std::type_index(typeid(*event));

}

void Monster::OnCollision(CollisionEvent* event)
{
	Player* player = event->from_->GetComponent<Player>();
	if (player)
	{
		std::chrono::duration<double> dt = std::chrono::system_clock::now() - timeStart_;
		if (dt.count() >= cooldown_)
		{
			timeStart_ = std::chrono::system_clock::now();
			
			if (player->shield_Attack->GetComponent<Shield>()->ac == true)
			{
				player->AddHp(-dmg_ / 7);
			}
			else
			{
				player->AddHp(-dmg_);
			}
		}
		return;
	}

	MeleeAttack* melee = event->from_->GetComponent<MeleeAttack>();
	if (melee != nullptr && melee->mode == MeleeAttack::fire)
	{
		hp_ -= melee->GetDmg();
		GameObjectManager::GetInstance().GetObjectA("player")->GetComponent<Player>()->SkillGage += 1;

		AEVec2 velocity = rb_->GetVelocity();
		rb_->ClearVelocity();
		rb_->AddVelocity(velocity * -knockback_);

		state_ = HURT;

		return;
	}
	
	Melee2Attack* melee2 = event->from_->GetComponent<Melee2Attack>();
	if (melee2 != nullptr && melee2->mode == Melee2Attack::fire)
	{
		hp_ -= melee2->GetDmg();
		GameObjectManager::GetInstance().GetObjectA("player")->GetComponent<Player>()->SkillGage += 0.1;

		AEVec2 velocity = rb_->GetVelocity();
		rb_->ClearVelocity();
		rb_->AddVelocity(velocity * -knockback_ * 2.5);

		state_ = HURT;

		return;
	}

	Monster* other = event->from_->GetComponent<Monster>();
	if (other)
	{
		AEVec2 otherVelocity = event->from_->GetComponent<RigidBody>()->GetVelocity();
		rb_->ClearVelocity();
		rb_->AddVelocity(otherVelocity);

		return;
	}
}

void Monster::ReserveDmg(int dmg)
{
	hp_ -= dmg;
}

ComponentSerializer* Monster::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<Monster>())
		std::cout << "Monster::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<Monster>();
}
