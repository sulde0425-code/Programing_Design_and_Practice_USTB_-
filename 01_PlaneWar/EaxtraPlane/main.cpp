// ============================================================
// Roguelike Plane Battle - Phase 4
// Boss Fight + Prop Pickups + putimage_alpha + Screen Flash
// ============================================================
#include "GameManager.h"

int main() {
    GameManager game;

    if (!game.Init()) {
        return -1;
    }

    game.Run();

    return 0;
}
