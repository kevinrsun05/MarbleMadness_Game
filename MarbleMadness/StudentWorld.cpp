#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

#include "Level.h" // required to use our provided class


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

void StudentWorld::Initmaze() {
    if (getLevel() == 100) {
        wonGame = true;
        return;
    }
    string level = "level";
    Level lev(assetPath());
    string s_level;
    addZero(getLevel(), s_level, 2, '0');
    level += s_level;
    level += ".txt";
    
    Level::LoadResult result = lev.loadLevel(level);
    if (result == Level::load_fail_file_not_found) {
        wonGame = true;
    }
    else if (result == Level::load_fail_bad_format)
        errorLevel = true;
    else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level\n";
        for (int i = 0; i < VIEW_WIDTH; i++) {
            for (int j = 0; j < VIEW_HEIGHT; j++) {
                Level::MazeEntry ge = lev.getContentsOf(i, j);
                switch(ge){
                    case Level::empty:
                        break;
                    case Level::player:
                        m_avatar = new Avatar(this, i, j);
                        break;
                    case Level::wall:
                        m_actorList.push_back(new Wall(this, i, j));
                        break;
                    case Level::marble:
                        m_actorList.push_back(new Marble(this, i, j));
                        break;
                    case Level::pit:
                        m_actorList.push_back(new Pit(this, i, j));
                        break;
                    case Level::crystal:
                        m_actorList.push_back(new Crystal(this, i, j));
                        m_crystalsLeft++;
                        break;
                    case Level::exit:
                        m_actorList.push_back(new Door(this, i, j));
                        break;
                    case Level::extra_life:
                        m_actorList.push_back(new ExtraLifeGoodie(this, i, j));
                        break;
                    case Level::restore_health:
                        m_actorList.push_back(new RestoreHealthGoodie(this, i, j));
                        break;
                    case Level::ammo:
                        m_actorList.push_back(new AmmoGoodie(this, i, j));
                        break;
                    case Level::horiz_ragebot:
                        m_actorList.push_back(new RageBot(this, i, j, Actor::right, getLevel()));
                        break;
                    case Level::vert_ragebot:
                        m_actorList.push_back(new RageBot(this, i, j, Actor::down, getLevel()));
                        break;
                    case Level::thiefbot_factory:
                        m_actorList.push_back(new ThiefBotFactory(this, i, j, false));
                        break;
                    case Level::mean_thiefbot_factory:
                        m_actorList.push_back(new ThiefBotFactory(this, i, j, true));
                        break;
                }
            }
        }
    }
}

int StudentWorld::init()
{
//    m_avatar = new Avatar(this, VIEW_WIDTH/2, VIEW_HEIGHT/2);
//    m_wall = new Wall(this, 2, 3);
    m_crystalsLeft = 0;
    Initmaze();
    if (wonGame) {
        return GWSTATUS_PLAYER_WON;
    }
    if (errorLevel) {
        return GWSTATUS_LEVEL_ERROR;
    }
    m_bonus = 1000;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::revealDoor() {
    list<Actor*>::iterator it;
    if (m_crystalsLeft == 0) {
        for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
            if ((*it)->canReveal() && !(*it)->isVisible()) {
                (*it)->setVisible(true);
                playSound(SOUND_REVEAL_EXIT);
            }
        }
    }
}

bool StudentWorld::completeLevel(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->canReveal() && (*it)->isVisible() && (*it)->getX() == x && (*it)->getY() == y) {
            return true;
        }
    }
    return false;
}

int StudentWorld::move()
{
    setDisplayText();
    if (m_bonus > 0)
        m_bonus--;
    if (completeLevel(m_avatar->getX(), m_avatar->getY())) {
        increaseScore(2000 + m_bonus);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    if (!m_avatar->getAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive()) {
            (*it)->doSomething();
        }
    }
    if (m_avatar->getAlive()) {
        m_avatar->doSomething();
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it = m_actorList.begin();
    while (it != m_actorList.end()) {
        delete (*it);
        it = m_actorList.erase(it);
    }
    delete m_avatar;
    m_avatar = nullptr;
}

bool StudentWorld::collision(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && !(*it)->canCollide()) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::marbleCanGoOn(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if (((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y) && ((*it)->canReveal() || (*it)->canPickup())) {
            return false;
        }
    }
    return true;
}

bool StudentWorld::canPushMarble(double x, double y, int direction) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->canPush()) {
            switch(direction) {
                case Actor::up:
                    y++;
                    break;
                case Actor::right:
                    x++;
                    break;
                case Actor::down:
                    y--;
                    break;
                case Actor::left:
                    x--;
                    break;
            }
            if (!collision(x, y) && marbleCanGoOn(x, y)) {
                (*it)->moveTo(x, y);
                return true;
            }
            else {
                if (canFill(x, y)) {
                    (*it)->moveTo(x, y);
                    return true;
                }
                return false;
            }
        }
    }
    return false;
}

bool StudentWorld::canFill(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->canFill()) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::colocationMarble(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->canPush()) {
            kill(it);
            return true;
        }
    }
    return false;
}

void StudentWorld::decreaseCrystal() {
    m_crystalsLeft--;
}

bool StudentWorld::playerOn(double x, double y) {
    if (m_avatar->getX() == x && m_avatar->getY() == y) {
        return true;
    }
    return false;
}

void StudentWorld::restoreHp() {
    m_avatar->setHp(20);
}

void StudentWorld::addAmmo() {
    m_avatar->addAmmo(20);
}

void StudentWorld::kill(list<Actor*>::iterator it) {
    (*it)->kill();
}

void StudentWorld::createPea(double x, double y, int dir) {
    switch(dir) {
        case(Actor::up):
            m_actorList.push_front(new Pea(this, x, y + 1, dir));
            break;
        case(Actor::right):
            m_actorList.push_front(new Pea(this, x + 1, y, dir));
            break;
        case(Actor::down):
            m_actorList.push_front(new Pea(this, x, y - 1, dir));
            break;
        case(Actor::left):
            m_actorList.push_front(new Pea(this, x - 1, y, dir));
            break;
    }
}

bool StudentWorld::canPlayerMove(double x, double y, int direction) {
    if (!collision(x, y)) {
        return true;
    }
    else if (canPushMarble(x, y, direction)){
        return true;
    }
    return false;
}

bool StudentWorld::canPassThrough(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->canShoot()) {
            return false;
        }
    }
    return true;
}

void StudentWorld::shootIt(double x, double y) {
    if (playerOn(x, y)) {
        m_avatar->setHp(m_avatar->getHp() - 2);
        playSound(SOUND_PLAYER_IMPACT);
        if (m_avatar->getHp() <= 0) {
            m_avatar->kill();
            playSound(SOUND_PLAYER_DIE);
        }
    }
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->getHp() > 0) {
            (*it)->setHp((*it)->getHp() - 2);
            if (!(*it)->canPush()) {
                playSound(SOUND_ROBOT_IMPACT);
            }
            if ((*it)->getHp() <= 0) {
                kill(it);
                if (!(*it)->canPush()) {
                    playSound(SOUND_ROBOT_DIE);
                    increaseScore((*it)->getBonus());
                    if ((*it)->canSteal()) {
                        (*it)->dropItem();
                    }
                }
                
            }
        }
    }
}

// SCOREBOARD STUFF

void StudentWorld::addZero(int num, string& s_num, int leading, char c) {
    int numDigits = 1;
    int tempNum = num;
    while (tempNum >= 10) {
        tempNum /= 10;
        numDigits++;
    }
    leading -= numDigits;
    for (int i = 0; i < leading; i++) {
        s_num += c;
    }
    s_num += to_string(num);
}

void StudentWorld::setDisplayText()
{
    int score = getScore();
    string s_score;
    addZero(score, s_score, 7, '0');
    
    int level = getLevel();
    string s_level;
    addZero(level, s_level, 2, '0');
    
    int livesLeft = getLives();
    string s_livesLeft;
    addZero(livesLeft, s_livesLeft, 2, ' ');
    
    double health = m_avatar->getHp() / 20.0 * 100;
    string s_health;
    addZero(health, s_health, 3, ' ');
    s_health += '%';
    
    int ammo = m_avatar->getAmmo();
    string s_ammo;
    addZero(ammo, s_ammo, 2, ' ');
    
    int bonus = m_bonus;
    string s_bonus;
    addZero(bonus, s_bonus, 4, ' ');
    
    string statusText = "Score: ";
    statusText.append(s_score);
    statusText.append(" Level: ");
    statusText.append(s_level);
    statusText.append(" Lives: ");
    statusText.append(s_livesLeft);
    statusText.append(" Health: ");
    statusText.append(s_health);
    statusText.append(" Ammo: ");
    statusText.append(s_ammo);
    statusText.append(" Bonus: ");
    statusText.append(s_bonus);
    
    setGameStatText(statusText);
}

bool StudentWorld::clearShot(double x, double y, int direction) {
    if (playerOn(x, y)) {
        return true;
    }
    switch(direction) {
        case Alive::up:
            if (collision(x, y + 1) && !canFill(x, y + 1)) {
                return false;
            }
            return clearShot(x, y+1, direction);
            break;
        case Alive::down:
            if (collision(x, y - 1) && !canFill(x, y - 1)) {
                return false;
            }
            return clearShot(x, y-1, direction);
            break;
        case Alive::left:
            if (collision(x - 1, y) && !canFill(x - 1, y)) {
                return false;
            }
            return clearShot(x-1, y, direction);
            break;
        case Alive::right:
            if (collision(x + 1, y) && !canFill(x + 1, y)) {
                return false;
            }
            return clearShot(x+1, y, direction);
            break;
        default:
            return false;
    }
}

bool StudentWorld::colocationGoodie(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->stealable()) {
            return true;
        }
    }
    return false;
}

int StudentWorld::stealGoodie(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->stealable()){
            kill(it);
            return (*it)->getBonus();
        }
    }
    return -1;
}

void StudentWorld::dropGoodie(double x, double y, int bonus) {
    if (bonus == 1000) {
        m_actorList.push_back(new ExtraLifeGoodie(this, x, y));
    }
    else if (bonus == 500) {
        m_actorList.push_back(new RestoreHealthGoodie(this, x, y));
    }
    else if (bonus == 100) {
        m_actorList.push_back(new AmmoGoodie(this, x, y));
    }
}

bool StudentWorld::colocationThief(double x, double y) {
    list<Actor*>::iterator it;
    for (it = m_actorList.begin(); it != m_actorList.end(); it++) {
        if ((*it)->getAlive() && (*it)->getX() == x && (*it)->getY() == y && (*it)->canSteal()){
            return true;
        }
    }
    return false;
}

int StudentWorld::countTBots(double x, double y) {
    int count = 0;
    for (int i = x - 3; i < x + 4; i++) {
        for (int j = y - 3; j < y + 4; j++) {
            if (colocationThief(i, j)) {
                count++;
            }
        }
    }
    return count;
}

void StudentWorld::spawnBot(double x, double y, bool mean) {
    if (mean) {
        m_actorList.push_back(new MeanThiefBot(this, x, y, getLevel()));
        
    }
    else {
        m_actorList.push_back(new RegThiefBot(this, x, y, getLevel()));
    }
}
