#include "SceneFactory.h"

#include "TitleScene.h"
#include "GamePlayScene.h"
#include "GameoverScene.h"
#include "Clear.h"

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	//次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName=="TITLE")
	{
		newScene = new TitleScene();
	}
	else if (sceneName == "GAMEPLAY")
	{
		newScene = new GamePlayScene();
	}
	else if (sceneName=="GAMEOVER")
	{
		newScene = new GameoverScene();
	}
	else if (sceneName == "CLEAR")
	{
		newScene = new Clear();
	}
	return newScene;
}