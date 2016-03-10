

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <iostream>
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* world, int health = 0, Direction dir = none) :
    GraphObject(imageID,startX,startY,dir) , m_world(world) , m_live(true) , m_health(health)
    {
        setVisible(true);
    }
    //standard doSomething function that calls every actor to act
    virtual void doSomething()=0;
    //returns the world the actor is in
    StudentWorld* getWorld() const{return m_world;}
    //property of blocking the player
    virtual bool blocksPlayer() {return false;}
    //property of blocking a boulder
    virtual bool blocksBoulder() {return false;}
    //property of blocking(destroying) a bullet
    virtual bool blocksBullet() {return false;}
    //property of blocking a robot
    virtual bool blocksRobot() {return false;}
    //property of getting damaged by a bullet
    virtual bool interactsBullet(){return false;}
    //property of being able to be picked up
    virtual bool isPickUpAble(){return false;}
    //property of being counted in robot census
    virtual bool countsInCensus(){return false;}
    //property of being swalloable by a wholer
    virtual bool isSwalloable(){return false;}
    //Sets a actor to be alive or dead
    void setLife(bool tf) {m_live = tf;}
    //returns whether the actor is alive or not
    bool isLive() const{return m_live;}
    //returns the health (#) of the actor
    int getHealth() const{return m_health;}
    //shoots the actor, reducing health by 2
    void shoot(){m_health = m_health - 2;}
    //handles the result of the shoot and what sound to play
    virtual void damage(){};
    //sets health to the given number
    void setHealth(int b){m_health = b;}
    
private:
    unsigned int m_health;
    bool m_live;
    StudentWorld* m_world;
};
class Shooter : public Actor
{
public:
    Shooter(int imageID, int startX, int startY, StudentWorld* world, int health, Direction dir  = none) : Actor(imageID,startX,startY,world,health)
    {
        setDirection(dir);
    }
    virtual void damage()=0;
    //all shooters block the player
    virtual bool blocksPlayer() {return true;}
    //all shooters block boulders
    virtual bool blocksBoulder() {return true;}
    //all shooters get damaged by bullets
    virtual bool interactsBullet() {return true;}
    //all shooters block robots
    virtual bool blocksRobot() {return true;}
private:

};
class Player : public Shooter
{
public:
    Player(int startX, int startY, StudentWorld* world, Direction dir = none) :
    Shooter(IID_PLAYER, startX, startY, world,20, right) , m_ammo(20)
    {
        setDirection(GraphObject::right);
    }
    virtual void doSomething();
    //damages the player
    virtual void damage();
    //increases the ammo by the given amount
    void incAmmo(int a){m_ammo+=a;}
    //returns the amount of ammo the player has
    unsigned int getAmmo(){return m_ammo;}
    //restores health to full amount
    void healthGoodie(){setHealth(20);}
    //returns the health in a form of percent;
    unsigned int getHealthPercent()
    {
        double conv = getHealth()/20.0 * 100;
        int ret = conv;
        return ret;
    }
private:
    int m_ammo;
};
class Robot : public Shooter
{
public:
    Robot(int image, int startX, int startY, StudentWorld* world,int health, Direction dir) :
        Shooter(image,startX,startY,world,health,dir) , m_tick(0)
    {}
    //damages the Robot and plays the robot sounds and calls the special for Kleptobots so they drop the goodie
    virtual void damage();
    //returns whether the robot should act on this turn
    bool shouldAct();
    //moves the robot in the direction its facing
    bool move();
    //attemps to shoot if the player is in sight returns whether it successfully shot
    virtual bool attemptShoot();
    //returns whether or not the robot can move in its direction
    virtual bool canMove();
    //each robot changes differently this changes the direction
    virtual Direction changeDirection()=0;
    //the amount for the score to be incerememnted when the robot dies
    virtual int killBonus()=0;
    //sepcial things the robot does when it dies
    virtual void special(){}
private:
    int m_tick;
};
class Snarlbot : public Robot
{
public:

    Snarlbot(int startX,int startY, StudentWorld* world, Direction dir) :
        Robot(IID_SNARLBOT,startX,startY,world,10,dir)
    {}
    virtual void doSomething();
    //Snarlbots reward 100 pts when they die
    virtual int killBonus(){return 100;}
    //changes the direction of the snarlBot
    virtual Direction changeDirection();
    
private:
};
class KleptoBot : public Robot
{
public:
    KleptoBot(int id, int startX, int startY, StudentWorld* world, int health, Direction dir) :
        Robot(id,startX,startY,world,health,dir) , id_stolen(0)
    {
        distleft = (rand() % 6) + 1;
    }
    virtual void doSomething();
    //kleptobots count in the census count
    virtual bool countsInCensus(){return true;}
    //resets the distance before turning
    void resetDistance(){distleft = (rand() % 6) + 1;}
    //returns whether the robot should conitnue in its path or turn
    virtual bool shouldMove()
    {
        if (distleft > 0)
            return true;
        return false;
    }
    //tells the kleptobot to drop its goodie
    virtual void special();
    //changes the kleptobots direction
    virtual Direction changeDirection();
private:
    int id_stolen,distleft;
    
};
class RegularKleptoBot : public KleptoBot
{
public:
    RegularKleptoBot(int startX, int startY, StudentWorld* world) :
        KleptoBot(IID_KLEPTOBOT,startX,startY,world,5,right)
    {}
    //Kleptobots never shoot so always false
    virtual bool attemptShoot(){return false;}
    //kleptobots grant 10 points
    virtual int killBonus(){return 10;}
private:
};
class AngryKleptoBot : public KleptoBot
{
public:
    AngryKleptoBot(int startX, int startY, StudentWorld* world) :
        KleptoBot(IID_ANGRY_KLEPTOBOT,startX,startY,world,8,right)
    {}
private:
    //kleptobots award 20 points
    virtual int killBonus(){return 20;}
    
};
class Wall : public Actor
{
public:
    Wall(int startX, int startY, StudentWorld* world) :
        Actor(IID_WALL, startX, startY, world)
    {}
    //walls dont do anything
    virtual void doSomething(){}
    //walls block players
    virtual bool blocksPlayer(){return true;}
    //walls block boulders
    virtual bool blocksBoulder(){return true;}
    //walls block bullets
    virtual bool blocksBullet(){return true;}
    //walls block robots
    virtual bool blocksRobot(){return true;}
private:
};
class Exit : public Actor
{
public:
    Exit(int startX, int startY, StudentWorld* world) :
    Actor(IID_EXIT, startX, startY, world)
    {
        setVisible(false);
    }
    virtual void doSomething();
    //exits block boulders
    virtual bool blocksBoulder(){return true;}
private:
};
class Goodie : public Actor
{
public:
    Goodie(int id,int startX, int startY, StudentWorld* world) :
        Actor(id,startX,startY,world) , m_stolen(false)
    {}
    virtual void doSomething();
    //rewards the amount depending on the type of goodie;
    virtual void reward()=0;
    //sets the goodie to stolen
    void setStolen(bool tf){setLife(false);}
    //returns whether the goodie is stolen or not
    bool isStolen(){return m_stolen;}
    //goodies block boulders
    virtual bool blocksBoulder(){return true;}
    //goodies can be picked up
    virtual bool isPickUpAble(){return true;}
    //goodies must have an id when they are picked up
    virtual int id() = 0;
private:
    bool m_stolen;
};
class Jewel : public Actor
{
public:
    Jewel(int startX,int startY, StudentWorld* world) :
        Actor(IID_JEWEL,startX,startY,world)
    {}
    virtual void doSomething();
private:
};
class AmmoGoodie : public Goodie
{
public:
    AmmoGoodie(int startX,int startY, StudentWorld* world) :
        Goodie(IID_AMMO,startX,startY,world)
    {}
    void reward();
    virtual int id(){return 1;}
private:
};
class LifeGoodie : public Goodie
{
public:
    LifeGoodie(int startX,int startY,StudentWorld* world) :
        Goodie(IID_EXTRA_LIFE,startX,startY,world)
    {}
    void reward();
    virtual int id(){return 2;}
    
};
class HealthGoodie : public Goodie
{
public:
    HealthGoodie(int startX,int startY,StudentWorld* world) :
        Goodie(IID_RESTORE_HEALTH,startX,startY,world)
    {}
    void reward();
    virtual int id(){return 3;}
};
class Hole : public Actor
{
public:
    Hole(int startX, int startY, StudentWorld* world) :
        Actor(IID_HOLE,startX,startY,world)
    {}
    virtual void doSomething();
    //holes block players
    virtual bool blocksPlayer(){return true;}
    //holes block robots
    virtual bool blocksRobot(){return true;}
};
class Boulder : public Actor
{
public:
    Boulder(int startX, int startY, StudentWorld* world) :
        Actor(IID_BOULDER,startX,startY,world,10)
    {}
    virtual void doSomething(){}
    //damages the boulder
    virtual void damage();
    //boulders block boulders
    virtual bool blocksBoulder(){return true;}
    //boulders are damages by bullets
    virtual bool interactsBullet(){return true;}
    //robots are blocked by boulders
    virtual bool blocksRobot(){return true;}
    //boulders can be swallowed
    virtual bool isSwalloable(){return true;}
};
class KleptoBotFactory : public Actor
{
public:
    enum ProductType { REGULAR, ANGRY };
    
    KleptoBotFactory(int startX, int startY, StudentWorld* world, ProductType type) :
        Actor(IID_ROBOT_FACTORY,startX,startY,world), m_type(type)
    {}
    virtual void doSomething();
    //kleptobot factories block players
    virtual bool blocksPlayer() {return true;}
    //kleptobot factories block boulders
    virtual bool blocksBoulder() {return true;}
    //kleptobot factories block robots
    virtual bool blocksRobot() {return true;}
    //kleptobot fatories block bullets
    virtual bool blocksBullet(){return true;}
private:
    ProductType m_type;
};
class Bullet : public Actor
{
public:
    Bullet(int startX, int startY, StudentWorld* world, Direction dir) :
        Actor(IID_BULLET,startX,startY,world)
    {
        setDirection(dir);
    }
    virtual void doSomething();
private:
};
#endif // ACTOR_H_
