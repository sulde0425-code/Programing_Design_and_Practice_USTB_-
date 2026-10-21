#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "Map.h"
#include "Snake.h"
#include "Food.h"
class GameManager{
private:
    static GameManager* instance;
    GameManager();
    GameManager(const GameManager&)=delete;
    Map m_map;
    Snake m_snake;
    Food m_food;
    bool gameOver=false;
    int score=0;
public:
    static GameManager* getInstance();
    void GameLoop();
    void DrawAll();
    void KeyControl();
};
#endif
