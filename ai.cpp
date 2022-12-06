#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "ai.hpp"

const int PHASE1DEPTH8 = 10;  // 8x8
const int PHASE2DEPTH8 = 20;  // 8x8

const int TOTAL_VALUE = 13752;

static uint32_t nodes = 0;
static uint32_t reached_depth = 0;

int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int min(int a, int b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

AI::AI(Color cl, Strength level) {
    this->color = cl;
    this->opponent = color_reverse(cl);
    this->level = level;
}

char* AI::move(const char* input) {
    BitBoard bd = BitBoard(input);
    nodes = 0;
    this->set_phase(bd);
    this->set_depth();

    clock_t t1 = clock();
    Node best = this->alpha_beta_helper(bd, this->depth);
    clock_t t2 = clock();
    double elapsed_time = (double)(t2 - t1) / (double)CLOCKS_PER_SEC;

    char* buf = (char*)malloc(sizeof(char) * 128);

    if (this->phase == 1) {
        snprintf(buf, 128,
                 "%d%d { 'value': %+.02f, 'depth': %d, 'nodes': %d, 'time': "
                 "%.4lf, 'NPS': %.0lf }",
                 best.loc % SIZE, best.loc / SIZE,
                 (double)best.value / (double)TOTAL_VALUE, reached_depth, nodes,
                 elapsed_time, (double)nodes / elapsed_time);
    } else {
        snprintf(buf, 128,
                 "%d%d { 'value': %+d, 'depth': %d, 'nodes': %d, 'time': "
                 "%.4lf, 'NPS': %.0lf }",
                 best.loc % SIZE, best.loc / SIZE, best.value, reached_depth, nodes,
                 elapsed_time, (double)nodes / elapsed_time);
    }
    return buf;
}

void AI::set_phase(BitBoard bd) {
    int empty_count = bd.empty_count();
    int phase2 = PHASE2DEPTH8 + (this->level - 4) * 4;
    if (empty_count > phase2) {
        this->phase = 1;
    } else {
        this->phase = 2;
    }
}

void AI::set_depth() {
    if (this->phase == 1) {
        this->depth = PHASE1DEPTH8 + (this->level - 4) * 2;
        if (this->depth <= 0) {
            this->depth = 1;
        }
    } else {
        this->depth = INT_MAX;
    }
}

int AI::heuristic(BitBoard bd) {
    if (this->phase == 1) {
        return bd.eval(this->color);
    } else {
        return bd.count(this->color) - bd.count(this->opponent);
    }
}

Nodes AI::sorted_valid_nodes(BitBoard bd, Color cl, uint64_t valid) {
    Nodes all = pool_get_clear_one();
    if (valid == 0) {
        return all;
    }

    if (this->phase == 1) {
        for (int loc = 0; loc < 64; loc++) {
            if (((1ULL << loc) & valid) != 0) {
                BitBoard tmp = bd;
                tmp.put(cl, loc);
                Node n = Node(loc, tmp.eval(cl));
                all.push(n);
            }
        }
        all.sort_desc();
    } else {
        Color op = color_reverse(cl);
        for (int loc = 0; loc < 64; loc++) {
            if (((1ULL << loc) & valid) != 0) {
                BitBoard tmp = bd;
                tmp.put(cl, loc);
                Node n = Node(loc, tmp.mobility(op));
                all.push(n);
            }
        }
        all.sort_asc();
    }
    return all;
}

Node AI::alpha_beta_helper(BitBoard bd, int depth) {
    return this->alpha_beta(bd, depth, INT_MIN, INT_MAX, true);
}

Node AI::alpha_beta(BitBoard bd, int depth, int alpha, int beta, bool max_layer) {
    nodes++;

    if (depth == 0) {
        reached_depth = this->depth;
        int v = this->heuristic(bd);
        return Node(-1, v);
    }

    uint64_t self_valid = bd.all_valid_loc(this->color);
    uint64_t opp_valid = bd.all_valid_loc(this->opponent);
    // game over
    if (self_valid == 0 && opp_valid == 0) {
        reached_depth = this->depth - depth;
        int v = this->heuristic(bd);
        return Node(-1, v);
    }

    if (max_layer) {
        int max_value = INT_MIN;
        Node best_node = Node(-1, max_value);

        Nodes ai_valid_nodes = this->sorted_valid_nodes(bd, this->color, self_valid);
        // ai don't have place to move, pass.
        if (ai_valid_nodes.len == 0) {
            pool_free_one();
            return this->alpha_beta(bd, depth, alpha, beta, false);
        }

        for (size_t i = 0; i < ai_valid_nodes.len; i++) {
            BitBoard tmp = bd;
            tmp.put(this->color, ai_valid_nodes.nodes[i].loc);

            int eval = this->alpha_beta(tmp, depth - 1, alpha, beta, false).value;

            if (eval > max_value) {
                max_value = eval;
                best_node = ai_valid_nodes.nodes[i];
            }
            alpha = max(alpha, max_value);
            if (beta <= alpha) {
                break;
            }
        }

        pool_free_one();
        return Node(best_node.loc, max_value);
    } else {
        int min_value = INT_MAX;
        Node best_node = Node(-1, min_value);

        Nodes opp_valid_nodes = this->sorted_valid_nodes(bd, this->opponent, opp_valid);
        // opponent has no place to move, pass.
        if (opp_valid_nodes.len == 0) {
            pool_free_one();
            return this->alpha_beta(bd, depth, alpha, beta, true);
        }

        for (size_t i = 0; i < opp_valid_nodes.len; i++) {
            BitBoard tmp = bd;
            tmp.put(this->opponent, opp_valid_nodes.nodes[i].loc);
            int eval = this->alpha_beta(tmp, depth - 1, alpha, beta, true).value;

            if (eval < min_value) {
                min_value = eval;
                best_node = opp_valid_nodes.nodes[i];
            }

            beta = min(beta, min_value);
            if (beta <= alpha) {
                break;
            }
        }

        pool_free_one();
        return Node(best_node.loc, min_value);
    }
}
