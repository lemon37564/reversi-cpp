#include "color.hpp"

#ifndef __bit_board_h__
#define __bit_board_h__

const int SIZE = 8;

class BitBoard {
   public:
    BitBoard(const char* input);
    char* string();
    void put(Color cl, int loc);
    uint64_t all_valid_loc(Color cl);
    bool has_valid_move(Color cl);
    bool is_valid_loc(Color cl, int loc);
    int count(Color cl);
    int eval(Color cl);
    int mobility(Color cl);
    int empty_count();

   private:
    uint64_t black;
    uint64_t white;

    void assign(Color cl, int loc);
    Color at(int loc);
    void flip(Color cl, int loc);
};

#endif