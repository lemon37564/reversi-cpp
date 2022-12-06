#include "bit_board.hpp"
#include "color.hpp"
#include "node.hpp"

#ifndef __ai_h__
#define __ai_h__

typedef enum { WEAK = 0, MEDIUM = 2, STRONG = 4 } Strength;

class AI {
   public:
    AI(Color color, Strength level);
    char* move(const char* input);

   private:
    Color color, opponent;
    int phase;
    int depth;
    Strength level;

    void set_phase(BitBoard bd);
    void set_depth();
    int heuristic(BitBoard bd);
    Nodes sorted_valid_nodes(BitBoard bd, Color cl, uint64_t valid);
    Node alpha_beta_helper(BitBoard bd, int depth);
    Node alpha_beta(BitBoard bd, int depth, int alpha, int beta, bool max_layer);
};

#endif