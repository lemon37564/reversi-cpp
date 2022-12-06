#include <assert.h>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>

#include "bit_board.hpp"

const uint64_t MASKS[8] = {
    0x7F7F7F7F7F7F7F7F, 0x007F7F7F7F7F7F7F, 0xFFFFFFFFFFFFFFFF, 0x00FEFEFEFEFEFEFE,
    0xFEFEFEFEFEFEFEFE, 0xFEFEFEFEFEFEFE00, 0xFFFFFFFFFFFFFFFF, 0x7F7F7F7F7F7F7F00,
};

const uint64_t LSHIFT[8] = {0, 0, 0, 0, 1, 9, 8, 7};
const uint64_t RSHIFT[8] = {1, 9, 8, 7, 0, 0, 0, 0};

void assign(BitBoard* bd, Color cl, int loc);
Color at(BitBoard bd, int loc);
void flip(BitBoard* bd, Color cl, int loc);
uint64_t shift(uint64_t disk, int dir);
int pop_count(uint64_t);

// This uses fewer arithmetic operations than any other known
// implementation on machines with fast multiplication.
// This algorithm uses 12 arithmetic operations, one of which is a multiply.
int pop_count(uint64_t x) {
    // put count of each 2 bits into those 2 bits
    x -= (x >> 1) & 0x5555555555555555;
    // put count of each 4 bits into those 4 bits
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    // put count of each 8 bits into those 8 bits
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
    // returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
    return (x * 0x0101010101010101) >> 56;
}

uint64_t shift(uint64_t disk, int dir) {
    if (dir < 8 / 2) {
        return (disk >> RSHIFT[dir]) & MASKS[dir];
    } else {
        return (disk << LSHIFT[dir]) & MASKS[dir];
    }
}

BitBoard::BitBoard(const char* input) {
    this->black = 0;
    this->white = 0;
    for (int i = 0; i < SIZE * SIZE; i++) {
        switch (input[i]) {
            case 'X':
                this->assign(BLACK, i);
                break;
            case 'O':
                this->assign(WHITE, i);
                break;
            case '+':
                break;
            default:
                assert(1 == 0);
        }
    }
}

char* BitBoard::string() {
    char* res = new char[65];
    for (int loc = 0; loc < SIZE * SIZE; loc++) {
        switch (this->at(loc)) {
            case NONE:
                res[loc] = '+';
                break;
            case BLACK:
                res[loc] = 'X';
                break;
            case WHITE:
                res[loc] = 'O';
                break;
            default:
                assert(1 == 0);
        }
    }
    res[64] = '\0';
    return res;
}

void BitBoard::assign(Color cl, int loc) {
    uint64_t sh = 1ULL << loc;
    if (cl == BLACK) {
        this->black |= sh;
    } else {
        this->white |= sh;
    }
}

Color BitBoard::at(int loc) {
    uint64_t sh = 1ULL << loc;
    if ((this->black & sh) != 0) {
        return BLACK;
    } else if ((this->white & sh) != 0) {
        return WHITE;
    }
    return NONE;
}

void BitBoard::put(Color cl, int loc) {
    this->assign(cl, loc);
    this->flip(cl, loc);
}

void BitBoard::flip(Color cl, int loc) {
    uint64_t x, bounding_disk;
    uint64_t new_disk = 1ULL << loc;
    uint64_t captured_disk = 0;

    if (cl == BLACK) {
        this->black |= new_disk;
        for (int dir = 0; dir < 8; dir++) {
            x = shift(new_disk, dir) & this->white;
            x |= shift(x, dir) & this->white;
            x |= shift(x, dir) & this->white;
            x |= shift(x, dir) & this->white;
            x |= shift(x, dir) & this->white;
            x |= shift(x, dir) & this->white;
            bounding_disk = shift(x, dir) & this->black;

            if (bounding_disk != 0) {
                captured_disk |= x;
            }
        }
        this->black ^= captured_disk;
        this->white ^= captured_disk;
    } else {
        this->white |= new_disk;
        for (int dir = 0; dir < 8; dir++) {
            x = shift(new_disk, dir) & this->black;
            x |= shift(x, dir) & this->black;
            x |= shift(x, dir) & this->black;
            x |= shift(x, dir) & this->black;
            x |= shift(x, dir) & this->black;
            x |= shift(x, dir) & this->black;
            bounding_disk = shift(x, dir) & this->white;

            if (bounding_disk != 0) {
                captured_disk |= x;
            }
        }
        this->black ^= captured_disk;
        this->white ^= captured_disk;
    }
}

uint64_t BitBoard::all_valid_loc(Color cl) {
    uint64_t mine, opp;
    uint64_t legal = 0;

    if (cl == BLACK) {
        mine = this->black;
        opp = this->white;
    } else {
        mine = this->white;
        opp = this->black;
    }
    uint64_t empty = ~(mine | opp);

    for (int dir = 0; dir < 8; dir++) {
        uint64_t x = shift(mine, dir) & opp;
        x |= shift(x, dir) & opp;
        x |= shift(x, dir) & opp;
        x |= shift(x, dir) & opp;
        x |= shift(x, dir) & opp;
        x |= shift(x, dir) & opp;
        legal |= shift(x, dir) & empty;
    }

    return legal;
}

bool BitBoard::has_valid_move(Color cl) {
    return this->all_valid_loc(cl) != 0;
}

bool BitBoard::is_valid_loc(Color cl, int loc) {
    uint64_t mask = 1ULL << loc;
    return (this->all_valid_loc(cl) & mask) != 0;
}

int BitBoard::count(Color cl) {
    if (cl == BLACK) {
        return pop_count(this->black);
    } else if (cl == WHITE) {
        return pop_count(this->white);
    } else {
        assert(1 == 0);
    }
}

int BitBoard::empty_count() {
    return SIZE * SIZE - pop_count(this->black | this->white);
}

int BitBoard::eval(Color cl) {
    int bv = 0, wv = 0, cnt;

    cnt = pop_count(this->black & 0x8100000000000081);
    bv += cnt * 800;
    cnt = pop_count(this->black & 0x0042000000004200);
    bv += cnt * -552;
    cnt = pop_count(this->black & 0x0000240000240000);
    bv += cnt * 62;
    cnt = pop_count(this->black & 0x0000001818000000);
    bv += cnt * -18;
    cnt = pop_count(this->black & 0x4281000000008142);
    bv += cnt * -286;
    cnt = pop_count(this->black & 0x2400810000810024);
    bv += cnt * 426;
    cnt = pop_count(this->black & 0x1800008181000018);
    bv += cnt * -24;
    cnt = pop_count(this->black & 0x0024420000422400);
    bv += cnt * -177;
    cnt = pop_count(this->black & 0x0018004242001800);
    bv += cnt * -82;
    cnt = pop_count(this->black & 0x0000182424180000);
    bv += cnt * 8;

    cnt = pop_count(this->white & 0x8100000000000081);
    wv += cnt * 800;
    cnt = pop_count(this->white & 0x0042000000004200);
    wv += cnt * -552;
    cnt = pop_count(this->white & 0x0000240000240000);
    wv += cnt * 62;
    cnt = pop_count(this->white & 0x0000001818000000);
    wv += cnt * -18;
    cnt = pop_count(this->white & 0x4281000000008142);
    wv += cnt * -286;
    cnt = pop_count(this->white & 0x2400810000810024);
    wv += cnt * 426;
    cnt = pop_count(this->white & 0x1800008181000018);
    wv += cnt * -24;
    cnt = pop_count(this->white & 0x0024420000422400);
    wv += cnt * -177;
    cnt = pop_count(this->white & 0x0018004242001800);
    wv += cnt * -82;
    cnt = pop_count(this->white & 0x0000182424180000);
    wv += cnt * 8;

    if (cl == BLACK) {
        return bv - wv;
    } else if (cl == WHITE) {
        return wv - bv;
    } else {
        assert(1 == 0);
    }
}

int BitBoard::mobility(Color cl) {
    uint64_t all_valid = this->all_valid_loc(cl);
    return pop_count(all_valid);
}
