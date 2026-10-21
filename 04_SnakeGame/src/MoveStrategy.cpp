#include "MoveStrategy.h"
void UpStrategy::Operate(int &x, int &y){y--;}
void DownStrategy::Operate(int &x, int &y){y++;}
void LeftStrategy::Operate(int &x, int &y){x--;}
void RightStrategy::Operate(int &x, int &y){x++;}
