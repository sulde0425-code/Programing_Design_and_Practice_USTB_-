#include "GameManager.h"
#include <cstdlib>
int main(){
    GameManager* game=GameManager::getInstance();
    game->GameLoop();
    system("pause");
    return 0;
}
