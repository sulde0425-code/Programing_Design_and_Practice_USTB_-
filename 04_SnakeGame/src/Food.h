#ifndef FOOD_H
#define FOOD_H
class Food{
private:
    int x,y;
public:
    int getX(){return x;}
    int getY(){return y;}
    void RandomCreate(const class Snake& snake);//随机生成食物
};
#endif
