// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"

#include "Utils/Utils.h"
#include "Manager/GameStateManager.h"
#include "Manager/EventManager.h"
#include "State/SampleSave.h"
#include "State/SampleLoad.h"
// ---------------------------------------------------------------------------
//BOSS_TEST
#include "State/BossSample/BossSampleSave.h"
#include "State/BossSample/BossSampleLoad.h"
// ---------------------------------------------------------------------------
//MENU TEST
#include "State/UISample/MenuScene.h"
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	int gGameRunning = 1;

	// Initialization of your own variables go here

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, windowWidth, windowHeight, 1, 60, true, NULL);
	
	// Changing the window title
	AESysSetWindowTitle("Blade of the Flame");

	// reset the system modules
	AESysReset();

	SampleSave sampleSave;
	SampleLoad sampleLoad;
	//GameStateManager& gsm = GameStateManager::GetInstance();
	//gsm.ChangeState(&sampleSave);
	// ---------------------------------------------------------------------------
	//BOSS_TEST
	BossSampleSave bossSampleSave;
	BossSampleLoad bossSampleLoad;

	GameStateManager& gsm = GameStateManager::GetInstance();
	// ---------------------------------------------------------------------------
	//MENU_TEST
	MenuScene menuScene;


	gsm.ChangeState(&bossSampleSave);
	
	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		AESysFrameStart();


		// Your own update logic goes here


		// Your own rendering logic goes here
		gsm.Update();

		// Informing the system about the loop's end
		AESysFrameEnd();

		// check if forcing the application to quit
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;
	}
	// free the system
	AESysExit();
}