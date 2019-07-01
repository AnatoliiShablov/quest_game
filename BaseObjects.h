#ifndef QUEST_BASEOBJECTS_H
#define QUEST_BASEOBJECTS_H

#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <list>
#include "ncurses.h"


// 0 color can't be changed
// 1..10 can be changed in-time

#define black_board 11
#define road 12
#define sky 13
#define leg_hand 14
#define body 15
#define head 16


#define EOD -1

enum class actions {
    left,
    right,
    up,
    down,
    action
};

enum class state {
    running,
    talking
};

class symbol {

    int32_t x_; // normal coord
    int32_t y_; // normal coord
    chtype ch_;

    int32_t attribute_pair;

public:
    symbol(int32_t x, int32_t y, chtype ch, int32_t attribute = 0) : x_(x), y_(y), ch_(ch), attribute_pair(attribute) {}


    template<bool B = true>
    void print(int32_t plus_x = 0, int32_t plus_y = 0) const {
        if constexpr (B) {
            attron(COLOR_PAIR(attribute_pair));
            mvaddch(getmaxy(stdscr) - (y_ + plus_y), x_ + plus_x, ch_);
            attroff(COLOR_PAIR(attribute_pair));
        } else {
            mvaddch(getmaxy(stdscr) - (y_ + plus_y), x_ + plus_x, ch_);
        }
    }

    int32_t &x() { return x_; }

    int32_t &y() { return x_; }

    int32_t const &x() const { return x_; }

    int32_t const &y() const { return x_; }

    chtype &ch() { return ch_; }

    chtype const &ch() const { return ch_; }

    int32_t &attr() { return attribute_pair; }

    int32_t const &attr() const { return attribute_pair; }

    symbol(FILE *data_file) {
        std::fread(this, sizeof(symbol), 1, data_file);
    }
};

const symbol sky_block{0, 0, ' ', sky};
const symbol cloud_block{0, 0, '#', sky};

template<typename InputIterator, bool B = true>
void print(InputIterator first, InputIterator last, int32_t plus_x = 0, int32_t plus_y = 0) {
    std::for_each(first, last, [plus_x, plus_y](symbol const &current) { current.print<B>(plus_x, plus_y); });
}

class static_object {
protected:
    std::vector<symbol> data_;
public:
    static_object(FILE *data_file) {
        uint32_t size;
        std::fread(&size, sizeof(uint32_t), 1, data_file);
        for (size_t i = 0; i < size; i++) {
            data_.push_back(symbol(data_file));
        }
    }

    static_object() = default;

    void print(int32_t attribute, int32_t plus_x = 0, int32_t plus_y = 0) const {
        attron(COLOR_PAIR(attribute));
        ::print < std::vector<symbol>::const_iterator, false > (data_.cbegin(), data_.cend(), plus_x, plus_y);
        attroff(COLOR_PAIR(attribute));
    }
};


class dialog_window : public static_object {
public:
    explicit dialog_window(std::vector<std::string> const &lines) {
        int32_t length = std::max_element(lines.begin(), lines.end(),
                                          [](std::string const &lhs, std::string const &rhs) {
                                              return lhs.length() < rhs.length();
                                          })->length();
        length = std::max(length, 5);
        int32_t height = lines.empty() ? 1 : lines.size();


        data_.emplace_back(3, 0, ACS_VLINE);
        data_.emplace_back(4, 0, '/');
        data_.emplace_back(3, 1, ACS_VLINE);
        data_.emplace_back(4, 1, ' ');
        data_.emplace_back(5, 1, '/');
        data_.emplace_back(3, 2, ACS_URCORNER);
        data_.emplace_back(4, 2, ' ');
        data_.emplace_back(5, 4, ACS_ULCORNER);

        data_.emplace_back(0, 2, ACS_LLCORNER);
        data_.emplace_back(0, height + 5, ACS_ULCORNER);
        data_.emplace_back(length + 3, 2, ACS_LRCORNER);
        data_.emplace_back(length + 3, height + 5, ACS_URCORNER);


        for (size_t i = 1; i < length + 3; i++) {
            data_.emplace_back(i, 3, ' ');
            if (i < 3 || i > 5) data_.emplace_back(i, 2, ACS_HLINE);
            data_.emplace_back(i, height + 4, ' ');
            data_.emplace_back(i, height + 5, ACS_HLINE);
        }

        for (size_t i = 1; i < height + 3; i++) {
            data_.emplace_back(0, i + 2, ACS_VLINE);
            data_.emplace_back(length + 3, i + 2, ACS_VLINE);
        }

        for (size_t i = 1; i < length + 3; i++) {
            for (size_t word = 0; word < height; word++) {
                chtype c = ' ';
                if (lines.empty() || i == 1 || lines[word].length() <= i - 2) {
                    c = ' ';
                } else {
                    c = lines[word][i - 2];
                }
                data_.emplace_back(i, height - word + 3, c);
            }
        }
    }

    dialog_window(FILE *data_file) : static_object(data_file) {}
};


class sign : public static_object {
public:
    explicit sign(std::vector<std::string> const &lines) {
        int32_t length = std::max_element(lines.begin(), lines.end(),
                                          [](std::string const &lhs, std::string const &rhs) {
                                              return lhs.length() < rhs.length();
                                          })->length();
        length = std::max(length, 1);
        int32_t height = lines.empty() ? 1 : lines.size();


        data_.emplace_back(0, 0, ACS_LLCORNER);
        data_.emplace_back(0, height + 3, ACS_ULCORNER);
        data_.emplace_back(length + 3, 0, ACS_LRCORNER);
        data_.emplace_back(length + 3, height + 3, ACS_URCORNER);


        for (size_t i = 1; i < length + 3; i++) {
            data_.emplace_back(i, 1, ' ');
            data_.emplace_back(i, 0, ACS_HLINE);
            data_.emplace_back(i, height + 2, ' ');
            data_.emplace_back(i, height + 3, ACS_HLINE);
        }

        for (size_t i = 1; i < height + 3; i++) {
            data_.emplace_back(0, i, ACS_VLINE);
            data_.emplace_back(length + 3, i, ACS_VLINE);
        }

        for (size_t i = 1; i < length + 3; i++) {
            for (size_t word = 0; word < height; word++) {
                chtype c = ' ';
                if (lines.empty() || i == 1 || lines[word].length() <= i - 2) {
                    c = ' ';
                } else {
                    c = lines[word][i - 2];
                }
                data_.emplace_back(i, height - word + 1, c);
            }
        }
    }

    sign(FILE *data_file) : static_object(data_file) {}
};

struct npc_tag {
};
constexpr npc_tag npc_tag_o;

class actor {
protected:
    std::vector<symbol> picture;
    int32_t left_border_;
public:
    actor() : left_border_(0) {
        picture.emplace_back(0, 0, ACS_LRCORNER | A_BOLD, leg_hand);
        picture.emplace_back(2, 0, ACS_LLCORNER | A_BOLD, leg_hand);
        picture.emplace_back(1, 1, ACS_UARROW | A_BOLD, leg_hand);
        picture.emplace_back(1, 2, ACS_CKBOARD | A_BOLD, body);
        picture.emplace_back(0, 2, ACS_ULCORNER | A_BOLD, leg_hand);
        picture.emplace_back(2, 2, ACS_URCORNER | A_BOLD, leg_hand);
        picture.emplace_back(1, 3, ACS_DIAMOND | A_BOLD, head);
    }

    actor(npc_tag) {
        picture.emplace_back(0, 0, ACS_LRCORNER | A_BOLD, leg_hand);
        picture.emplace_back(2, 0, ACS_LLCORNER | A_BOLD, leg_hand);
        picture.emplace_back(1, 1, ACS_UARROW | A_BOLD, leg_hand);
        picture.emplace_back(1, 2, ACS_CKBOARD | A_BOLD, body);
        picture.emplace_back(0, 2, ACS_VLINE | A_BOLD, leg_hand);
        picture.emplace_back(2, 2, ACS_VLINE | A_BOLD, leg_hand);
        picture.emplace_back(1, 3, ACS_DIAMOND | A_BOLD, head);
    }

    void operator++() {
        left_border_++;
    }

    void operator++(int) {
        left_border_++;
    }

    void operator--() {
        left_border_--;
    }

    void operator--(int) {
        left_border_--;
    }

    int32_t left_border() const {
        return left_border_;
    }

    int32_t right_border() const {
        return left_border_ + 2;
    }

    void print(int32_t height, int32_t left) const {
        ::print(picture.cbegin(), picture.cend(), left_border_ - left, height);
    }
};

class actor_npc : virtual public actor {
    struct info {
        int32_t current;
        std::vector<int32_t> next;

        info(FILE *actor_file) {
            uint32_t size;
            std::fread(&size, sizeof(uint32_t), 1, actor_file);

            if (size == 0) {
                std::fread(&current, sizeof(int32_t), 1, actor_file);
            } else {
                next.resize(size);
                std::fread(next.data(), sizeof(int32_t), size, actor_file);
                current = 0;
            }
        }
    };

    std::vector<std::pair<dialog_window, info>> logic;
    int32_t current_state;
public:
    actor_npc(FILE *actor_file) : actor(npc_tag_o), current_state(EOD) {
        std::fread(&left_border_, sizeof(int32_t), 1, actor_file);
        uint32_t size;
        std::fread(&size, sizeof(uint32_t), 1, actor_file);
        for (size_t i = 0; i < size; i++) {
            logic.push_back({dialog_window(actor_file), info(actor_file)});
        }

    }

    int32_t tick(actions act) {
        if (act == actions::action && !logic.empty()) {
            if (current_state == EOD) {
                current_state = 0;
                if (!logic[0].second.next.empty()) { logic[0].second.current = 0; }
            } else {
                current_state = logic[current_state].second.next.empty() ?
                                logic[current_state].second.current :
                                logic[current_state].second.next[logic[current_state].second.current];
            }
        }
        if (current_state == EOD) {
            return EOD;
        }
        if (act == actions::up) {
            if (!logic[current_state].second.next.empty() && logic[current_state].second.current) {
                --logic[current_state].second.current;
            }
        }
        if (act == actions::down) {
            if (logic[current_state].second.current + 1 < logic[current_state].second.next.size()) {
                ++logic[current_state].second.current;
            }
        }
        return current_state;
    }

    bool close_to(actor const &player) const {
        return player.left_border() > left_border() - 5 && player.right_border() < right_border() + 5;
    }

    bool visible(int32_t left, int32_t right) const {
        return left_border() > left + 10 && right_border() < right - 10;
    }

    void print(int32_t height, int32_t left, actor const &player) const {
        actor::print(height, left);
        if (current_state == EOD && !logic.empty() && close_to(player)) {
            sign{{"Press E to talk"}}.print(black_board, left_border() - left - 8, height + 5);
        }
        if (current_state != EOD) {
            logic[current_state].first.print(black_board, left_border() - left - 3, height + 5);
            if (!logic[current_state].second.next.empty()) {
                symbol(left_border() - left - 2, height + 10 - logic[current_state].second.current,
                       ACS_RARROW, black_board).print();
            }
        }
    }
};


#endif //QUEST_BASEOBJECTS_H
