#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <list>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Avatar;
class Wall;

class StudentWorld : public GameWorld
{
  public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    void Initmaze();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool collision(double x, double y); //Is there a collision?
    
    bool canPushMarble(double x, double y, int direction); //Can you push the marble
    bool marbleCanGoOn(double x, double y); //Can the marble go on that square (not consumable or exit)
    bool canFill(double x, double y); //Can you fill it
    bool colocationMarble(double x, double y);
    
    void setDisplayText();
    void addZero(int num, string& s_num, int leading, char c);
    
    void kill(list<Actor*>::iterator it); //Kill object, set not visible not alive
    
    void revealDoor(); //Can you reveal the door yet
    bool completeLevel(double x, double y); //Did you complete level
    
    bool canPlayerMove(double x, double y, int direction);
    bool playerOn(double x, double y); //is player on that space
    
    void createPea(double x, double y, int dir); //Create Pea
    bool canPassThrough(double x, double y); //can the object be passed through
    void shootIt(double x, double y); //shoot object
    
    void decreaseCrystal();
    void restoreHp();
    void addAmmo();
    
    bool clearShot(double x, double y, int direction);
    bool colocationGoodie(double x, double y);
    int stealGoodie(double x, double y);
    void dropGoodie(double x, double y, int bonus);
    
    bool colocationThief(double x, double y);
    int countTBots(double x, double y);
    void spawnBot(double x, double y, bool mean);

private:
    list<Actor*> m_actorList;
    Avatar* m_avatar;
    double m_bonus = 1000;
    int m_crystalsLeft = 0;
    bool wonGame = false;
    bool errorLevel = false;
};



#endif // STUDENTWORLD_H_
