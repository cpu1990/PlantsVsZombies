﻿/**
 *Copyright (c) 2021 LZ.All Right Reserved
 *Author : LZ
 *Date: 2021.2.09
 *Email: 2117610943@qq.com
 */

#include "LZIPauseQuiteLayer.h"
#include "LZIZombiesScene.h"
#include "LZIControlLayer.h"
#include "Scenes/MainMenuScene/LZMainMenu.h"
#include "Scenes/GameScenes/IZombies/GameScene/LZIZombiesData.h"
#include "Based/LZPlayMusic.h"
#include "Based/LZUserInformation.h"

IPauseQuitLayer::IPauseQuitLayer()
{
	_levelName = new char[32];
	strcpy(_levelName, "IZombiesData");
}

IPauseQuitLayer::~IPauseQuitLayer()
{
	if (_levelName)delete _levelName;
}

bool IPauseQuitLayer::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 180)))return false;

	createDialog();

	return true;
}

void IPauseQuitLayer::setRestart()
{
	CREATEDIALOG(
	    IControlLayer::beginNewGame();

	    _director->getScheduler()->setTimeScale(1.0f);
	    _director->replaceScene(IZombiesScene::create());

	    IZombiesData::getInstance()->createNewLevelDataDocument();
	    IZombiesData::getInstance()->removeLevelData(_levelName);
	)
}

void IPauseQuitLayer::setQuitGame()
{
	CREATEDIALOG(
	    _director->getScheduler()->setTimeScale(1.0f);

	    if (GameScene::isRunGameScene) /* 如果运行了游戏场景则保存 */
	    {
		    IZombiesData::getInstance()->createNewLevelDataDocument();
			IZombiesData::getInstance()->caveLevelData(_levelName, true);
	    }

	    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MainMenu::create()));
	)
}
