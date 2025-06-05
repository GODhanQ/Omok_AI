#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int board_2024180014[19][19]{};

constexpr vector<pair<int, int>> No_Move{null, null};
constexpr int INF{numeric_limit<int>::min()};
struct Move {
    vector<pair<int, int>> location;
    
    Move() {}
    ~Move() {}
};
struct Node
{
    
    vector<pair<int, int>> location;
    int static_value{};
    Node* parent;
    vector<Node*> childrens;
    
    Node(Node* parent, vector<Move>* locaction_sum, Move gen_location) {
        for (Move temp : location_sum) {
            location.push_back(temp);
        }
        locatiton.push_back(gen_location);
    }
    ~Node() {
        childrens.clear();
    }
    
    int get_static_value {
        return static_value;
    }
    vector<Move>* get_possible_moves(Move next_location) {
        vector<Move> possible_moves;
        for(Node* next_node : childrens) {
            possible_moves.push_back(next_node.location);
        }
        
        return &possible_moves;
    }
    Node* make_move(Move location) {
        for (Node* target_node : childrens) {
            if (location == target_node->location) return target_node;
        }
    }
    vector<Node*> get_children() {
        return children;
    }
};
class GameTree
{
    Node* Root;
    int Size{};
    
public:
    GameTree() : Root(nullptr) a{}
    ~GameTree() {
        delete Root;
    }
    
    
}

Move find_best_move(Node current_root_node, int search_depth)
{
    Move best_move{No_Move};
    int best_value{-INF};

    for(Move possible_moves : current_root_node.get_possible_moves()) {
        Node next_node_state{current_root_node.make_move(possible_moves)};
        int next_node_value{minimax(next_node_state, search_depth - 1, false)};
        
        if (next_node_value > best_value) {
          best_value = next_node_value;
          best_move = possible_moves;
        }
    }
    
    return best_move;
}

int minimax(Node node, int depth, bool is_myTurn) {
    if (depth = 0 || node.is_leaf()) {
        return static_eval_func(node.get_location_sum());
    }

    if (is_myTurn) {
        int max_eval = -INF;
        for (Node child_node : node.get_children()) {
            int eval{minimax(child_node, depth - 1, false)};
            vector<pair<int, int>> temp{minmanx(max_eval, eval)};
            max_eval = temp.second;
        }
        return max_eval;
    } else {
          int min_eval = INF;
          for (Node chile_node : node.get_children())
    }
}



