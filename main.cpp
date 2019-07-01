#include <cstdio>
#include "World.h"

int main() {
    FILE *start_file = std::fopen("world.bin", "rb");

    World world(start_file);

    world.play();

    std::fclose(start_file);

    return 0;
}
