#include "TileableWorld.h"
#include "GameManager.h"

using namespace cocos2d;

TileableWorld::TileableWorld() { }

TileableWorld::~TileableWorld()
{
	std::vector<WorldTile*>::iterator it;
	for (it = mTiles.begin(); it < mTiles.end(); it++)
	{
		WorldTile* iTile = *it;
		delete iTile;
	}
}

void TileableWorld::init(GameManager* gameManager)
{
    this->gameManager = gameManager;
	mOffset = 0;
	mTileIndex = 0;
	this->generateTiles();
}

void TileableWorld::generateTiles()
{
	for (int i = 0; i < 50; i++)
	{
		WorldTile* newTile = new WorldTile();
		newTile->generate(gameManager);
		newTile->cacheToPool();
		mTiles.push_back(newTile);
	}

	for (int i = 0; i < 3; i++)
	{
		mVisibleTiles[i] = mTiles[i];
		mVisibleTiles[i]->uncacheFromPool();
		mTileIndex++;
	}
}

void TileableWorld::update(float delta)
{
	mOffset = mOffset + 300*delta;
	if (mOffset > 4096)
	{
		mOffset = 0;
		this->stepTiles();
	}
	for (int i = 0; i < 3; i++)
	{
		mVisibleTiles[i]->update(0, -mOffset + (4096.0f*i), delta);
	}
}

void TileableWorld::stepTiles()
{
	mTileIndex++;
	if (mTileIndex >= 50) mTileIndex = 0;

	mVisibleTiles[0]->cacheToPool();
	mVisibleTiles[0] = mVisibleTiles[1];
	mVisibleTiles[1] = mVisibleTiles[2];

	mVisibleTiles[2] = mTiles[mTileIndex];
	mVisibleTiles[2]->uncacheFromPool();
}
