#include "StudentWorld.h"
#include "Level.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    completed = false;
    m_jewels = 0;
    soundPlayed = false;
    m_actors.clear();
    m_bonus = 1000;
    int k = loadlevel();
    
    if(k == -1)
    {
        cerr << "failed to load" << curlevel << endl;
        return GWSTATUS_LEVEL_ERROR;
    }
    if(k == 0)
    {
        cerr << "level 99 reached" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::loadlevel()
{
    ostringstream level,getlev;
    getlev.fill('0');
    getlev << setw(2) << getLevel();
    level << "level" << getlev.str() << ".dat";
    curlevel = level.str();

    //curlevel = "level01.dat";
    Level lev(assetDirectory());
    Level::LoadResult result = lev.loadLevel(curlevel);
    if (result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format)
        return -1;
    for (int x = 0;x<15;x++)
    {
        for (int y = 0; y<15; y++)
        {
            Level::MazeEntry ge = lev.getContentsOf(x,y);
            switch (ge)
            {
                case Level::empty:
                    break;
                case Level::exit:
                    m_actors.push_back(new Exit(x,y,this));
                    break;
                case Level::player:
                    m_player = new Player(x,y,this);
                    break;
                case Level::horiz_snarlbot:
                    m_actors.push_back(new Snarlbot(x,y,this,GraphObject::right));
                    break;
                case Level::vert_snarlbot:
                    m_actors.push_back(new Snarlbot(x,y,this,GraphObject::down));
                    break;
                case Level::kleptobot_factory:
                    m_actors.push_back(new KleptoBotFactory(x,y,this,KleptoBotFactory::REGULAR));
                    //m_actors.push_back(new RegularKleptoBot(x,y,this));
                    break;
                case Level::angry_kleptobot_factory:
                    m_actors.push_back(new KleptoBotFactory(x,y,this,KleptoBotFactory::ANGRY));
                    //m_actors.push_back(new AngryKleptoBot(x,y,this));
                    break;
                case Level::wall:
                    m_actors.push_back(new Wall(x,y,this));
                    break;
                case Level::jewel:
                    addJewel();
                    m_actors.push_back(new Jewel(x,y,this));
                    break;
                case Level::restore_health:
                    m_actors.push_back(new HealthGoodie(x,y,this));
                    break;
                case Level::extra_life:
                    m_actors.push_back(new LifeGoodie(x,y,this));
                    break;
                case Level::ammo:
                    m_actors.push_back(new AmmoGoodie(x,y,this));
                    break;
                case Level::boulder:
                    m_actors.push_back(new Boulder(x,y,this));
                    break;
                case Level::hole:
                    m_actors.push_back(new Hole(x,y,this));
                    break;
                default: cerr << "Invalid Map Input!";
                    break;
            }
        }
    }
    return 1;
}

int StudentWorld::move()
{
    int k = m_actors.size();
    printStatus();
    
    for (int i = 0; i < k;i++)
    {
        if (m_actors[i]->isLive())
            m_actors[i]->doSomething();
        if (!m_player->isLive())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if (hasCompleted())
        {
            increaseScore(2000+m_bonus);
            return GWSTATUS_FINISHED_LEVEL;
        }
        
    }
    
    m_player->doSomething();
    if (!m_player->isLive())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    if (hasCompleted())
    {
        increaseScore(2000+m_bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }
    for (int i = 0 ; i < m_actors.size(); i++)
    {
        if (!m_actors[i]->isLive())
        {
            vector<Actor*>::iterator p = m_actors.begin() + i;
            delete m_actors[i];
            m_actors.erase(p);
        }
    }
    decBonus();
    if (!m_player->isLive())
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    if (hasCompleted())
    {
        increaseScore(2000+m_bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }
    exitCheck();
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::decBonus()
{
    if (m_bonus > 0)
        m_bonus--;
    
}

void StudentWorld::printStatus()
{
    ostringstream score, level, lives, health, ammo, bonus;
    score.fill('0');
    score << setw(7) << getScore();
    level.fill('0');
    level << setw(2) <<  getLevel();
    lives.fill(' ');
    lives << setw(2) << getLives();
    lives.fill(' ');
    health << setw(3) << m_player->getHealthPercent() << '%';
    ammo.fill(' ');
    ammo << setw(3) << m_player->getAmmo();
    bonus.fill(' ');
    bonus << setw(4) << getBonus();
    ostringstream final;
    final << "  Score: " << score.str();
    final << "  Level: " << level.str();
    final << "  Lives: " << lives.str();
    final << "  Health: " << health.str();
    final << "  Ammo: " << ammo.str();
    final << "  Bonus: " << bonus.str();
    string sfinal = final.str();
    setGameStatText(sfinal);
    
}
Actor* StudentWorld::getBoulder(int x, int y)
{
    for (int i = 0 ; i < m_actors.size() ;i++)
    {
        if (m_actors[i]->isSwalloable())
        {
            if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
            {
                return m_actors[i];
            }
        }
    }
    return nullptr;
}

bool StudentWorld::moveBoulder(int x, int y, GraphObject::Direction dir)
{
    Actor* bp = getBoulder(x,y);
    if (bp == nullptr)
        return false;
    switch (dir)
    {
        case GraphObject::right:
            if (!blocksBoulder(x+1,y))
            {
                bp->moveTo(x+1, y);
                return true;
            }
            break;
        case GraphObject::left:
            if (!blocksBoulder(x-1,y))
            {
                bp->moveTo(x-1,y);
                return true;
            }
            break;
        case GraphObject::up:
            if (!blocksBoulder(x,y+1))
            {
                bp->moveTo(x,y+1);
                return true;
            }
            break;
        case GraphObject::down:
            if (!blocksBoulder(x,y-1))
            {
                bp->moveTo(x,y-1);
                return true;
            }
            break;
        case GraphObject::none:
            break;
    }
    return false;
}


Actor* StudentWorld::getAt(int x, int y)
{
    if (playerX() == x && playerY() == y)
        return getPlayer();
    for (int i = 0; i < m_actors.size();i++)
    {
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            return m_actors[i];
        }
    }
    return nullptr;
}
bool StudentWorld::killsBullet(int x, int y)
{
    for ( int i = 0; i < m_actors.size();i++)
    {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if (m_actors[i]->blocksBullet())
            {
                m_actors[i]->damage();
                return true;
            }
        }
    }
    return false;
}
bool StudentWorld::interactsBullet(int x, int y)
{
    if (playerX() == x && playerY() == y)
    {
        getPlayer()->damage();
        return true;
    }
    for ( int i = 0; i < m_actors.size();i++)
    {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if (m_actors[i]->interactsBullet())
            {
                m_actors[i]->damage();
                return true;
            }
        }
    }
    return false;
}

void StudentWorld::exitCheck()
{
    if (getJewel() == 0 && !soundPlayed)
    {
        soundPlayed = true;
        playSound(SOUND_REVEAL_EXIT);
        for (int i = 0 ; i < m_actors.size();i++)
        {
            Actor* ap = m_actors[i];
            if (ap != nullptr)
            {
                Exit* ep = dynamic_cast<Exit*>(ap);
                if (ep != nullptr)
                {
                    {
                        ep->setVisible(true);
                    }
                }
            }
        }
    }
}


void StudentWorld::cleanUp()
{
    for (int i = 0;i<m_actors.size();i++)
    {
        delete m_actors[i];
        vector<Actor*>::iterator p = m_actors.begin();
        m_actors.erase(p);
        i--;
    }
    delete m_player;
    while (getJewel() != 0)
    {
        decJewel();
    }
}
bool StudentWorld::blocksPlayer(int x, int y)
{
    for (int i = 0; i < m_actors.size();i++)
    {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if (m_actors[i]->blocksPlayer())
                return true;
        }
    }
    return false;
}
bool StudentWorld::blocksRobot(int x, int y)
{
    if (playerX() == x && playerY() == y)
        return true;
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if (m_actors[i]->blocksRobot())
                return true;
        }
    }
    return false;
}
bool StudentWorld::blocksBoulder(int x, int y)
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if (m_actors[i]->blocksBoulder())
                return true;
        }
    }
    return false;
}
bool StudentWorld::boulderCheck(int x, int y)
{
    Actor* bp = getBoulder(x,y);
    if (bp != nullptr)
    {
        return true;
    }
    return false;
}

void StudentWorld::newBullet(int x, int y,GraphObject::Direction dir)
{
    
    GraphObject::Direction m = dir;
    switch(dir)
    {
        case GraphObject::up:
            y=y+1;
            break;
        case GraphObject::down:
            y=y-1;
            break;
        case GraphObject::left:
            x=x-1;
            break;
        case GraphObject::right:
            x=x+1;
            break;
        case GraphObject::none:
            break;
    }
    m_actors.push_back(new Bullet(x,y,this,dir));
    m_actors.back()->setDirection(m);
}
bool StudentWorld::playerSight(int x, int y, GraphObject::Direction dir)
{
    switch(dir)
    {
        case GraphObject::down:
            while (true)
            {
                Actor* actor = getAt(x,y-1);
                if (actor == nullptr)
                {
                    y = y-1;
                    continue;
                }
                if (actor->blocksBullet() || actor->interactsBullet())
                {
                    if (playerX() == x && playerY() == y-
                        1)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                y = y-1;
            }
            break;
        case GraphObject::up:
            while (true)
            {
                Actor* actor = getAt(x,y+1);
                if (actor == nullptr)
                {
                    y = y+1;
                    continue;
                }
                if (actor->interactsBullet() || actor->blocksBullet())
                {
                    if (playerX() == x && playerY() == y+1)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                y = y+1;
            }
            break;
        case GraphObject::right:
            while (true)
            {
                Actor* actor = getAt(x+1, y);
                if (actor == nullptr)
                {
                    x=x+1;
                    continue;
                }
                if (actor->interactsBullet() || actor->blocksBullet())
                {
                    if(playerX() == x+1 && playerY() == y)
                    {
                        
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                x=x+1;
            }
            break;
        case GraphObject::left:
            while (true)
            {
                Actor* actor = getAt(x-1,y);
                if (actor == nullptr)
                {
                    x = x-1;
                    continue;
                }
                if (actor->interactsBullet() || actor->blocksBullet())
                {
                    if (playerX() == x-1 && playerY() == y)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                x=x-1;
            }
            break;
        case GraphObject::none:
            break;
    }
    return false;
}
bool StudentWorld::goodieAt(int x,int y, Goodie*&good)
{

    for (int i = 0 ; i < m_actors.size() ; i++)
    {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
        {
            if (m_actors[i]->isPickUpAble())
            {
                Goodie* gp = dynamic_cast<Goodie*>(m_actors[i]);
                if (gp == nullptr)
                {
                    return false;
                }
                else
                {
                    good = gp;
                    return true;
                }
            }
        }
    }
    return false;
}
void StudentWorld::addActor(Actor* ap)
{
    m_actors.push_back(ap);
}
int StudentWorld::census(int x, int y)
{
    int sum = 0;
    int xmax = x + 3;
    int xmin = x - 3;
    int ymax = y + 3;
    int ymin = y - 3;
    if (xmax > 14)
        xmax = 14;
    if (xmin < 0)
        xmin = 0;
    if (ymax > 14)
        ymax = 14;
    if (ymin < 0)
        ymin = 0;
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (!m_actors[i]->countsInCensus())
            continue;
        for (int p = xmin; p <= xmax; p++)
        {
            for(int k = ymin; k <= ymax; k++)
            {
                if(m_actors[i]->getX() == p && m_actors[i]->getY() == k)
                {
                    sum++;
                }
            }
        }
    }
    
    return sum;
}
bool StudentWorld::kleptoAt(int x, int y)
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i]->countsInCensus())
        {
            if (m_actors[i]->getX() == x && m_actors[i]->getY() == y)
            {
                return true;
            }
        }
    }
    return false;
}
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
