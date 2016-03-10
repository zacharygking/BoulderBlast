#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
//gets keypress, in the case of up down left or right the
//player checks if it can move in that direction, if it can
//it moves, if it is blocked it checks if it’s a boulder, if
//it is, it checks if the boulder can move, if it does the
//player and boulder then move, otherwise the function returns.
//In the case of escape it sets the player dead so that it will
//end the level in the studentworld function. In the case of space
//the player shoots in its direction and plays the shoot sound if
//ammo is available
void Player::doSomething()
{
    if (!isLive())
        return;
    int ch=0;
    if (getWorld()->getKey(ch))
    {
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                if (!getWorld()->blocksPlayer(getX()-1,getY()))
                {
                    if (!getWorld()->boulderCheck(getX()-1,getY()))
                        moveTo(getX()-1,getY());
                    else
                    {
                        if(getWorld()->moveBoulder(getX()-1,getY(),GraphObject::left))
                        {
                            moveTo(getX()-1, getY());
                        }
                    }
                }
                setDirection(left);
                break;
            case KEY_PRESS_RIGHT:
                if (!getWorld()->blocksPlayer(getX()+1,getY()))
                {
                    if(!getWorld()->boulderCheck(getX()+1,getY()))
                        moveTo(getX()+1,getY());
                    else
                    {
                        if(getWorld()->moveBoulder(getX()+1,getY(),GraphObject::right))
                        {
                            moveTo(getX()+1,getY());
                        }
                    }
                }
                setDirection(right);
                break;
            case KEY_PRESS_UP:
                if (!getWorld()->blocksPlayer(getX(),getY()+1))
                {
                    if (!getWorld()->boulderCheck(getX(),getY()+1))
                        moveTo(getX(),getY()+1);
                    else
                    {
                        if(getWorld()->moveBoulder(getX(),getY()+1,GraphObject::up))
                        {
                            moveTo(getX(), getY()+1);
                        }
                    }
                }
                setDirection(up);
                break;
            case KEY_PRESS_DOWN:
                if (!getWorld()->blocksPlayer(getX(), getY()-1))
                {
                    if (!getWorld()->boulderCheck(getX(),getY()-1))
                        moveTo(getX(),getY()-1);
                    else
                    {
                        if(getWorld()->moveBoulder(getX(),getY()-1,GraphObject::down))
                        {
                            moveTo(getX(), getY()-1);
                        }
                    }
                }
                setDirection(down);
                break;
            case KEY_PRESS_ESCAPE:
                setLife(false);
                break;
            case KEY_PRESS_SPACE:
                if (getAmmo() > 0)
                {
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    getWorld()->newBullet(getX(),getY(),GraphObject::getDirection());
                    incAmmo(-1);
                }
                break;
        }
    }
    return;
}
void Goodie::doSomething()
{
    if (!isLive())
    {
        return;
    }
    if (getWorld()->playerX() == getX() && getWorld()->playerY() == getY() && !isStolen())
    {
        getWorld()->playSound(SOUND_GOT_GOODIE);
        setLife(false);
        reward();
    }
    return;
}
void Jewel::doSomething()
{
    if (!isLive())
    {
        return;
    }
    if (getWorld()->playerX() == getX() && getWorld()->playerY() == getY())
    {
        getWorld()->increaseScore(50);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        setLife(false);
        getWorld()->decJewel();
    }
    return;
}
void LifeGoodie::reward()
{
    getWorld()->increaseScore(1000);
    getWorld()->incLives();
    
}
void AmmoGoodie::reward()
{
    getWorld()->increaseScore(100);
    getWorld()->getPlayer()->incAmmo(20);
}
void HealthGoodie::reward()
{
    getWorld()->increaseScore(500);
    getWorld()->getPlayer()->healthGoodie();
}
void Hole::doSomething()
{
    if (!isLive())
    {
        return;
    }
    Actor* temp = getWorld()->getBoulder(getX(),getY());
    if (temp != nullptr)
    {
        temp->setLife(false);
        setLife(false);
    }
    return;
}
void Snarlbot::doSomething()
{
    if (!isLive())
        return;
    if (shouldAct())
    {
        if (attemptShoot())
            return;
        if (canMove())
            move();
        else
            setDirection(changeDirection());
    }
    return;
}
bool Robot::attemptShoot()
{
    switch (getDirection())
    {
        case GraphObject::down:
            if (getWorld()->playerSight(getX(), getY(), down))
            {
                getWorld()->newBullet(getX(),getY(),down);
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
                break;
        case GraphObject::up:
            if (getWorld()->playerSight(getX(), getY(), up))
            {
                getWorld()->newBullet(getX(),getY(),up);
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        case GraphObject::right:
            if (getWorld()->playerSight(getX(), getY(), right))
            {
                getWorld()->newBullet(getX(),getY(),right);
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        case GraphObject::left:
            if (getWorld()->playerSight(getX(), getY(), left))
            {
                getWorld()->newBullet(getX(),getY(),left);
                getWorld()->playSound(SOUND_ENEMY_FIRE);
                return true;
            }
            break;
        default:
            break;
            
    }
    return false;
}
void Bullet::doSomething()
{
    if (!isLive())
        return;
    if (getWorld()->killsBullet(getX(), getY()))
    {
        setLife(false);
        return;
    }
    else
    {
        if(!getWorld()->interactsBullet(getX(),getY()))
        {
            switch(getDirection())
            {
                case GraphObject::up:
                    moveTo(getX(), getY()+1);
                    break;
                case GraphObject::down:
                    moveTo(getX(),getY()-1);
                    break;
                case GraphObject::right:
                    moveTo(getX()+1, getY());
                    break;
                case GraphObject::left:
                    moveTo(getX()-1, getY());
                    break;
                default:
                    break;
            }
        }
        else
        {
            setLife(false);
            return;
        }
        if (getWorld()->interactsBullet(getX(), getY()))
        {
            setLife(false);
            return;
        }
    }
    return;
}
//reduces players health by 2 if this sets health below or equal
//to zero it sets the players life false and plays player dead
//otherwise plays player impact sound
void Player::damage()
{
    shoot();
    if (getHealth() == 0)
    {
        setLife(false);
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    else
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
}
void Robot::damage()
{
    shoot();
    if (getHealth() <= 0)
    {
        special();
        setLife(false);
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(killBonus());
    }
    else
    {
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
}
void Boulder::damage()
{
    shoot();
    if (getHealth() <= 0)
    {
        setLife(false);
    }
}
bool Robot::shouldAct()
{
    int k = (28-getWorld()->getLevel())/4;
    if (m_tick % k == 0)
    {
        m_tick = 1;
        return true;
    }
    m_tick++;
    return false;
}
void KleptoBot::doSomething()
{
    if (!isLive())
    {
        return;
    }
    if (shouldAct())
    {
        if (attemptShoot())
            return;
        Goodie* good;
        if(getWorld()->goodieAt(getX(),getY(),good))
        {
            if (rand() % 10 == 0 && id_stolen == 0)
            {
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
                good->setStolen(true);
                id_stolen = good->id();
                return;
            }
        }
        if (shouldMove() && canMove())
        {
            distleft--;
            move();
        }
        else
        {
            resetDistance();
            set<Direction> dir;
            dir.clear();
            Direction d = changeDirection();
            setDirection(d);
            while(dir.size() < 4 && !canMove())
            {
                dir.insert(d);
                setDirection(d);
                d = changeDirection();
            }
            if (!canMove())
            {
                setDirection(d);
            }
            else
            {
                move();
                distleft--;
            }
        }
    }
    return;
}
void KleptoBot::special()
{
    switch(id_stolen)
    {
        case(0):
            break;
        case 1:
            getWorld()->addActor(new AmmoGoodie(getX(),getY(),getWorld()));
            break;
        case 2:
            getWorld()->addActor(new LifeGoodie(getX(),getY(),getWorld()));
            break;
        case 3:
            getWorld()->addActor(new HealthGoodie(getX(),getY(),getWorld()));
            break;
        default:
            break;
    }
}
bool Robot::move()
{
    switch (getDirection())
    {
        case GraphObject::down:
                moveTo(getX(),getY()-1);
            break;
        case GraphObject::up:
                moveTo(getX(),getY()+1);
            break;
        case GraphObject::right:
                moveTo(getX()+1,getY());
            break;
        case GraphObject::left:
                moveTo(getX()-1,getY());
            break;
        default:
            return false;
    }
    return false;
}
bool Robot::canMove()
{
    switch (getDirection())
    {
        case GraphObject::down:
            if (!getWorld()->blocksRobot(getX(),getY()-1))
                return true;
            break;
        case GraphObject::up:
            if (!getWorld()->blocksRobot(getX(),getY()+1))
                return true;
            break;
        case GraphObject::right:
            if (!getWorld()->blocksRobot(getX()+1,getY()))
                return true;
            break;
        case GraphObject::left:
            if (!getWorld()->blocksRobot(getX()-1,getY()))
                return true;
            break;
        default:
            return false;
    }
    return false;
}
Actor::Direction KleptoBot::changeDirection()
{
    int k = rand()%4;
    switch (k)
    {
        case 0:return up;break;
        case 1:return down;break;
        case 2:return left;break;
        case 3:return right;break;
    }
    return none;
}
Actor::Direction Snarlbot::changeDirection()
{
    switch(getDirection())
    {
        case up:return down;break;
        case down:return up;break;
        case left:return right;break;
        case right:return left;break;
    }
    return none;
}
void KleptoBotFactory::doSomething()
{
    if (getWorld()->census(getX(), getY()) < 3 && !getWorld()->kleptoAt(getX(),getY()))
    {
        if (rand()%50 == 0)
        {
            switch (m_type)
            {
                case ANGRY:
                    getWorld()->addActor(new AngryKleptoBot(getX(),getY(),getWorld()));
                        break;
                case REGULAR:
                    getWorld()->addActor(new RegularKleptoBot(getX(),getY(),getWorld()));
                        break;
            }
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
    return;
}
void Exit::doSomething()
{
    if(getWorld()->getJewel() == 0)
    {
        if(getWorld()->playerX() == getX() && getWorld()->playerY() == getY())
        {
            getWorld()->setCompleted();
            getWorld()->playSound(SOUND_FINISHED_LEVEL);
        }
    }
}
