#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include "Level.h"
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    
    enum Direction { none, up, down, left, right};
    
	StudentWorld(std::string assetDir)
    : GameWorld(assetDir), m_jewels(0), completed(false), soundPlayed(false)
	{}
    ~StudentWorld();
    //Necessary Functions
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    //Updates:
    // updates the score status
    void printStatus();
    
    //CheckFunctions:
    //returns an actor pointer to an actor at the given coordinates
    Actor* getAt(int x, int y);
    
    //checks if a boulder is at the location
    bool boulderCheck(int x, int y);
    
    //checks if the location has an actor that blocks the player
    bool blocksPlayer(int x, int y);
    
    //chekcs if the location has an actor that blocks a boulder
    bool blocksBoulder(int x, int y);
    
    //checks if the location has an actor that blocks a robot
    bool blocksRobot(int x, int y);
    
    //checks if the location has an actor that kill the bullet
    bool killsBullet(int x, int y);
    
    //chekcs if the location has an actor that get damaged by the bullet
    bool interactsBullet(int x, int y);
    
    //checks if a KleptoBot is at the location
    bool kleptoAt(int x, int y);
    
    //checks if a goodie is at a location and changes a pointer to the goodie if there is a goodie there
    bool goodieAt(int x, int y, Goodie*&good);
    
    //creates a new bullet relative to the x and y coordinates and direction adjacent to the given coordinate
    void newBullet(int x, int y, GraphObject::Direction dir);
    
    //moves a boulder in the given direction
    bool moveBoulder(int x, int y, GraphObject::Direction Dir);
    
    //checks if the player is in sight of the given coordinates
    bool playerSight(int x, int y, GraphObject::Direction dir);
    
    //adds the given actor pointer to the vector of actors
    void addActor(Actor* ap);
    
    //checks the number of kleptobots in range of the given coordinates
    int census(int x, int y);
    
    //returns a pointer to the player
    Player* getPlayer(){return m_player;}
    
    //returns a pointer to the given boulder at a location if one exists
    Actor* getBoulder(int x, int y);
    
    //returns the bonus
    unsigned int getBonus(){return m_bonus;}
    
    //returns the X coordinate of the player
    int playerX(){return m_player->getX();}
    
    //returns the y coordinate of the player
    int playerY(){return m_player->getY();}
    
    //increments the jewel count by one
    void addJewel(){m_jewels++;}
    
    //decrements the jewel count by one
    void decJewel(){m_jewels--;}
    
    //returns the number of jewels
    int getJewel(){return m_jewels;}
    
    //decreases the bonus by one
    void decBonus();
    
    //checks if the level is completed
    bool hasCompleted(){return completed;}
    
    //sets level to completed
    void setCompleted(){completed = true;};
    
    void exitCheck();
    

private:
    bool completed, soundPlayed;
    string curlevel;
    int loadlevel();
    unsigned int m_bonus, m_jewels;
    Level* lev;
    Player* m_player;
    vector<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
