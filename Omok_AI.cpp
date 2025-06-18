#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#define NOMINMAX
#include <Windows.h>
using namespace std;

// 뒤에 학번 붙이는 건 이후 진행할 예정
constexpr int BLACK_2024180014{ 1 };
constexpr int WHITE_2024180014{ 2 };
constexpr int EMPTY_2024180014{ 0 };
constexpr int Board_Size_2024180014{ 19 };

enum class Pattern {
    NONE,
    OPEN_THREE,
    CLOSED_THREE,
    GAPPED_OPEN_THREE, // _X_XX_ 와 같은 패턴
    DOUBLE_BLANKED_OPEN_THREE, // _X_X_X_ 와 같은 패턴
    OPEN_FOUR,
    CLOSED_FOUR,
    FIVE
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
enum class StoneType {
    EMPTY,
    BLACK,
    WHITE,
    WALL
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
    /*
    string toString() const {
        if (isNull()) {
            return "Move(Null)";
        }
        stringstream ss;
        ss << "Move(" << row << ", " << col << ")";
        return ss.str();
    }
    */
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
};
class GameTree {
    Node* Root;
    int Size{};

public:
    GameTree() : Root(nullptr) {}
    ~GameTree() {
        delete Root;
    }


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
    }
};
class PatternAnalyzer {
private:
    PlayerPatterns ai_patterns;      // AI의 패턴 정보
    PlayerPatterns opponent_patterns; // 상대방의 패턴 정보

    void checkPatternsAfterMove(const Board& board, int r, int c, StoneType player, PlayerPatterns& patterns);
    void analyzeLine(const Board& board, int r, int c, int dy, int dx, StoneType player, PlayerPatterns& patterns);
    void findPatternsInSegment(const vector<StoneType>& segment, int r, int c, int dy, int dx, StoneType player, PlayerPatterns& patterns);

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
    const vector<PatternInfo>& getOpponentForbiddenSpot() const { return opponent_patterns.forbidden_spot; }
};

Board board_2024180014;

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
int static_eval_func(const Board& board, StoneType ai_stone_type)
{
    StoneType opponent = (ai_stone_type == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;
    int my_eval{}, opponent_eval{};
    vector<pair<int, int>> checkForFourThree;
    // 열린 3목 & 한 칸 띈 열린 3목 : 3-3 임시 정의
    // 상관없이 4목 & 4목 : 4-4 임시 정의

    for (int checking_order = 0; checking_order < 4; ++checking_order) {
        if (0 == checking_order) {
            for (int r = 0; r < Board::SIZE; ++r) {
                vector<StoneType> line = extract_horizontal_line(board, r);
                my_eval += analyze_patterns_in_line(line, ai_stone_type);
                opponent_eval += analyze_patterns_in_line(line, opponent);
            }
        }
        else if (1 == checking_order) {
            for (int c = 0; c < Board::SIZE; ++c) {
                vector<StoneType> line = extract_vertical_line(board, c);
                my_eval += analyze_patterns_in_line(line, ai_stone_type);
                opponent_eval += analyze_patterns_in_line(line, opponent);
            }
        }
        else if (2 == checking_order) {
            for (int k = -(Board::SIZE - 5); k <= (Board::SIZE - 5); ++k) {
                vector<StoneType> line = extract_diagonal_main(board, k);
                my_eval += analyze_patterns_in_line(line, ai_stone_type);
                opponent_eval += analyze_patterns_in_line(line, opponent);
            }
        }
        else {
            for (int k = 4; k <= 2 * (Board::SIZE - 1) - 4; ++k) {
                vector<StoneType> line = extract_diagonal_anti(board, k);
                my_eval += analyze_patterns_in_line(line, ai_stone_type);
                opponent_eval += analyze_patterns_in_line(line, opponent);
            }
        }
    }

    return my_eval - opponent_eval;
}
int analyze_patterns_in_line(vector<StoneType> line, StoneType stone_type)
{
    int score{};
    StoneType empty{ StoneType::EMPTY };
    StoneType opponent = (stone_type == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;

    constexpr int SCORE_FIVE = 100'000'000;
    constexpr int SCORE_OPEN_FOUR = 1'000'000;
    constexpr int SCORE_BLOCKED_FOUR = 100'000;
    constexpr int SCORE_BLANKED_FOUR = 100'000;        // new
    constexpr int SCORE_OPEN_THREE = 10'000;
    constexpr int SCORE_BLANKED_OPEN_THREE = 5'000;
    constexpr int SCORE_BLOCKED_THREE = 1'000;
    constexpr int SCORE_BLANKED_BLOCK_THREE = 500;
    constexpr int SCORE_OPEN_TWO = 100;
    constexpr int SCORE_BLANKED_TWO = 80;
    constexpr int SCORE_DOUBLE_BLANKED_OPEN_THREE = 50;
    constexpr int SCORE_DOUBLE_BLANKED_BLOCK_THREE = 25;
    constexpr int SCORE_BLOCKED_TWO = 10;
    constexpr int SCORE_BLANKED_BLOCK_TWO = 5;
    constexpr int SCORE_DOUBLE_BLANKED_TWO = 5;              // new

    for (int i = 0; i <= (int)line.size(); ++i) {
        // 1. --- 5목 체크: ...XXXXX... ---
        if (i <= (int)line.size() - 5) {
            if (line[i] == stone_type &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type) {
                score += SCORE_FIVE;
                i += 4;
                continue;
            }
        }

        // 2. --- 열린 4목 체크: ..._XXXX_... ---
        if (i <= (int)line.size() - 6) {
            if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type &&
                line[i + 5] == empty) {
                score += SCORE_OPEN_FOUR;
                i += 5;
                continue;
            }
        }

        // 3. --- 막힌 4목 체크: ...BXXXX_... | ..._XXXXB... /  ... ---
        if (i <= (int)line.size() - 6) {
            if (isBlocker(line[i], opponent) &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type &&
                line[i + 5] == empty) {
                score += SCORE_BLOCKED_FOUR;
                i += 5;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type &&
                isBlocker(line[i + 5], opponent)) {
                score += SCORE_BLOCKED_FOUR;
                i += 5;
                continue;
            }
        }

        // 4. --- 한 칸 띈 4목 체크: ...X_XXX... | ...XX_XX... | ...XXX_X... ---
        if (i <= (int)line.size() - 5) {
            if (line[i] == stone_type &&
                line[i + 1] == empty && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type) {
                score += SCORE_BLANKED_FOUR;
                i += 4;
                continue;
            }
            else if (line[i] == stone_type &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == stone_type) {
                score += SCORE_BLANKED_FOUR;
                i += 4;
                continue;
            }
            else if (line[i] == stone_type &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == empty && line[i + 4] == stone_type) {
                score += SCORE_BLANKED_FOUR;
                i += 4;
                continue;
            }
        }

        // 5. --- 열린 3목 체크: ..._XXX_... ---
        if (i <= (int)line.size() - 5) {
            if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_OPEN_THREE;
                i += 4;
                continue;
            }
        }

        // 6. --- 한 칸 띈 열린 3목 체크: ..._X_XX_... | ..._XX_X_... ---
        if (i <= (int)line.size() - 6) {
            if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == stone_type &&
                line[i + 5] == empty) {
                score += SCORE_BLANKED_OPEN_THREE;
                i += 5;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == empty && line[i + 4] == stone_type &&
                line[i + 5] == empty) {
                score += SCORE_BLANKED_OPEN_THREE;
                i += 5;
                continue;
            }
        }

        // 7. --- 막힌 3목 체크 : ...BXXX_... | ..._XXXB... ---
        if (i <= (int)line.size() - 5) {
            if (isBlocker(line[i], opponent) &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_BLOCKED_THREE;
                i += 4;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && isBlocker(line[i + 4], opponent)) {
                score += SCORE_BLOCKED_THREE;
                i += 4;
                continue;
            }
        }

        // 8. --- 막힌 한 칸 띈 3목 체크 : ...BX_XX_... | ...BXX_X_... | ..._X_XXB... | ..._XX_XB... ---
        if (i <= (int)line.size() - 6) {
            if (isBlocker(line[i], opponent) &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == stone_type &&
                line[i + 5] == empty) {
                score += SCORE_BLANKED_BLOCK_THREE;
                i += 5;
                continue;
            }
            else if (isBlocker(line[i], opponent) &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == empty && line[i + 4] == stone_type &&
                line[i + 5] == empty) {
                score += SCORE_BLANKED_BLOCK_THREE;
                i += 5;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == stone_type &&
                isBlocker(line[i + 5], opponent)) {
                score += SCORE_BLANKED_BLOCK_THREE;
                i += 5;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == empty && line[i + 4] == stone_type &&
                isBlocker(line[i + 5], opponent)) {
                score += SCORE_BLANKED_BLOCK_THREE;
                i += 5;
                continue;
            }
        }

        // 9. --- 열린 2목 체크: ..._XX_... ---
        if (i <= (int)line.size() - 4) {
            if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == empty) {
                score += SCORE_OPEN_TWO;
                i += 3;
                continue;
            }
        }

        //****
        // 10. --- 두 칸 띈 열린 3목 체크: ..._X_X_X_... ---
        if (i <= (int)line.size() - 7) {
            if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == empty &&
                line[i + 5] == stone_type && line[i + 6] == empty) {
                score += SCORE_DOUBLE_BLANKED_OPEN_THREE;
                i += 6;
                continue;
            }
        }

        // 11. --- 두 칸 띈 막힌 3목 체크: ...BX_X_X_... | ..._X_X_XB...
        if (i <= (int)line.size() - 7) {
            if (isBlocker(line[i], opponent) &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == empty &&
                line[i + 5] == stone_type && line[i + 6] == empty) {
                score += SCORE_DOUBLE_BLANKED_BLOCK_THREE;
                i += 6;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == empty &&
                line[i + 5] == stone_type && isBlocker(line[i + 6], opponent)) {
                score += SCORE_DOUBLE_BLANKED_BLOCK_THREE;
                i += 6;
                continue;
            }
        }

        // 12. --- 막힌 2목 체크: ...BXX_... | ..._XXB... ---
        if (i <= (int)line.size() - 4) {
            if (isBlocker(line[i], opponent) &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == empty) {
                score += SCORE_BLOCKED_TWO;
                i += 3;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == stone_type &&
                isBlocker(line[i + 3], opponent)) {
                score += SCORE_BLOCKED_TWO;
                i += 3;
                continue;
            }
        }

        // 13. --- 한 칸 띈 2목 체크: ..._X_X_... ---
        if (i <= (int)line.size() - 5) {
            if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_BLANKED_TWO;
                i += 4;
                continue;
            }
        }

        // 14. --- 두 칸 띈 2목 체크: ..._X__X_... ---
        if (i <= (int)line.size() - 6) {
            if (line[i] == empty &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == empty && line[i + 4] == stone_type &&
                line[i + 5] == empty) {
                score += SCORE_DOUBLE_BLANKED_TWO;
                i += 5;
                continue;
            }
        }

        // 15. --- 막힌 한 칸 띈 2목 체크: ...BX_X_... | ..._X_XB... ---
        if (i <= (int)line.size() - 5) {
            if (isBlocker(line[i], opponent) &&
                line[i + 1] == stone_type && line[i + 2] == empty &&
                line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_BLANKED_BLOCK_TWO;
                i += 4;
                continue;
            }
            else if (line[i] == empty &&
                line[i + 1] == empty && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && isBlocker(line[i + 4], opponent)) {
                score += SCORE_BLANKED_BLOCK_TWO;
                i += 4;
                continue;
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

// Node 멤버 함수
void Node::generate_children(const Board& current_board, StoneType player_to_move) {
    StoneType opponent_player = (player_to_move == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;
    vector<Move> candidate_moves;
    PatternAnalyzer analyzer;
    analyzer.analyze(current_board, player_to_move);
    
    // 1순위: 내가 즉시 이기는 수
    const auto& ai_win_moves = analyzer.getAIWinMoves();
    // vector<PatternInfo> 
    if (!ai_win_moves.empty()) {
        for (const PatternInfo& p_info : ai_win_moves) {
            candidate_moves.push_back(p_info.move);
        }
    }
    // 2순위: 상대가 즉시 이기는 수 (1순위가 없을 경우)
    else {
        const auto& opponent_win_moves = analyzer.getOpponentWinMoves();
        if (!opponent_win_moves.empty()) {
            for (const PatternInfo& p_info : opponent_win_moves) {
                candidate_moves.push_back(p_info.move);
            }
        }
        else {
            const auto& opponent_four_three_moves = analyzer.getOpponentFourThreeMoves();
            // 🥉 3순위: 내가 '백돌'이고, '사삼'으로 이길 수 있는가?
            if (player_to_move == StoneType::WHITE) {
                const auto& ai_four_three_moves = analyzer.getAIFourThreeMoves();
                if (!ai_four_three_moves.empty()) {
                    for (const PatternInfo& p_info : ai_four_three_moves) {
                        candidate_moves.push_back(p_info.move);
                    }
                }
            }
            // 🏅 4순위: 상대가 '백돌'이고, '사삼'으로 이길 수 있는가? -> 반드시 방어
            else if (opponent_player == StoneType::WHITE) {
                const auto& opponent_four_three_moves = analyzer.getOpponentFourThreeMoves();
                if (!opponent_four_three_moves.empty()) {
                    for (const PatternInfo& p_info : opponent_four_three_moves) {
                        candidate_moves.push_back(p_info.move);
                    }
                }
             }
            // 4순위 이후: 여러 중요 패턴들을 조합
            // 여기 추가해야함.
            else {
                const auto& ai_unstoppable = analyzer.getAIFourThreeMoves();
                candidate_moves.insert(candidate_moves.end(), ai_unstoppable.begin(), ai_unstoppable.end());
                const auto& ai_open_fours = analyzer.getAIOpenFourMoves();
                candidate_moves.insert(candidate_moves.end(), ai_open_fours.begin(), ai_open_fours.end());

                const auto& opponent_unstoppable = analyzer.getOpponentFourThreeMoves();
                candidate_moves.insert(candidate_moves.end(), opponent_unstoppable.begin(), opponent_unstoppable.end());
                const auto& opponent_open_fours = analyzer.getOpponentOpenFourMoves();
                candidate_moves.insert(candidate_moves.end(), opponent_open_fours.begin(), opponent_open_fours.end());

                const auto& ai_open_threes = analyzer.getAIOpenThreeMoves();
                candidate_moves.insert(candidate_moves.end(), ai_open_threes.begin(), ai_open_threes.end());
                const auto& opponent_open_threes = analyzer.getOpponentOpenThreeMoves();
                candidate_moves.insert(candidate_moves.end(), opponent_open_threes.begin(), opponent_open_threes.end());
            }
        }
    }

    if (player_to_move == StoneType::BLACK) {
        const auto& forbidden_spots = analyzer.getAIForbiddenSpot(); // 새로운 Getter
        if (!forbidden_spots.empty()) {

            // candidate_moves에서 forbidden_spots에 포함된 모든 수를 제거
            candidate_moves.erase(
                remove_if(candidate_moves.begin(), candidate_moves.end(),
                    [&](const Move& m) {
                        // m이 forbidden_spots 안에 있는지 확인
                        return std::find(forbidden_spots.begin(), forbidden_spots.end(), m) != forbidden_spots.end();
                    }),
                candidate_moves.end());
        }
    }

    // 3. 최종 후보 수 목록 처리

    // 만약 위에서 유의미한 후보 수를 하나도 찾지 못했다면,
    // 주변부 탐색으로 기본적인 후보 수를 생성
    if (candidate_moves.empty()) {
        candidate_moves = generate_neighborhood_moves(current_board);
    }
    // 만약 게임 시작 첫 수라 주변에 아무 돌도 없다면, 중앙 근처에 두도록 처리
    if (candidate_moves.empty() && current_board.isEmpty()) {
        candidate_moves.push_back(Move(Board::SIZE / 2, Board::SIZE / 2));
    }


    // 4. 기존 자식 노드를 정리하고 새로운 자식 노드를 생성합니다.

    for (Node* child : this->children) {
        delete child;
    }
    this->children.clear();

    for (const Move& move_candidate : candidate_moves) {
        this->children.push_back(new Node(this, move_candidate));
    }
}
// PatternAnalyzer 멤버 함수
void PatternAnalyzer::checkPatternsAfterMove(const Board& board, int r, int c, StoneType player, PlayerPatterns& patterns) {
    // 4개의 축(가로, 세로, 대각선 \, 대각선 /)에 대해 라인 분석
    analyzeLine(board, r, c, 0, 1, player, patterns);  // 가로 (dy=0, dx=1)
    analyzeLine(board, r, c, 1, 0, player, patterns);  // 세로 (dy=1, dx=0)
    analyzeLine(board, r, c, 1, 1, player, patterns);  // 대각선 \ (dy=1, dx=1)
    analyzeLine(board, r, c, 1, -1, player, patterns); // 대각선 / (dy=1, dx=-1)

    vector<PatternInfo> temp_fours;
    vector<PatternInfo> temp_open_threes;
    bool is_win = false;

    // 가상으로 돌을 놓은 보드 생성
    Board temp_board = board;
    temp_board.placeStone(Move(r, c), player);

    // 4개의 방향(축)을 순회
    const int directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };
    const LineType line_types[4] = { LineType::HORIZONTAL, LineType::VERTICAL, LineType::DIAGONAL_MAIN, LineType::DIAGONAL_ANTI };

    for (int i = 0; i < 4; ++i) {
        vector<StoneType> segment = extractLineSegment(temp_board, r, c, directions[i][0], directions[i][1]);
        Pattern result = findBestPatternInSegment(segment, player);

        // 분석 결과를 임시 변수에 저장
        if (result == Pattern::FIVE) {
            is_win = true;
            break; // 5목이면 더 볼 필요 없음
        }
        if (result == Pattern::OPEN_FOUR || result == Pattern::CLOSED_FOUR) {
            temp_fours.push_back({ Move(r, c), line_types[i] });
        }
        // "열린 삼 계열" 패턴들을 모두 수집
        if (result == Pattern::OPEN_THREE || result == Pattern::GAPPED_OPEN_THREE || result == Pattern::DOUBLE_BLANKED_OPEN_THREE) {
            temp_open_threes.push_back({ Move(r, c), line_types[i] });
        }
    }

    // --- 2. 수집된 결과들을 조합하여 최종 패턴 판단 및 저장 ---

    // 5목이 최우선
    if (is_win) {
        patterns.win_moves.push_back({ Move(r, c), LineType::DONTCARE });
        return;
    }

    // "사사" 또는 "쌍삼"이 만들어졌는지 확인
    bool is_double_four = (temp_fours.size() >= 2);
    bool is_double_three = (temp_open_threes.size() >= 2);

    // "사삼"이 만들어졌는지 확인
    bool is_four_three = (!temp_fours.empty() && !temp_open_threes.empty());

    // 흑돌의 금수(쌍사, 쌍삼) 처리
    if (player == StoneType::BLACK && (is_double_four || is_double_three)) {
        patterns.forbidden_spot.push_back({ Move(r, c), LineType::DONTCARE });
    }
    
    if (is_four_three || (player == StoneType::WHITE && (is_double_four || is_double_three))) {
        patterns.four_three_moves.push_back({ Move(r, c), LineType::DONTCARE });
    }
    /*
    // 필승기가 아니라면, 단일 패턴들을 각자 리스트에 추가
    else {
        if (!temp_fours.empty()) {
            patterns.all_four_moves.insert(patterns.all_four_moves.end(), temp_fours.begin(), temp_fours.end());
        }
        if (!temp_open_threes.empty()) {
            patterns.all_open_three_moves.insert(patterns.all_open_three_moves.end(), temp_open_threes.begin(), temp_open_threes.end());
        }
    }
    */
}
vector<Pattern> findBestPatternInSegment(vector<StoneType> segment, StoneType player)
{

}
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
// - analyzeLine 함수: 특정 라인을 9칸짜리 벡터로 추출
void PatternAnalyzer::analyzeLine(const Board& board, int r, int c, int dy, int dx, StoneType player, PlayerPatterns& patterns) {
    vector<StoneType> segment;
    segment.reserve(9);

    // (r, c)를 중심으로 -4칸부터 +4칸까지 총 9칸의 정보를 추출
    for (int i = -4; i <= 4; ++i) {
        int nr = r + i * dy;
        int nc = c + i * dx;

        if (board.isOnBoard(nr, nc)) {
            // 만약 현재 위치(i=0)라면, 방금 놓은 돌(player)로 간주
            if (i == 0) segment.push_back(player);
            else segment.push_back(board.getStone(nr, nc));
        }
        else {
            segment.push_back(StoneType::WALL); // 보드 밖은 벽으로 처리
        }
    }

    // 추출된 9칸짜리 세그먼트를 분석 함수에 넘김
    findPatternsInSegment(segment, r, c, dy, dx, player, patterns);
}
inline bool isBlocker(StoneType stone_to_check, StoneType opponent_player) {
    return stone_to_check == opponent_player || stone_to_check == StoneType::WALL;
}
// - findPatternsInSegment 함수: 추출된 벡터에서 패턴을 찾아내는 핵심 로직
void PatternAnalyzer::findPatternsInSegment(const vector<StoneType>& segment, int r, int c, int dy, int dx, StoneType player, PlayerPatterns& patterns) {
    // segment의 크기는 9, 방금 놓은 돌은 항상 중앙인 인덱스 4에 위치함.
    StoneType opponent = (player == StoneType::BLACK) ? StoneType::WHITE : StoneType::BLACK;
    StoneType empty = StoneType::EMPTY;
    LineType line;
    if (dy == 0 && dx == 1) line = LineType::HORIZONTAL;
    else if (dy == 1 && dx == 0) line = LineType::VERTICAL;
    else if (dy == 1 && dx == 1) line = LineType::DIAGONAL_MAIN;
    else if (dy == 1 && dx == -1) line = LineType::DIAGONAL_ANTI;

    // 1. --- 5목 체크: ...XXXXX...---
    // 9칸 세그먼트 안에서 5목이 되는 모든 경우를 확인
    for (int i = 0; i <= 9 - 5; ++i) {
        // 이 패턴이 중앙(index 4)의 돌을 포함하는지 확인
        if (i <= 4 && i + 4 >= 4) {
            if (segment[i] == player &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == player && segment[i + 4] == player) {
                patterns.win_moves.emplace_back(Move(r, c), line);
                return; // 5목을 찾으면 이 라인에 대한 더 이상의 분석은 불필요
            }
        }
    }

    // 2. --- 열린 4목 체크: ..._XXXX_... ---
    // 9칸 세그먼트 안에서 창문 크기 6인 _XXXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == player && segment[i + 4] == player &&
                segment[i + 5] == empty) {
                patterns.open_four_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 3. --- 막힌 4목 체크: ...BXXXX_... | ..._XXXXB... ---
    // 9칸 세그먼트 안에서 6목이 되는 모든 경우를 확인
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) {
            if (isBlocker(segment[i], opponent) &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == player && segment[i + 4] == player &&
                segment[i + 5] == empty) {
                patterns.close_four_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == player && segment[i + 4] == player &&
                isBlocker(segment[i + 5], opponent)) {
                patterns.close_four_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 4. --- 한 칸 띈 4목 체크: ...X_XXX... | ...XX_XX... | ...XXX_X... ---
    // 9칸 세그먼트 안에서 5목이 되는 모든 경우를 확인
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            if (segment[i] == player &&
                segment[i + 1] == empty && segment[i + 2] == player &&
                segment[i + 3] == player && segment[i + 4] == player) {
                patterns.blanked_four_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == player &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && segment[i + 4] == player) {
                patterns.blanked_four_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == player &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == empty && segment[i + 4] == player) {
                patterns.blanked_four_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 5. --- 열린 3목 체크: ..._XXX_... ---
    // 9칸 세그먼트 안에서 창문 크기 5인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == player && segment[i + 4] == empty) {
                patterns.open_three_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 6. --- 한 칸 띈 열린 3목 체크: ..._X_XX_... | ..._XX_X_... ---
    // 9칸 세그먼트 안에서 창문 크기 6인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && segment[i + 4] == player &&
                segment[i + 5] == empty) {
                patterns.blanked_open_three_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == empty && segment[i + 4] == player &&
                segment[i + 5] == empty) {
                patterns.blanked_open_three_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 7. --- 막힌 3목 체크 : ...BXXX_... | ..._XXXB... ---
    // 9칸 세그먼트 안에서 창문 크기 5인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (isBlocker(segment[i], opponent) &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == player && segment[i + 4] == empty) {
                patterns.close_three_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == player && isBlocker(segment[i + 4], opponent)) {
                patterns.close_three_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 8. --- 막힌 한 칸 띈 3목 체크 : ...BX_XX_... | ...BXX_X_... | ..._X_XXB... | ..._XX_XB... ---
    // 9칸 세그먼트 안에서 창문 크기 6인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (isBlocker(segment[i], opponent) &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && segment[i + 4] == player &&
                segment[i + 5] == empty) {
                patterns.blanked_close_three_moves.emplace_back(Move(r, c), line);
            }
            else if (isBlocker(segment[i], opponent) &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == empty && segment[i + 4] == player &&
                segment[i + 5] == empty) {
                patterns.blanked_close_three_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && segment[i + 4] == player &&
                isBlocker(segment[i + 5], opponent)) {
                patterns.blanked_close_three_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == empty && segment[i + 4] == player &&
                isBlocker(segment[i + 5], opponent)) {
                patterns.blanked_close_three_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 9. --- 열린 2목 체크: ..._XX_... ---
    // 9칸 세그먼트 안에서 창문 크기 4인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 4; ++i) {
        if (i <= 4 && i + 3 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == empty) {
                patterns.open_two_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 10. --- 두 칸 띈 열린 3목 체크: ..._X_X_X_... ---
    // 9칸 세그먼트 안에서 창문 크기 7인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 7; ++i) {
        if (i <= 4 && i + 6 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && segment[i + 4] == empty &&
                segment[i + 5] == player && segment[i + 6] == empty) {
                patterns.double_blanked_open_three_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 11. --- 막힌 2목 체크: ...BXX_... | ..._XXB... ---
    // 9칸 세그먼트 안에서 창문 크기 4인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 4; ++i) {
        if (i <= 4 && i + 3 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (isBlocker(segment[i], opponent) &&
                segment[i + 1] == player && segment[i + 2] == player &&
                segment[i + 3] == empty) {
                patterns.close_two_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == player &&
                isBlocker(segment[i + 3], opponent)) {
                patterns.close_two_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 12. --- 한 칸 띈 열린 2목 체크: ..._X_X_... ---
    // 9칸 세그먼트 안에서 창문 크기 5인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && segment[i + 4] == empty) {
                patterns.blanked_open_two_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 13. --- 한 칸 띈 막힌 2목 체크: ...BX_X_... | ..._X_XB...
    // 9칸 세그먼트 안에서 창문 크기 5인 ...BX_X_... | ..._X_XB... 패턴을 확인
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (isBlocker(segment[i], opponent) &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && segment[i + 4] == empty) {
                patterns.blanked_close_two_moves.emplace_back(Move(r, c), line);
            }
            else if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == player && isBlocker(segment[i + 4], opponent)) {
                patterns.blanked_close_two_moves.emplace_back(Move(r, c), line);
            }
        }
    }

    // 14. --- 두 칸 띈 2목 체크: ..._X__X_... ---
    // 9칸 세그먼트 안에서 창문 크기 6인 _XXX_ 패턴을 확인
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) { // 패턴이 중앙의 돌을 포함하는지
            if (segment[i] == empty &&
                segment[i + 1] == player && segment[i + 2] == empty &&
                segment[i + 3] == empty && segment[i + 4] == player &&
                segment[i + 5] == empty) {
                patterns.double_blanked_open_two_moves.emplace_back(Move(r, c), line);
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
