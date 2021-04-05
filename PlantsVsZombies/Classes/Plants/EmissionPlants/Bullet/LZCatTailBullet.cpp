/**
 *Copyright (c) 2021 LZ.All Right Reserved
 *Author : LZ
 *Date: 2021.2.7
 *Email: 2117610943@qq.com
 */

#include "LZCatTailBullet.h"
#include "Zombies/LZZombies.h"
#include "Scenes/GameScenes/Adventure/GameScene/LZAGSData.h"
#include "Scenes/GameScenes/Adventure/GameScene/LZAGSControlLayer.h"
#include <cocos/editor-support/spine/extension.h>

CatTailBullet::CatTailBullet(Node* node):
    _targetZombie(nullptr),
    _distance(0xffff),
    _speed{ 10,0 },
    _distanceY(69),
    _locationY(0),
    _isTrack(false)
{
    _node = node;
    _attack = 20;
    _bulletType = BulletType::CatTailBullet;
}

CatTailBullet::~CatTailBullet()
{
}

void CatTailBullet::createBullet()
{
    bulletInit("CatTailBullet", "CatTailBullet_Normal");
    _bulletAnimation->setPosition(_position + Vec2(70, 85));
    _bulletAnimation->setScale(0.6f);
    _bulletAnimation->setLocalZOrder(_bulletAnimation->getLocalZOrder() - 100);
    //createShadow();
    createListener();
}

void CatTailBullet::createListener()
{
    _locationY = _bulletAnimation->getPositionY();
    _bulletAnimation->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.02f),
        CallFunc::create([=]()
            {
                if (fabs(_locationY - _bulletAnimation->getPositionY()) > _distanceY) /* ��һ�μ����������localZorder */
                {
                    auto order = _bulletAnimation->getLocalZOrder();
                    if (order > 0 && order < 5070)
                    {
                        _bulletAnimation->setLocalZOrder(order + (_locationY > _bulletAnimation->getPositionY() ? 100 : -100));
                    }
                    _locationY = _bulletAnimation->getPositionY();

                    _distanceY = 138; /* ��һ��֮��ÿ��138����һ�� */
                }
            }), nullptr)));

    _bulletAnimation->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.016f),
        CallFunc::create([this]() { calculateBulletPosition(); }), nullptr)));
}

void CatTailBullet::createShadow()
{ 
    auto shadow = Sprite::createWithSpriteFrameName("plantshadow.png");
    shadow->setOpacity(180);
    shadow->setName("shadow");
    shadow->setIgnoreAnchorPointForPosition(true);
    shadow->setScale(0.6f);
    shadow->setPosition(Vec2(0, -120));
    shadow->setLocalZOrder(_bulletAnimation->getLocalZOrder());
    _bulletAnimation->addChild(shadow);
    shadow->runAction(RepeatForever::create(Sequence::create(
        CallFunc::create([=]()
            {
                shadow->setPosition(Vec2(0, -120));
            }), DelayTime::create(0.02f), nullptr)));
}

void CatTailBullet::bulletAndZombiesCollision()
{
    for (auto zombie : ZombiesGroup)
    {
        if (!_isUsed && zombie->getZombieIsSurvive() &&                              /* û�б�ʹ�� &&  ��ʬû������ */
            zombie->getZombieIsEnterMap() && getBulletIsEncounterWithZombie(zombie)) /* ��ʬ�����ͼ�� && �㶹�뽩ʬ��ײ */
        {
            selectSoundEffect(zombie->getZombieBodyAttackSoundEffect(),
                zombie->getZombieHeadAttackSoundEffect());  /* ����ָ������ */

            bulletAttackHurtZombies(zombie);   /* ��ʬ��������ֵ */
            zombie->setZombieHurtBlink();

            createBulletExplode(zombie);
            setBulletAttack(0);
            setBulletIsUsed(true);

            break; /* һ���㶹ֻ�ܻ���һ����ʬ */
        }
    }
}

void CatTailBullet::calculateBulletPosition()
{
    do
    {
        if (_targetZombie)
        {
            if (!bulletIsInMap() || !_targetZombie->getZombieIsSurvive())
            {
                _targetZombie = nullptr;
                break;
            }

            auto rad = calculateAngle(_targetZombie->getZombieAnimation()->getPosition() + Vec2(0, 100));
            if (rad >= PI)
            {
                _targetZombie = nullptr;
                break;
            }

            auto deg = CC_RADIANS_TO_DEGREES(rad);
            auto deltaD = min(1.f, deg / _bulletAnimation->getPosition().distance(_targetZombie->getZombieAnimation()->getPosition() + Vec2(0, 100)));
            auto deltaR = min(1.f, deltaD / PI);

            switch (calculateDirection(_targetZombie->getZombieAnimation()->getPosition() + Vec2(0, 100)))
            {
            case CatTailBullet::RatateDirtection::COUNTERCLOCKWISE:
                _speed = _speed.rotateByAngle(Vec2::ZERO, deltaR);
                _bulletAnimation->setRotation(_bulletAnimation->getRotation() - deltaD * 20);
                break;
            case CatTailBullet::RatateDirtection::CLOCKWISE:
                _speed = _speed.rotateByAngle(Vec2::ZERO, -deltaR);
                _bulletAnimation->setRotation(_bulletAnimation->getRotation() + deltaD * 20);
                break;
            default:break;
            }

            _isTrack = true;
        }
        else
        {
            if (bulletIsInMap() && !_isTrack)
            {
                seekZombie();
            }
        }
    } while (0);

    _bulletAnimation->setPosition(Vec2(_bulletAnimation->getPosition() + _speed));
}

bool CatTailBullet::getBulletIsEncounterWithZombie(Zombies* zombie)
{
    auto& rect = zombie->getZombieAnimation()->getBoundingBox();
    return _bulletAnimation->getBoundingBox().intersectsRect(Rect(rect.origin.x + 60, rect.origin.y + 110, 90, 30));
}

void CatTailBullet::seekZombie()
{
    for (auto zombie : ZombiesGroup)
    {
        if (!_isUsed && zombie->getZombieIsSurvive() && zombie->getZombieIsEnterMap())
        {
            auto distance = calculateBulletAndZombie(zombie);
            if (distance < _distance)
            {
                _distance = distance;
                _targetZombie = zombie;
            }
        }
    }
}

float CatTailBullet::calculateBulletAndZombie(Zombies* zombie)
{
    return sqrt(pow(zombie->getZombieAnimation()->getPositionX() - _bulletAnimation->getPositionX(), 2.f) +
        pow(zombie->getZombieAnimation()->getPositionY() - _bulletAnimation->getPositionY(), 2.f));
}

float CatTailBullet::calculateAngle(Vec2 vec2)
{
    return Vec2::angle(_speed, vec2 - _bulletAnimation->getPosition());
}

CatTailBullet::RatateDirtection CatTailBullet::calculateDirection(Vec2 vec2)
{
    auto p = vec2 - _bulletAnimation->getPosition();
    if (_speed.cross(p) > 0)
    {
        return CatTailBullet::RatateDirtection::COUNTERCLOCKWISE;
    }
    else if(_speed.cross(p) <0)
    {
        return CatTailBullet::RatateDirtection::CLOCKWISE;
    }
    else
    {
        return CatTailBullet::RatateDirtection::NO_ROTATE;
    }
}

bool CatTailBullet::bulletIsInMap()
{
    return (_bulletAnimation->getPosition() <
        Vec2(controlLayerInformation->gameMapInformation->mapRight,
            controlLayerInformation->gameMapInformation->mapTop + 160) &&
        _bulletAnimation->getPosition() >
        Vec2(controlLayerInformation->gameMapInformation->mapLeft,
            controlLayerInformation->gameMapInformation->mapBottom));
}

void CatTailBullet::createBulletExplode(Zombies* zombie)
{
    _bulletAnimation->setLocalZOrder(zombie->getZombieAnimation()->getLocalZOrder() + 1);
    _bulletAnimation->setVisible(false);
   
    auto explode= SkeletonAnimation::createWithData(_global->userInformation->getAnimationData().find("CatTailBullet")->second);
	explode->setAnimation(0, "CatTailBullet_Explode", false);
    explode->setPosition(zombie->getZombieAnimation()->getPosition() + Vec2(0, 80));
	explode->setLocalZOrder(zombie->getZombieAnimation()->getLocalZOrder() + 1);
    explode->setScale(1.f + rand() % 5 / 10.f);
    explode->update(0);
    explode->runAction(Sequence::create(DelayTime::create(0.5f), CallFunc::create([explode]() {explode->removeFromParent(); }), nullptr));
    _node->addChild(explode);
}

void CatTailBullet::caveBulletInformation(rapidjson::Value& object, rapidjson::Document::AllocatorType& allocator)
{
    object.AddMember("RotateTion", _bulletAnimation->getRotation(), allocator);
}

void CatTailBullet::readBulletAnimationInformation(rapidjson::Document* levelDataDocument, char* key, int i)
{
    _bulletAnimation->setPosition(Vec2(
        (*levelDataDocument)[key]["Bullet"][to_string(i).c_str()]["PositionX"].GetFloat(),
        (*levelDataDocument)[key]["Bullet"][to_string(i).c_str()]["PositionY"].GetFloat()));
    _bulletAnimation->setRotation((*levelDataDocument)[key]["Bullet"][to_string(i).c_str()]["RotateTion"].GetFloat());
}