#include "../src/components/components.h"

v2d v2d_add(v2d v1, v2d v2)
{
    return v2d{v1.x + v2.x, v1.y + v2.y};
}


int main(){
    WorldPosition pos = {1.0, 2.0};

    WorldPosition pos2 = WorldPosition{v2d_add(pos.v2d_val, pos.v2d_val)};

    return 0;
}