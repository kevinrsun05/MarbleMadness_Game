#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "Level.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
  public:
    Actor(StudentWorld* sw, int ID, double x, double y, int direction);
    virtual void doSomething() = 0;
    StudentWorld* getWorld();
    bool getAlive();
    void setAlive();
    void kill();
    
    virtual bool canCollide(); //Are you allowed to walk onto it? Default: False
    
    virtual bool canPush(); //Can you push it? Default: False
    
    virtual bool canFill(); //Can you fill it up? Default: False
    
    virtual bool canPickup(); //Can you pick it up? Default: False
    
    double getBonus();
    void setBonus(int bonus);
    
    virtual bool canReveal(); //Can you reveal it? Default: False
    
    virtual bool canShoot(); //Can the object be shot? Default: False
    
    virtual ~Actor();
    
    int getHp();
    
    virtual void setHp(int hp);
    
    virtual bool canSteal(); //Can the actor steal? Default: false
    virtual bool stealable(); //Can object be stolen? Default false
    
    virtual void dropItem() {}
    
  private:
    StudentWorld* m_world;
    bool m_alive;
    double m_bonus;
    int m_hp;
};

//ALIVE THINGS======================================================================
class Alive : public Actor {
  public:
    Alive(StudentWorld* sw, int ID, double x, double y, int direction, int hp);
    virtual void doSomething() = 0;
    virtual ~Alive() = 0;
    virtual bool canShoot() {
        return true;
    }
};

class Avatar : public Alive {
  public:
    Avatar(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual ~Avatar();
//    int getScore();
    int getAmmo();
    void addAmmo(int ammo) {
        m_ammo += ammo;
    }
  private:
    int m_ammo;
//    int m_score;
};

class Marble : public Alive {
  public:
    Marble(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual ~Marble();
    virtual bool canPush() {
        return true;
    }
};

class Robot : public Alive {
  public:
    Robot(StudentWorld* sw, int ID, double x, double y, int direction, int hp, int level, int bonus);
    virtual void doSomething();
    virtual ~Robot();
    void setTick();
    int getTick() {
        return m_tick;
    }
    void decTick() {
        m_tick--;
    }
    virtual void doAction(double x, double y, int direction) = 0;
    
  private:
    int m_level;
    int m_tick;
};

class RageBot : public Robot {
  public:
    RageBot(StudentWorld* sw, double x, double y, int direction, int level);
    virtual ~RageBot();
    virtual void doAction(double x, double y, int direction);
};

class ThiefBot : public Robot {
  public:
    ThiefBot(StudentWorld* sw, int ID, double x, double y, int direction, int hp, int level, int bonus, bool shooterRob);
    virtual ~ThiefBot() {}
    virtual void doAction(double x, double y, int direction);
    void setDistance() {
        m_distance = rand() % 6 + 1;
    }
    int getDistance() { return m_distance; }
    void decDistance() { m_distance--; }
    bool getShooterRob() { return m_shooterRob; }
    void changeDirection();
    bool getAlreadyPicked() { return m_alreadyPicked; }
    void pickedUp() { m_alreadyPicked = true; }
    int getPickedUpGoodieBonus() { return m_goodieBonus; }
    void setGoodieBonus(int bonus) { m_goodieBonus = bonus; }
    virtual void dropItem();
    virtual bool canSteal() {
        return true;
    }
  private:
    int m_distance;
    bool m_shooterRob;
    bool m_alreadyPicked;
    int m_goodieBonus;
    
};

class RegThiefBot : public ThiefBot {
  public:
    RegThiefBot(StudentWorld* sw, double x, double y, int level);
    virtual ~RegThiefBot() {}
};

class MeanThiefBot : public ThiefBot {
  public:
    MeanThiefBot(StudentWorld* sw, double x, double y, int level);
    virtual ~MeanThiefBot() {}
};

//FACTORY======================================================================

class ThiefBotFactory : public Actor {
  public:
    ThiefBotFactory(StudentWorld* sw, double x, double y, bool mean);
    ~ThiefBotFactory() {}
    virtual void doSomething();
    bool getIsMean() { return m_meanBot; }
    virtual bool canShoot() {
        return true;
    }
    
  private:
    bool m_meanBot;
};

//======================================================================

class Wall : public Actor {
  public:
    Wall(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual ~Wall();
    virtual bool canShoot() {
        return true;
    }
};

class Pit : public Actor {
  public:
    Pit(StudentWorld* sw, double x, double y);
    virtual ~Pit();
    virtual void doSomething();
    virtual bool canFill() {
        return true;
    }
};

class Door : public Actor {
  public:
    Door(StudentWorld* sw, double x, double y);
    virtual ~Door();
    virtual void doSomething();
    virtual bool
    canCollide() {
        return true;
    }
    virtual bool canReveal() {
        return true;
    }
};

class Pea : public Actor {
  public:
    Pea(StudentWorld* sw, double x, double y, int dir);
    virtual ~Pea();
    virtual void doSomething();
    virtual bool canCollide() {
        return true;
    }
};

//PICK UPPABLE THINGS======================================================================
class Picker : public Actor {
  public:
    Picker(StudentWorld* sw, int ID, double x, double y, double bonus);
    virtual ~Picker() = 0;
    virtual bool canCollide() {
        return true;
    }
    virtual bool canPickup() {
        return true;
    }
    void increaseScore();
};

class Crystal : public Picker {
  public:
    Crystal(StudentWorld* sw, double x, double y);
    virtual ~Crystal();
    virtual void doSomething();
};

class Goodie : public Picker {
  public:
    Goodie(StudentWorld* sw, int ID, double x, double y, int bonus);
    virtual ~Goodie();
    virtual void doSomething();
    virtual bool stealable() {
        return true;
    }
};

class ExtraLifeGoodie : public Goodie {
  public:
    ExtraLifeGoodie(StudentWorld *sw, double x, double y);
    virtual ~ExtraLifeGoodie();
    virtual void doSomething();
};

class RestoreHealthGoodie : public Goodie {
  public:
    RestoreHealthGoodie(StudentWorld *sw, double x, double y);
    virtual ~RestoreHealthGoodie();
    virtual void doSomething();
};

class AmmoGoodie : public Goodie {
  public:
    AmmoGoodie(StudentWorld *sw, double x, double y);
    virtual ~AmmoGoodie();
    virtual void doSomething();
};
#endif // ACTOR_H_
