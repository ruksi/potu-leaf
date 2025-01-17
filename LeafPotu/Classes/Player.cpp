#include "Player.h"
#include "standards.h"
#include "Helpers.h"
#include "SimpleAudioEngine.h"
#include "GameManager.h"

USING_NS_CC;

using namespace CocosDenshion;

Player::Player() { }
Player::~Player() 
{
	std::vector<ForceNode*>::iterator it;
	for(it = mForceNodes.begin(); it < mForceNodes.end(); it++)
	{
		ForceNode* iNode = *it;
		delete iNode;
	}
}

void Player::init(cocos2d::Layer* layer, b2World* world)
{
    maxEnergy = 20.0f;
    currentEnergy = maxEnergy;
    deathHandled = false;
    
    auto dir = Director::getInstance();
    auto screen = dir->getWinSize();
    
    b2BodyDef bd;
    bd.position = b2Vec2(screen.width/2/PTM_RATIO, screen.height/2/PTM_RATIO);
    bd.type = b2BodyType::b2_dynamicBody;
    bd.linearDamping = 0.95f;
    bd.angularDamping = 0.5f;
    bd.fixedRotation = false;
    mBody = world->CreateBody(&bd);
    
    b2CircleShape shape;
    shape.m_radius = 0.15f;
    b2FixtureDef fd;
    fd.shape = &shape;
    fd.restitution = 0.9f;
    fd.density = 0.75f;
    fd.friction = 0.5f;
    mBody->CreateFixture(&fd);
    
    mRoot = Node::create();
    mRoot->setPosition(Vec2(mBody->GetPosition().x*PTM_RATIO, mBody->GetPosition().y*PTM_RATIO));
    layer->addChild(mRoot);
    
	mSprite = Sprite::createWithSpriteFrameName("leaf.png");
    mSprite->setPosition(Vec2(0, 0));
    mRoot->addChild(mSprite);
}

Sprite* Player::getSprite()
{
	return mSprite;
}

void Player::updateLeafColor()
{
    // Change leaf color between these values:
    //mSprite->setColor(Color3B(60, 220, 60));
    //mSprite->setColor(Color3B(175, 80, 30));
    float ee = (currentEnergy / maxEnergy);
    mSprite->setColor(Color3B(
        (1.0f - ee) * 175.0f + ee * 60.0f,
        (1.0f - ee) * 80.0f + ee * 220.0f,
        (1.0f - ee) * 30.0f + ee * 60.0f));
}

bool Player::isDead()
{
    return (currentEnergy < 0.0f);
}

void Player::changeEnergy(float amount)
{
    currentEnergy = currentEnergy + amount;
    if (currentEnergy > maxEnergy) {
        currentEnergy = maxEnergy;
    }
}

void Player::update(float delta)
{
    if (deathHandled) return;
    currentEnergy = currentEnergy - delta;
    if (currentEnergy < 0.0f && !deathHandled) {
        deathHandled = true;
        return;
    }
    updateLeafColor();
    
    auto velocity = mBody->GetLinearVelocity();
    mBody->SetLinearVelocity(b2Vec2(velocity.x, velocity.y - 0.01f));
    if (mRoot)
    {
        auto pos = mBody->GetPosition();
        mRoot->setPosition(pos.x*PTM_RATIO, pos.y*PTM_RATIO);
        mRoot->setRotation(-rd::RadToDeg(mBody->GetAngle()));
    }

	// Update force timers
	if (!mForceNodes.empty())
	{
		std::vector<ForceNode*>::iterator it;
		for(it = mForceNodes.begin(); it < mForceNodes.end(); it++)
		{
			ForceNode* iNode = *it;
			iNode->timer = iNode->timer - delta;
		}
	}

	// Update force triggers
	if (!mForceNodes.empty())
	{
		std::vector<ForceNode*>::iterator it;
		for(it = mForceNodes.begin(); it < mForceNodes.end(); it++)
		{
			ForceNode* iNode = *it;
			if(iNode->timer < 0)
			{
				mBody->ApplyForceToCenter(iNode->force, true);
				mForceNodes.erase(it);
				delete iNode;
				break;
			}
		}
	}
}

void Player::moveInResponseToTouchAt(cocos2d::Vec2 coordinates)
{
    auto maxInteractionDistance = 600.0f;
    auto distance = mRoot->getPosition().distance(coordinates);
    float distanceFactor;
    if (distance > maxInteractionDistance) {
        distanceFactor = 0.0f;
    }
    else {
        auto t = (distance / maxInteractionDistance);
        if (t > 1.0f) { t = 1.0f; }
        t = (1.0f - t);
        distanceFactor = t;
    }
    
    auto difference = coordinates - mRoot->getPosition();
    
    float distanceX = abs(difference.x);
    auto tt = (distanceX / maxInteractionDistance);
    if (tt > 1.0f) { tt = 1.0f; }
    if (tt < -1.0f) { tt = -1.0f; }
    tt = (1.0f - tt);
    auto distanceFactorX = tt;
    if (difference.x < 0) {
        distanceFactorX = -distanceFactorX;
    }
    
    float distanceY = abs(difference.y);
    auto ttt = (distanceY / maxInteractionDistance);
    if (ttt > 1.0f) { ttt = 1.0f; }
    if (ttt < -1.0f) { ttt = -1.0f; }
    ttt = (1.0f - ttt);
    auto distanceFactorY = ttt;
    if (difference.y < 0) {
        distanceFactorY = -distanceFactorY;
    }
    
    auto maxForceX = 5.0f;
    auto maxForceY = 5.0f;
    auto forceX = -(maxForceX * distanceFactorX * distanceFactor);
    auto forceY = -(maxForceY * distanceFactorY * distanceFactor);

    float maxTriggerTime = 1.0f;
    float triggerTime;
    if (distanceFactor > 0.7f) {
        triggerTime = 0.0f;
        int r = rd::RandInt(0, 1);
        if (r == 0) {
            SimpleAudioEngine::getInstance()->playEffect("Audio/ripple_iso.mp3");
        }
        else {
            SimpleAudioEngine::getInstance()->playEffect("Audio/ripple2.mp3");
        }
    }
    else {
        triggerTime = (maxTriggerTime * (1.0f - distanceFactor));
        SimpleAudioEngine::getInstance()->playEffect("Audio/ripple_pieni.mp3");
    }
	this->addForceToQueue(b2Vec2(forceX, forceY), triggerTime);
    
    CCLOG("Ripple: x = %4.2f y = %4.2f t = %4.2f", forceX, forceY, triggerTime);
    
    int torqueSign = 1;
    if (difference.x < 0) {
        torqueSign = -1;
    }
    mBody->ApplyTorque(0.02f * torqueSign, true);
}

void Player::addForceToQueue(b2Vec2 forceVec, float timeToTrigger)
{
	ForceNode* newForceNode = new ForceNode();
	newForceNode->timer = timeToTrigger;
	newForceNode->force = forceVec;
	mForceNodes.push_back(newForceNode);
}