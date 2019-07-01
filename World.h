#ifndef QUEST_WORLD_H
#define QUEST_WORLD_H


#include <cstdio>
#include <vector>
#include <string>
#include "Level.h"

class World {
    std::vector<Level> levels;
    size_t current_level;

public:
    World(FILE *world_file) : current_level(0) {
        initscr();
        start_color();
        noecho();
        curs_set(0);

        init_pair(black_board, COLOR_WHITE, COLOR_BLACK);
        init_pair(road, COLOR_BLUE, COLOR_BLACK);
        init_pair(sky, COLOR_WHITE, COLOR_BLUE);
        init_pair(leg_hand, COLOR_BLACK, COLOR_BLUE);
        init_pair(body, COLOR_BLACK, COLOR_BLUE);
        init_pair(head, COLOR_MAGENTA, COLOR_BLUE);

        uint32_t amount{0};
        char name[64];
        std::fread(&amount, sizeof(uint32_t), 1, world_file);
        for (uint32_t i = 0; i < amount; i++) {
            uint32_t length{0};
            std::fread(&length, sizeof(uint32_t), 1, world_file);
            std::fread(name, 1, length, world_file);
            name[length] = '\0';
            FILE *level_file = std::fopen(name, "rb");
            levels.emplace_back(level_file);
            std::fclose(level_file);
        }
    }

    void play() {
        int c;
        while ((c = getch()) != 'q') {
            if (c == 'w' || c == 'W' || c == KEY_UP) {
                levels[current_level].tick(actions::up);
            }
            if (c == 'a' || c == 'A' || c == KEY_LEFT) {
                levels[current_level].tick(actions::left);
            }
            if (c == 's' || c == 'S' || c == KEY_DOWN) {
                levels[current_level].tick(actions::down);
            }
            if (c == 'd' || c == 'D' || c == KEY_RIGHT) {
                levels[current_level].tick(actions::right);
            }
            if (c == 'e' || c == 'E' || c == KEY_ENTER) {
                levels[current_level].tick(actions::action);
            }
            refresh();

        }
    }

    ~World() {
        endwin();
    }
// Quest general_quest_line; // under construction


};


#endif //QUEST_WORLD_H
