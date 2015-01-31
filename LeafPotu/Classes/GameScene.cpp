#include "GameScene.h"
#include "GameManager.h"
#include "StartUpScene.h"
#include "TileableWorld.h"
#include "cocostudio/CocoStudio.h"
#include "ui\CocosGUI.h"
#include "Helpers.h"

USING_NS_CC;

using namespace cocos2d;
using namespace cocos2d::ui;

Scene* GameScene::scene()
{
    Scene *scene = Scene::createWithPhysics();
    GameScene *layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

	listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::potuTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(GameScene::potuTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::potuTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	

	mBgLayer = Layer::create();
	this->addChild(mBgLayer);

	mGameLayer = Layer::create();
	this->addChild(mGameLayer);

	mUILayer = Layer::create();
	this->addChild(mUILayer);

	auto gravityVec = b2Vec2(0, 0);
	mWorld = new b2World(gravityVec);
    
	tw = NULL;
    tw = new TileableWorld();
    tw->init(mBgLayer, mWorld);

	//Node* uiNode = CSLoader::createNode("MenuScene.csb");
	//this->addChild(uiNode);

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto windowSize = Director::getInstance()->getWinSize();


	//ui::Button* pauseButton = (ui::Button*)uiNode->getChildByName("BTN_pause");
	//pauseButton->addTouchEventListener(this, toucheventselector(GameManager::PauseGame));

	//ui::Text* labelScore = (ui::Text*)uiNode->getChildByName("LABEL_score");
	//labelScore->setText(rd::StringFromInt(mGameManager->Score));

#ifdef DEBUG_PHYSICS
	debugDraw = new GLESDebugDraw( PTM_RATIO );
	mWorld->SetDebugDraw(debugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_jointBit;
	//flags += b2Draw::e_aabbBit;
	//flags += b2Draw::e_pairBit;
	//flags += b2Draw::e_centerOfMassBit;
	debugDraw->SetFlags(flags);
#endif


	mGameManager = NULL;
	mGameManager = new GameManager();

	this->scheduleUpdate();

    return true;
}

void GameScene::onExit()
{
	if(mGameManager) delete mGameManager;
	if(tw) delete tw;
	delete mWorld;
	Layer::onExit();
}

void GameScene::update(float delta)
{
	tw->update(delta);

	static double UPDATE_INTERVAL = 1.0f/60.0f;
	static double MAX_CYCLES_PER_FRAME = 5;
	static double timeAccumulator = 0;
 
	timeAccumulator += delta;    
	if (timeAccumulator > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL)) {
		timeAccumulator = UPDATE_INTERVAL;
	}
	int32 velocityIterations = 6;
	int32 positionIterations = 6;
	while (timeAccumulator >= UPDATE_INTERVAL) {        
		timeAccumulator -= UPDATE_INTERVAL;        
		mWorld->Step(UPDATE_INTERVAL, velocityIterations, positionIterations);
		mWorld->ClearForces();
	}

#ifdef DEBUG_PHYSICS
	this->custdraw();
#endif
}


void GameScene::potuTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

bool GameScene::potuTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
	return true;
}

void GameScene::potuTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
}

void GameScene::potuTouchCanceled(cocos2d::Touch* touch, cocos2d::Event* event)
{
}



#ifdef DEBUG_PHYSICS
void GameScene::custdraw()
{
	GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
	kmGLPushMatrix();
	kmGLGetMatrix(KM_GL_MODELVIEW, &_modelViewMV);
	_customCommand.init(_globalZOrder);
	_customCommand.func = CC_CALLBACK_0(GameScene::onDraw, this);
	Director::getInstance()->getRenderer()->addCommand(&_customCommand);
	kmGLPopMatrix();  
}
void GameScene::onDraw()
{
    kmMat4 oldMV;
    kmGLGetMatrix(KM_GL_MODELVIEW, &oldMV);
    kmGLLoadMatrix(&_modelViewMV);
    mWorld->DrawDebugData();
    kmGLLoadMatrix(&oldMV);
}
#endif