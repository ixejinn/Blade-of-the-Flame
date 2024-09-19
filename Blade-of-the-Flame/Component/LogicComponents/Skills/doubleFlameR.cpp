#include "doubleFlameR.h"
#include "../../Event/Event.h"
#include "../Monster.h"

doubleFlameR::doubleFlameR(GameObject* owner) : BaseAttack(owner)
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

doubleFlameR::~doubleFlameR()
{
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Transform>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<RigidBody>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<Sprite>())));
	owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<AnimationComp>())));
	GameObjectManager::GetInstance().RemoveObject(owner_->GetName());
}

namespace
{
	AEVec2 convert(AEVec2 a)
	{
		AEVec2 worldPos;
		worldPos.x = a.x - windowWidth / 2;
		worldPos.y = -a.y + windowHeight / 2;
		AEVec2 pos;
		AEGfxGetCamPosition(&pos.x, &pos.y);
		AEVec2 result;
		result.x = worldPos.x + pos.x;
		result.y = worldPos.y + pos.y;
		return result;
	}
}

void doubleFlameR::Update()
{
	if (mode == set) {
		dmg_ = 0;

		// �÷��̾��� ��ġ�� ��Ȯ�� ����
		AEVec2 playerPos = player_->GetComponent<Transform>()->GetPosition();

		// ���� �߻�ü: �÷��̾��� ��ġ���� ������ ���ݸ�ŭ �����ʿ� �����ǵ��� ����
		float offsetDistance = 30.0f;  // �¿� ����
		AEVec2 perpOffset{ dir.y, -dir.x };  // ���� ���� �������� ���� ����
		AEVec2 rightFlamePos = playerPos + (perpOffset * offsetDistance);

		// ������ �߻�ü�� ��ġ ����
		owner_->GetComponent<Transform>()->SetPosition(rightFlamePos);

		// ���콺 ��ġ�� ������ ���� ��ǥ�� ��ȯ
		s32 x, y;
		AEInputGetCursorPosition(&x, &y);
		AEVec2 mousePosF({ static_cast<float>(x), static_cast<float>(y) });
		AEVec2 worldMousePos = convert(mousePosF);

		if (!AEInputCheckCurr(AEVK_LBUTTON))
		{
			// ���� ���� ���� ��� (��ũ���� ���콺������ ����)
			AEVec2 attackDir = worldMousePos - owner_->GetComponent<Transform>()->GetPosition();
			AEVec2Normalize(&dir, &attackDir);

			// ���� ����(90�� ȸ��) ���
			AEVec2 perpOffset;
			perpOffset.x = -dir.y;  // 90�� ȸ��
			perpOffset.y = dir.x;

			// �¿� ���� ���� (������ �߻�ü�� ���� �������� ���������� �̵�)
			float offsetDistance = 30.0f;  // ���� ����
			AEVec2 rightFlamePos = owner_->GetComponent<Transform>()->GetPosition() + (perpOffset * offsetDistance);

			// ������ �߻�ü�� ��ġ ����
			owner_->GetComponent<Transform>()->SetPosition(rightFlamePos);
			owner_->GetComponent<Transform>()->SetRotation(dir);  // �߻� �������� ȸ��

			// ���� ��� ����
			mode = fire;
			dmg_ = tempdmg;
			owner_->GetComponent<Transform>()->SetScale({ 80, 50 });
		}
	}

	if (mode == fire) {
		// �߻�ü�� ���ư��� ������ ����
		if (lifetime > 0) {
			float dt = AEFrameRateControllerGetFrameRate();
			lifetime -= dt;
			owner_->GetComponent<RigidBody>()->AddVelocity(dir * 300);  // �߻�ü�� �ӵ�
		}
		else {
			// �߻�ü�� ������ ���� ��� ó��
			owner_->active_ = false;
			owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<doubleFlameR>())));
		}
	}
}

void doubleFlameR::LevelUp()
{
	dmg_ += int(dmg_ * dmgGrowthRate_ / 100);
}

void doubleFlameR::AttackObject()
{
}

ComponentSerializer* doubleFlameR::CreateComponent(GameObject* owner)
{
	if (!owner->AddComponent<doubleFlameR>())
		std::cout << "doubleFlameR::CreateComponent() Component already exists" << std::endl;

	return owner->GetComponent<doubleFlameR>();
}

void doubleFlameR::OnEvent(BaseEvent*)
{
}

void doubleFlameR::OnCollision(CollisionEvent* event)
{
	Monster* monster = event->from_->GetComponent<Monster>();
	if (monster)
	{
		monster->ReserveDmg(dmg_);
		owner_->active_ = false;
		owner_->DeleteComponent(std::type_index(typeid(owner_->GetComponent<doubleFlameR>())));
	}
}

void doubleFlameR::LoadFromJson(const json&)
{
}

json doubleFlameR::SaveToJson()
{
	return json();
}