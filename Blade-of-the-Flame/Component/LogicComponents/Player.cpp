#include "Player.h"

#include <typeindex>
#include <string>
#include "AEVec2.h"
#include "AEGraphics.h"
#include "BaseAttack.h"
#include "../../Event/Event.h"
#include "../../Manager/EventManager.h"
#include "../../Manager/GameObjectManager.h"
#include "../../Manager/GameStateManager.h"
#include "../../Utils/Utils.h"
#include "../../Utils/MathUtils.h"
#include "../../State/GameOver.h"
#include "../AnimationComp.h"

bool enablePrint;
int Player::count = 0;

#include "../LogicComponents/Skills/Meteor.h"
#include "../LogicComponents/Skills/Flame.h"
#include "../LogicComponents/Skills/doubleFlameL.h"
#include "../LogicComponents/Skills/doubleFlameR.h"

void Player::SetAnimation()
{
	ani_->AddAnimation("Idle");
	for (int i = 2; i < 5; i++)
	{
		std::string name = "Assets/Player_anime/Idle/image_" + std::to_string(i) + ".png";
		ani_->AddDetail(name, "Idle");
	}
	for (int i = 4; i >= 2; i--)
	{
		std::string name = "Assets/Player_anime/Idle/image_" + std::to_string(i) + ".png";
		ani_->AddDetail(name, "Idle");
	}
	ani_->SetTerm(500);

	ani_->ChangeAnimation("Idle");
}


Player::Player(GameObject* owner) : LogicComponent(owner)
{
	level_ = 7;
	/* Set Player component */
	owner_->AddComponent<BoxCollider>();
	owner_->AddComponent<CircleCollider>();
	owner_->AddComponent<Sprite>();
	owner_->AddComponent<PlayerController>();
	owner_->AddComponent<Audio>();
	owner_->AddComponent<AnimationComp>();

	trans_ = owner_->GetComponent<Transform>();
	trans_->SetScale({ 25, 50 });
	AEVec2 limit{ windowWidth, windowHeight };
	limit = limit * 4.f;
	trans_->SetLimit(limit);

	owner_->GetComponent<RigidBody>()->SetUseAcceleration(false);

	BoxCollider* boxCol = owner_->GetComponent<BoxCollider>();
	boxCol->SetLayer(Collider::P_AABB);
	boxCol->SetHandler(static_cast<EventEntity*>(this));

	CircleCollider* circleCol = owner_->GetComponent<CircleCollider>();
	circleCol->SetLayer(Collider::P_CIRCLE);
	circleCol->SetRadius(attractionRadius_);

	PlayerController* pCtrl = owner_->GetComponent<PlayerController>();
	pCtrl->SetDashKey(AEVK_SPACE);
	pCtrl->MultiplyMoveSpeed(moveSpeed_);

	audio_ = owner_->GetComponent<Audio>();
	audio_->SetAudio("Assets/ore.mp3");
	audio_->SetLoop(false);
	audio_->SetPlaying(false);

	ani_ = owner_->GetComponent<AnimationComp>();
	SetAnimation();

	ParticleSystem::getPtr()->SetParticle(50, { 10, 10 }, 1500);

	/* BASIC ATTACK GameObject */
	melee_Attack = GameObjectManager::GetInstance().CreateObject("MeleeAttack");
	melee_Attack->AddComponent<MeleeAttack>();
	melee_Attack->GetComponent<MeleeAttack>()->SetPlayer(owner_);

	/* Special ATTACK GameObject */
	//meteor_Attack = GameObjectManager::GetInstance().CreateObject("MeteorAttack");
	//meteor_Attack->AddComponent<Meteor>();
	//meteor_Attack->GetComponent<Meteor>()->SetPlayer(owner_);

	//----------------------------------//
	curAttack_ = melee_Attack->GetComponent<MeleeAttack>();
}

void Player::RemoveFromManager()
{
	ComponentManager<LogicComponent>::GetInstance().DeleteComponent(static_cast<LogicComponent*>(this));
}

void Player::Update()
{
	/* CHECK */
	// Level up
	if (exp_ >= maxExp_)
		LevelUp();

	if (hp_ < 6)
		enablePrint = true;

	// Death
	if (hp_ <= 0)
	{
		owner_->active_ = false;

		GameOver* newState = new GameOver();
		GameStateManager::GetInstance().ChangeState(newState);
		return;
	}

	/* SET CAMERA */
	AEVec2 pos = trans_->GetPosition();
	AEGfxSetCamPosition(pos.x, pos.y);

	/* ATTACK */
	if (1 <= level_ && level_ < 4)
	{
		meleeCool += AEFrameRateControllerGetFrameTime();
		if (SkillGage >= 100)
		{
			//쉴드스킬
		}
		else
		{
			curAttack_ = melee_Attack->GetComponent<MeleeAttack>();
		}
	}
	else if (4 <= level_ && level_ < 7)
	{
		flameCool += AEFrameRateControllerGetFrameRate();
		if (SkillGage >= 100)
		{
			//장판스킬
		}
		else
		{
			if (AEInputCheckCurr(AEVK_LBUTTON) && flameCool >= 2000)
			{
				GameObject* flame_Attack = nullptr;
				flame_Attack = GameObjectManager::GetInstance().CreateObject("FlameAttack" + std::to_string(count));
				count++;
				flame_Attack->AddComponent<Flame>();
				flame_Attack->GetComponent<Flame>()->SetPlayer(owner_);
				curAttack_ = flame_Attack->GetComponent<Flame>();
				curAttack_->On();
			}
		}
	}
	else if (7 <= level_ && level_ < 10)
	{
		doubleflameCool += AEFrameRateControllerGetFrameRate();
		if (SkillGage >= 100)
		{
			//메테오
		}
		else
		{
			if (AEInputCheckCurr(AEVK_LBUTTON) && doubleflameCool >= 2000)
			{
				GameObject* doubleflameL_Attack = nullptr;
				doubleflameL_Attack = GameObjectManager::GetInstance().CreateObject("doubleFlameAttackL" + std::to_string(count));
				count++;
				doubleflameL_Attack->AddComponent<doubleFlameL>();
				doubleflameL_Attack->GetComponent<doubleFlameL>()->SetPlayer(owner_);
				curAttack_ = doubleflameL_Attack->GetComponent<doubleFlameL>();
				curAttack_->On();

				GameObject* doubleflameR_Attack = nullptr;
				doubleflameR_Attack = GameObjectManager::GetInstance().CreateObject("doubleFlameAttackR" + std::to_string(count));
				count++;
				doubleflameR_Attack->AddComponent<doubleFlameR>();
				doubleflameR_Attack->GetComponent<doubleFlameR>()->SetPlayer(owner_);
				curAttack_ = doubleflameR_Attack->GetComponent<doubleFlameR>();
				curAttack_->On();
			}
		}
	}
	else
	{
		//관통 더블 플레임
		// 파이어 버블
	}

	if (curAttack_ == melee_Attack->GetComponent<MeleeAttack>() &&
		curAttack_->GetCooldown() <= meleeCool && 
		AEInputCheckCurr(AEVK_LBUTTON))
	{
		audio_->SetPlaying(true);
		curAttack_->AttackObject();
		meleeCool = 0;
	}

	/* NEXT STAGE */
	static bool callBoss = true;
	if (getCompass_ && findAltar_ && callBoss)
	{
		std::cout << "Next stage!!" << std::endl;
		SpawnBossEvent* event = new SpawnBossEvent();
		event->from_ = owner_;
		EventManager::GetInstance().AddEvent(event);
		callBoss = false;
	}
}

void Player::LoadFromJson(const json& data)
{
}

json Player::SaveToJson()
{
	return json();
}

void Player::OnEvent(BaseEvent* event)
{
	std::type_index eventType = std::type_index(typeid(*event));

}

void Player::OnCollision(CollisionEvent* event)
{
	FlameAltar* altar = event->from_->GetComponent<FlameAltar>();
	if (altar)
	{
		if (getCompass_)
			findAltar_ = true;
	}
		
}

void Player::LevelUp()
{
	if (level_ >= maxLevel_)
		return;

	level_++;

	maxExp_ += int(maxExp_ * expRequirement_ / 100);
	exp_ = 0;

	ParticleSystem::getPtr()->Update();

	maxHp_ += int(maxHp_ * hpGrowthRate_ / 100);
	hp_ = maxHp_;
}

void Player::AddHp(int hp)
{
	std::cout << hp_ << " ";
	hp_ += hp;
	std::cout << hp_ << std::endl;
	
	if (hp_ > maxHp_)
		hp_ = maxHp_;
}

void Player::AddExp(int exp)
{
	exp_ += exp;
}

ComponentSerializer* Player::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<Player>())
		std::cout << "Transform::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<Player>();
}
