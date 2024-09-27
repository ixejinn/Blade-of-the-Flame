#include "Player.h"

#include <iostream>
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

int Player::count = 0;

#include "Skills/Meteor.h"
#include "Skills/Flame.h"
#include "Skills/doubleFlameL.h"
#include "Skills/doubleFlameR.h"
#include "Skills/PenetrableDoubleFlameL.h"
#include "Skills/PenetrableDoubleFlameR.h"
#include "Skills/Shield.h"
#include "Skills/boomerang.h"
#include "Skills/bubble.h"
#include "Skills/gtae.h"
#include "Skills/grab.h"
#include "Skills/Recall.h"
#include "Skills/oppositeFlame.h"
#include "Skills/Melee2.h"
#include "Skills/Pet.h"

#include "../../Manager/Camera.h"

void Player::SetAnimation()
{
	ani_->AddAnimation("Idle");
	for (int i = 1; i <= 18; i++)
	{
		std::string name = "Assets/Player_anime/Idle/row-1-column-" + std::to_string(i) + ".png";
		ani_->AddDetail(name, "Idle");
	}

	ani_->AddAnimation("Run");
	for (int i = 1; i <= 24; i++)
	{
		std::string name = "Assets/Player_anime/run/row-1-column-" + std::to_string(i) + ".png";
		ani_->AddDetail(name, "Run");
	}

	ani_->AddAnimation("Attack");
	for (int i = 1; i <= 12; i++)
	{
		std::string name = "Assets/Player_anime/Atk/row-1-column-" + std::to_string(i) + ".png";
		ani_->AddDetail(name, "Attack");
	}

	ani_->ChangeAnimation("Idle");
	ani_->SetTerm(300);
}


Player::Player(GameObject* owner) : LogicComponent(owner)
{
	level_ = 1;
	SkillGage = 0;
	/* Set Player component */
	owner_->AddComponent<CircleCollider>();
	owner_->AddComponent<BoxCollider>();
	owner_->AddComponent<PlayerController>();
	owner_->AddComponent<Audio>();
	owner_->AddComponent<AnimationComp>();

	trans_ = owner_->GetComponent<Transform>();
	trans_->SetScale({ windowWidth / 13.f, windowWidth / 13.f });

	owner_->GetComponent<RigidBody>()->SetUseAcceleration(false);

	BoxCollider* boxCol = owner_->GetComponent<BoxCollider>();
	boxCol->SetLayer(Collider::P_AABB);
	boxCol->SetHandler(static_cast<EventEntity*>(this));
	boxCol->SetScale({ 0.25, 0.4 });
	boxCol->SetCenter({ 0.f, -0.04f });

	CircleCollider* circleCol = owner_->GetComponent<CircleCollider>();
	circleCol->SetLayer(Collider::P_CIRCLE);
	circleCol->SetRadius(50.f);

	pCtrl_ = owner_->GetComponent<PlayerController>();
	pCtrl_->SetDashKey(AEVK_SPACE);
	pCtrl_->MultiplyMoveSpeed(moveSpeed_);

	audio_ = owner_->GetComponent<Audio>();
	audio_->SetAudio("Assets/ore.mp3");
	audio_->SetLoop(false);
	audio_->SetPlaying(false);

	owner_->GetComponent<Sprite>()->SetLocalPos(0.15f, 0.f);
	ani_ = owner_->GetComponent<AnimationComp>();
	SetAnimation();

	sp_ = owner_->GetComponent<Sprite>();
	ParticleSystem::getPtr()->SetParticle(50, { 10, 10 }, 1500);

	/* BASIC ATTACK GameObject */
	melee_Attack = GameObjectManager::GetInstance().CreateObject("MeleeAttack");
	melee_Attack->AddComponent<MeleeAttack>();
	melee_Attack->GetComponent<MeleeAttack>()->SetPlayer(owner_);

	melee_Attack2 = GameObjectManager::GetInstance().CreateObject("MeleeAttack2");
	melee_Attack2->AddComponent<Melee2Attack>();
	melee_Attack2->GetComponent<Melee2Attack>()->SetPlayer(owner_);

	gtae_Attack = GameObjectManager::GetInstance().CreateObject("GtaeAttack");
	gtae_Attack->AddComponent<Gtae>();
	gtae_Attack->GetComponent<Gtae>()->SetPlayer(owner_);

	/* Special ATTACK GameObject */
	meteor = GameObjectManager::GetInstance().CreateObject("MeteorAttack");
	meteor->AddComponent<Meteor>();
	meteor->GetComponent<Meteor>()->SetPlayer(owner_);

	grab_Attack = GameObjectManager::GetInstance().CreateObject("Grab");
	grab_Attack->AddComponent<Grab>();
	grab_Attack->GetComponent<Grab>()->SetPlayer(owner_);

	shield_Attack = GameObjectManager::GetInstance().CreateObject("Shield");
	shield_Attack->AddComponent<Shield>();
	shield_Attack->GetComponent<Shield>()->SetPlayer(owner_);

	boomerang_Attack = GameObjectManager::GetInstance().CreateObject("Boomerang");
	boomerang_Attack->AddComponent<boomerang>();
	boomerang_Attack->GetComponent<boomerang>()->SetPlayer(owner_);

	fire_bubble_Attack = GameObjectManager::GetInstance().CreateObject("Bubble");
	fire_bubble_Attack->AddComponent<bubble>();
	fire_bubble_Attack->GetComponent<bubble>()->SetPlayer(owner_);

	TimeRecall = GameObjectManager::GetInstance().CreateObject("TimeRecall");
	TimeRecall->AddComponent<Recall>();
	TimeRecall->GetComponent<Recall>()->SetPlayer(owner_);

	Pet_ = GameObjectManager::GetInstance().CreateObject("Pet");
	Pet_->AddComponent<Pet>();
	Pet_->GetComponent<Pet>()->SetPlayer(owner_);

	//----------------------------------//
	curAttack_ = melee_Attack->GetComponent<MeleeAttack>();
}

void Player::RemoveFromManager()
{
	ComponentManager<LogicComponent>::GetInstance().DeleteComponent(static_cast<LogicComponent*>(this));
}

void Player::Update()
{
	TimeRecall->active_ = true;
	//std::cout << "스킬 게이지 : " << SkillGage << std::endl;
	/* CHECK */
	// Level up
	if (exp_ >= maxExp_)
		LevelUp();

	// Death
	static int preHp = hp_;
	if (hp_ <= 0)
	{
		owner_->active_ = false;

		GameOver* newState = new GameOver();
		GameStateManager::GetInstance().ChangeState(newState);
		return;
	}

	// MOUSE BUTTON & State
	static State preState = IDLE;
	State curState = IDLE;
	Direction curDir = dir_;
	if (AEInputCheckCurr(AEVK_LBUTTON))
		curState = ATTACK;
	else if (hp_ < preHp)
	{
		curState = HURT;
		preHp = hp_;
	}
	else if (pCtrl_->GetMoveState())
	{
		curState = RUN;
		curDir = pCtrl_->GetMoveDir();
	}
	else
		curState = IDLE;

	/* ATTACK */
	if (1 <= level_ && level_ < 4)
	{
		meleeCool += AEFrameRateControllerGetFrameRate();
		if (SkillGage >= maxSkillGage)
		{
			//쉴드스킬
			curAttack_ = shield_Attack->GetComponent<Shield>();
			curAttack_->On();
			SkillGage = 0;
		}
		else
		{
			//curAttack_ = nullptr;
			//if (melee_Attack->GetComponent<MeleeAttack>()->GetCooldown() <= meleeCool &&
			//	AEInputCheckCurr(AEVK_LBUTTON))
			//{
			//	curAttack_ = melee_Attack->GetComponent<MeleeAttack>();
			//	curAttack_->On();
			//}

			//curAttack_ = nullptr;
			//if (melee_Attack2->GetComponent<Melee2Attack>()->GetCooldown() <= meleeCool &&
			//	AEInputCheckCurr(AEVK_LBUTTON))
			//{
			//	curAttack_ = melee_Attack2->GetComponent<Melee2Attack>();
			//	curAttack_->On();
			//}

			curAttack_ = nullptr;
			if (gtae_Attack->GetComponent<Gtae>()->GetCooldown() <= meleeCool &&
				AEInputCheckCurr(AEVK_LBUTTON))
			{
				curAttack_ = gtae_Attack->GetComponent<Gtae>();
				curAttack_->On();
			}
		}
	}
	else if (4 <= level_ && level_ < 7)
	{
		flameCool += AEFrameRateControllerGetFrameRate();
		if (SkillGage >= maxSkillGage)
		{
			////부메랑 스킬
			//curAttack_ = boomerang_Attack->GetComponent<boomerang>();
			//curAttack_->On();
			//SkillGage = 0;

			//if (AEInputCheckCurr(AEVK_LBUTTON))
			//{
			//	curAttack_ = grab_Attack->GetComponent<Grab>();
			//	curAttack_->On();
			//}

			//if (AEInputCheckCurr(AEVK_LBUTTON))
			//{
			//	curAttack_ = TimeRecall->GetComponent<Recall>();
			//	curAttack_->On();
			//}

			if (AEInputCheckCurr(AEVK_LBUTTON))
			{
				curAttack_ = Pet_->GetComponent<Pet>();
				curAttack_->On();
				SkillGage = 0;
			}
		}
		else
		{
			//if (AEInputCheckCurr(AEVK_LBUTTON) && flameCool >= 3000)
			//{
			//	GameObject* flame_Attack = nullptr;
			//	flame_Attack = GameObjectManager::GetInstance().CreateObject("FlameAttack" + std::to_string(count));
			//	count++;
			//	flame_Attack->AddComponent<Flame>();
			//	flame_Attack->GetComponent<Flame>()->SetPlayer(owner_);
			//	curAttack_ = flame_Attack->GetComponent<Flame>();
			//	curAttack_->On();

			//	flameCool = 0;
			//}

			//if (AEInputCheckCurr(AEVK_LBUTTON) && flameCool >= 3000)
			//{
			//	GameObject* oflame_Attack = nullptr;
			//	oflame_Attack = GameObjectManager::GetInstance().CreateObject("oFlameAttack" + std::to_string(count));
			//	count++;
			//	oflame_Attack->AddComponent<oFlame>();
			//	oflame_Attack->GetComponent<oFlame>()->SetPlayer(owner_);
			//	curAttack_ = oflame_Attack->GetComponent<oFlame>();
			//	curAttack_->On();

			//	flameCool = 0;
			//}
		}
	}
	else if (7 <= level_ && level_ < 10)
	{
		TimeRecall->active_ = false;
		doubleflameCool += AEFrameRateControllerGetFrameRate();
		if (SkillGage >= maxSkillGage)
		{
			// 파이어 버블
			curAttack_ = nullptr;
			if (AEInputCheckCurr(AEVK_LBUTTON))
			{
				fire_bubble_Attack->GetComponent<bubble>()->SetPlayer(owner_);
				curAttack_ = fire_bubble_Attack->GetComponent<bubble>();
				curAttack_->On();
			}
		}
		else
		{
			if (AEInputCheckCurr(AEVK_LBUTTON) && doubleflameCool >= 3000)
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

				doubleflameCool = 0;
			}
		}
	}
	else
	{
		pendoubleflameCool += AEFrameRateControllerGetFrameRate();
		if (SkillGage >= maxSkillGage)
		{
			//메테오
			curAttack_ = meteor->GetComponent<Meteor>();
			curAttack_->On();
		}
		else
		{
			if (AEInputCheckCurr(AEVK_LBUTTON) && pendoubleflameCool >= 3000)
			{
				GameObject* doubleflameL_Attack = nullptr;
				doubleflameL_Attack = GameObjectManager::GetInstance().CreateObject("PendoubleFlameAttackL" + std::to_string(count));
				count++;
				doubleflameL_Attack->AddComponent<PenetrableDoubleFlameL>();
				doubleflameL_Attack->GetComponent<PenetrableDoubleFlameL>()->SetPlayer(owner_);
				curAttack_ = doubleflameL_Attack->GetComponent<PenetrableDoubleFlameL>();
				curAttack_->On();

				GameObject* doubleflameR_Attack = nullptr;
				doubleflameR_Attack = GameObjectManager::GetInstance().CreateObject("PendoubleFlameAttackR" + std::to_string(count));
				count++;
				doubleflameR_Attack->AddComponent<PenetrableDoubleFlameR>();
				doubleflameR_Attack->GetComponent<PenetrableDoubleFlameR>()->SetPlayer(owner_);
				curAttack_ = doubleflameR_Attack->GetComponent<PenetrableDoubleFlameR>();
				curAttack_->On();

				pendoubleflameCool = 0;
			}
		}
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

	/* SET ANIMATION */
	if (dir_ != curDir && (curDir == LEFT || curDir == RIGHT))
	{
		trans_->SetFlip();
		dir_ = curDir;
	}

	if (curState == ATTACK)
	{
		ani_->ChangeAnimation("Attack");
		ani_->SetTerm(50);
	}
	else if ((preState == ATTACK && ani_->CurrentAnimationOver()) || preState != ATTACK)
	{
		if (curState == RUN)
		{
			ani_->ChangeAnimation("Run");
			ani_->SetTerm(60);
		}
		else if (curState == HURT)
			sp_->SetColor({ 255, 0, 0 });
		else
		{
			ani_->ChangeAnimation("Idle");
			ani_->SetTerm(300);
		}
	}

	if (curState != HURT && preState != HURT)
		sp_->SetColor({ 0, 0, 0 });

	preState = curState;
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
	ParticleSystem::getPtr()->Update();

	maxExp_ += int(maxExp_ * expRequirement_ / 100);
	exp_ = 0;

	maxHp_ += int(maxHp_ * hpGrowthRate_ / 100);
	hp_ = maxHp_;

	if (level_ >= maxLevel_)
		return;

	level_++;

	if (level_ >= 4)
		maxSkillGage = 100;

	LevelUpEvent* event = new LevelUpEvent();
	event->from_ = owner_;
	event->level = level_;
	EventManager::GetInstance().AddEvent(event);
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
