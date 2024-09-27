#include "doubleFlameL.h"
#include "../../Event/Event.h"
#include "../Monster.h"
#include "../../Utils/Utils.h"

doubleFlameL::doubleFlameL(GameObject* owner) : BaseAttack(owner)
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
	owner->GetComponent<Transform>()->SetScale({ 0, 0 });

	owner_->AddComponent<BoxCollider>();
	BoxCollider* col = owner_->GetComponent<BoxCollider>();
	col->SetLayer(Collider::P_ATTACK);
	col->SetType(Collider::AABB_TYPE);
	col->SetHandler(static_cast<EventEntity*>(this));
}

doubleFlameL::~doubleFlameL()
{
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Transform>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<RigidBody>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Sprite>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<AnimationComp>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<BoxCollider>())));

	// GameObject 삭제
	GameObjectManager::GetInstance().RemoveObject(owner_->GetName());
}


void doubleFlameL::Update()
{
	if (mode == set)
	{
		dmg_ = 0;

		// 플레이어의 위치를 정확히 참조
		AEVec2 playerPos = player_->GetComponent<Transform>()->GetPosition();

		// 좌측 발사체: 플레이어의 위치에서 일정한 간격만큼 왼쪽에 생성되도록 설정
		float offsetDistance = 30.0f;  // 좌우 간격
		AEVec2 perpOffset{ -dir.y, dir.x };  // 수직 벡터 방향으로 좌측 설정
		AEVec2 leftFlamePos = playerPos + (perpOffset * offsetDistance);

		// 왼쪽 발사체의 위치 설정
		owner_->GetComponent<Transform>()->SetPosition(leftFlamePos);

		// 마우스 위치를 가져와 월드 좌표로 변환
		s32 x, y;
		AEInputGetCursorPosition(&x, &y);
		AEVec2 mousePosF({ static_cast<float>(x), static_cast<float>(y) });
		AEVec2 worldMousePos = convert(mousePosF);

		if (!AEInputCheckCurr(AEVK_LBUTTON))
		{
			// 공격 방향 벡터 계산 (탱크에서 마우스까지의 방향)
			AEVec2 attackDir = worldMousePos - owner_->GetComponent<Transform>()->GetPosition();
			AEVec2Normalize(&dir, &attackDir);

			// 수직 벡터(90도 회전) 계산
			AEVec2 perpOffset;
			perpOffset.x = dir.y;  // 왼쪽 발사체는 수직 벡터 방향 반대로
			perpOffset.y = -dir.x;

			// 좌우 간격 설정 (왼쪽 발사체는 수직 방향으로 왼쪽으로 이동)
			float offsetDistance = 30.0f;  // 간격 조절
			AEVec2 leftFlamePos = owner_->GetComponent<Transform>()->GetPosition() + (perpOffset * offsetDistance);

			// 왼쪽 발사체의 위치 설정
			owner_->GetComponent<Transform>()->SetPosition(leftFlamePos);
			owner_->GetComponent<Transform>()->SetRotation(dir);  // 발사 방향으로 회전

			// 공격 모드 설정
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
			owner_->GetComponent<RigidBody>()->AddVelocity(dir * 300);  // 발사체 속도 적용
		}
		else
		{
			owner_->active_ = false;
			owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<doubleFlameL>())));
		}
	}
}

void doubleFlameL::LevelUp()
{
	dmg_ += int(dmg_ * dmgGrowthRate_ / 100);
}

void doubleFlameL::AttackObject()
{
}

ComponentSerializer* doubleFlameL::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<doubleFlameL>())
		std::cout << "doubleFlameL::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<doubleFlameL>();
}

void doubleFlameL::OnEvent(BaseEvent*)
{
}

void doubleFlameL::OnCollision(CollisionEvent* event)
{
	Monster* monster = event->from_->GetComponent<Monster>();
	if (monster)
	{
		player_->GetComponent<Player>()->SkillGage += 1;
		monster->ReserveDmg(dmg_);
		owner_->active_ = false;
		owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<doubleFlameL>())));
	}

	Boss1* boss = event->from_->GetComponent<Boss1>();
	if (boss)
	{
		player_->GetComponent<Player>()->SkillGage += 1;
		boss->ReserveDmg(dmg_);
		owner_->active_ = false;
		owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<doubleFlameL>())));
	}
}

void doubleFlameL::LoadFromJson(const json&)
{
}

json doubleFlameL::SaveToJson()
{
	return json();
}