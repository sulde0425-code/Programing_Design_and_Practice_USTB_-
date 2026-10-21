#include "GameManager.h"
#include "MoveStrategy.h"
#include <windows.h>
#include <conio.h>
#include <iostream>
using namespace std;
GameManager* GameManager::instance=nullptr;
GameManager* GameManager::getInstance(){
    if(!instance) instance=new GameManager();
    return instance;
}
GameManager::GameManager(){
    srand((unsigned int)time(NULL));
    m_food.RandomCreate(m_snake);
    //隐藏控制台光标
    CONSOLE_CURSOR_INFO c={1,false};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&c);
}
void GameManager::DrawAll(){
    COORD pos={0,0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
    m_map.DrawBorder();
    auto& body=m_snake.getBody();
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<=WIDTH;x++){
            if(x==0||x==WIDTH) cout<<"#";
            else if(x==m_food.getX()&&y==m_food.getY()) cout<<"O";
            else{
                bool flag=false;
                //蛇头
                if(body[0].first==x&&body[0].second==y){cout<<"●";flag=true;}
                else{
                    //蛇身
                    for(size_t i=1;i<body.size();i++){
                        if(body[i].first==x&&body[i].second==y){cout<<"□";flag=true;break;}
                    }
                }
                if(!flag) cout<<" ";
            }
        }
        cout<<endl;
    }
    m_map.DrawBorder();
    cout<<"得分："<<score<<" | WASD移动 ESC退出";
}
void GameManager::KeyControl(){
    if(_kbhit()){
        char ch=_getch();
        switch(ch){
        case 27:gameOver=true;break;
        case 'w':m_snake.setStrategy(new UpStrategy());break;
        case 's':m_snake.setStrategy(new DownStrategy());break;
        case 'a':m_snake.setStrategy(new LeftStrategy());break;
        case 'd':m_snake.setStrategy(new RightStrategy());break;
        }
    }
}
void GameManager::GameLoop(){
    while(!gameOver){
        DrawAll();
        KeyControl();
        m_snake.Move();
        //吃到食物
        auto head=m_snake.getBody()[0];
        if(head.first==m_food.getX()&&head.second==m_food.getY()){
            score+=10;
            m_snake.Grow();
            m_food.RandomCreate(m_snake);
        }
        //碰撞判断
        if(m_snake.CheckCrashWall()||m_snake.CheckSelf()) gameOver=true;
        Sleep(120);
    }
    cout<<"\n游戏结束！最终得分："<<score;
}
