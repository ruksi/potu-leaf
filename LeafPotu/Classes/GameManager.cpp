#include "GameManager.h"
#include "GameScene.h"
#include "MenuScene.h"
#include "Player.h"
#include "Helpers.h"
#include "PauseScene.h"
#include <SimpleAudioEngine.h>

using namespace cocos2d;


GameManager::GameManager()
{
}
GameManager::~GameManager()
{
}


void GameManager::init(cocos2d::Layer* gameLayer, TileableWorld* tileableWorld, b2World* physWorld)
{
	this->gameLayer = gameLayer;
	this->tileableWorld = tileableWorld;
	this->physWorld = physWorld;
    player = new Player();
	if (CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume() == 0.0f)
	{
		Muted = true;
	}
	Paused = false;
    player->init(this->gameLayer, this->physWorld);
}

void GameManager::update(float delta)
{
    player->update(delta);
	
}

void GameManager::InputCoordinates(Vec2 coordinates)
{
	// If hits water, apply force to player.
    player->moveInResponseToTouchAt(coordinates);
}


void GameManager::PauseGame(bool Pause)
{
	if (Pause == true)
	{
		// Pause Game
		Director::getInstance()->pushScene(PauseScene::scene());
	}
	else
	{
		// Un-pause Game
		Director::getInstance()->popScene();
	}
}

void GameManager::MuteGame()
{
	if (Muted == true)
	{
		Muted = false;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0);
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(1.0);
	}
	else
	{
		Muted = true;
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0);
		CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.0);
	}
}

void GameManager::ResetGame()
{
	Director::getInstance()->replaceScene(GameScene::scene());
}

void GameManager::EndGame()
{

}

