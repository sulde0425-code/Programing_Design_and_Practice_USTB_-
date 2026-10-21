#ifndef MOVESTRATEGY_H
#define MOVESTRATEGY_H
//抽象移动策略：策略模式
class MoveStrategy
{
public:
    virtual ~MoveStrategy()=default;
    virtual void Operate(int& headX,int& headY)=0;
};
//向上
class UpStrategy:public MoveStrategy{
public:
    void Operate(int& x,int& y)override;
};
//向下
class DownStrategy:public MoveStrategy{
public:
    void Operate(int& x,int& y)override;
};
//向左
class LeftStrategy:public MoveStrategy{
public:
    void Operate(int& x,int& y)override;
};
//向右
class RightStrategy:public MoveStrategy{
public:
    void Operate(int& x,int& y)override;
};
#endif
