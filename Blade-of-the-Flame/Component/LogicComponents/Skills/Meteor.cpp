#include "Meteor.h"
#include "../../Event/Event.h"
#include "../../Manager/EventManager.h"
#include "../../AnimationComp.h"
#include "MeteorExplosion.h"
#include <iostream>
#include "AEUtil.h"

Meteor::Meteor(GameObject* owner) : BaseAttack(owner)
{
	spin = 0;
	dmg_ = 20;
	range_ = 700.f;
	cooldown_ = 10;
	dmgGrowthRate_ = 10.f;
	cState = df;
	owner->AddComponent<Transform>();
	owner->AddComponent<RigidBody>();
	owner->AddComponent<Sprite>();
	owner->AddComponent<AnimationComp>();
	owner->GetComponent<AnimationComp>()->AddDetail("Assets/MeteorAnime/meteor.png", "Attack");
	owner->GetComponent<AnimationComp>()->AddDetail("Assets/MeteorAnime/MagicCircle.png", "Idle");
	owner->GetComponent<AnimationComp>()->SetTerm(200.0f);
	owner->GetComponent<RigidBody>()->ClearVelocity();
	owner_->GetComponent<Transform>()->SetScale({ range_, range_ });
}

void Meteor::Update()
{
	if (!AEInputCheckCurr(AEVK_LBUTTON) && cState == df)
		cState = ready;
	if (cState == ready)
	{
		AEInputInit();
		s32 x, y;
		AEInputGetCursorPosition(&x, &y);
		AEVec2 mousePosF({ static_cast<float>(x), static_cast<float>(y) });
		owner_->GetComponent<Transform>()->SetScale({ 300, 300 });
		convertPos = convert(mousePosF);
		owner_->GetComponent<Transform>()->SetPosition(convertPos);
		//MagicCircle mode
		owner_->GetComponent<AnimationComp>()->ChangeAnimation("Idle");
		if (AEInputCheckCurr(AEVK_LBUTTON))
		{
			owner_->GetComponent<Transform>()->SetPosition(convertPos.x + startingOffset.x, convertPos.y + startingOffset.y);
			//Meteor mode
			owner_->GetComponent<AnimationComp>()->ChangeAnimation("Attack");
			cState = shoot;
			meteorLifetime = 11000;
		}
	}
	else if (cState == shoot)
	{
		AEVec2 attackDir{ convertPos -  owner_->GetComponent<Transform>()->GetPosition()}, unitDir;
		AEVec2Normalize(&unitDir, &attackDir);
		owner_->GetComponent<Transform>()->SetRotation(spin / 10);
		spin += AEFrameRateControllerGetFrameRate();
		owner_->GetComponent<RigidBody>()->AddVelocity(unitDir * 200);

		meteorLifetime -= AEFrameRateControllerGetFrameRate();
		if (meteorLifetime <= 2000)
		{
			if (meteorLifetime <= 0)
			{
				AttackObject();
				owner_->GetComponent<RigidBody>()->ClearVelocity();
				cState = df;
				owner_->active_ = false;
			}
			else
			{
				owner_->GetComponent<Transform>()->SetScale
				({ float(owner_->GetComponent<Transform>()->GetScale().x - AEFrameRateControllerGetFrameRate() / 10),
					float(owner_->GetComponent<Transform>()->GetScale().y - AEFrameRateControllerGetFrameRate() / 10) });
			}
		}
	}
}

void Meteor::LevelUp()
{

}

void Meteor::AttackObject()
{
	GameObjectManager::GetInstance().GetObjectA("player")->GetComponent<Player>()->SkillGage = 0;
	GameObject* p = GameObjectManager::GetInstance().CreateObject();
	p->AddComponent<MeteorExplosion>();
	p->GetComponent	<MeteorExplosion>()->SetPlayer(owner_);
	p->active_ = true;
}

void Meteor::LoadFromJson(const json&)
{
}

json Meteor::SaveToJson()
{
	return json();
}

ComponentSerializer* Meteor::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<Meteor>())
		std::cout << "Meteor::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<Meteor>();
}
