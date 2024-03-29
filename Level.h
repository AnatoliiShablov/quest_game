#ifndef QUEST_LEVEL_H
#define QUEST_LEVEL_H

#include "BaseObjects.h"
#include <random>

class Level {
//    std::vector<std::vector<symbol>> map;
    int32_t left_border_;
    uint32_t length_;
    uint32_t height_;

    actor player;

    std::vector<actor_npc> npc;

    state current;
    size_t talking_with;
public:
    Level(FILE *level_file) : left_border_{0}, current{state::running} {
        std::fread(&length_, sizeof(uint32_t), 1, level_file);
        std::fread(&height_, sizeof(uint32_t), 1, level_file);

//        for (size_t i = 0; i < height_; i++) {
//            map.emplace_back();
//            for (size_t j = 0; j < length; j++) {
//                map[i].emplace_back(level_file);
//            }
//        }


        uint32_t amount{0};
        std::fread(&amount, sizeof(uint32_t), 1, level_file);

        for (size_t i = 0; i < amount; i++) {
            npc.emplace_back(level_file);
        }
        tick();
    }

    void tick(actions act) {
        if (current == state::running) {
            if (act == actions::left) {
                if (player.left_border() == 0) {
                    return;
                }
                if (left_border() > 0 && player.left_border() - left_border() < 20) {
                    left_border_--;
                }
                player--;
            }
            if (act == actions::right) {
                if (player.right_border() == length_) {
                    return;
                }
                if (right_border() < length_ && right_border() - player.right_border() < 20) {
                    left_border_++;
                }
                player++;
            }
            if (act == actions::action) {
                for (size_t i = 0; i < npc.size(); i++) {
                    if (npc[i].close_to(player)) {
                        current = state::talking;
                        npc[talking_with].tick(actions::action);
                        talking_with = i;
                        break;
                    }
                }
            }
        } else if (current == state::talking) {
            if (npc[talking_with].tick(act) == EOD) {
                current = state::running;
            }
        }
        tick();
    }

    void tick() {
        for (size_t i = 0; i < getmaxx(stdscr); i++) {
            for (size_t j = 0; j < height_; j++) {
                symbol(0, 0, '#', road).print(i, j);
            }
        }

        std::mt19937 engine{static_cast<uint32_t>(time(nullptr))};
        for (size_t i = 0; i < getmaxx(stdscr); i++) {
            for (size_t j = height_; j < getmaxy(stdscr); j++) {
                uint32_t generated = engine() % 50;
                if (generated) {
                    sky_block.print(i, j);
                } else {
                    cloud_block.print(i, j);
                }
            }
        }

        player.print(height_, left_border());

        for (auto const &np : npc) {
            if (np.visible(left_border(), right_border()))
                np.print(height_, left_border(), player);
        }
    }

    int32_t left_border() const {
        return left_border_;
    }

    int32_t right_border() const {
        return left_border_ + getmaxx(stdscr);
    }
};


#endif //QUEST_LEVEL_H
