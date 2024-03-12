#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//ACTOR===========================================================================================

Actor::Actor(StudentWorld* sw, int ID, double x, double y, int direction)
 : GraphObject(ID, x, y, direction), m_alive(true), m_world(sw), m_bonus(0), m_hp(0)
{
    setVisible(true);
}

Actor::~Actor() {}

StudentWorld* Actor::getWorld() {
    return m_world;
}

bool Actor::getAlive() {
    return m_alive;
}

void Actor::setAlive() {
    m_alive = true;
    setVisible(true);
    setDirection(right);
}

void Actor::kill() {
    m_alive = false;
    setVisible(false);
}

bool Actor::canCollide() {
    return false;
}

bool Actor::canPush() {
    return false;
}

bool Actor::canFill() {
    return false;
}

bool Actor::canPickup() {
    return false;
}

double Actor::getBonus() {
    return m_bonus;
}

void Actor::setBonus(int bonus) {
    m_bonus = bonus;
}

bool Actor::canReveal() {
    return false;
}

bool Actor::canShoot() {
    return false;
}

int Actor::getHp() {
    return m_hp;
}

void Actor::setHp(int hp) {
    m_hp = hp;
}

bool Actor::canSteal() {
    return false;
}

bool Actor::stealable() {
    return false;
}

//WALL===========================================================================================

Wall::Wall(StudentWorld* sw, double x, double y)
: Actor(sw, IID_WALL, x, y, none)
{}

Wall::~Wall() {}

void Wall::doSomething() {
}

//PIT===========================================================================================

Pit::Pit(StudentWorld*sw, double x, double y)
: Actor(sw, IID_PIT, x, y, none)
{}

Pit::~Pit() {}

void Pit::doSomething() {
    if (!getAlive()) {
        return;
    }
    if (getWorld()->colocationMarble(getX(), getY())) {
        kill();
    }
}

//PICKER===========================================================================================

Picker::Picker(StudentWorld* sw, int ID, double x, double y, double bonus)
: Actor(sw, ID, x, y, none)
{
    setBonus(bonus);
}

void Picker::increaseScore() {
    getWorld()->increaseScore(getBonus());
    kill();
}

Picker::~Picker() {}

//CRYSTAL===========================================================================================

Crystal::Crystal(StudentWorld* sw, double x, double y)
: Picker(sw, IID_CRYSTAL, x, y, 50)
{}

Crystal::~Crystal() {}

void Crystal::doSomething() {
    if (!getAlive()) {
        return;
    }
    if (getWorld()->playerOn(getX(), getY())) {
        getWorld()->decreaseCrystal();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        increaseScore();
    }
}

//GOODIE===========================================================================================

Goodie::Goodie(StudentWorld* sw, int ID, double x, double y, int bonus)
: Picker(sw, ID, x, y, bonus)
{}

Goodie::~Goodie() {}

void Goodie::doSomething() {}

//EXTRALIFE===========================================================================================

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* sw, double x, double y)
: Goodie(sw, IID_EXTRA_LIFE, x, y, 1000)
{}

ExtraLifeGoodie::~ExtraLifeGoodie() {}

void ExtraLifeGoodie::doSomething() {
    if (!getAlive()) {
        return;
    }
    if (getWorld()->playerOn(getX(), getY())) {
        getWorld()->incLives();
        increaseScore();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

//RESTOREHEALTH===========================================================================================

RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* sw, double x, double y)
: Goodie(sw, IID_RESTORE_HEALTH, x, y, 500)
{}

RestoreHealthGoodie::~RestoreHealthGoodie() {}

void RestoreHealthGoodie::doSomething() {
    if (!getAlive()) {
        return;
    }
    if (getWorld()->playerOn(getX(), getY())) {
        getWorld()->restoreHp();
        increaseScore();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

//AMMO===========================================================================================

AmmoGoodie::AmmoGoodie(StudentWorld* sw, double x, double y)
: Goodie(sw, IID_AMMO, x, y, 100)
{}

AmmoGoodie::~AmmoGoodie() {}

void AmmoGoodie::doSomething() {
    if (!getAlive()) {
        return;
    }
    if (getWorld()->playerOn(getX(), getY())) {
        getWorld()->addAmmo();
        increaseScore();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

//DOOR===========================================================================================

Door::Door(StudentWorld* sw, double x, double y)
:Actor(sw, IID_EXIT, x, y, none)
{
    setVisible(false);
}

Door::~Door() {}

void Door::doSomething() {
    getWorld()->revealDoor();
}

//ALIVE===========================================================================================

Alive::Alive(StudentWorld* sw, int ID, double x, double y, int direction, int hp)
: Actor(sw, ID, x, y, direction)
{
    setHp(hp);
}

Alive::~Alive() {}


//AVATAR===========================================================================================

Avatar::Avatar(StudentWorld* sw, double x, double y)
 : Alive(sw, IID_PLAYER, x, y, right, 20), m_ammo(20)
{}

Avatar::~Avatar() {}

void Avatar::doSomething() {
    int c;
    if (getWorld()->getKey(c))
    {
        double x = getX();
        double y = getY();
        switch(c) {
            case KEY_PRESS_ESCAPE:
                kill();
                getWorld()->playSound(SOUND_PLAYER_DIE);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                y++;
                if (getWorld()->canPlayerMove(x, y, getDirection())){
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                x++;
                if (getWorld()->canPlayerMove(x, y, getDirection())){
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                y--;
                if (getWorld()->canPlayerMove(x, y, getDirection())){
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_LEFT:
                setDirection(left);
                x--;
                if (getWorld()->canPlayerMove(x, y, getDirection())){
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_SPACE:
                if (getAmmo() > 0) {
                    getWorld()->createPea(x, y, getDirection());
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_ammo--;
                }
                break;
        }
    }
}

//int Avatar::getScore() {
//    return m_score;
//}

int Avatar::getAmmo() {
    return m_ammo;
}

//MARBLE===========================================================================================

Marble::Marble(StudentWorld* sw, double x, double y)
: Alive(sw, IID_MARBLE, x, y, none, 10)
{}

Marble::~Marble() {}

void Marble::doSomething() {}

//PEA===========================================================================================

Pea::Pea(StudentWorld* sw, double x, double y, int dir)
:Actor(sw, IID_PEA, x, y, dir)
{}

Pea::~Pea() {}

void Pea::doSomething() {
    double x = getX();
    double y = getY();
    if (getWorld()->playerOn(x, y) || (getWorld()->collision(x, y) && !getWorld()->canPassThrough(x, y))) {
        setVisible(false);
        kill();
        getWorld()->shootIt(x, y);
        return;
    }
    switch(getDirection()) {
        case(up):
            y++;
            break;
        case(right):
            x++;
            break;
        case(down):
            y--;
            break;
        case(left):
            x--;
            break;
    }
    moveTo(x, y);
    if (getWorld()->playerOn(x, y) || (getWorld()->collision(x, y) && !getWorld()->canPassThrough(x, y))) {
        setVisible(false);
        kill();
        getWorld()->shootIt(x, y);
        return;
    }
}

//ROBOT===========================================================================================
Robot::Robot(StudentWorld* sw, int ID, double x, double y, int direction, int hp, int level, int bonus)
: Alive(sw, ID, x, y, direction, hp), m_level(level)
{
    setTick();
    setBonus(bonus);
}
Robot::~Robot() {}

void Robot::doSomething() {
    if (!getAlive()) {
        return;
    }
    if (getTick() != 1) {
        decTick();
        return;
    }
    else {
        setTick();
        doAction(getX(), getY(), getDirection());
    }
}

void Robot::setTick() {
    int tick = (28 - m_level) / 4;
    if (tick < 3)
        tick = 3;
    m_tick = tick;
}

//RAGEBOT===========================================================================================

RageBot::RageBot(StudentWorld* sw, double x, double y, int direction, int level)
: Robot(sw, IID_RAGEBOT, x, y, direction, 10, level, 100)
{}

RageBot::~RageBot() {}

void RageBot::doAction(double x, double y, int direction) {
    if (getWorld()->clearShot(x, y, direction)) {
        getWorld()->createPea(x, y, direction);
        getWorld()->playSound(SOUND_ENEMY_FIRE);
    }
    else {
        switch(direction) {
            case up:
                y++;
                if (!getWorld()->collision(x, y)) {
                    moveTo(x, y);
                }
                else {
                    setDirection(down);
                }
                break;
            case down:
                y--;
                if (!getWorld()->collision(x, y)) {
                    moveTo(x, y);
                }
                else {
                    setDirection(up);
                }
                break;
            case left:
                x--;
                if (!getWorld()->collision(x, y)) {
                    moveTo(x, y);
                }
                else {
                    setDirection(right);
                }
                break;
            case right:
                x++;
                if (!getWorld()->collision(x, y)) {
                    moveTo(x, y);
                }
                else {
                    setDirection(left);
                }
                break;
        }
    }
}

//THIEFBOT===========================================================================================

ThiefBot::ThiefBot(StudentWorld* sw, int ID, double x, double y, int direction, int hp, int level, int bonus, bool shooterRob)
: Robot(sw, ID, x, y, direction, hp, level, bonus), m_shooterRob(shooterRob), m_alreadyPicked(false)
{}

void ThiefBot::doAction(double x, double y, int direction) {
    if (getShooterRob()) {
        if (getWorld()->clearShot(x, y, direction)) {
            getWorld()->createPea(x, y, direction);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            return;
        }
    }
    if (getWorld()->colocationGoodie(getX(), getY()) && !getAlreadyPicked()) {
        int num = (randInt(1, 10));
        if (num == 1) {
            setGoodieBonus(getWorld()->stealGoodie(getX(), getY()));
            pickedUp();
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
            return;
        }
    }
    
    if (getDistance() == 0) {
        changeDirection();
        return;
    }
    
    switch(direction) {
        case up:
            if (!getWorld()->collision(x, y+1) && !getWorld()->playerOn(x, y+1)) {
                moveTo(x, y+1);
                return;
            }
            else {
                changeDirection();
                return;
            }
            break;
        case down:
            if (!getWorld()->collision(x, y-1) && !getWorld()->playerOn(x, y - 1)) {
                moveTo(x, y-1);
                return;
            }
            else {
                changeDirection();
                return;
            }
            break;
        case right:
            if (!getWorld()->collision(x+1, y) && !getWorld()->playerOn(x + 1, y)) {
                moveTo(x+1, y);
                return;
            }
            else {
                changeDirection();
                return;
            }
            break;
        case left:
            if (!getWorld()->collision(x-1, y) && !getWorld()->playerOn(x-1, y)) {
                moveTo(x-1, y);
                return;
            }
            else {
                changeDirection();
                return;
            }
            break;
    }
}

void ThiefBot::changeDirection() {
    int randNum = randInt(1, 4);
    int dir = -1;
    switch(randNum) {
        case 1:
            dir = up;
            break;
        case 2:
            dir = down;
            break;
        case 3:
            dir = right;
            break;
        case 4:
            dir = left;
            break;
    }
    int origDir = dir;
    for (int i = 0; i < 4; i++) {
        switch(dir) {
            case up:
                if (!getWorld()->collision(getX(), getY()+1) && !getWorld()->playerOn(getX(), getY()+1)) {
                    setDirection(dir);
                    moveTo(getX(), getY()+1);
                    return;
                }
                break;
            case down:
                if (!getWorld()->collision(getX(), getY()-1) && !getWorld()->playerOn(getX(), getY()-1)) {
                    setDirection(dir);
                    moveTo(getX(), getY()-1);
                    return;
                }
                break;
            case right:
                if (!getWorld()->collision(getX()+1, getY()) && !getWorld()->playerOn(getX()+1, getY())) {
                    setDirection(dir);
                    moveTo(getX()+1, getY());
                    return;
                }
                break;
            case left:
                if (!getWorld()->collision(getX()-1, getY()) && !getWorld()->playerOn(getX()-1, getY())) {
                    setDirection(dir);
                    moveTo(getX()-1, getY());
                    return;
                }
                break;
        }
        dir = (dir + 90) % 360;
    }
    setDirection(origDir);
}

void ThiefBot::dropItem() {
    if (getAlreadyPicked()) {
        getWorld()->dropGoodie(getX(), getY(), getPickedUpGoodieBonus());
    }
}

//REGULAR THIEF BOT ===========================================================================================

RegThiefBot::RegThiefBot(StudentWorld* sw, double x, double y, int level)
: ThiefBot(sw, IID_THIEFBOT, x, y, right, 5, level, 10, false)
{}

//MEAN THIEF BOT ===========================================================================================
MeanThiefBot::MeanThiefBot(StudentWorld* sw, double x, double y, int level)
: ThiefBot(sw, IID_MEAN_THIEFBOT, x, y, right, 8, level, 10, true)
{}

//THIEF BOT FACTORY ===========================================================================================
ThiefBotFactory::ThiefBotFactory(StudentWorld* sw, double x, double y, bool mean)
: Actor(sw, IID_ROBOT_FACTORY, x, y, none), m_meanBot(mean)
{}

void ThiefBotFactory::doSomething() {
    if (getWorld()->countTBots(getX(), getY()) < 3 && !getWorld()->colocationThief(getX(), getY())) {
        int chance = randInt(1, 50);
        
        if (chance == 1) {
            getWorld()->spawnBot(getX(), getY(), getIsMean());
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
}
