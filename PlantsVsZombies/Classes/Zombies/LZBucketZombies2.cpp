﻿/**
 *Copyright (c) 2021 LZ.All Right Reserved
 *Author : LZ
 *Date: 2021.4.21
 *Email: 2117610943@qq.com
 */

#include "LZBucketZombies2.h"

BucketZombies2::BucketZombies2(Node* node)
{
	_node = node;
	_attackHeadSoundEffectType = 1;
	_attackBodySoundEffectType = 0;
	_bloodVolume = 200;
	_headShieldVolume = 1400;
	_bodyShieldVolume = 0;
	_currentBloodVolume = 200;
	_currentHeadShieldVolume = 1400;
	_currentBodyShieldVolume = 0;
	_speed = 40;
	_currentSpeed = 40;
	_isHaveShield = true;
	_headShieldType = ShieldType::IronHeadShield;
	_zombiesType = ZombiesType::BucketZombies2;
}

BucketZombies2* BucketZombies2::create(Node* node)
{
	BucketZombies2* bucketZombies2 = new (std::nothrow) BucketZombies2(node);
	if (bucketZombies2 && bucketZombies2->init())
	{
		bucketZombies2->autorelease();
		return bucketZombies2;
	}
	CC_SAFE_DELETE(bucketZombies2);
	return nullptr;
}

void BucketZombies2::createZombie()
{
	zombieInit();

	setZombieAnimation(rand() % 3 + 2);

	setZombieScale();

	createZombieShadow();

	_zombiesAnimation->setAttachment("Zombie_bucket", "6");
}

void BucketZombies2::createPreviewZombie()
{
	zombieInit();

	setZombieAnimation(rand() % 2);

	setZombieScale();

	createZombieShadow();

	_zombiesAnimation->setAttachment("Zombie_bucket", "6");
}

void BucketZombies2::zombieInjuredEventUpdate()
{
	if (_isHaveShield)/* 如果有头部护盾 */
	{
		if (_currentHeadShieldVolume <= _headShieldVolume * 2.0f / 3.0f) /* 头部护盾一级损伤 */
		{
			setZombieHeadShieldPrimaryInjure("Zombie_bucket", "7");
		}
		if (_currentHeadShieldVolume <= _headShieldVolume / 3.0f)       /* 头部护盾二级损伤 */
		{
			setZombieHeadShieldSecondaryInjure("Zombie_bucket", "8");
		}
		if (_currentHeadShieldVolume <= 0)                              /* 头部护盾消失 */
		{
			setZombieHeadShieldThirdInjure("Zombie_bucket", "tt_innerleg_foot3");
		}
	}
	if (_currentBloodVolume <= _bloodVolume / 2.0f)   /* 僵尸血量小于一定值，僵尸掉胳膊 */
	{
		setZombiePrimaryInjure();
	}
	if (_currentBloodVolume <= 0)                     /* 僵尸血量小于零，僵尸死亡 */
	{
		setZombieSecondaryInjure();
	}
}

void BucketZombies2::setZombieHeadShieldThirdInjure(const string& oldName, const string& newName)
{
	if (_headShieldAnimationId == 3) /* 护盾三级伤害 */
	{
		_zombiesAnimation->setAttachment("Zombie_bucket", "tt_innerleg_foot3");
		_headShieldAnimationId = 4;
		_isHaveShield = false;
		_attackHeadSoundEffectType = 0;
		_headShieldType = ShieldType::none;

		/* 僵尸掉护盾 */
		zombieLoseShieldAnimation("Zombie_bucket2", 1.f);
	}
}