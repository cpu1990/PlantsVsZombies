/**
 *Copyright (c) 2020 LZ.All Right Reserved
 *Author : LZ
 *Date: 2021.4.16
 *Email: 2117610943@qq.com
 */

#include "LZBalloonZombies.h"

BalloonZombies::BalloonZombies(Node* node)
{
	_node = node;
	_attackHeadSoundEffectType = 0;
	_attackBodySoundEffectType = 0;
	_bloodVolume = 200;
	_headShieldVolume = 0;
	_bodyShieldVolume = 0;
	_currentBloodVolume = 200;
	_currentHeadShieldVolume = 0;
	_currentBodyShieldVolume = 0;
	_speed = 40;
	_currentSpeed = 40;
	_isHaveShield = false;
	_zombiesType = ZombiesType::BalloonZombies;
	srand(time(nullptr));
}

BalloonZombies* BalloonZombies::create(Node* node)
{
	BalloonZombies* balloonZombies = new (std::nothrow) BalloonZombies(node);
	if (balloonZombies && balloonZombies->init())
	{
		balloonZombies->autorelease();
		return balloonZombies;
	}
	CC_SAFE_DELETE(balloonZombies);
	return nullptr;
}

void BalloonZombies::createZombie()
{
	zombieInit("BalloonZombies");

	setZombieAnimation("BalloonZombies_Normal");

	createZombieShadow();
}

void BalloonZombies::createPreviewZombie()
{
	zombieInit("BalloonZombies");

	setZombieAnimation("BalloonZombies_Normal");

	createZombieShadow();
}

void BalloonZombies::playZombieSoundEffect()
{
	Zombies::playZombieSoundEffect(rand() % 2 ? "groan6" : "sukhbir6");
}

void BalloonZombies::playZombiesAshesAnimation()
{
	const uniform_real_distribution<float>number(0.f, 0.4f);
	auto ashes = SkeletonAnimation::createWithData(_global->userInformation->getAnimationData().find("BalloonZombiesAsh")->second);
	ashes->setPosition(_zombiesAnimation->getPosition() + Vec2(0, -15));
	ashes->setLocalZOrder(_zombiesAnimation->getLocalZOrder());
	ashes->setTimeScale(0.6f + number(_random));
	ashes->setAnimation(0, "animation", false);
	ashes->update(0);
	_node->addChild(ashes);

	ashes->runAction(Sequence::create(DelayTime::create(5.f),
		CallFunc::create([ashes]()
		{
			ashes->removeFromParent();
		}), nullptr));

	setZombieAttributeForGameType(ashes);
}

bool BalloonZombies::getZombieIsEatPlants()
{
	return false;
}

bool BalloonZombies::getZombieIsCanBeAttack()
{
	return false;
}

void BalloonZombies::zombieInjuredEventUpdate()
{
	if (_currentBloodVolume <= _bloodVolume / 2.0f)  /* ��ʬѪ��С��һ��ֵ����ʬ���첲 */
	{
		setZombiePrimaryInjure();
	}
	if (_currentBloodVolume <= 0)   /* ��ʬѪ��С���㣬��ʬ���� */
	{
		setZombieSecondaryInjure();
	}
}

void BalloonZombies::setZombiePrimaryInjure()
{
	if (_bodyAnimationId == 1) /* ��ʬ��ʼ���첲 */
	{
		_zombiesAnimation->setAnimation(0, "BalloonZombies_Normal2", true);
		_bodyAnimationId = 2;

		zombieLoseArmAnimation("BalloonZombiesArm");
	}
}

void BalloonZombies::setZombieSecondaryInjure()
{
	if (_bodyAnimationId == 2)
	{
		_zombiesAnimation->setAnimation(0, "BalloonZombies_Die", false);
		_currentSpeed = 0; /* ֹͣ�˶� */
		_bodyAnimationId = 10;

		zombieLoseHeadAnimation("BalloonZombiesHead", 1.3f);
		playZombiesFillDownAnimation();
	}
}