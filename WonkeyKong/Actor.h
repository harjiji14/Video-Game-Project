#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

// Actor
class Actor : public GraphObject
{
public:
    Actor(int imageID,
          int startX,
          int startY,
          StudentWorld* world,
          int startDirection=none);
    ~Actor() {}
    StudentWorld* world() const {return m_world;}
    virtual bool isObstacle() const {return false;}
    virtual bool canClimb() const {return false;}
    virtual bool fireProof() const {return true;}
    virtual bool isEnemy() const {return false;}
    bool isDead() const {return m_dead;}
    virtual void setDead() {m_dead = true;}
    virtual void doSomething() { if (isDead()) return;}
    virtual int dropGoodie() {return -1;}
    
private:
    StudentWorld* m_world;
    bool m_dead;
};

// Floor
class Floor : public Actor
{
public:
    Floor(int startX,
          int startY,
          StudentWorld* world);
    ~Floor() {}
    
    virtual bool isObstacle() const {return true;}
};

// Ladder
class Ladder : public Actor
{
public:
    Ladder(int startX,
          int startY,
          StudentWorld* world);
    ~Ladder() {}
    virtual bool canClimb() const {return true;}
};

// Player
class Player : public Actor
{
public:
    Player(int startX,
           int startY,
           StudentWorld* world);
    ~Player() {}
    
    int getBurps() const {return m_burps;}
    void addBurps(int n) {m_burps += n;}
    void frozen() {m_freezeTimer += 50;}
    virtual void setDead();
    virtual void doSomething();
private:
    int m_burps;
    int m_freezeTimer;
    int m_jumpSequence;
};

// Bonfire
class Bonfire : public Actor
{
public:
    Bonfire(int startX,
            int startY,
            StudentWorld* world);
    ~Bonfire() {}
    
    virtual void doSomething();
};

// Goodies
class Goodie : public Actor
{
public:
    Goodie(int imageID,
           int startX,
           int startY,
           StudentWorld* world,
           int score);
    ~Goodie() {}
    virtual void doSomething();
    virtual void buff() const = 0;
private:
    int m_scoreWorth;
    void incScore();
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(int startX,
                    int startY,
                    StudentWorld* world);
    ~ExtraLifeGoodie() {}
    virtual void buff() const;
};

class GarlicGoodie : public Goodie
{
public:
    GarlicGoodie(int startX,
                 int startY,
                 StudentWorld* world);
    ~GarlicGoodie() {}
    virtual void buff() const;

};

// Enemies
class Enemy : public Actor
{
public:
    Enemy(int imageID,
          int startX,
          int startY,
          StudentWorld* world);
    ~Enemy() {}
    
    virtual bool Attack();
    virtual void EnemyOnly() {}
    virtual bool isEnemy() const {return true;}
    virtual void specialMove() = 0;
    virtual void doSomething();
    void reverseOrGo(int x, int y);
    int reverseHelper(int direction);
    virtual void setDead();
private:
    int m_countDown;
};

class Fireball : public Enemy
{
public:
    Fireball(int startX,
             int startY,
             StudentWorld* world);
    ~Fireball() {}
    
    virtual void specialMove();
    virtual int dropGoodie() {return 2;}
private:
    int m_climbState;
};

class Koopa : public Enemy
{
public:
    Koopa(int startX,
          int startY,
          StudentWorld* world);
    ~Koopa() {}
    
    virtual bool Attack();
    virtual void specialMove();
    virtual void EnemyOnly();
    virtual int dropGoodie() {return 1;}
private:
    int m_freezeCD;
};

class Barrel : public Enemy
{
public:
    Barrel (int startX,
            int startY,
            StudentWorld* world,
            int direction);
    ~Barrel() {}
    
    virtual bool fireProof() const {return false;}
    virtual void EnemyOnly();
    virtual void specialMove();
private:
    bool m_fallen;
};

// Kong
class Kong : public Actor
{
public:
    Kong(int startX, int startY, StudentWorld* world, int direction);
    ~Kong() {}
    
    virtual void doSomething();

private:
    bool m_flee;
    int m_countDown;
    int m_ticksElapsed;
    
    double distance(int x, int y) const;
    
};

// Burp
class Burp : public Actor
{
public:
    Burp(int startX,
         int startY,
         StudentWorld* world,
         int direction);
    ~Burp() {}
    
    virtual void doSomething();
private:
    int m_life;
};
#endif // ACTOR_H_
