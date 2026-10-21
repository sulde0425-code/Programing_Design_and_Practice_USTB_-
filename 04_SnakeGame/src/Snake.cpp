#include "Snake.h"
#include "MoveStrategy.h"
#include "Map.h"
Snake::Snake(){
    int cx=WIDTH/2,cy=HEIGHT/2;
    body={{cx,cy},{cx-1,cy},{cx-2,cy}};
    curStr=new RightStrategy();
    lastStr=new RightStrategy();
}
Snake::~Snake(){
    delete curStr;
    delete lastStr;
}
void Snake::setStrategy(MoveStrategy* s){
    //防止180度掉头：上一步是Up时不能设为Down，以此类推
    if((dynamic_cast<UpStrategy*>(s)&&dynamic_cast<DownStrategy*>(lastStr))||
       (dynamic_cast<DownStrategy*>(s)&&dynamic_cast<UpStrategy*>(lastStr))||
       (dynamic_cast<LeftStrategy*>(s)&&dynamic_cast<RightStrategy*>(lastStr))||
       (dynamic_cast<RightStrategy*>(s)&&dynamic_cast<LeftStrategy*>(lastStr))){
        return;//忽略反向移动
    }
    delete curStr;
    curStr=s;
}
void Snake::Move(){
    for(int i=body.size()-1;i>0;i--) body[i]=body[i-1];
    curStr->Operate(body[0].first,body[0].second);
    updateLastStrategy();
}
void Snake::updateLastStrategy(){
    delete lastStr;
    if(dynamic_cast<UpStrategy*>(curStr)) lastStr=new UpStrategy();
    else if(dynamic_cast<DownStrategy*>(curStr)) lastStr=new DownStrategy();
    else if(dynamic_cast<LeftStrategy*>(curStr)) lastStr=new LeftStrategy();
    else if(dynamic_cast<RightStrategy*>(curStr)) lastStr=new RightStrategy();
}
bool Snake::CheckCrashWall(){
    auto [hx,hy]=body[0];
    if(hx<=0||hx>=WIDTH||hy<=0||hy>=HEIGHT) return true;
    return false;
}
bool Snake::CheckSelf(){
    auto [hx,hy]=body[0];
    for(int i=1;i<body.size();i++){
        if(body[i].first==hx&&body[i].second==hy) return true;
    }
    return false;
}
