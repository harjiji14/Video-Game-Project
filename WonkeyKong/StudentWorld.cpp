#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr) {}

StudentWorld::~StudentWorld() {cleanUp();}

int StudentWorld::init()
{
    ostringstream currLevName;
    currLevName.fill('0');
    currLevName <<"level" << setw(2) << getLevel() << ".txt";
    
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(currLevName.str());
    if (getLevel() > MAX_LEVELS || result == Level::load_fail_file_not_found) return GWSTATUS_PLAYER_WON;
    else if (result == Level::load_fail_bad_format) return GWSTATUS_LEVEL_ERROR;
    
    for (int x = 0; x < VIEW_WIDTH; x++)
    {
        for (int y = 0; y < VIEW_HEIGHT; y++)
        {
            switch (lev.getContentsOf(x, y))
            {
                case Level::empty :
                    break;
                case Level::player :
                    m_player = new Player(x, y, this);
                    break;
                case Level::left_kong :
                    m_actors.push_back(new Kong(x, y, this, 180));
                    break;
                case Level::right_kong :
                    m_actors.push_back(new Kong(x, y, this, 0));
                    break;
                case Level::floor :
                    m_actors.push_back(new Floor(x, y, this));
                    break;
                case Level::ladder :
                    m_actors.push_back(new Ladder(x, y, this));
                    break;
                case Level::bonfire :
                    m_actors.push_back(new Bonfire(x, y, this));
                    break;
                case Level::fireball :
                    m_actors.push_back(new Fireball(x, y, this));
                    break;
                case Level::koopa :
                    m_actors.push_back(new Koopa(x, y, this));
                    break;
                case Level::extra_life :
                    m_actors.push_back(new ExtraLifeGoodie(x, y, this));
                    break;
                case Level::garlic :
                    m_actors.push_back(new GarlicGoodie(x, y, this));
                    break;
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q
    updateDisplayText();
    
    m_player->doSomething();
    for (Actor* actor : m_actors)
    {
        actor->doSomething();
    }
    
    if (clearDead())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    else if (m_win)
    {
        m_win = false;
        increaseScore(1000);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr;
    
    while (!m_actors.empty())
    {
        delete m_actors.back();
        m_actors.pop_back();
    }
}

bool StudentWorld::isBlocked(int x, int y) const
{
    for (Actor* actor : m_actors)
    {
        if (isAt(actor, x, y))
        {
            if (actor->isObstacle()) return true;
        }
    }
    
    return false;
}

bool StudentWorld::isAt(Actor* ap, int x, int y) const
{
    if (x == ap->getX() && y == ap->getY()) return true;
    else return false;
}

void StudentWorld::burnAt(int x, int y)
{
    if (isAt(m_player, x, y)) m_player->setDead();
    for (Actor* actor : m_actors)
    {
        if (isAt(actor, x, y))
        {
            if (!actor->fireProof())
            {
                actor->setDead();
            }
        }
    }
}

void StudentWorld::attackAt(int x, int y)
{
    for (Actor* actor : m_actors)
    {
        if (isAt(actor, x, y))
        {
            if (actor->isEnemy())
            {
                actor->setDead();
                
                int r = randInt(1,3), id = actor->dropGoodie();
                if (r == 1)
                {
                    switch (id)
                    {
                        case -1:
                            break;
                        case 1:
                            m_actors.push_back(new ExtraLifeGoodie(actor->getX(), actor->getY(), this));
                            break;
                        case 2:
                            m_actors.push_back(new GarlicGoodie(actor->getX(), actor->getY(), this));
                            break;
                    }
                }
            }
        }
    }
}

bool StudentWorld::freeFall(int x, int y) const
{
    if (isBlocked(x, y - 1) || canClimb(x, y) || canClimb(x, y - 1)) return false;
    return true;
}

bool StudentWorld::canClimb(int x, int y) const
{
    for (Actor* actor: m_actors)
    {
        if (isAt(actor, x, y))
        {
            if (actor->canClimb()) return true;
        }
    }
    return false;
}

void StudentWorld::addBarrel(int x, int y, int direction)
{
    m_actors.push_back(new Barrel(x, y, this, direction));
}

void StudentWorld::addBurp(int x, int y, int direction)
{
    m_actors.push_back(new Burp(x, y, this, direction));
}

// Private & Nonmember functions

bool StudentWorld::clearDead()
{
    vector<Actor*>::iterator it;
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->isDead())
        {
            delete (*it);
            it = m_actors.erase(it);
            continue;
        }
        it++;
    }
    if (m_player->isDead()) return true;
    else return false;
}

void StudentWorld::updateDisplayText() 
{
    int score = getScore();
    int level = getLevel();
    int livesLeft = getLives();
    unsigned int burps = m_player->getBurps();
    
    string s = generate_stats(score, level, livesLeft, burps);
    
    setGameStatText(s);
}

string generate_stats(int score, int level, int livesLeft, int burps)
{
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << score << " Level: " << setw(2) << level << " Lives: " << setw(2) << livesLeft << " Burps: " << setw(2) << burps << endl;
    return oss.str();
}
