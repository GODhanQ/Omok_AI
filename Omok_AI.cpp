#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#define NOMINMAX
#include <Windows.h>
using namespace std;

// constexpr variables
constexpr int Board_Size_2024180014{ 19 };
constexpr int SearchDepth_2024180014{ 5 };

// class & struct
enum class StoneType {
    EMPTY,
    BLACK,
    WHITE,
    WALL
};
enum class Pattern {
    NONE,
    FIVE,
    OPEN_FOUR,
    CLOSED_FOUR,
    BLANKED_FOUR,
    OPEN_THREE,
    BLANKED_OPEN_THREE,
    CLOSED_THREE,
    BLANKED_CLOSE_THREE,
    OPEN_TWO,
    DOUBLE_BLANKED_OPEN_THREE,
    DOUBLE_BLANKED_CLOSE_THREE,
    CLOSE_TWO,
    BLANKED_OPEN_TWO,
    BLANKED_CLOSE_TWO,
    DOUBLE_BLANKED_OPEN_TWO,
    DOUBLE_BLANKED_CLOSE_TWO
};
enum class LineType {
    DONTCARE,       // 상관없음
    HORIZONTAL,     // 가로
    VERTICAL,       // 세로
    DIAGONAL_MAIN,  // 대각선 \'
    DIAGONAL_ANTI   // 대각선 /'
};
struct PatternInfo {
    Move move;
    LineType line;
};
class Board {
    vector<vector<StoneType>> board;

public:
    static constexpr int SIZE{ Board_Size_2024180014 };

    Board() : board(SIZE, vector<StoneType>(SIZE, StoneType::EMPTY)) {}

    bool placeStone(const Move& move, StoneType stone) {
        if (!isOnBoard(move.row, move.col) || getStone(move.row, move.col) != StoneType::EMPTY) {
            return false;
        }
        board[move.row][move.col] = stone;
        return true;
    }
    void retractMove(const Move& move) {
        if (isOnBoard(move.row, move.col)) {
            board[move.row][move.col] = StoneType::EMPTY;
        }
    }

    bool isEmpty() const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                if (this->board[r][c] != StoneType::EMPTY) {
                    return false;
                }
            }
        }
        return true;
    }
    bool isFull() const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                if (this->board[r][c] == StoneType::EMPTY) {
                    return false;
                }
            }
        }
        return true;
    }
    StoneType getStone(int r, int c) const {
        if (!isOnBoard(r, c)) {
            throw out_of_range("Board coordinates are out of range.");
        }
        return board[r][c];
    }
    bool isOnBoard(int r, int c) const {
        return r >= 0 && r < SIZE && c >= 0 && c < SIZE;
    }
    void show() const {
        cout << "  ";
        for (int i = 0; i < SIZE; ++i) {
            printf("%2d", i);
        }
        cout << endl;

        for (int r = 0; r < SIZE; ++r) {
            printf("%2d ", r);
            for (int c = 0; c < SIZE; ++c) {
                char stone_char = '.';
                if (board[r][c] == StoneType::BLACK) stone_char = 'X';
                else if (board[r][c] == StoneType::WHITE) stone_char = 'O';
                cout << stone_char << " ";
            }
            cout << endl;
        }
    }
    StoneType checkForWin(const Move& last_move) const;
};
struct Move {
    int row; // 행 (y 좌표)
    int col; // 열 (x 좌표)

    Move() : row(-1), col(-1) {}
    Move(int r, int c) : row(r), col(c) {}

    // --- 유틸리티 함수들 (선택 사항이지만 구현을 강력히 권장합니다) ---
    bool isNull() const {
        return row == -1 && col == -1;
    }
    bool operator==(const Move& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Move& other) const {
        return !(*this == other);
    }
    bool operator<(const Move& other) const {
        if (row != other.row) {
            return row < other.row;
        }
        return col < other.col;
    }
};
struct Node {
    Move location;
    int static_value{};
    Node* parent;
    vector<Node*> children;

    Node() : parent(nullptr), location(Move()) {}
    Node(Node* parent, Move gen_location) {}
    ~Node() {
        for (Node* child : children) {
            delete child;
        }
    }
    Move getMove() {
        return location;
    }
    vector<Move> get_possible_moves() {
        vector<Move> possible_moves;
        for (auto next_node : children) {
            Move next_node_location = next_node->location;
            possible_moves.push_back(next_node_location);
        }

        return possible_moves;
    }
    Node make_move(Move location) {
        for (auto target_node : children) {
            if (target_node->location == location) return *target_node;
        }
    }
    // bool is_leaf(const Board& board) const;
    void generate_children(const Board& board, StoneType current_player);
    vector<Move> extract_moves(const vector<PatternInfo>& infos);
    void createChildNodesFromMoves(std::vector<Move>& moves);
};
struct PlayerPatterns {
    // 우선순위 나열
    vector<PatternInfo> win_moves;                         // 1. ...XXXXX...
    vector<PatternInfo> open_four_moves;                   // 2. ..._XXXX_...
    vector<PatternInfo> four_three_moves;                  // 사삼(4-3)을 만드는 위치
    vector<PatternInfo> close_four_moves;                  // 3. ...BXXXX_... | ..._XXXXB...
    vector<PatternInfo> blanked_four_moves;                // 4. ...X_XXX... | ...XX_XX... | ...XXX_X...
    vector<PatternInfo> open_three_moves;                  // 5. ..._XXX_...
    vector<PatternInfo> blanked_open_three_moves;          // 6. ..._X_XX_... | ..._XX_X_...
    vector<PatternInfo> close_three_moves;                 // 7. ...BXXX_... | ..._XXXB...
    vector<PatternInfo> blanked_close_three_moves;         // 8. ...BX_XX_... | ...BXX_X_... | ..._X_XXB... | ..._XX_XB...
    vector<PatternInfo> open_two_moves;                    // 9. ..._XX_...
    vector<PatternInfo> double_blanked_open_three_moves;   // 10. ..._X_X_X_...
    vector<PatternInfo> double_blanked_close_three_moves;  // 11. ...BX_X_X_... | ..._X_X_XB...
    vector<PatternInfo> close_two_moves;                   // 12. ...BXX_... | ..._XXB...
    vector<PatternInfo> blanked_open_two_moves;            // 13. ..._X_X_...
    vector<PatternInfo> blanked_close_two_moves;           // 14. ...BX_X_... | ..._X_XB...
    vector<PatternInfo> double_blanked_open_two_moves;     // 15. ..._X__X_...
    vector<PatternInfo> double_blanked_close_two_moves;    // 16. ...BX__X_... | ..._X__XB...
    vector<PatternInfo> forbidden_spot;                    // 금수 자리

    void clear() {
        win_moves.clear();
        open_four_moves.clear();
        four_three_moves.clear();
        close_four_moves.clear();
        blanked_four_moves.clear();
        open_three_moves.clear();
        blanked_open_three_moves.clear();
        close_three_moves.clear();
        blanked_close_three_moves.clear();
        open_two_moves.clear();
        double_blanked_open_three_moves.clear();
        double_blanked_close_three_moves.clear();
        close_two_moves.clear();
        blanked_open_two_moves.clear();
        blanked_close_two_moves.clear();
        double_blanked_open_two_moves.clear();
        forbidden_spot.clear();
        double_blanked_close_two_moves.clear();
    }
};
class PatternAnalyzer {
private:
    PlayerPatterns ai_patterns;      // AI의 패턴 정보
    PlayerPatterns opponent_patterns; // 상대방의 패턴 정보

    void checkPatternsAfterMove(const Board& board, int r, int c, StoneType player, PlayerPatterns& patterns);

public:
    PatternAnalyzer() = default;

    void analyze(const Board& board, StoneType ai_player);

    // Get AI Moves
    const vector<PatternInfo>& getAIWinMoves() const { return ai_patterns.win_moves; }
    const vector<PatternInfo>& getAIOpenFourMoves() const { return ai_patterns.open_four_moves; }
    const vector<PatternInfo>& getAIFourThreeMoves() const { return ai_patterns.four_three_moves; }
    const vector<PatternInfo>& getAICloseFourMoves() const { return ai_patterns.close_four_moves; }
    const vector<PatternInfo>& getAIBlankedFourMoves() const { return ai_patterns.blanked_four_moves; }
    const vector<PatternInfo>& getAIOpenThreeMoves() const { return ai_patterns.open_three_moves; }
    const vector<PatternInfo>& getAIBlankedOpenThreeMoves() const { return ai_patterns.blanked_open_three_moves; }
    const vector<PatternInfo>& getAICloseThreeMoves() const { return ai_patterns.close_three_moves; }
    const vector<PatternInfo>& getAIBlankedCloseThreeMoves() const { return ai_patterns.blanked_close_three_moves; }
    const vector<PatternInfo>& getAIOpenTwoMoves() const { return ai_patterns.open_two_moves; }
    const vector<PatternInfo>& getAIDoubleBlankedOpenThreeMoves() const { return ai_patterns.double_blanked_open_three_moves; }
    const vector<PatternInfo>& getAIDoubleBlankedCloseThreeMoves() const { return ai_patterns.double_blanked_close_three_moves; }
    const vector<PatternInfo>& getAICloseTwoMoves() const { return ai_patterns.close_two_moves; }
    const vector<PatternInfo>& getAIBlankedOpenTwoMoves() const { return ai_patterns.blanked_open_two_moves; }
    const vector<PatternInfo>& getAIBlankedCloseTwoMoves() const { return ai_patterns.blanked_close_two_moves; }
    const vector<PatternInfo>& getAIDoubleBlankedOpenTwoMoves() const { return ai_patterns.double_blanked_open_two_moves; }
    const vector<PatternInfo>& getAIDoubleBlankedOpenTwoMoves() const { return ai_patterns.double_blanked_close_two_moves; }
    const vector<PatternInfo>& getAIForbiddenSpot() const { return ai_patterns.forbidden_spot; }

    // Get Opponent Moves
    const vector<PatternInfo>& getOpponentWinMoves() const { return opponent_patterns.win_moves; }
    const vector<PatternInfo>& getOpponentOpenFourMoves() const { return opponent_patterns.open_four_moves; }
    const vector<PatternInfo>& getOpponentFourThreeMoves() const { return opponent_patterns.four_three_moves; }
    const vector<PatternInfo>& getOpponentCloseFourMoves() const { return opponent_patterns.close_four_moves; }
    const vector<PatternInfo>& getOpponentBlankedFourMoves() const { return opponent_patterns.blanked_four_moves; }
    const vector<PatternInfo>& getOpponentOpenThreeMoves() const { return opponent_patterns.open_three_moves; }
    const vector<PatternInfo>& getOpponentBlankedOpenThreeMoves() const { return opponent_patterns.blanked_open_three_moves; }
    const vector<PatternInfo>& getOpponentCloseThreeMoves() const { return opponent_patterns.close_three_moves; }
    const vector<PatternInfo>& getOpponentBlankedCloseThreeMoves() const { return opponent_patterns.blanked_close_three_moves; }
    const vector<PatternInfo>& getOpponentOpenTwoMoves() const { return opponent_patterns.open_two_moves; }
    const vector<PatternInfo>& getOpponentDoubleBlankedOpenThreeMoves() const { return opponent_patterns.double_blanked_open_three_moves; }
    const vector<PatternInfo>& getOpponentDoubleBlankedCloseThreeMoves() const { return opponent_patterns.double_blanked_close_three_moves; }
    const vector<PatternInfo>& getOpponentCloseTwoMoves() const { return opponent_patterns.close_two_moves; }
    const vector<PatternInfo>& getOpponentBlankedOpenTwoMoves() const { return opponent_patterns.blanked_open_two_moves; }
    const vector<PatternInfo>& getOpponentBlankedCloseTwoMoves() const { return opponent_patterns.blanked_close_two_moves; }
    const vector<PatternInfo>& getOpponentDoubleBlankedOpenTwoMoves() const { return opponent_patterns.double_blanked_open_two_moves; }
    const vector<PatternInfo>& getOpponentDoubleBlankedOpenTwoMoves() const { return opponent_patterns.double_blanked_close_two_moves; }
    const vector<PatternInfo>& getOpponentForbiddenSpot() const { return opponent_patterns.forbidden_spot; }
};

// variables
Board board_2024180014;

// functions
Move find_best_move(const Board& current_real_board, int search_depth, StoneType ai_stone_type) {
    Node* root_node = new Node();
    Board board_for_search = current_real_board;

    Move best_move;
    int best_value = -INFINITY;

    root_node->generate_children(board_for_search, ai_stone_type);

    int alpha = -INFINITY;
    int beta = +INFINITY;

    for (Node* child_node : root_node->children) {
        board_for_search.placeStone(child_node->getMove(), ai_stone_type);

        int move_value = minimax_alphabeta(child_node, board_for_search, search_depth - 1, alpha, beta, false, ai_stone_type);

        board_for_search.retractMove(child_node->getMove());

        if (move_value > best_value) {
            best_value = move_value;
            best_move = child_node->getMove();
        }

        // 최상위 레벨에서도 alpha 값을 갱신해 줄 수 있습니다.
        alpha = std::max(alpha, best_value);
    }

    delete root_node;
    return best_move;
}
/*
Move find_best_move(const Board& current_real_board, int search_depth, StoneType ai_stone_type) {
    Node* root_node = new Node(); // 루트 노드 생성
    Board board_for_search = current_real_board; // 탐색을 위한 보드 복사

    Move best_move;
    int best_value = -INFINITY;

    // 루트 노드의 자식들을 먼저 생성
    root_node->generate_children(board_for_search, ai_stone_type);

    // 각 후보 수에 대해 minimax 값을 계산
    for (Node* child_node : root_node->children) {
        // AI가 수를 두는 상황을 시뮬레이션
        board_for_search.placeStone(child_node->location, ai_stone_type);

        // 다음은 상대 턴(Min)이므로 is_my_turn = false
        int move_value = minimax(child_node, board_for_search, search_depth - 1, false, ai_stone_type);

        // 시뮬레이션 했던 수를 되돌림
        board_for_search.retractMove(child_node->location);

        if (move_value > best_value) {
            best_value = move_value;
            best_move = child_node->location;
        }
    }

    delete root_node; // 탐색이 끝난 트리의 메모리 해제
    return best_move;
}
*/
int minimax_alphabeta(Node* node, Board& board, int depth, int alpha, int beta, bool is_my_turn, StoneType ai_stone_type) {
    // --- 종료 조건 ---
    if (depth == 0 || is_game_over(board, node->getMove())) { // is_game_over는 승패/무승부를 판단
        return static_eval_func(board, ai_stone_type);
    }

    // --- 자식 노드 생성 ---
    StoneType current_player = is_my_turn ? ai_stone_type : get_opponent_color(ai_stone_type);
    node->generate_children(board, current_player);
    if (node->children.empty()) {
        return static_eval_func(board, ai_stone_type);
    }

    // --- 재귀 탐색 ---
    if (is_my_turn) { // Max 플레이어 (나의 턴)
        int max_eval = -INFINITY;
        for (Node* child_node : node->children) {
            board.placeStone(child_node->getMove(), current_player);

            int eval = minimax_alphabeta(child_node, board, depth - 1, alpha, beta, false, ai_stone_type);

            board.retractMove(child_node->getMove());

            max_eval = std::max(max_eval, eval);
            alpha = std::max(alpha, eval); // ★★★ alpha 값 갱신 ★★★

            if (beta <= alpha) {
                break; // ★★★ 가지치기(Pruning) 발생! ★★★
            }
        }
        return max_eval;
    }
    else { // Min 플레이어 (상대방 턴)
        int min_eval = +INFINITY;
        for (Node* child_node : node->children) {
            board.placeStone(child_node->getMove(), current_player);

            int eval = minimax_alphabeta(child_node, board, depth - 1, alpha, beta, true, ai_stone_type);

            board.retractMove(child_node->getMove());

            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval); // ★★★ beta 값 갱신 ★★★

            if (beta <= alpha) {
                break; // ★★★ 가지치기(Pruning) 발생! ★★★
            }
        }
        return min_eval;
    }
}
/*
int minimax(Node* node, Board& board, int depth, bool ismy_turn, StoneType ai_stone_type)
{
    if (depth == 0) {
        return static_eval_func(board, ai_stone_type);
    }

    // 모든 함수들과 로직을 구현하는 지금 백돌을 기준으로
    StoneType current_player = ismy_turn ? ai_stone_type : (ai_stone_type == StoneType::BLACK ? StoneType::WHITE : StoneType::BLACK);
    node->generate_children(board, current_player);

    if (node->children.empty()) {
        return static_eval_func(board, ai_stone_type);
    }

    if (ismy_turn) {
        int max_eval = -INFINITY;
        for (Node* child_node : node->children) {
            board.placeStone(child_node->location, current_player);
            if (board.checkForWin(child_node->location) != StoneType::EMPTY) {
                max_eval = std::max(max_eval, static_eval_func(board, ai_stone_type));
            }
            else {
                int eval = minimax(child_node, board, depth - 1, false, ai_stone_type);
                max_eval = std::max(max_eval, eval);
            }

            board.retractMove(child_node->location);
        }
        return max_eval;
    }
    else {
        int min_eval = INFINITY;
        for (Node* child_node : node->children) {
            board.placeStone(child_node->location, current_player);
            if (board.checkForWin(child_node->location) != StoneType::EMPTY) {
                min_eval = std::min(min_eval, static_eval_func(board, ai_stone_type));
            }
            else {
                int eval = minimax(child_node, board, depth - 1, true, ai_stone_type);
                min_eval = std::min(min_eval, eval);
            }

            board.retractMove(child_node->location);
        }
        return min_eval;
    }
}
*/
StoneType get_opponent_color(StoneType player_color) {
    return (player_color == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;
}

// 게임이 끝났는지 확인하는 함수
bool is_game_over(const Board& board, const Move& last_move) {
    if (board.checkForWin(last_move) != StoneType::EMPTY) {
        return true;
    }
    if (board.isFull()) { // isFull() 함수가 있다고 가정
        return true;
    }
    return false;
}
int static_eval_func(const Board& board, StoneType ai_player) {
    int my_score = 0;
    int opponent_score = 0;
    StoneType opponent_player = (ai_player == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;

    // 가로 라인 전체 평가
    for (int r = 0; r < Board::SIZE; ++r) {
        std::vector<StoneType> line = extract_horizontal_line(board, r);
        my_score += analyze_patterns_in_line(line, ai_player);
        opponent_score += analyze_patterns_in_line(line, opponent_player);
    }

    // 세로 라인 전체 평가
    for (int c = 0; c < Board::SIZE; ++c) {
        std::vector<StoneType> line = extract_vertical_line(board, c);
        my_score += analyze_patterns_in_line(line, ai_player);
        opponent_score += analyze_patterns_in_line(line, opponent_player);
    }

    // 대각선 '\' 라인 전체 평가
    for (int k = -(Board::SIZE - 5); k <= (Board::SIZE - 5); ++k) {
        std::vector<StoneType> line = extract_diagonal_main(board, k);
        my_score += analyze_patterns_in_line(line, ai_player);
        opponent_score += analyze_patterns_in_line(line, opponent_player);
    }

    // 대각선 '/' 라인 전체 평가
    for (int k = 4; k <= 2 * (Board::SIZE - 1) - 4; ++k) {
        std::vector<StoneType> line = extract_diagonal_anti(board, k);
        my_score += analyze_patterns_in_line(line, ai_player);
        opponent_score += analyze_patterns_in_line(line, opponent_player);
    }

    return my_score - opponent_score;
}
int analyze_patterns_in_line(const vector<StoneType>& line, StoneType stone_type) {
    int score = 0;
    StoneType empty = StoneType::EMPTY;
    StoneType opponent = (stone_type == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;

    // 패턴별 점수 정의
    constexpr int SCORE_FIVE = 100000000;
    constexpr int SCORE_OPEN_FOUR = 1000000;
    constexpr int SCORE_CLOSED_FOUR = 100000;
    constexpr int SCORE_BLANKED_FOUR = 100000; // 띈 넷도 막힌 넷과 유사한 위력
    constexpr int SCORE_OPEN_THREE = 10000;
    constexpr int SCORE_BLANKED_OPEN_THREE = 5000;
    constexpr int SCORE_CLOSED_THREE = 1000;
    constexpr int SCORE_BLANKED_CLOSED_THREE = 500;
    constexpr int SCORE_DOUBLE_BLANKED_OPEN_THREE = 50;
    constexpr int SCORE_OPEN_TWO = 100;
    constexpr int SCORE_BLANKED_OPEN_TWO = 80;
    constexpr int SCORE_CLOSED_TWO = 10;
    constexpr int SCORE_BLANKED_CLOSED_TWO = 5;
    constexpr int SCORE_DOUBLE_BLANKED_OPEN_TWO = 5;
    constexpr int SCORE_DOUBLE_BLANKED_CLOSE_TWO = 3;

    // ★★★ 중요: for 루프의 종료 조건을 i < line.size() 로 수정 ★★★
    for (int i = 0; i < line.size(); ++i) {

        // --- 5목 체크 (가장 높은 우선순위) ---
        if (i < (int)line.size() - 5) {
            if (line[i] == stone_type && line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type) {
                score += SCORE_FIVE;
                i += 4; continue;
            }
        }

        // --- 4목 계열 패턴 ---
        else if (i < (int)line.size() - 6) { // 창문 크기 6
            // 열린 4목: _XXXX_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) {
                score += SCORE_OPEN_FOUR;
                i += 5; continue;
            }
            // 막힌 4목: BXXXX_ 또는 _XXXXB
            if ((isBlocker(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == stone_type && isBlocker(line[i + 5], opponent))) {
                score += SCORE_CLOSED_FOUR;
                i += 5; continue;
            }
        }
        else if (i < (int)line.size() - 5) { // 창문 크기 5
            // 한 칸 띈 4목: X_XXX, XX_XX, XXX_X
            if ((line[i] == stone_type && line[i + 1] == empty && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == stone_type) ||
                (line[i] == stone_type && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type) ||
                (line[i] == stone_type && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type)) {
                score += SCORE_BLANKED_FOUR;
                i += 4; continue;
            }
        }

        // --- 3목 계열 패턴 ---
        else if (i < (int)line.size() - 5) { // 창문 크기 5
            // 열린 3목: _XXX_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_OPEN_THREE;
                i += 4; continue;
            }
        }
        else if (i < (int)line.size() - 6) { // 창문 크기 6
            // 한 칸 띈 열린 3목: _X_XX_ 또는 _XX_X_
            if ((line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty)) {
                score += SCORE_BLANKED_OPEN_THREE;
                i += 5; continue;
            }
        }
        else if (i < (int)line.size() - 5) { // 창문 크기 5
            // 막힌 3목: BXXX_ 또는 _XXXB
            if ((isBlocker(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && isBlocker(line[i + 4], opponent))) {
                score += SCORE_CLOSED_THREE;
                i += 4; continue;
            }
        }
        else if (i < (int)line.size() - 6) { // 창문 크기 6
            // 막힌 한 칸 띈 3목: BX_XX_, BXX_X_ 등
            if ((isBlocker(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (isBlocker(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type && isBlocker(line[i + 5], opponent)) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type && isBlocker(line[i + 5], opponent))) {
                score += SCORE_BLANKED_CLOSED_THREE;
                i += 5; continue;
            }
        }
        else if (i < (int)line.size() - 7) { // 창문 크기 7
            // 두 칸 띈 열린 3목: _X_X_X_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == empty && line[i + 5] == stone_type && line[i + 6] == empty) {
                score += SCORE_DOUBLE_BLANKED_OPEN_THREE;
                i += 6; continue;
            }
        }

        // --- 2목 계열 패턴 ---
        else if (i < (int)line.size() - 4) { // 창문 크기 4
            // 열린 2목: _XX_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty) {
                score += SCORE_OPEN_TWO;
                i += 3; continue;
            }
        }
        else if (i < (int)line.size() - 5) { // 창문 크기 5
            // 한 칸 띈 열린 2목: _X_X_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_BLANKED_OPEN_TWO;
                i += 4; continue;
            }
        }
        else if (i < (int)line.size() - 4) { // 창문 크기 4
            // 막힌 2목: BXX_ 또는 _XXB
            if ((isBlocker(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && isBlocker(line[i + 3], opponent))) {
                score += SCORE_CLOSED_TWO;
                i += 3; continue;
            }
        }
        else if (i < (int)line.size() - 6) { // 창문 크기 6
            // 두 칸 띈 열린 2목: _X__X_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty) {
                score += SCORE_DOUBLE_BLANKED_OPEN_TWO;
                i += 5; continue;
            }
        }
        else if (i < (int)line.size() - 6) { // 창문 크기 6
            // 두 칸 띈 열린 2목: BX__X_ | _X__XB
            if ((isBlocker(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == empty && line[i + 4] == stone_type && isBlocker(line[i + 5], opponent))) {
                score += SCORE_DOUBLE_BLANKED_OPEN_TWO;
                i += 5; continue;
            }
        }
        else if (i < (int)line.size() - 5) { // 창문 크기 5
            // 막힌 한 칸 띈 2목: BX_X_ 또는 _X_XB
            if ((isBlocker(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && isBlocker(line[i + 4], opponent))) {
                score += SCORE_BLANKED_CLOSED_TWO;
                i += 4; continue;
            }
        }
    }
    return score;
}
vector<StoneType> extract_horizontal_line(const Board& board, int row)
{
    vector<StoneType> line;
    line.reserve(Board::SIZE); // 메모리 미리 할당

    for (int c = 0; c < Board::SIZE; ++c) {
        line.push_back(board.getStone(row, c));
    }
    return line;
}
vector<StoneType> extract_vertical_line(const Board& board, int col)
{
    vector<StoneType> line;
    line.reserve(Board::SIZE);

    for (int r = 0; r < Board::SIZE; ++r) {
        line.push_back(board.getStone(r, col));
    }
    return line;
}
vector<StoneType> extract_diagonal_main(const Board& board, int k)
{
    vector<StoneType> line;
    line.reserve(Board::SIZE);

    // 라인을 왼쪽 위에서 오른쪽 아래 방향으로 일관되게 추출
    for (int c = 0; c < Board::SIZE; ++c) {
        int r = k + c;
        if (board.isOnBoard(r, c)) {
            line.push_back(board.getStone(r, c));
        }
    }
    return line;
}
vector<StoneType> extract_diagonal_anti(const Board& board, int k)
{
    vector<StoneType> line;
    line.reserve(Board::SIZE);

    // 라인을 위쪽에서 아래쪽 방향으로 일관되게 추출
    int r_start = std::max(0, k - (Board::SIZE - 1));
    int r_end = std::min(Board::SIZE - 1, k);

    for (int r = r_start; r <= r_end; ++r) {
        int c = k - r;
        line.push_back(board.getStone(r, c));
    }
    return line;
}
vector<Move> generate_children_pattern_based(const Board& board, StoneType ai_player) {
    PatternAnalyzer analyzer;
    analyzer.analyze(board, ai_player);

    // 여기도 뭔가 더 해줘야 할거 같고.
    vector<Move> level3_moves; // 나의 필승 패턴 (사삼, 열린 넷)
    vector<Move> level4_moves; // 상대의 필승 패턴 방어
    vector<Move> level5_moves; // 나의 열린 삼
    vector<Move> level6_moves; // 상대의 열린 삼 방어

    // level 1
    if (!analyzer.getAIWinMoves().empty()) {
        vector<PatternInfo> win_moves = analyzer.getAIWinMoves();
        vector<Move> win_moves_MoveVec;
        for (const PatternInfo& m : win_moves) {
            win_moves_MoveVec.push_back(m.move);
        }
        return win_moves_MoveVec;
    }
    // level 2
    if (!analyzer.getOpponentWinMoves().empty()) {
        vector<PatternInfo> opponent_win_moves = analyzer.getOpponentWinMoves();
        vector<Move> opponent_win_moves_MoveVec;
        for (const PatternInfo& m : opponent_win_moves) {
            opponent_win_moves_MoveVec.push_back(m.move);
        }
        return opponent_win_moves_MoveVec;
    }

    // 3, 4, 5, 6 순위의 수를 조합하여 반환
    vector<Move> candidates;
    candidates.insert(candidates.end(), level3_moves.begin(), level3_moves.end());
    candidates.insert(candidates.end(), level4_moves.begin(), level4_moves.end());
    candidates.insert(candidates.end(), level5_moves.begin(), level5_moves.end());
    candidates.insert(candidates.end(), level6_moves.begin(), level6_moves.end());

    // 만약 중요한 후보 수가 하나도 없다면? -> 주변부 탐색으로 대체 (Fallback)
    if (candidates.empty()) {
        // 휴리스틱 (돌 주위의 위치를 파악하여 빈곳에 대해 탐색)
        return generate_neighborhood_moves(board);
    }
    else {
        // 중복 제거 후 반환 (한 수가 여러 카테고리에 속할 수 있으므로)
        sort(candidates.begin(), candidates.end()); // 정렬
        candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end()); // 중복 제거
        return candidates;
    }
}
vector<StoneType> extractLineSegment(const Board& board, int r, int c, int dy, int dx, StoneType player)
{
    vector<StoneType> segment;
    segment.reserve(9);

    for (int i = -4; i <= 4; ++i) {
        // (r, c)는 방금 돌을 놓은 위치이므로 i=0일 때를 따로 처리
        if (i == 0) {
            segment.push_back(player);
            continue;
        }

        int nr = r + i * dy;
        int nc = c + i * dx;

        if (board.isOnBoard(nr, nc)) {
            segment.push_back(board.getStone(nr, nc));
        }
        else {
            segment.push_back(StoneType::WALL); // 보드 밖은 벽으로 처리
        }
    }
    return segment;
}
void findBestPatternInSegment(const vector<StoneType>& segment, int r, int c, LineType line, StoneType player, PlayerPatterns& patterns) {
    StoneType opponent = (player == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;

    // --- 이 라인에서 발견되는 모든 패턴을 독립적으로 체크하고 추가합니다 ---

    // 5목 체크 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isFive(window, player)) {
                patterns.win_moves.push_back({ Move(r, c), line });
                // 5목은 가장 강력하므로, 이 라인에서 다른 패턴은 더 이상 의미가 없을 수 있습니다.
                // 따라서 여기서 return하여 함수를 종료하는 것이 효율적입니다.
                return;
            }
        }
    }

    // 4목 계열 체크 (window: 6)
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 6);
            if (PatternUtils::isOpenFour(window, player)) {
                patterns.open_four_moves.push_back({ Move(r, c), line });
            }
            else if (PatternUtils::isClosedFour(window, player, opponent)) {
                patterns.close_four_moves.push_back({ Move(r, c), line });
            }
        }
    }
    
    // 4목 계열 체크 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isBlankedFour(window, player)) {
                patterns.blanked_four_moves.push_back({ Move(r, c), line });
                return;
            }
        }
    }

    // 3목 계열 체크
    // 열린 삼 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isOpenThree(window, player)) {
                patterns.open_three_moves.push_back({ Move(r, c), line });
            }
            else if (PatternUtils::isClosedThree(window, player, opponent)) {
                patterns.close_three_moves.push_back({ Move(r, c), line });
            }
        }
    }

    // 한 칸 띈 열린/닫힌 삼 (window: 6)
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 6);
            // 열린 삼
            if (PatternUtils::isBlankedOpenThree(window, player)) {
                patterns.blanked_open_three_moves.push_back({ Move(r, c), line });
            }
            // 닫힌 삼
            else if (PatternUtils::isBlankedCloseThree(window, player, opponent)) {
                patterns.blanked_close_three_moves.push_back({ Move(r, c), line });
            }
        }
    }

    // 두 칸 띈 열린 삼 (window: 7)
    for (int i = 0; i <= 9 - 7; ++i) {
        if (i <= 4 && i + 6 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 7);
            if (PatternUtils::isDoubleBlankedOpenThree(window, player)) {
                patterns.double_blanked_open_three_moves.push_back({ Move(r, c), line });
            }
            else if (PatternUtils::isDoubleBlankedCloseThree(window, player, opponent)) {
                patterns.double_blanked_close_three_moves.push_back({ Move(r, c), line });
            }
        }
    }
    
    // 2목 계열 체크
    // 열린 2목 (window: 4)
    for (int i = 0; i <= 9 - 4; ++i) {
        if (i <= 4 && i + 3 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 4);
            if (PatternUtils::isOpenTwo(window, player)) {
                patterns.open_two_moves.push_back({ Move(r, c), line });
            }
        }
    }

    // 한 칸 띈 열린/닫힌 2목 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isBlankedOpenTwo(window, player)) {
                patterns.blanked_open_two_moves.push_back({ Move(r, c), line });
            }
            else if(PatternUtils::isBlankedCloseTwo(window, player, opponent)) {
                patterns.blanked_open_two_moves.push_back({ Move(r, c), line });
            }
        }
    }

    // 두 칸 띈 열린/닫힌 2목 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isDoubleBlankedOpenTwo(window, player)) {
                patterns.blanked_open_two_moves.push_back({ Move(r, c), line });
            }
            else if (PatternUtils::isDoubleBlankedCloseTwo(window, player, opponent)) {
                patterns.blanked_open_two_moves.push_back({ Move(r, c), line });
            }
        }
    }
}
vector<Move> generate_neighborhood_moves(const Board& board) {
    unordered_set<Move> candidate_set;

    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            if (board.getStone(r, c) == StoneType::EMPTY) {
                continue;
            }
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    if (dx == 0 && dy == 0) continue;

                    int nr = r + dy;
                    int nc = c + dx;

                    if (board.isOnBoard(nr, nc) && board.getStone(nr, nc) == StoneType::EMPTY) {
                        candidate_set.insert(Move(nr, nc));
                    }
                }
            }
        }
    }

    // set에 저장된 후보들을 vector로 변환하여 반환
    return vector<Move>(candidate_set.begin(), candidate_set.end());
}
inline bool isBlocker(StoneType stone_to_check, StoneType opponent_player) {
    return stone_to_check == opponent_player || stone_to_check == StoneType::WALL;
}

// Node 멤버 함수
void Node::generate_children(const Board& current_board, StoneType player_to_move) {
    // 최종 후보 Move들을 담을 하나의 벡터
    std::vector<Move> candidate_moves;

    // 1. PatternAnalyzer로 보드를 단 한 번 분석합니다.
    PatternAnalyzer analyzer;
    analyzer.analyze(current_board, player_to_move);

    StoneType opponent_player = (player_to_move == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;

    // 2. 가장 높은 우선순위부터 순서대로 확인하여 후보군을 결정합니다.

    // 🥇 1순위: 내가 즉시 이기는 수 (오목)
    const auto& my_wins = analyzer.getAIWinMoves();
    if (!my_wins.empty()) {
        candidate_moves = extract_moves(my_wins);
    }
    // 🥈 2순위: 상대가 즉시 이기는 수 (방어)
    else if (!analyzer.getOpponentWinMoves().empty()) {
        candidate_moves = extract_moves(analyzer.getOpponentWinMoves());
    }
    // 🥉 3순위: 나의 필승기 (사삼 등)
    else if (!analyzer.getAIFourThreeMoves().empty()) {
        candidate_moves = extract_moves(analyzer.getAIFourThreeMoves());
    }
    // 🏅 4순위: 상대의 필승기 방어
    else if (!analyzer.getOpponentFourThreeMoves().empty()) {
        candidate_moves = extract_moves(analyzer.getOpponentFourThreeMoves());
    }
    // 🏅 5순위: 위에서 결정적인 수가 없었을 경우, 일반적인 중요 패턴들을 조합합니다.
    else {
        // 나의 열린 넷 만들기
        const auto& my_open_fours = extract_moves(analyzer.getAIOpenFourMoves());
        candidate_moves.insert(candidate_moves.end(), my_open_fours.begin(), my_open_fours.end());

        // 상대의 열린 넷 막기
        const auto& opponent_open_fours = extract_moves(analyzer.getOpponentOpenFourMoves());
        candidate_moves.insert(candidate_moves.end(), opponent_open_fours.begin(), opponent_open_fours.end());

        // 나의 열린 삼 계열 (기본 + 띈 것) 모두 추가
        const auto& my_open_threes = extract_moves(analyzer.getAIOpenThreeMoves());
        candidate_moves.insert(candidate_moves.end(), my_open_threes.begin(), my_open_threes.end());

        // 상대의 열린 삼 계열 막기
        const auto& opponent_open_threes = extract_moves(analyzer.getOpponentOpenThreeMoves());
        candidate_moves.insert(candidate_moves.end(), opponent_open_threes.begin(), opponent_open_threes.end());

        // 나의 막힌 넷 만들기 (공격의 마무리)
        const auto& my_closed_fours = extract_moves(analyzer.getAICloseFourMoves());
        candidate_moves.insert(candidate_moves.end(), my_closed_fours.begin(), my_closed_fours.end());
    }
    
    if (player_to_move == StoneType::BLACK) {
        const auto& forbidden_infos = analyzer.getAIForbiddenSpot();
        if (!forbidden_infos.empty()) {
            const auto& forbidden_spots = extract_moves(forbidden_infos);
            candidate_moves.erase(
                std::remove_if(candidate_moves.begin(), candidate_moves.end(),
                    [&](const Move& m) {
                        return std::find(forbidden_spots.begin(), forbidden_spots.end(), m) != forbidden_spots.end();
                    }),
                candidate_moves.end());
        }
    }

    // 6. 폴백(Fallback) 전략: 위에서 유의미한 후보 수를 하나도 찾지 못했다면
    if (candidate_moves.empty()) {
        candidate_moves = generate_neighborhood_moves(current_board);
        // 게임 첫 수 처리
        if (candidate_moves.empty() && current_board.isEmpty()) {
            candidate_moves.push_back(Move(Board::SIZE / 2, Board::SIZE / 2));
        }
    }

    // 7. 최종 후보 목록으로 자식 노드를 생성
    createChildNodesFromMoves(candidate_moves);
}
void Node::createChildNodesFromMoves(vector<Move>& moves) {
    // 기존 자식 노드 정리
    for (Node* child : this->children) { delete child; }
    this->children.clear();

    // 중복 제거
    if (moves.size() > 1) {
        std::sort(moves.begin(), moves.end());
        moves.erase(std::unique(moves.begin(), moves.end()), moves.end());
    }

    // 자식 노드 생성
    for (const Move& m : moves) {
        this->children.push_back(new Node(this, m));
    }
}
vector<Move> Node::extract_moves(const vector<PatternInfo>& infos) {
    std::vector<Move> moves;
    moves.reserve(infos.size());
    for (const auto& info : infos) {
        moves.push_back(info.move);
    }
    return moves;
}

// PatternAnalyzer 멤버 함수
void PatternAnalyzer::analyze(const Board& board, StoneType ai_player) {
    // playerPattern type
    ai_patterns.clear();
    opponent_patterns.clear();
    StoneType opponent_player = (ai_player == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;

    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            if (board.getStone(r, c) == StoneType::EMPTY) {
                checkPatternsAfterMove(board, r, c, ai_player, ai_patterns);
                checkPatternsAfterMove(board, r, c, opponent_player, opponent_patterns);
            }
        }
    }
}
void PatternAnalyzer::checkPatternsAfterMove(const Board& board, int r, int c, StoneType player, PlayerPatterns& final_patterns) {

    // --- 1. 이 Move(r,c) 하나만을 위한 임시 패턴 저장소 생성 ---
    PlayerPatterns temp_patterns;

    // 가상으로 돌을 놓아볼 임시 보드 생성
    Board temp_board = board;
    temp_board.placeStone(Move(r, c), player);

    // 4개의 방향(축)을 순회하며 임시 저장소(temp_patterns)를 채움
    const int directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };
    const LineType line_types[4] = { LineType::HORIZONTAL, LineType::VERTICAL, LineType::DIAGONAL_MAIN, LineType::DIAGONAL_ANTI };

    for (int i = 0; i < 4; ++i) {
        vector<StoneType> segment = extractLineSegment(temp_board, r, c, directions[i][0], directions[i][1], player);

        // ★ 중요: findBestPatternInSegment 함수는 이제 temp_patterns를 직접 채웁니다.
        findBestPatternInSegment(segment, r, c, line_types[i], player, temp_patterns);
    }

    // --- 2. 임시 저장소(temp_patterns)의 내용을 보고 최종 패턴 판단 ---

    // 🥇 1순위: 오목(Five) 체크
    if (!temp_patterns.win_moves.empty()) {
        final_patterns.win_moves.push_back({ Move(r, c), LineType::DONTCARE });
        return; // 게임이 끝났으므로 다른 어떤 패턴도 더 이상 중요하지 않음
    }

    // "열린 삼 계열"과 "넷 계열" 패턴의 개수를 센다
    int four_count = temp_patterns.open_four_moves.size() +
        temp_patterns.close_four_moves.size() +
        temp_patterns.blanked_four_moves.size();

    int open_three_count = temp_patterns.open_three_moves.size() +
        temp_patterns.blanked_open_three_moves.size() +
        temp_patterns.double_blanked_open_three_moves.size();

    bool is_44 = (four_count >= 2);
    bool is_33 = (open_three_count >= 2);
    bool is_43 = (four_count >= 1 && open_three_count >= 1);

    // 🥈 금수(쌍사, 쌍삼) 체크 (흑돌일 경우)
    if (player == StoneType::BLACK && (is_44 || is_33)) {
        final_patterns.forbidden_spot.push_back({ Move(r, c), LineType::DONTCARE });
        // 금수라도 다른 패턴일 수 있으므로 return하지 않고, 나중에 generate_children에서 필터링
    }

    // 🥉 필승기(사삼, 백돌의 쌍사/쌍삼) 체크
    if (is_43 || (player == StoneType::WHITE && (is_44 || is_33))) {
        final_patterns.four_three_moves.push_back({ Move(r, c), LineType::DONTCARE });
        return; // 필승기를 찾았으면, 더 낮은 순위의 단일 패턴으로 중복 분류하지 않음
    }

    // --- 3. 단일 패턴들을 진짜 final_patterns 객체에 추가 ---
    // 위의 필승기나 금수에 해당하지 않았을 경우, 발견된 단일 패턴들을 그대로 옮겨 담는다.
    // 4목 패턴
    final_patterns.open_four_moves.insert(final_patterns.open_four_moves.end(), temp_patterns.open_four_moves.begin(), temp_patterns.open_four_moves.end());
    final_patterns.close_four_moves.insert(final_patterns.close_four_moves.end(), temp_patterns.close_four_moves.begin(), temp_patterns.close_four_moves.end());
    final_patterns.blanked_four_moves.insert(final_patterns.blanked_four_moves.end(), temp_patterns.blanked_four_moves.begin(), temp_patterns.blanked_four_moves.end());

    // 3목 패턴
    final_patterns.open_three_moves.insert(final_patterns.open_three_moves.end(), temp_patterns.open_three_moves.begin(), temp_patterns.open_three_moves.end());
    final_patterns.blanked_open_three_moves.insert(final_patterns.blanked_open_three_moves.end(), temp_patterns.blanked_open_three_moves.begin(), temp_patterns.blanked_open_three_moves.end());
    final_patterns.double_blanked_open_three_moves.insert(final_patterns.double_blanked_open_three_moves.end(), temp_patterns.double_blanked_open_three_moves.begin(), temp_patterns.double_blanked_open_three_moves.end());

    final_patterns.close_three_moves.insert(final_patterns.close_three_moves.end(), temp_patterns.close_three_moves.begin(), temp_patterns.close_three_moves.end());
    final_patterns.blanked_close_three_moves.insert(final_patterns.blanked_close_three_moves.end(), temp_patterns.blanked_close_three_moves.begin(), temp_patterns.blanked_close_three_moves.end());
    final_patterns.double_blanked_close_three_moves.insert(final_patterns.double_blanked_close_three_moves.end(), temp_patterns.double_blanked_close_three_moves.begin(), temp_patterns.double_blanked_close_three_moves.end());

    // 2목 패턴
    final_patterns.open_two_moves.insert(final_patterns.open_two_moves.end(), temp_patterns.open_two_moves.begin(), temp_patterns.open_two_moves.end());
    final_patterns.blanked_open_two_moves.insert(final_patterns.blanked_open_two_moves.end(), temp_patterns.blanked_open_two_moves.begin(), temp_patterns.blanked_open_two_moves.end());
    final_patterns.double_blanked_open_two_moves.insert(final_patterns.double_blanked_open_two_moves.end(), temp_patterns.double_blanked_open_two_moves.begin(), temp_patterns.double_blanked_open_two_moves.end());

    final_patterns.close_two_moves.insert(final_patterns.close_two_moves.end(), temp_patterns.close_two_moves.begin(), temp_patterns.close_two_moves.end());
    final_patterns.blanked_close_two_moves.insert(final_patterns.blanked_close_two_moves.end(), temp_patterns.blanked_close_two_moves.begin(), temp_patterns.blanked_close_two_moves.end());
    final_patterns.double_blanked_close_two_moves.insert(final_patterns.double_blanked_close_two_moves.end(), temp_patterns.double_blanked_close_two_moves.begin(), temp_patterns.double_blanked_close_two_moves.end());
    
}

// Board 멤버 함수
StoneType Board::checkForWin(const Move& last_move) const {
    int r = last_move.row;
    int c = last_move.col;
    StoneType stone = this->getStone(r, c);

    if (stone == StoneType::EMPTY) {
        return StoneType::EMPTY;
    }

    // 4개의 축(가로, 세로, 대각선\, 대각선/)에 대한 방향 벡터
    const int dr[] = { 0, 1, 1, 1 };
    const int dc[] = { 1, 0, 1, -1 };

    for (int i = 0; i < 4; ++i) {
        int consecutive_count = 1; // 방금 놓은 돌 포함

        // 정방향(+)으로 같은 돌 세기
        for (int j = 1; j < 5; ++j) {
            int nr = r + j * dr[i];
            int nc = c + j * dc[i];
            if (isOnBoard(nr, nc) && getStone(nr, nc) == stone) {
                consecutive_count++;
            }
            else {
                break;
            }
        }

        // 역방향(-)으로 같은 돌 세기
        for (int j = 1; j < 5; ++j) {
            int nr = r - j * dr[i];
            int nc = c - j * dc[i];
            if (isOnBoard(nr, nc) && getStone(nr, nc) == stone) {
                consecutive_count++;
            }
            else {
                break;
            }
        }

        if (consecutive_count >= 5) {
            return stone; // 오목 완성!
        }
    }

    return StoneType::EMPTY; // 오목이 완성되지 않음
}

// namespace
namespace std {
    template <>
    struct hash<Move> {
        size_t operator()(const Move& m) const {
            // 간단하면서도 효과적인 해시 조합 방식
            size_t h1 = hash<int>()(m.row);
            size_t h2 = hash<int>()(m.col);
            return h1 ^ (h2 << 1);
        }
    };
}
namespace PatternUtils {

    /**
     * @brief 주어진 돌이 특정 플레이어에게 방해물(Blocker)인지 확인합니다.
     * @param stone_to_check 검사할 돌의 타입
     * @param opponent_player 상대방 플레이어의 돌 타입
     * @return 방해물(상대방 돌 또는 벽)이면 true, 아니면 false
     */
    inline bool isBlocker_PatternUtils(StoneType stone_to_check, StoneType opponent_player) {
        return stone_to_check == opponent_player || stone_to_check == StoneType::WALL;
    }

    // -------------------------------------------------------------------
    // --- 5목 패턴 ---
    // -------------------------------------------------------------------

    // XXXXX 형태를 확인합니다.
    inline bool isFive(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 5) return false;
        return window[0] == player && window[1] == player && window[2] == player &&
            window[3] == player && window[4] == player;
    }

    // -------------------------------------------------------------------
    // --- 4목 계열 패턴 ---
    // -------------------------------------------------------------------

    // _XXXX_ 형태를 확인합니다.
    inline bool isOpenFour(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 6) return false;
        return window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == player &&
            window[3] == player && window[4] == player &&
            window[5] == StoneType::EMPTY;
    }

    // BXXXX_ 또는 _XXXXB 형태를 확인합니다.
    inline bool isClosedFour(const std::vector<StoneType>& window, StoneType player, StoneType opponent) {
        if (window.size() != 6) return false;
        bool case1 = isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == player && window[3] == player && window[4] == player &&
            window[5] == StoneType::EMPTY;
        bool case2 = window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == player && window[3] == player && window[4] == player &&
            isBlocker_PatternUtils(window[5], opponent);
        return case1 || case2;
    }

    // X_XXX, XX_XX, XXX_X 형태를 확인합니다.
    inline bool isBlankedFour(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 5) return false;
        return (window[0] == player && window[1] == StoneType::EMPTY && window[2] == player && window[3] == player && window[4] == player) ||
            (window[0] == player && window[1] == player && window[2] == StoneType::EMPTY && window[3] == player && window[4] == player) ||
            (window[0] == player && window[1] == player && window[2] == player && window[3] == StoneType::EMPTY && window[4] == player);
    }

    // -------------------------------------------------------------------
    // --- 3목 계열 패턴 ---
    // -------------------------------------------------------------------

    // _XXX_ 형태를 확인합니다.
    inline bool isOpenThree(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 5) return false;
        return window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == player && window[3] == player &&
            window[4] == StoneType::EMPTY;
    }

    // _X_XX_ 또는 _XX_X_ 형태를 확인합니다.
    inline bool isBlankedOpenThree(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 6) return false;
        return (window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == StoneType::EMPTY && window[3] == player &&
            window[4] == player && window[5] == StoneType::EMPTY) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == player && window[3] == StoneType::EMPTY &&
                window[4] == player && window[5] == StoneType::EMPTY);
    }

    // BXXX_ 또는 _XXXB 형태를 확인합니다.
    inline bool isClosedThree(const std::vector<StoneType>& window, StoneType player, StoneType opponent) {
        if (window.size() != 5) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == player && window[3] == player &&
            window[4] == StoneType::EMPTY) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == player && window[3] == player &&
                isBlocker_PatternUtils(window[4], opponent));
    }

    // BX_XX_ | BXX_X_ | _X_XXB | _XX_XB 형태를 확인
    inline bool isBlankedCloseThree(const std::vector<StoneType>& window, StoneType player, StoneType opponent) {
        if (window.size() != 6) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType::EMPTY && window[3] == player &&
            window[4] == player && window[5] == StoneType::EMPTY) ||
            (isBlocker_PatternUtils(window[0], opponent) &&
                window[1] == player && window[2] == player && window[3] == StoneType::EMPTY &&
                window[4] == player && window[5] == StoneType::EMPTY) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == StoneType::EMPTY && window[3] == player &&
                window[4] == player && isBlocker_PatternUtils(window[5], opponent)) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == player && window[3] == StoneType::EMPTY &&
                window[4] == player && isBlocker_PatternUtils(window[5], opponent));
    }

    // _X_X_X_ 형태를 확인합니다.
    inline bool isDoubleBlankedOpenThree(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 7) return false;
        return window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == StoneType::EMPTY &&
            window[3] == player && window[4] == StoneType::EMPTY &&
            window[5] == player && window[6] == StoneType::EMPTY;
    }

    // BX_X_X_ | _X_X_XB 형태를 확인합니다.
    inline bool isDoubleBlankedCloseThree(const std::vector<StoneType>& window, StoneType player, StoneType opponent) {
        if (window.size() != 7) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType::EMPTY &&
            window[3] == player && window[4] == StoneType::EMPTY &&
            window[5] == player && window[6] == StoneType::EMPTY) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == StoneType::EMPTY &&
                window[3] == player && window[4] == StoneType::EMPTY &&
                window[5] == player && isBlocker_PatternUtils(window[6], opponent));
    }

    // -------------------------------------------------------------------
    // --- 2목 계열 패턴 ---
    // -------------------------------------------------------------------

    // _XX_ 형태를 확인합니다.
    inline bool isOpenTwo(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 4) return false;
        return window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == player &&
            window[3] == StoneType::EMPTY;
    }

    // BXX_ 또는 _XXB 형태를 확인합니다.
    inline bool isClosedTwo(const std::vector<StoneType>& window, StoneType player, StoneType opponent) {
        if (window.size() != 4) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == player &&
            window[3] == StoneType::EMPTY) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == player &&
                isBlocker_PatternUtils(window[3], opponent));
    }

    // _X_X_ 형태를 확인합니다.
    inline bool isBlankedOpenTwo(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 5) return false;
        return window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == StoneType::EMPTY &&
            window[3] == player && window[4] == StoneType::EMPTY;
    }

    // BX_X_ | _X_XB 형태를 확인합니다.
    inline bool isBlankedCloseTwo(const std::vector<StoneType>& window, StoneType player, StoneType opponent) {
        if (window.size() != 5) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType::EMPTY &&
            window[3] == player && window[4] == StoneType::EMPTY) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == StoneType::EMPTY &&
                window[3] == player && isBlocker_PatternUtils(window[4], opponent));
    }

    // _X__X_ 형태를 확인합니다.
    inline bool isDoubleBlankedOpenTwo(const std::vector<StoneType>& window, StoneType player) {
        if (window.size() != 6) return false;
        return window[0] == StoneType::EMPTY &&
            window[1] == player && window[2] == StoneType::EMPTY &&
            window[3] == StoneType::EMPTY && window[4] == player &&
            window[5] == StoneType::EMPTY;
    }

    // BX__X_ | _X__XB 형태를 확인합니다.
    inline bool isDoubleBlankedCloseTwo(const std::vector<StoneType>& window, StoneType player, StoneType opponent) {
        if (window.size() != 6) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType::EMPTY &&
            window[3] == StoneType::EMPTY && window[4] == player &&
            window[5] == StoneType::EMPTY) ||
            (window[0] == StoneType::EMPTY &&
                window[1] == player && window[2] == StoneType::EMPTY &&
                window[3] == StoneType::EMPTY && window[4] == player &&
                isBlocker_PatternUtils(window[5], opponent));
    }
}


void WhiteAttack_2020180014(int* x, int* y)
{
    Board tempBoard = board_2024180014;
    Move location = find_best_move(tempBoard, SearchDepth_2024180014, StoneType::WHITE);

    *y = location.row;
    *x = location.col;

    board_2024180014.placeStone(location, StoneType::WHITE);
}
void WhiteDefence_2020180014(int x, int y)
{
    Move location(y, x);

    board_2024180014.placeStone(location, StoneType::BLACK);
}
void BlackAttack_2020180014(int* x, int* y)
{
    Move location = find_best_move(board_2024180014, SearchDepth_2024180014, StoneType::BLACK);

    *y = location.row;
    *x = location.col;

    board_2024180014.placeStone(location, StoneType::BLACK);
}
void BlackDefence_2020180001(int x, int y)
{
    Move location(y, x);

    board_2024180014.placeStone(location, StoneType::WHITE);
}
