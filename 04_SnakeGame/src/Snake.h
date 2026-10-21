#ifndef SNAKE_H
#define SNAKE_H
#include <vector>
#include <utility>
#include "MoveStrategy.h"
class Snake{
private:
    std::vector<std::pair<int,int> > body;
    MoveStrategy* curStr;
    MoveStrategy* lastStr;//记录上一步的方向策略
public:
    Snake();
    ~Snake();
    void setStrategy(MoveStrategy* s);
    void updateLastStrategy();
    void Move();
    void Grow(){body.emplace_back(body.back());}//蛇身变长
    std::vector<std::pair<int,int> >& getBody(){return body;}
    const std::vector<std::pair<int,int> >& getBody() const {return body;}
    bool CheckCrashWall();//ײǽ
    bool CheckSelf();//撞到自己
};
#endif
