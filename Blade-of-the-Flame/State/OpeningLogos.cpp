#include "OpeningLogos.h"

#include "../Manager/GameObjectManager.h"
#include "../Manager/GameStateManager.h"
#include "../State/MainMenu.h"

namespace Manager
{
	extern GameObjectManager& objMgr;
	extern GameStateManager& gsMgr;
}

void OpeningLogos::Init()
{
	digiLogo = Manager::objMgr.CreateObject("digiLogo");
	digiLogo->AddComponent<Transform>();
	//digiLogo->GetComponent<Transform>()->SetScale({ 505, 122 });
	digiLogo->GetComponent<Transform>()->SetScale({ 1500, 362 });
	// 
	//digiLogo->GetComponent<Transform>()->SetScale({ 2101 / 2.f, 506 / 2.f });
	digiLogo->AddComponent<AnimationComp>();
	digiLogo->GetComponent<Sprite>()->SetTexture("Assets/Logo/DigiPen_RGB_Red.jpg");
	digiAni = digiLogo->GetComponent<AnimationComp>();
	digiAni->SetType(AnimationComp::APPEAR);
	digiAni->SetTerm(60);
	//digiAni->SetTerm(100);

	teamLogo = Manager::objMgr.CreateObject("teamLogo");
	teamLogo->AddComponent<Transform>();
	teamLogo->GetComponent<Transform>()->SetScale({ 753.8, 130.6 });
	teamLogo->AddComponent<AnimationComp>();
	teamLogo->GetComponent<Sprite>()->SetTexture("Assets/Logo/TeamLogo.png");
	teamAni = teamLogo->GetComponent<AnimationComp>();
	teamAni->SetType(AnimationComp::APPEAR);
	teamAni->SetTerm(60);
	teamLogo->active_ = false;
}

void OpeningLogos::Update()
{
	if (digiLogo->active_)
	{
		AnimationComp::Type digiAniType = digiAni->GetType();

		if (digiAniType == AnimationComp::APPEAR && digiAni->CurrentAnimationOver())
			digiAni->SetType(AnimationComp::DISAPPEAR);
		else if (digiAniType == AnimationComp::DISAPPEAR && digiAni->CurrentAnimationOver())
		{
			teamLogo->active_ = true;
			digiLogo->active_ = false;
		}
	}
	else if (teamLogo->active_)
	{
		AnimationComp::Type teamAniType = teamAni->GetType();

		if (teamAniType == AnimationComp::APPEAR && teamAni->CurrentAnimationOver())
		{
			static double elapsedTime = 0.0;
			double dt = AEFrameRateControllerGetFrameRate();
			elapsedTime += dt;

			if (elapsedTime >= 2200)
				teamAni->SetType(AnimationComp::DISAPPEAR);
			else if (elapsedTime >= 1000)
				AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
		}
		else if (teamAniType == AnimationComp::DISAPPEAR && teamAni->CurrentAnimationOver())
		{
			MainMenu* mainMenu = new MainMenu();
			Manager::gsMgr.ChangeState(mainMenu);
		}
	}
}

void OpeningLogos::Exit()
{
	Manager::objMgr.Clear();
}
