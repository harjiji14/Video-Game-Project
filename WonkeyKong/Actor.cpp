#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
#include <algorithm>

// Actor Implementation
Actor::Actor(int imageID,
             int startX,
             int startY,
             StudentWorld* world,
             int startDirection)
:
GraphObject(imageID, startX, startY, startDirection),
m_world(world),
m_dead(false)
{}

// Floor Implementation
Floor::Floor(int startX,
             int startY,
             StudentWorld* world)
:
Actor(IID_FLOOR, startX, startY, world)
{}

// Ladder Implementation
Ladder::Ladder(int startX,
               int startY,
               StudentWorld* world)
:
Actor(IID_LADDER, startX, startY, world)
{}

// Player Implementation
Player::Player(int startX,
               int startY,
               StudentWorld* world)
:
Actor(IID_PLAYER, startX, startY, world, right),
m_burps(0),
m_freezeTimer(0),
m_jumpSequence(0)
{}

void Player::setDead()
{
    world()->playSound(SOUND_PLAYER_DIE);
    Actor::setDead();
}

void Player::doSomething()
{
    Actor::doSomething();
    
    if (!world()->canClimb(getX(), getY()))
    {
        int x = 0, y = 0;
        switch (m_jumpSequence)
        {
            case 1:
            case 2:
            case 3:
                getPositionInThisDirection(getDirection(), 1, x, y);
                if (!world()->isBlocked(x, y))
                {
                    moveTo(x, y);
                }
                m_jumpSequence++;
                return;
            case 4:
                if (!world()->isBlocked(getX(), getY() - 1))
                {
                    moveTo(getX(), getY() - 1);
                }
                m_jumpSequence = 0;
                return;
            default:
                break;
        }
    }
    m_jumpSequence = 0;
    
    if (m_freezeTimer > 0)
    {
        m_freezeTimer--;
        return;
    }
    
    if (world()->freeFall(getX(), getY()))
    {
        moveTo(getX(), getY() - 1);
        return;
    }
    
    int ch;
    if (world()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                if (getDirection() != left)
                {
                    setDirection(left);
                    break;
                }
                if (!world()->isBlocked(getX() - 1, getY()))
                {
                    moveTo(getX() - 1, getY());
                }
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right)
                {
                    setDirection(right);
                    break;
                }
                if (!world()->isBlocked(getX() + 1, getY()))
                {
                    moveTo(getX() + 1, getY());
                }
                break;
            case KEY_PRESS_UP:
                if (!world()->isBlocked(getX(), getY() + 1) && world()->canClimb(getX(), getY()))
                {
                    moveTo(getX(), getY() + 1);
                }
                break;
            case KEY_PRESS_DOWN:
                if (!world()->isBlocked(getX(), getY() - 1))
                {
                    moveTo(getX(), getY() - 1);
                }
                break;
            case KEY_PRESS_SPACE:
                world()->playSound(SOUND_JUMP);
                if (!world()->isBlocked(getX(), getY() + 1))
                {
                    moveTo(getX(), getY() + 1);
                    m_jumpSequence++;
                }
                break;
            case KEY_PRESS_TAB:
                if (m_burps > 0)
                {
                    int x = 0, y = 0;
                    getPositionInThisDirection(getDirection(), 1, x, y);
                    world()->playSound(SOUND_BURP);
                    world()->addBurp(x, y, getDirection());
                }
                break;
        }
    }
}

// Bonfire Implementation
Bonfire::Bonfire(int startX,
                 int startY,
                 StudentWorld* world)
:
Actor(IID_BONFIRE, startX, startY, world)
{}

void Bonfire::doSomething()
{
    increaseAnimationNumber();
    world()->burnAt(getX(), getY());
}

// Goodie Implementation
Goodie::Goodie(int imageID,
               int startX,
               int startY,
               StudentWorld* world,
               int score)
: Actor(imageID, startX, startY, world), m_scoreWorth(score)
{}

void Goodie::incScore() {world()->increaseScore(m_scoreWorth);}

void Goodie::doSomething()
{
    Actor::doSomething();
    
    if (world()->isAt(world()->player(), getX(), getY()))
    {
        incScore();
        buff();
        world()->playSound(SOUND_GOT_GOODIE);
        setDead();
    }
}

// ExtraLifeGoodie Implementation
ExtraLifeGoodie::ExtraLifeGoodie(int startX,
                                 int startY,
                                 StudentWorld* world)
:
Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, world, 50)
{}

void ExtraLifeGoodie::buff() const {world()->incLives();}

// GarlicGoodie Implementation
GarlicGoodie::GarlicGoodie(int startX,
                           int startY,
                           StudentWorld* world)
:
Goodie(IID_GARLIC_GOODIE, startX, startY, world, 25)
{}

void GarlicGoodie::buff() const {world()->player()->addBurps(5);}

// Enemy Implementation
Enemy::Enemy(int imageID,
             int startX,
             int startY,
             StudentWorld* world)
:
Actor(imageID, startX, startY, world),
m_countDown(0)
{
    int i = randInt(0,1);
    switch (i)
    {
        case 0:
            setDirection(left);
            break;
        case 1:
            setDirection(right);
            break;
    }
}

bool Enemy::Attack()
{
    if (world()->isAt(world()->player(), getX(), getY()))
    {
        world()->player()->setDead();
        return true;
    }
    return false;
}

void Enemy::doSomething()
{
    Actor::doSomething();
    if (Attack()) return;
    EnemyOnly();
    
    if (m_countDown % 10 == 0)
    {
        m_countDown = 0;
        specialMove();
        if (Attack()) return;
    }
    
    m_countDown++;
}

void Enemy::reverseOrGo(int x, int y)
{
    getPositionInThisDirection(getDirection(), 1, x, y);
    if (world()->isBlocked(x, y) || world()->freeFall(x, y))
    {
        setDirection(reverseHelper(getDirection()));
    }
    else moveTo(x, y);
}

int Enemy::reverseHelper(int direction) {return (direction + 180) % 360;}

void Enemy::setDead()
{
    world()->increaseScore(100);
    world()->playSound(SOUND_ENEMY_DIE);
    Actor::setDead();
}

// Fireball Implementation
Fireball::Fireball(int startX,
                   int startY,
                   StudentWorld* world)
:
Enemy(IID_FIREBALL, startX, startY, world),
m_climbState(none)
{}

void Fireball::specialMove()
{
    
    
    if (world()->canClimb(getX(), getY()) && !world()->isBlocked(getX(), getY() + 1) && m_climbState != down)
    {
        int r = randInt(1,3);
        if (m_climbState == up || r == 1)
        {
            m_climbState = up;
            moveTo(getX(), getY() + 1);
            return;
        }
    }
    if (world()->canClimb(getX(), getY() - 1) && m_climbState != up)
    {
        int r = randInt(1, 3);
        if (m_climbState == down || r == 1)
         {
             m_climbState = down;
             moveTo(getX(), getY() - 1);
             return;
         }
    }
    else
    {
        int x = 0,y = 0;
        if (m_climbState != none)
        {
            getPositionInThisDirection(m_climbState, 1, x, y);
            if ((world()->isBlocked(x, y) || !world()->canClimb(getX(), getY())))
            {
                m_climbState = none;
            }
        }
        
        reverseOrGo(x, y);
    }
}

// Koopa Implementation
Koopa::Koopa(int startX,
             int startY,
             StudentWorld* world)
:
Enemy(IID_KOOPA, startX, startY, world),
m_freezeCD(0)
{}

bool Koopa::Attack()
{
    if (world()->isAt(world()->player(), getX(), getY()) && m_freezeCD == 0)
    {
        m_freezeCD +=50;
        world()->player()->frozen();
        return true;
    }
    return false;
}

void Koopa::specialMove()
{
    reverseOrGo(getX(), getY());
}

void Koopa::EnemyOnly()
{
    if (m_freezeCD > 0)
    {
        m_freezeCD--;
    }
}

// Barrel Implementation
Barrel::Barrel(int startX,
               int startY,
               StudentWorld* world,
               int direction)
:
Enemy(IID_BARREL, startX, startY, world),
m_fallen(false)
{setDirection(direction);}

void Barrel::EnemyOnly()
{
    if (!world()->isBlocked(getX(), getY() - 1))
    {
        moveTo(getX(), getY() - 1);
        m_fallen = true;
    }
    else if (m_fallen)
    {
        m_fallen = false;
        setDirection(reverseHelper(getDirection()));
    }

        
}

void Barrel::specialMove()
{
    int x = 0, y = 0;
    getPositionInThisDirection(getDirection(), 1, x, y);
    if (world()->isBlocked(x, y)) setDirection(reverseHelper(getDirection()));
    else moveTo(x, y);
}

// Kong Implementation
Kong::Kong(int startX,
           int startY,
           StudentWorld* world,
           int direction)
           
:
Actor(IID_KONG, startX, startY, world, direction),
m_flee(false),
m_countDown(0),
m_ticksElapsed(0)
{}

double Kong::distance(int x, int y) const
{
    return std::sqrt(std::pow(getX() - x, 2) + std::pow(getY() - y, 2));
}

void Kong::doSomething()
{
    increaseAnimationNumber();
    m_ticksElapsed++;
    if (distance(world()->player()->getX(), world()->player()->getY()) <= 2.0)
    {
        m_flee = true;
    }
    
    if (!m_flee)
    {
        int N = std::max(200 - 50 * world()->getLevel(), 50);
        
        if (m_ticksElapsed == N)
        {
            m_ticksElapsed = 0;
            int x = 0, y = 0;
            getPositionInThisDirection(getDirection(), 1, x, y);
            world()->addBarrel(x, y, getDirection());
        }
        
    }
    if (m_countDown % 5 == 0)
    {
        m_countDown = 0;
        if (m_flee)
        {
            if (getY() + 1 == VIEW_HEIGHT)
            {
                world()->win();
            }
            moveTo(getX(), getY() + 1);
        }
            
    }
    m_countDown++;
}

// Burp Implementation
Burp::Burp(int startX,
           int startY,
           StudentWorld* world,
           int direction)
:
Actor(IID_BURP, startX, startY, world, direction),
m_life(5)
{}

void Burp::doSomething()
{
    Actor::doSomething();
    
    m_life--;
    if (m_life == 0)
    {
        setDead();
        return;
    }
    
    world()->attackAt(getX(), getY());
    
}
