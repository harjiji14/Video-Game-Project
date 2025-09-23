#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool isBlocked(int x, int y) const;
    Player* player() const {return m_player;}
    bool isAt(Actor* ap, int x, int y) const;
    void burnAt(int x, int y);
    void attackAt(int x, int y);
    bool freeFall(int x, int y) const;
    bool canClimb(int x, int y) const;
    void addBarrel(int x, int y, int direction);
    void addBurp(int x, int y, int direction);
    void win() {m_win = true;}
    
private:
    void updateDisplayText();
    bool clearDead();
    
    std::vector<Actor*> m_actors;
    Player* m_player;
    bool m_win;
};

std::string generate_stats(int score, int level, int livesLeft, int burps);

#endif // STUDENTWORLD_H_
