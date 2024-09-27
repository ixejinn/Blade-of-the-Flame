#include "SkillManager.h"

#include "../Component/LogicComponents/Skills/Meteor.h"
#include "../Component/LogicComponents/Skills/Flame.h"
#include "../Component/LogicComponents/Skills/doubleFlameL.h"
#include "../Component/LogicComponents/Skills/doubleFlameR.h"
#include "../Component/LogicComponents/Skills/PenetrableDoubleFlameL.h"
#include "../Component/LogicComponents/Skills/PenetrableDoubleFlameR.h"
#include "../Component/LogicComponents/Skills/Shield.h"
#include "../Component/LogicComponents/Skills/boomerang.h"
#include "../Component/LogicComponents/Skills/bubble.h"
#include "../Component/LogicComponents/Skills/gtae.h"
#include "../Component/LogicComponents/Skills/grab.h"
#include "../Component/LogicComponents/Skills/Recall.h"
#include "../Component/LogicComponents/Skills/oppositeFlame.h"
#include "../Component/LogicComponents/Skills/Melee2.h"
#include "../Component/LogicComponents/Skills/Pet.h"
#include "../Component/LogicComponents/Skills/petBullet.h"

SkillManager* SkillManager::ptr = nullptr;

SkillManager::SkillManager()
{
	/*****Gtae*****/
	std::vector<GameObject*> gtae_Attack;
	for (int i = 0; i < 3; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("gtae" + std::to_string(i));
		p->AddComponent<Gtae>();
		gtae_Attack.push_back(p);
	}

	/*****DoubleFlame*****/
	std::vector<GameObject*> doubleFlameL_Attack;
	for (int i = 0; i < 3; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("doubleflamel" + std::to_string(i));
		p->AddComponent<doubleFlameL>();
		doubleFlameL_Attack.push_back(p);
	}
	std::vector<GameObject*> doubleFlameR_Attack;
	for (int i = 0; i < 3; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("doubleflamer" + std::to_string(i));
		p->AddComponent<doubleFlameR>();
		doubleFlameR_Attack.push_back(p);
	}

	/*****PentrableDoubleFlame*****/
	std::vector<GameObject*> Pentrable_doubleFlameL_Attack;
	for (int i = 0; i < 3; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("pentrabledoubleflamel" + std::to_string(i));
		p->AddComponent<PenetrableDoubleFlameL>();
		Pentrable_doubleFlameL_Attack.push_back(p);
	}
	std::vector<GameObject*> Pentrable_doubleFlameR_Attack;
	for (int i = 0; i < 3; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("pentrabledoubleflamer" + std::to_string(i));
		p->AddComponent<PenetrableDoubleFlameR>();
		Pentrable_doubleFlameR_Attack.push_back(p);
	}

	/*****OppositeFlame*****/
	std::vector<GameObject*> oppositeFlame_Attack;
	for (int i = 0; i < 3; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("oppositeflameattack" + std::to_string(i));
		p->AddComponent<oFlame>();
		oppositeFlame_Attack.push_back(p);
	}

	/*****PetBullet*****/
	std::vector<GameObject*> petbullet_Attack;
	for (int i = 0; i < 6; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("petbullet" + std::to_string(i));
		p->AddComponent<PB>();
		petbullet_Attack.push_back(p);
	}

	/*****MeleeAttack*****/
	std::vector<GameObject*> petbullet_Attack;
	for (int i = 0; i < 1; i++)
	{
		GameObject* p = GameObjectManager::GetInstance().CreateObject("petbullet" + std::to_string(i));
		p->AddComponent<PB>();
		petbullet_Attack.push_back(p);
	}

	skills.push_back(std::make_pair("GT", gtae_Attack));
	skills.push_back(std::make_pair("DFL", doubleFlameL_Attack));
	skills.push_back(std::make_pair("DFR", doubleFlameR_Attack));
	skills.push_back(std::make_pair("PDFL", Pentrable_doubleFlameL_Attack));
	skills.push_back(std::make_pair("PDFR", Pentrable_doubleFlameR_Attack));
	skills.push_back(std::make_pair("OF", oppositeFlame_Attack));
	skills.push_back(std::make_pair("PB", petbullet_Attack));
}
