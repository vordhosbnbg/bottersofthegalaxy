#pragma GCC optimize "O3,omit-frame-pointer,inline"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <deque>

using namespace std;

#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#define DBG(x) {cerr << x << endl;}
#else
#define DBG(x)
#endif

/**
 * Made with love by AntiSquid, Illedan and Wildum.
 * You can help children learn to code while you participate by donating to CoderDojo.
 **/

enum class HeroType
{
    DEADPOOL=0,
    HULK=1,
    VALKYRIE=2,
    IRONMAN=3,
    DOCTOR_STRANGE=4,
    COUNT=5,
    UNKNOWN=6
};

HeroType heroTypeFromString(string s)
{
    if(s == "DEADPOOL")
    {
        return HeroType::DEADPOOL;
    }
    else if(s == "HULK")
    {
        return HeroType::HULK;
    }
    else if(s == "VALKYRIE")
    {
        return HeroType::VALKYRIE;
    }
    else if(s == "IRONMAN")
    {
        return HeroType::IRONMAN;
    }
    else if(s == "DOCTOR_STRANGE")
    {
        return HeroType::DOCTOR_STRANGE;
    }
    else
    {
        return HeroType::UNKNOWN;
    }
}

enum class UnitType
{
    UNIT=0,
    TOWER=1,
    HERO=2,
    GROOT=3,
    COUNT=4,
    UNKNOWN=5
};

UnitType unitTypeFromString(string s)
{
    if(s == "UNIT")
    {
        return UnitType::UNIT;
    }
    else if(s == "TOWER")
    {
        return UnitType::TOWER;
    }
    else if(s == "HERO")
    {
        return UnitType::HERO;
    }
    else if(s == "GROOT")
    {
        return UnitType::GROOT;
    }
    else
    {
        DBG("[unitTypeFromString] Unexpected UnitType: " << s);
        return UnitType::UNKNOWN;
    }
}

struct Position
{
    Position(): x(0), y(0){}
    Position(int xPos, int yPos): x(xPos), y(yPos){}

    inline void readFromInput()
    {
        cin >> x >> y;
        DBG("[Input][Position::readFromInput] x:" << x << " , y:" << y);
    }

    int x;
    int y;
};

class ObjectWithPosition
{
public:
    inline int distanceTo(const ObjectWithPosition& other)
    {
        return static_cast<int>(std::sqrt((other._pos.x - _pos.x)*(other._pos.x - _pos.x) + (other._pos.y - _pos.y)*(other._pos.y - _pos.y)));
    }

    inline void readFromInput()
    {
        _pos.readFromInput();
    }
private:
    Position _pos;
};

class ObjectWithPositionAndRadius : public ObjectWithPosition
{
public:
    ObjectWithPositionAndRadius(): _radius(0) {}

    inline void readFromInput()
    {
        ObjectWithPosition::readFromInput();
        cin >> _radius;
        DBG("[Input][ObjectWithPositionAndRadius::readFromInput] _radius:" << _radius);
    }
private:
    int _radius;
};

class Unit : public ObjectWithPosition
{
public:
    Unit(int id, int team, UnitType uType): _unitId(id), _team(team), _unitType(uType) {}

    inline void readFromInput(bool useDummy = true)
    {
        ObjectWithPosition::readFromInput();
        cin >> _attackRange >> _health >> _maxHealth >> _shield >> _attackDamage >> _movementSpeed >> _stunDuration >> _goldValue;
        DBG("[Input][Unit::readFromInput] _health: " << _health
            << " ,_maxHealth:" << _maxHealth
            << " ,_shield:" << _shield
            << " ,_attackDamage:" << _attackDamage
            << " ,_movementSpeed:" << _movementSpeed
            << " ,_stunDuration:" << _stunDuration
            << " ,_goldValue:" << _goldValue);

        if(useDummy)
        {
            int dummy;
            string dumdum;
            cin >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dumdum >> dummy >> dummy;
        }
    }

    inline UnitType getType(){ return _unitType;}
private:
    int _unitId;
    int _team;
    UnitType _unitType;
    int _attackRange;
    int _health;
    int _maxHealth;
    int _shield;
    int _attackDamage;
    int _movementSpeed;
    int _stunDuration;
    int _goldValue;
};

class Hero : public Unit
{
public:
    Hero(int id, int team, UnitType uType): Unit(id, team, uType) {}

    inline void readFromInput()
    {
        Unit::readFromInput(false);
        string heroTypeString;
        cin >> _cd1 >> _cd2 >> _cd3 >> _mana >> _maxMana >> _manaRegen >> heroTypeString >> _isVisible >> _itemsOwned;
        DBG("[Input][Hero::readFromInput] _cd1: " << _cd1
            << " ,_cd2:" << _cd2
            << " ,_cd3:" << _cd3
            << " ,_mana:" << _mana
            << " ,_maxMana:" << _maxMana
            << " ,_manaRegen:" << _manaRegen
            << " ,heroTypeString:" << heroTypeString
            << " ,_isVisible:" << _isVisible
            << " ,_itemsOwned:" << _itemsOwned );
        _heroType = heroTypeFromString(heroTypeString);
    }
private:
    HeroType _heroType;
    int _mana;
    int _maxMana;
    int _manaRegen;
    int _cd1;
    int _cd2;
    int _cd3;
    int _itemsOwned;
    bool _isVisible;
};

class Item
{
public:
    Item(): _itemCost(0), _damage(0), _health(0), _maxHealth(0), _mana(0), _maxMana(0), _moveSpeed(0), _manaRegen(0), _isPotion(false){}
    inline void readFromInput()
    {
        int isPotion;
        cin >> _itemName >> _itemCost >> _damage >> _health >> _maxHealth >> _mana >> _maxMana >> _moveSpeed >> _manaRegen >> isPotion;
        _isPotion = isPotion==1 ? true : false;
    }
private:
    string _itemName; // contains keywords such as BRONZE, SILVER and BLADE, BOOTS connected by "_" to help you sort easier
    int _itemCost; // BRONZE items have lowest cost, the most expensive items are LEGENDARY
    int _damage; // keyword BLADE is present if the most important item stat is damage
    int _health;
    int _maxHealth;
    int _mana;
    int _maxMana;
    int _moveSpeed; // keyword BOOTS is present if the most important item stat is moveSpeed
    int _manaRegen;
    bool _isPotion; // 0 if it's not instantly consumed
};

template <int Width, int Height>
class Map
{
public:
    Map();
    ~Map();
private:
    int _array2d[Width*Height];
};

struct TeamState
{
    int gold;
    std::vector<std::shared_ptr<Unit>> _units;
    std::vector<std::shared_ptr<Unit>> _towers;
    std::vector<std::shared_ptr<Hero>> _heroes;
};

struct RoundState
{
    RoundState(int myTeam): ourTeamN(myTeam){}
    inline void readEntities()
    {
        unsigned int entityCount;
        cin >> entityCount; cin.ignore();
        DBG("[Input][RoundState::readEntities] entityCount: " << entityCount);

        for(unsigned int i = 0; i < entityCount; ++i)
        {
            DBG("[Input][RoundState::readEntities] Parsing entity # " << i);
            int unitId;
            int team;
            string unitTypeString;
            cin >> unitId >> team >> unitTypeString;
            DBG("[Input][Unit::readFromInput] _unitId:" << unitId << " , team:" << team << " , unitTypeString:" << unitTypeString);
            UnitType unitType = unitTypeFromString(unitTypeString);
            TeamState* teamStatePtr = team == ourTeamN ? &ourTeam : &enemyTeam;
            switch (unitType)
            {
                case UnitType::UNIT:
                    teamStatePtr->_units.emplace_back(std::make_shared<Unit>(unitId, team, unitType));
                    teamStatePtr->_units.back()->readFromInput();
                    break;
                case UnitType::HERO:
                    teamStatePtr->_heroes.emplace_back(std::make_shared<Hero>(unitId, team, unitType));
                    teamStatePtr->_heroes.back()->readFromInput();
                    break;
                case UnitType::TOWER:
                    teamStatePtr->_towers.emplace_back(std::make_shared<Unit>(unitId, team, unitType));
                    teamStatePtr->_towers.back()->readFromInput();
                    break;
                case UnitType::GROOT:
                    _groots.emplace_back(std::make_shared<Unit>(unitId, team, unitType));
                    _groots.back()->readFromInput();
                    break;
                default:
                    DBG("[RoundState::readEntities] ERROR - Unexpected UnitType")
                    return;
            }
            cin.ignore();
        }
    }

    inline void readFromInput()
    {
        cin >> ourTeam.gold; cin.ignore();
        cin >> enemyTeam.gold; cin.ignore();
        cin >> roundType; cin.ignore();
        DBG("[Input][RoundState::readFromInput] ourTeam.gold: " << ourTeam.gold << " , enemyTeam.gold: " << enemyTeam.gold << " , roundType:" << roundType);
        readEntities();
    }


    int roundType;
    TeamState ourTeam;
    TeamState enemyTeam;
    int ourTeamN;
    int enemyTeamN;
    std::vector<std::shared_ptr<Unit>> _groots;
};

class GameContext
{
public:
    GameContext(): _myTeam(0) {}

    inline void readInit()
    {
        // read bushes and spawn points
        cin >> _bushAndSpawnPointCount; cin.ignore();

        cin >> _myTeam; cin.ignore();

        DBG("[Input][GameContext::readInit] _bushAndSpawnPointCount: " << _bushAndSpawnPointCount << " , _myTeam: " << _myTeam);
    }

    inline void readBushesAndSpawnPoints()
    {
        for (unsigned int i = 0; i < _bushAndSpawnPointCount; i++)
        {
            string entityType; // BUSH, from wood1 it can also be SPAWN
            ObjectWithPositionAndRadius* objectPtr=nullptr;
            cin >> entityType;
            DBG("[Input][GameContext::readBushesAndSpawnPoints] entityType: " << entityType);
            if(entityType == "BUSH")
            {
                _bushes.emplace_back();
                objectPtr = &_bushes.back();
            }
            else if (entityType == "SPAWN")
            {
                _spawnPoints.emplace_back();
                objectPtr = &_spawnPoints.back();
            }
            if(objectPtr)
                objectPtr->readFromInput(); cin.ignore();
        }
    }

    inline void readItems()
    {
        unsigned int itemCount; // useful from wood2
        cin >> itemCount; cin.ignore();
        DBG("[Input][GameContext::readItems] itemCount: " << itemCount);
        _items.resize(itemCount);

        for (unsigned int i = 0; i < itemCount; i++)
        {
            Item& item = _items[i];
            item.readFromInput(); cin.ignore();
        }
    }

    inline void processOneTurn()
    {
        DBG("[GameContext] Processing turn...");
        if(_currentRound)
        {
            DBG("[GameContext] Saving last turn to history");
            _roundHistory.emplace_front(std::move(_currentRound));
        }
        _currentRound = std::unique_ptr<RoundState>(new RoundState(_myTeam));
        _currentRound->readFromInput();
    }

    inline void takeAction()
    {
        cout << "WAIT" << endl;
    }

private:
    int _myTeam;
    unsigned int _bushAndSpawnPointCount; // useful from wood1, represents the number of bushes and the number of places where neutral units can spawn
    std::deque<std::unique_ptr<RoundState>> _roundHistory;
    std::unique_ptr<RoundState> _currentRound;
    std::vector<ObjectWithPositionAndRadius> _spawnPoints;
    std::vector<ObjectWithPositionAndRadius> _bushes;
    std::vector<Item> _items;
};

int main()
{
    GameContext gc;
    gc.readInit();
    gc.readBushesAndSpawnPoints();
    gc.readItems();

    // game loop
    while (1)
    {
        gc.processOneTurn();
        gc.takeAction();
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // If roundType has a negative value then you need to output a Hero name, such as "DEADPOOL" or "VALKYRIE".
        // Else you need to output roundType number of any valid action, such as "WAIT" or "ATTACK unitId"
    }
}
