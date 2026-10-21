#include "Food.h"
#include "Snake.h"
#include "Map.h"
#include <cstdlib>
#include <ctime>
void Food::RandomCreate(const Snake &snake){
again:
    x=rand()%(WIDTH-2)+1;
    y=rand()%(HEIGHT-2)+1;
    //避免食物生成在蛇身上
    for(auto& pos:snake.getBody()){
        if(pos.first==x&&pos.second==y) goto again;
    }
}
