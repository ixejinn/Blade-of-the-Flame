#include "Player.h"

#include <typeindex>
#include <string>
#include "AEGraphics.h"
#include "../../Event/Event.h"
#include "../../Manager/EventManager.h"
#include "../../Manager/GameObjectManager.h"
#include "../../Utils/Utils.h"

Player::Player(GameObject* owner) : LogicComponent(owner)
{
	timeStart_ = std::chrono::system_clock::now();

	/* Set Player component */
	owner_->AddComponent<BoxCollider>();
	owner_->AddComponent<Sprite>();
	owner_->AddComponent<PlayerController>();
	owner_->AddComponent<Audio>();
	owner_->AddComponent<Text>();

	PlayerController* pCtrl = owner_->GetComponent<PlayerController>();
	pCtrl->SetRotKeys(PlayerController::LEFT, AEVK_Q);
	pCtrl->SetRotKeys(PlayerController::RIGHT, AEVK_E);
	pCtrl->SetStopKey(AEVK_SPACE);

	trans_ = owner_->GetComponent<Transform>();
	trans_->SetScale({ 30, 100 });
	owner_->GetComponent<PlayerController>()->MultiplyMoveSpeed(moveSpeed_);
	owner_->GetComponent<RigidBody>()->SetUseAcceleration(false);
	owner_->GetComponent<Sprite>()->SetColor({ 200, 200, 200 });
	owner_->GetComponent<Audio>()->SetAudio("Assets/bouken.mp3");

	text_ = owner_->GetComponent<Text>();
	text_->SetFont("Assets/Roboto-Bold.ttf");
	text_->SetSize(1.f);
	text_->SetColor({ 255, 0, 0 });
	text_->SetPosition({ -0.05f, 0.1f });

	/* BASIC ATTACK GameObject */
	meleeAttack_ = GameObjectManager::GetInstance().CreateObject("playerMeleeAttack");
	meleeAttack_->AddComponent<MeleeAttack>();
	meleeAttack_->GetComponent<MeleeAttack>()->SetPlayer(owner_);
	//rangedAttack_ = GameObjectManager::GetInstance().CreateObject("playerRangedAttack");

	curAttack_ = meleeAttack_->GetComponent<MeleeAttack>();

	EventManager::GetInstance().RegisterEntity(std::type_index(typeid(CollisionEvent)), static_cast<EventEntity*>(this));
	EventManager::GetInstance().RegisterEntity(std::type_index(typeid(MonsterAttackPlayer)), static_cast<EventEntity*>(this));
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

	// Death
	if (hp_ <= 0)
	{
		std::cout << "Game over" << std::endl;
		GameOverEvent* event{ new GameOverEvent() };
		event->from_ = owner_;
		EventManager::GetInstance().AddEvent(event);
		owner_->active_ = false;
	}

	/* SET CAMERA */
	Transform* trans = owner_->GetComponent<Transform>();
	AEVec2 pos = trans->GetPosition();
	AEGfxSetCamPosition(pos.x, pos.y);

	/* ATTACK */
	std::chrono::duration<double> dt = std::chrono::system_clock::now() - timeStart_;
	if (dt.count() >= curAttack_->GetCooldown() && AEInputCheckCurr(AEVK_LBUTTON))
	{
		timeStart_ = std::chrono::system_clock::now();

		//std::cout << x << ", " << y << std::endl;
		curAttack_->AttackObject();
	}
	else
		GameObjectManager::GetInstance().GetObjectA("playerMeleeAttack")->active_ = false;

	text_->SetString(std::to_string(hp_) + "/" + std::to_string(maxHp_));
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

	// Collision event
	if (eventType == std::type_index(typeid(CollisionEvent)))
	{
		CollisionEvent* colEvent = static_cast<CollisionEvent*>(event);

		//// TODO: 해당 부분 Collision system 수정 필요
		if (colEvent->to_ != owner_ ||
			colEvent->attackMonster)
			return;

		//hp -= colEvent->from_->GetComponent<Monster>()->
	}
	else if (eventType == std::type_index(typeid(MonsterAttackPlayer)))
	{
		MonsterAttackPlayer* ev = static_cast<MonsterAttackPlayer*>(event);
		hp_ -= ev->dmg;
	}
}

void Player::LevelUp()
{
	if (level_ >= maxLevel_)
		return;

	level_++;
	exp_ = 0;

	maxHp_ += int(maxHp_ * hpGrowthRate_ / 100);
	hp_ = maxHp_;
	
	//if (level_ == 3)
	//	curAttack_ = rangedAttack_;
}

ComponentSerializer* Player::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<Player>())
		std::cout << "Transform::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<Player>();
}
