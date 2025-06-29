#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <limits>
#define NOMINMAX
#include <Windows.h>
using namespace std;

// constexpr variables
constexpr int Board_Size_2024180014{ 19 };
constexpr int TimeOut_2024180014{ 500 };
constexpr int INFINIT{ 2'147'483'647 };

// class & struct
enum class StoneType_2024180014 {
    EMPTY,
    BLACK_2024180014,
    WHITE_2024180014,
    WALL
};
enum class Pattern_2024180014 {
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
enum class LineType_2024180014 {
    DONTCARE,       // 상관없음
    HORIZONTAL,     // 가로
    VERTICAL,       // 세로
    DIAGONAL_MAIN,  // 대각선 \'
    DIAGONAL_ANTI   // 대각선 /'
};
struct Move_2024180014 {
    int row; // 행 (y 좌표)
    int col; // 열 (x 좌표)

    Move_2024180014() : row(-1), col(-1) {}
    Move_2024180014(int r, int c) : row(r), col(c) {}

    bool isNull() const {
        return row == -1 && col == -1;
    }
    bool operator==(const Move_2024180014& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Move_2024180014& other) const {
        return !(*this == other);
    }
    bool operator<(const Move_2024180014& other) const {
        if (row != other.row) {
            return row < other.row;
        }
        return col < other.col;
    }
};
struct PatternInfo_2024180014 {
    Move_2024180014 move;
    LineType_2024180014 line;
};
class Board_2024180014 {
    vector<vector<StoneType_2024180014>> board;

public:
    static constexpr int SIZE{ Board_Size_2024180014 };

    Board_2024180014() : board(SIZE, vector<StoneType_2024180014>(SIZE, StoneType_2024180014::EMPTY)) {}

    bool placeStone(const Move_2024180014& move, StoneType_2024180014 stone) {
        if (!isOnBoard(move.row, move.col) || getStone(move.row, move.col) != StoneType_2024180014::EMPTY) {
            return false;
        }
        board[move.row][move.col] = stone;
        return true;
    }
    void retractMove(const Move_2024180014& move) {
        if (isOnBoard(move.row, move.col)) {
            board[move.row][move.col] = StoneType_2024180014::EMPTY;
        }
    }

    bool isEmpty() const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                if (this->board[r][c] != StoneType_2024180014::EMPTY) {
                    return false;
                }
            }
        }
        return true;
    }
    bool isFull() const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                if (this->board[r][c] == StoneType_2024180014::EMPTY) {
                    return false;
                }
            }
        }
        return true;
    }
    StoneType_2024180014 getStone(int r, int c) const {
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
                if (board[r][c] == StoneType_2024180014::BLACK_2024180014) stone_char = 'X';
                else if (board[r][c] == StoneType_2024180014::WHITE_2024180014) stone_char = 'O';
                cout << stone_char << " ";
            }
            cout << endl;
        }
    }
    string toStringKey() const {
        string key = "";
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                key += to_string(static_cast<int>(board[r][c]));
            }
        }
        return key;
    }
    StoneType_2024180014 checkForWin_2024180014(const Move_2024180014& last_move) const;
};
struct Node_2024180014 {
    Move_2024180014 location;
    int static_value{};
    Node_2024180014* parent;
    vector<Node_2024180014*> children;

    Node_2024180014() : parent(nullptr), location(Move_2024180014()) {}
    Node_2024180014(Node_2024180014* parent, Move_2024180014 gen_location) : parent(parent), location(gen_location) {}
    ~Node_2024180014() {
        for (Node_2024180014* child : children) {
            delete child;
        }
    }
    Move_2024180014 getMove() {
        return location;
    }
    vector<Move_2024180014> get_possible_moves() {
        vector<Move_2024180014> possible_moves;
        for (auto next_node : children) {
            Move_2024180014 next_node_location = next_node->location;
            possible_moves.push_back(next_node_location);
        }

        return possible_moves;
    }
    Node_2024180014 make_move(Move_2024180014 location) {
        for (auto target_node : children) {
            if (target_node->location == location) return *target_node;
        }
    }
    void generate_children_2024180014(const Board_2024180014& board, StoneType_2024180014 current_player);
    vector<Move_2024180014> extract_moves_2024180014(const vector<PatternInfo_2024180014>& infos);
    void createChildNodesFromMoves_2024180014(std::vector<Move_2024180014>& moves);
};
struct PlayerPatterns_2024180014 {
    // 우선순위 나열
    vector<PatternInfo_2024180014> win_moves;                         // 1. ...XXXXX...
    vector<PatternInfo_2024180014> open_four_moves;                   // 2. ..._XXXX_...
    vector<PatternInfo_2024180014> four_three_moves;                  // 사삼(4-3)을 만드는 위치
    vector<PatternInfo_2024180014> close_four_moves;                  // 3. ...BXXXX_... | ..._XXXXB...
    vector<PatternInfo_2024180014> blanked_four_moves;                // 4. ...X_XXX... | ...XX_XX... | ...XXX_X...
    vector<PatternInfo_2024180014> open_three_moves;                  // 5. ..._XXX_...
    vector<PatternInfo_2024180014> blanked_open_three_moves;          // 6. ..._X_XX_... | ..._XX_X_...
    vector<PatternInfo_2024180014> close_three_moves;                 // 7. ...BXXX_... | ..._XXXB...
    vector<PatternInfo_2024180014> blanked_close_three_moves;         // 8. ...BX_XX_... | ...BXX_X_... | ..._X_XXB... | ..._XX_XB...
    vector<PatternInfo_2024180014> open_two_moves;                    // 9. ..._XX_...
    vector<PatternInfo_2024180014> double_blanked_open_three_moves;   // 10. ..._X_X_X_...
    vector<PatternInfo_2024180014> double_blanked_close_three_moves;  // 11. ...BX_X_X_... | ..._X_X_XB...
    vector<PatternInfo_2024180014> close_two_moves;                   // 12. ...BXX_... | ..._XXB...
    vector<PatternInfo_2024180014> blanked_open_two_moves;            // 13. ..._X_X_...
    vector<PatternInfo_2024180014> blanked_close_two_moves;           // 14. ...BX_X_... | ..._X_XB...
    vector<PatternInfo_2024180014> double_blanked_open_two_moves;     // 15. ..._X__X_...
    vector<PatternInfo_2024180014> double_blanked_close_two_moves;    // 16. ...BX__X_... | ..._X__XB...
    vector<PatternInfo_2024180014> forbidden_spot;                    // 금수 자리

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
class PatternAnalyzer_2024180014 {
private:
    PlayerPatterns_2024180014 ai_patterns;      // AI의 패턴 정보
    PlayerPatterns_2024180014 opponent_patterns; // 상대방의 패턴 정보

    void checkPatternsAfterMove_2024180014(const Board_2024180014& board, int r, int c, StoneType_2024180014 player, PlayerPatterns_2024180014& patterns);

public:
    PatternAnalyzer_2024180014() = default;

    void analyze_2024180014(const Board_2024180014& board, StoneType_2024180014 ai_player);

    // Get AI Moves
    const vector<PatternInfo_2024180014>& getAIWinMoves() const { return ai_patterns.win_moves; }
    const vector<PatternInfo_2024180014>& getAIOpenFourMoves() const { return ai_patterns.open_four_moves; }
    const vector<PatternInfo_2024180014>& getAIFourThreeMoves() const { return ai_patterns.four_three_moves; }
    const vector<PatternInfo_2024180014>& getAICloseFourMoves() const { return ai_patterns.close_four_moves; }
    const vector<PatternInfo_2024180014>& getAIBlankedFourMoves() const { return ai_patterns.blanked_four_moves; }
    const vector<PatternInfo_2024180014>& getAIOpenThreeMoves() const { return ai_patterns.open_three_moves; }
    const vector<PatternInfo_2024180014>& getAIBlankedOpenThreeMoves() const { return ai_patterns.blanked_open_three_moves; }
    const vector<PatternInfo_2024180014>& getAICloseThreeMoves() const { return ai_patterns.close_three_moves; }
    const vector<PatternInfo_2024180014>& getAIBlankedCloseThreeMoves() const { return ai_patterns.blanked_close_three_moves; }
    const vector<PatternInfo_2024180014>& getAIOpenTwoMoves() const { return ai_patterns.open_two_moves; }
    const vector<PatternInfo_2024180014>& getAIDoubleBlankedOpenThreeMoves() const { return ai_patterns.double_blanked_open_three_moves; }
    const vector<PatternInfo_2024180014>& getAIDoubleBlankedCloseThreeMoves() const { return ai_patterns.double_blanked_close_three_moves; }
    const vector<PatternInfo_2024180014>& getAICloseTwoMoves() const { return ai_patterns.close_two_moves; }
    const vector<PatternInfo_2024180014>& getAIBlankedOpenTwoMoves() const { return ai_patterns.blanked_open_two_moves; }
    const vector<PatternInfo_2024180014>& getAIBlankedCloseTwoMoves() const { return ai_patterns.blanked_close_two_moves; }
    const vector<PatternInfo_2024180014>& getAIDoubleBlankedOpenTwoMoves() const { return ai_patterns.double_blanked_open_two_moves; }
    const vector<PatternInfo_2024180014>& getAIDoubleBlankedCloseTwoMoves() const { return ai_patterns.double_blanked_close_two_moves; }
    const vector<PatternInfo_2024180014>& getAIForbiddenSpot() const { return ai_patterns.forbidden_spot; }

    // Get Opponent Moves
    const vector<PatternInfo_2024180014>& getOpponentWinMoves() const { return opponent_patterns.win_moves; }
    const vector<PatternInfo_2024180014>& getOpponentOpenFourMoves() const { return opponent_patterns.open_four_moves; }
    const vector<PatternInfo_2024180014>& getOpponentFourThreeMoves() const { return opponent_patterns.four_three_moves; }
    const vector<PatternInfo_2024180014>& getOpponentCloseFourMoves() const { return opponent_patterns.close_four_moves; }
    const vector<PatternInfo_2024180014>& getOpponentBlankedFourMoves() const { return opponent_patterns.blanked_four_moves; }
    const vector<PatternInfo_2024180014>& getOpponentOpenThreeMoves() const { return opponent_patterns.open_three_moves; }
    const vector<PatternInfo_2024180014>& getOpponentBlankedOpenThreeMoves() const { return opponent_patterns.blanked_open_three_moves; }
    const vector<PatternInfo_2024180014>& getOpponentCloseThreeMoves() const { return opponent_patterns.close_three_moves; }
    const vector<PatternInfo_2024180014>& getOpponentBlankedCloseThreeMoves() const { return opponent_patterns.blanked_close_three_moves; }
    const vector<PatternInfo_2024180014>& getOpponentOpenTwoMoves() const { return opponent_patterns.open_two_moves; }
    const vector<PatternInfo_2024180014>& getOpponentDoubleBlankedOpenThreeMoves() const { return opponent_patterns.double_blanked_open_three_moves; }
    const vector<PatternInfo_2024180014>& getOpponentDoubleBlankedCloseThreeMoves() const { return opponent_patterns.double_blanked_close_three_moves; }
    const vector<PatternInfo_2024180014>& getOpponentCloseTwoMoves() const { return opponent_patterns.close_two_moves; }
    const vector<PatternInfo_2024180014>& getOpponentBlankedOpenTwoMoves() const { return opponent_patterns.blanked_open_two_moves; }
    const vector<PatternInfo_2024180014>& getOpponentBlankedCloseTwoMoves() const { return opponent_patterns.blanked_close_two_moves; }
    const vector<PatternInfo_2024180014>& getOpponentDoubleBlankedOpenTwoMoves() const { return opponent_patterns.double_blanked_open_two_moves; }
    const vector<PatternInfo_2024180014>& getOpponentDoubleBlankedCloseTwoMoves() const { return opponent_patterns.double_blanked_close_two_moves; }
    const vector<PatternInfo_2024180014>& getOpponentForbiddenSpot() const { return opponent_patterns.forbidden_spot; }
};
class TimeoutException_2024180014 : public std::exception {};

// namespace
namespace std {
    template <>
    struct hash<Move_2024180014> {
        size_t operator()(const Move_2024180014& m) const {
            size_t h1 = hash<int>()(m.row);
            size_t h2 = hash<int>()(m.col);
            return h1 ^ (h2 << 1);
        }
    };
}
namespace PatternUtils {
    inline bool isBlocker_PatternUtils(StoneType_2024180014 stone_to_check, StoneType_2024180014 opponent_player) {
        return stone_to_check == opponent_player || stone_to_check == StoneType_2024180014::WALL;
    }

    // -------------------------------------------------------------------
    // --- 5목 패턴 ---
    // -------------------------------------------------------------------

    // XXXXX 형태를 확인합니다.
    inline bool isFive(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 5) return false;
        return window[0] == player && window[1] == player && window[2] == player &&
            window[3] == player && window[4] == player;
    }

    // -------------------------------------------------------------------
    // --- 4목 계열 패턴 ---
    // -------------------------------------------------------------------

    // _XXXX_ 형태를 확인합니다.
    inline bool isOpenFour(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 6) return false;
        return window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == player &&
            window[3] == player && window[4] == player &&
            window[5] == StoneType_2024180014::EMPTY;
    }

    // BXXXX_ 또는 _XXXXB 형태를 확인합니다.
    inline bool isClosedFour(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player, StoneType_2024180014 opponent) {
        if (window.size() != 6) return false;
        bool case1 = isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == player && window[3] == player && window[4] == player &&
            window[5] == StoneType_2024180014::EMPTY;
        bool case2 = window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == player && window[3] == player && window[4] == player &&
            isBlocker_PatternUtils(window[5], opponent);
        return case1 || case2;
    }

    // X_XXX, XX_XX, XXX_X 형태를 확인합니다.
    inline bool isBlankedFour(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 5) return false;
        return (window[0] == player && window[1] == StoneType_2024180014::EMPTY && window[2] == player && window[3] == player && window[4] == player) ||
            (window[0] == player && window[1] == player && window[2] == StoneType_2024180014::EMPTY && window[3] == player && window[4] == player) ||
            (window[0] == player && window[1] == player && window[2] == player && window[3] == StoneType_2024180014::EMPTY && window[4] == player);
    }

    // -------------------------------------------------------------------
    // --- 3목 계열 패턴 ---
    // -------------------------------------------------------------------

    // _XXX_ 형태를 확인합니다.
    inline bool isOpenThree(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 5) return false;
        return window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == player && window[3] == player &&
            window[4] == StoneType_2024180014::EMPTY;
    }

    // _X_XX_ 또는 _XX_X_ 형태를 확인합니다.
    inline bool isBlankedOpenThree(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 6) return false;
        return (window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY && window[3] == player &&
            window[4] == player && window[5] == StoneType_2024180014::EMPTY) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == player && window[3] == StoneType_2024180014::EMPTY &&
                window[4] == player && window[5] == StoneType_2024180014::EMPTY);
    }

    // BXXX_ 또는 _XXXB 형태를 확인합니다.
    inline bool isClosedThree(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player, StoneType_2024180014 opponent) {
        if (window.size() != 5) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == player && window[3] == player &&
            window[4] == StoneType_2024180014::EMPTY) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == player && window[3] == player &&
                isBlocker_PatternUtils(window[4], opponent));
    }

    // BX_XX_ | BXX_X_ | _X_XXB | _XX_XB 형태를 확인
    inline bool isBlankedCloseThree(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player, StoneType_2024180014 opponent) {
        if (window.size() != 6) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY && window[3] == player &&
            window[4] == player && window[5] == StoneType_2024180014::EMPTY) ||
            (isBlocker_PatternUtils(window[0], opponent) &&
                window[1] == player && window[2] == player && window[3] == StoneType_2024180014::EMPTY &&
                window[4] == player && window[5] == StoneType_2024180014::EMPTY) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == StoneType_2024180014::EMPTY && window[3] == player &&
                window[4] == player && isBlocker_PatternUtils(window[5], opponent)) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == player && window[3] == StoneType_2024180014::EMPTY &&
                window[4] == player && isBlocker_PatternUtils(window[5], opponent));
    }

    // _X_X_X_ 형태를 확인합니다.
    inline bool isDoubleBlankedOpenThree(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 7) return false;
        return window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
            window[3] == player && window[4] == StoneType_2024180014::EMPTY &&
            window[5] == player && window[6] == StoneType_2024180014::EMPTY;
    }

    // BX_X_X_ | _X_X_XB 형태를 확인합니다.
    inline bool isDoubleBlankedCloseThree(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player, StoneType_2024180014 opponent) {
        if (window.size() != 7) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
            window[3] == player && window[4] == StoneType_2024180014::EMPTY &&
            window[5] == player && window[6] == StoneType_2024180014::EMPTY) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
                window[3] == player && window[4] == StoneType_2024180014::EMPTY &&
                window[5] == player && isBlocker_PatternUtils(window[6], opponent));
    }

    // -------------------------------------------------------------------
    // --- 2목 계열 패턴 ---
    // -------------------------------------------------------------------

    // _XX_ 형태를 확인합니다.
    inline bool isOpenTwo(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 4) return false;
        return window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == player &&
            window[3] == StoneType_2024180014::EMPTY;
    }

    // BXX_ 또는 _XXB 형태를 확인합니다.
    inline bool isClosedTwo(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player, StoneType_2024180014 opponent) {
        if (window.size() != 4) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == player &&
            window[3] == StoneType_2024180014::EMPTY) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == player &&
                isBlocker_PatternUtils(window[3], opponent));
    }

    // _X_X_ 형태를 확인합니다.
    inline bool isBlankedOpenTwo(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 5) return false;
        return window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
            window[3] == player && window[4] == StoneType_2024180014::EMPTY;
    }

    // BX_X_ | _X_XB 형태를 확인합니다.
    inline bool isBlankedCloseTwo(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player, StoneType_2024180014 opponent) {
        if (window.size() != 5) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
            window[3] == player && window[4] == StoneType_2024180014::EMPTY) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
                window[3] == player && isBlocker_PatternUtils(window[4], opponent));
    }

    // _X__X_ 형태를 확인합니다.
    inline bool isDoubleBlankedOpenTwo(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player) {
        if (window.size() != 6) return false;
        return window[0] == StoneType_2024180014::EMPTY &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
            window[3] == StoneType_2024180014::EMPTY && window[4] == player &&
            window[5] == StoneType_2024180014::EMPTY;
    }

    // BX__X_ | _X__XB 형태를 확인합니다.
    inline bool isDoubleBlankedCloseTwo(const std::vector<StoneType_2024180014>& window, StoneType_2024180014 player, StoneType_2024180014 opponent) {
        if (window.size() != 6) return false;
        return (isBlocker_PatternUtils(window[0], opponent) &&
            window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
            window[3] == StoneType_2024180014::EMPTY && window[4] == player &&
            window[5] == StoneType_2024180014::EMPTY) ||
            (window[0] == StoneType_2024180014::EMPTY &&
                window[1] == player && window[2] == StoneType_2024180014::EMPTY &&
                window[3] == StoneType_2024180014::EMPTY && window[4] == player &&
                isBlocker_PatternUtils(window[5], opponent));
    }
}

Board_2024180014 board_2024180014;
unordered_map<string, Move_2024180014> opening_book;

// function define
Move_2024180014 find_best_move_2024180014(const Board_2024180014& current_real_board, StoneType_2024180014 ai_stone_type);
int minimax_alphabeta_2024180014(Node_2024180014* node, Board_2024180014& board, int depth, int alpha, int beta, bool is_my_turn, StoneType_2024180014 ai_stone_type, const std::chrono::steady_clock::time_point& deadline);
StoneType_2024180014 get_opponent_color_2024180014(StoneType_2024180014 player_color);
bool is_game_over_2024180014(const Board_2024180014& board, const Move_2024180014& last_move);
int static_eval_func_2024180014(const Board_2024180014& board, StoneType_2024180014 ai_player);
int analyze_patterns_in_line_2024180014(const vector<StoneType_2024180014>& line, StoneType_2024180014 stone_type);
vector<StoneType_2024180014> extract_horizontal_line_2024180014(const Board_2024180014& board, int row);
vector<StoneType_2024180014> extract_vertical_line_2024180014(const Board_2024180014& board, int col);
vector<StoneType_2024180014> extract_diagonal_main_2024180014(const Board_2024180014& board, int k);
vector<StoneType_2024180014> extract_diagonal_anti_2024180014(const Board_2024180014& board, int k);
vector<StoneType_2024180014> extractLineSegment_2024180014(const Board_2024180014& board, int r, int c, int dy, int dx, StoneType_2024180014 player);
void findBestPatternInSegment_2024180014(const vector<StoneType_2024180014>& segment, int r, int c, LineType_2024180014 line, StoneType_2024180014 player, PlayerPatterns_2024180014& patterns);
vector<Move_2024180014> generate_neighborhood_moves_2024180014(const Board_2024180014& board);
inline bool isBlocker_2024180014(StoneType_2024180014 stone_to_check, StoneType_2024180014 opponent_player);
void add_sequence_to_book(const vector<Move_2024180014>& sequence);
void initialize_opening_book();

// functions
Move_2024180014 find_best_move_2024180014(const Board_2024180014& current_real_board, StoneType_2024180014 ai_stone_type) {
    // --- 1. 시간 설정 ---
    auto start_time = std::chrono::steady_clock::now();
    const int TimeOut = 500;
    auto deadline = start_time + std::chrono::milliseconds(TimeOut - 20);

    // --- 2. opening book 조회 ---
    string current_board_key = current_real_board.toStringKey();

    if (opening_book.count(current_board_key)) {
        //cout << "[AI Turn] Found in Opening Book!" << endl;
        return opening_book[current_board_key];
    }
    else {
        //cout << "[AI Turn] Not in Book. Starting search..." << endl;
    }

    // --- 3. IDS를 위한 변수 설정 ---
    Move_2024180014 best_move_so_far;
    const int MAX_DEPTH = 15;

    // --- 4. IDS 메인 루프 ---
    try {
        for (int current_depth = 1; current_depth <= MAX_DEPTH; ++current_depth) {
            Node_2024180014* root_node = new Node_2024180014();
            Board_2024180014 board_for_search = current_real_board;
            Move_2024180014 best_move_this_depth;
            constexpr int INF{ INFINIT };

            int best_value = -INF;
            int alpha = -INF;
            int beta = +INF;

            root_node->generate_children_2024180014(board_for_search, ai_stone_type);

            for (Node_2024180014* child_node : root_node->children) {
                board_for_search.placeStone(child_node->getMove(), ai_stone_type);

                int move_value = minimax_alphabeta_2024180014(child_node, board_for_search, current_depth - 1, alpha, beta, false, ai_stone_type, deadline);

                board_for_search.retractMove(child_node->getMove());

                if (move_value > best_value) {
                    best_value = move_value;
                    best_move_this_depth = child_node->getMove();
                }
                alpha = max(alpha, best_value);
            }

            best_move_so_far = best_move_this_depth;

            delete root_node;
        }
    }
    catch (const TimeoutException_2024180014& e) {
        std::cerr << "Timeout! Using best move from last completed depth." << std::endl;
    }

    // 최적 수를 못 찾으면 주변 2칸 수 두기
    if (best_move_so_far.isNull()) {
        auto emergency_moves = generate_neighborhood_moves_2024180014(current_real_board);
        if (!emergency_moves.empty()) return emergency_moves[0];
        else {
            return Move_2024180014(Board_2024180014::SIZE / 2, Board_2024180014::SIZE / 2);
        }
    }

    return best_move_so_far;
}
int minimax_alphabeta_2024180014(Node_2024180014* node, Board_2024180014& board, int depth, int alpha, int beta, bool is_my_turn, StoneType_2024180014 ai_stone_type, const std::chrono::steady_clock::time_point& deadline) {
    // --- 종료 조건 ---
    if (depth == 0 || is_game_over_2024180014(board, node->getMove())) {
        return static_eval_func_2024180014(board, ai_stone_type);
    }
    if (std::chrono::steady_clock::now() > deadline) {
        throw TimeoutException_2024180014();
    }

    // --- 자식 노드 생성 ---
    constexpr int INF{ INFINIT };
    StoneType_2024180014 current_player = is_my_turn ? ai_stone_type : get_opponent_color_2024180014(ai_stone_type);
    node->generate_children_2024180014(board, current_player);
    if (node->children.empty()) {
        return static_eval_func_2024180014(board, ai_stone_type);
    }

    // --- 재귀 탐색 ---
    if (is_my_turn) {
        int max_eval = -INF;
        for (Node_2024180014* child_node : node->children) {
            board.placeStone(child_node->getMove(), current_player);

            int eval = minimax_alphabeta_2024180014(child_node, board, depth - 1, alpha, beta, false, ai_stone_type, deadline);

            board.retractMove(child_node->getMove());

            max_eval = max(max_eval, eval);
            alpha = max(alpha, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return max_eval;
    }
    else {
        int min_eval = +INF;
        for (Node_2024180014* child_node : node->children) {
            board.placeStone(child_node->getMove(), current_player);

            int eval = minimax_alphabeta_2024180014(child_node, board, depth - 1, alpha, beta, true, ai_stone_type, deadline);

            board.retractMove(child_node->getMove());

            min_eval = min(min_eval, eval);
            beta = min(beta, eval);

            if (beta <= alpha) {
                break;
            }
        }
        return min_eval;
    }
}
StoneType_2024180014 get_opponent_color_2024180014(StoneType_2024180014 player_color) {
    return (player_color == StoneType_2024180014::BLACK_2024180014) ? StoneType_2024180014::WHITE_2024180014 : StoneType_2024180014::BLACK_2024180014;
}
bool is_game_over_2024180014(const Board_2024180014& board, const Move_2024180014& last_move) {
    if (board.checkForWin_2024180014(last_move) != StoneType_2024180014::EMPTY) {
        return true;
    }
    if (board.isFull()) {
        return true;
    }
    return false;
}
int static_eval_func_2024180014(const Board_2024180014& board, StoneType_2024180014 ai_player) {
    int my_score = 0;
    int opponent_score = 0;
    StoneType_2024180014 opponent_player = (ai_player == StoneType_2024180014::BLACK_2024180014) ? StoneType_2024180014::WHITE_2024180014 : StoneType_2024180014::BLACK_2024180014;

    // 가로 라인 전체 평가
    for (int r = 0; r < Board_2024180014::SIZE; ++r) {
        std::vector<StoneType_2024180014> line = extract_horizontal_line_2024180014(board, r);
        my_score += analyze_patterns_in_line_2024180014(line, ai_player);
        opponent_score += analyze_patterns_in_line_2024180014(line, opponent_player);
    }

    // 세로 라인 전체 평가
    for (int c = 0; c < Board_2024180014::SIZE; ++c) {
        std::vector<StoneType_2024180014> line = extract_vertical_line_2024180014(board, c);
        my_score += analyze_patterns_in_line_2024180014(line, ai_player);
        opponent_score += analyze_patterns_in_line_2024180014(line, opponent_player);
    }

    // 대각선 '\' 라인 전체 평가
    for (int k = -(Board_2024180014::SIZE - 5); k <= (Board_2024180014::SIZE - 5); ++k) {
        std::vector<StoneType_2024180014> line = extract_diagonal_main_2024180014(board, k);
        my_score += analyze_patterns_in_line_2024180014(line, ai_player);
        opponent_score += analyze_patterns_in_line_2024180014(line, opponent_player);
    }

    // 대각선 '/' 라인 전체 평가
    for (int k = 4; k <= 2 * (Board_2024180014::SIZE - 1) - 4; ++k) {
        std::vector<StoneType_2024180014> line = extract_diagonal_anti_2024180014(board, k);
        my_score += analyze_patterns_in_line_2024180014(line, ai_player);
        opponent_score += analyze_patterns_in_line_2024180014(line, opponent_player);
    }

    return my_score - opponent_score;
}
int analyze_patterns_in_line_2024180014(const vector<StoneType_2024180014>& line, StoneType_2024180014 stone_type) {
    int score = 0;
    StoneType_2024180014 empty = StoneType_2024180014::EMPTY;
    StoneType_2024180014 opponent = (stone_type == StoneType_2024180014::BLACK_2024180014) ? StoneType_2024180014::WHITE_2024180014 : StoneType_2024180014::BLACK_2024180014;

    // 패턴별 점수 정의
    constexpr int SCORE_FIVE = 1000000000;
    constexpr int SCORE_OPEN_FOUR = 6000000;
    constexpr int SCORE_CLOSED_FOUR = 500000;
    constexpr int SCORE_BLANKED_FOUR = 500000;
    constexpr int SCORE_OPEN_THREE = 400000;
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

    for (int i = 0; i < line.size(); ++i) {

        // --- 5목 체크 (가장 높은 우선순위) ---
        if (i <= (int)line.size() - 5) {
            if (line[i] == stone_type && line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type) {
                return SCORE_FIVE;
            }
        }

        // --- 4목 계열 패턴 ---
        if (i < (int)line.size() - 6) { // 창문 크기 6
            // 열린 4목: _XXXX_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) {
                score += SCORE_OPEN_FOUR;
                i += 5; continue;
            }
            // 막힌 4목: BXXXX_ 또는 _XXXXB
            if ((isBlocker_2024180014(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == stone_type && isBlocker_2024180014(line[i + 5], opponent))) {
                score += SCORE_CLOSED_FOUR;
                i += 5; continue;
            }
        }
        if (i < (int)line.size() - 5) { // 창문 크기 5
            // 한 칸 띈 4목: X_XXX, XX_XX, XXX_X
            if ((line[i] == stone_type && line[i + 1] == empty && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == stone_type) ||
                (line[i] == stone_type && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type) ||
                (line[i] == stone_type && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type)) {
                score += SCORE_BLANKED_FOUR;
                i += 4; continue;
            }
        }

        // --- 3목 계열 패턴 ---
        if (i < (int)line.size() - 5) { // 창문 크기 5
            // 열린 3목: _XXX_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type &&
                line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_OPEN_THREE;
                i += 4; continue;
            }
        }
        if (i < (int)line.size() - 6) { // 창문 크기 6
            // 한 칸 띈 열린 3목: _X_XX_ 또는 _XX_X_
            if ((line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty)) {
                score += SCORE_BLANKED_OPEN_THREE;
                i += 5; continue;
            }
        }
        if (i < (int)line.size() - 5) { // 창문 크기 5
            // 막힌 3목: BXXX_ 또는 _XXXB
            if ((isBlocker_2024180014(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && line[i + 4] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == stone_type && isBlocker_2024180014(line[i + 4], opponent))) {
                score += SCORE_CLOSED_THREE;
                i += 4; continue;
            }
        }
        if (i < (int)line.size() - 6) { // 창문 크기 6
            // 막힌 한 칸 띈 3목: BX_XX_, BXX_X_ 등
            if ((isBlocker_2024180014(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (isBlocker_2024180014(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == stone_type && isBlocker_2024180014(line[i + 5], opponent)) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty && line[i + 4] == stone_type && isBlocker_2024180014(line[i + 5], opponent))) {
                score += SCORE_BLANKED_CLOSED_THREE;
                i += 5; continue;
            }
        }
        if (i < (int)line.size() - 7) { // 창문 크기 7
            // 두 칸 띈 열린 3목: _X_X_X_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == empty && line[i + 5] == stone_type && line[i + 6] == empty) {
                score += SCORE_DOUBLE_BLANKED_OPEN_THREE;
                i += 6; continue;
            }
        }

        // --- 2목 계열 패턴 ---
        if (i < (int)line.size() - 4) { // 창문 크기 4
            // 열린 2목: _XX_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty) {
                score += SCORE_OPEN_TWO;
                i += 3; continue;
            }
        }
        if (i < (int)line.size() - 5) { // 창문 크기 5
            // 한 칸 띈 열린 2목: _X_X_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == empty) {
                score += SCORE_BLANKED_OPEN_TWO;
                i += 4; continue;
            }
        }
        if (i < (int)line.size() - 4) { // 창문 크기 4
            // 막힌 2목: BXX_ 또는 _XXB
            if ((isBlocker_2024180014(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == stone_type && line[i + 3] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == stone_type && isBlocker_2024180014(line[i + 3], opponent))) {
                score += SCORE_CLOSED_TWO;
                i += 3; continue;
            }
        }
        if (i < (int)line.size() - 6) { // 창문 크기 6
            // 두 칸 띈 열린 2목: _X__X_
            if (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty) {
                score += SCORE_DOUBLE_BLANKED_OPEN_TWO;
                i += 5; continue;
            }
        }
        if (i < (int)line.size() - 6) { // 창문 크기 6
            // 두 칸 띈 열린 2목: BX__X_ | _X__XB
            if ((isBlocker_2024180014(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == empty && line[i + 4] == stone_type && line[i + 5] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == empty && line[i + 4] == stone_type && isBlocker_2024180014(line[i + 5], opponent))) {
                score += SCORE_DOUBLE_BLANKED_OPEN_TWO;
                i += 5; continue;
            }
        }
        if (i < (int)line.size() - 5) { // 창문 크기 5
            // 막힌 한 칸 띈 2목: BX_X_ 또는 _X_XB
            if ((isBlocker_2024180014(line[i], opponent) && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && line[i + 4] == empty) ||
                (line[i] == empty && line[i + 1] == stone_type && line[i + 2] == empty && line[i + 3] == stone_type && isBlocker_2024180014(line[i + 4], opponent))) {
                score += SCORE_BLANKED_CLOSED_TWO;
                i += 4; continue;
            }
        }
    }
    return score;
}
vector<StoneType_2024180014> extract_horizontal_line_2024180014(const Board_2024180014& board, int row)
{
    vector<StoneType_2024180014> line;
    line.reserve(Board_2024180014::SIZE);

    for (int c = 0; c < Board_2024180014::SIZE; ++c) {
        line.push_back(board.getStone(row, c));
    }
    return line;
}
vector<StoneType_2024180014> extract_vertical_line_2024180014(const Board_2024180014& board, int col)
{
    vector<StoneType_2024180014> line;
    line.reserve(Board_2024180014::SIZE);

    for (int r = 0; r < Board_2024180014::SIZE; ++r) {
        line.push_back(board.getStone(r, col));
    }
    return line;
}
vector<StoneType_2024180014> extract_diagonal_main_2024180014(const Board_2024180014& board, int k)
{
    vector<StoneType_2024180014> line;
    line.reserve(Board_2024180014::SIZE);

    for (int c = 0; c < Board_2024180014::SIZE; ++c) {
        int r = k + c;
        if (board.isOnBoard(r, c)) {
            line.push_back(board.getStone(r, c));
        }
    }
    return line;
}
vector<StoneType_2024180014> extract_diagonal_anti_2024180014(const Board_2024180014& board, int k)
{
    vector<StoneType_2024180014> line;
    line.reserve(Board_2024180014::SIZE);

    int r_start = max(0, k - (Board_2024180014::SIZE - 1));
    int r_end = min(Board_2024180014::SIZE - 1, k);

    for (int r = r_start; r <= r_end; ++r) {
        int c = k - r;
        line.push_back(board.getStone(r, c));
    }
    return line;
}
vector<StoneType_2024180014> extractLineSegment_2024180014(const Board_2024180014& board, int r, int c, int dy, int dx, StoneType_2024180014 player)
{
    vector<StoneType_2024180014> segment;
    segment.reserve(9);

    for (int i = -4; i <= 4; ++i) {
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
            segment.push_back(StoneType_2024180014::WALL);
        }
    }
    return segment;
}
void findBestPatternInSegment_2024180014(const vector<StoneType_2024180014>& segment, int r, int c, LineType_2024180014 line, StoneType_2024180014 player, PlayerPatterns_2024180014& patterns) {
    StoneType_2024180014 opponent = (player == StoneType_2024180014::BLACK_2024180014) ? StoneType_2024180014::WHITE_2024180014 : StoneType_2024180014::BLACK_2024180014;

    // 5목 체크 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isFive(window, player)) {
                patterns.win_moves.push_back({ Move_2024180014(r, c), line });
                return;
            }
        }
    }

    // 4목 계열 체크 (window: 6)
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 6);
            if (PatternUtils::isOpenFour(window, player)) {
                patterns.open_four_moves.push_back({ Move_2024180014(r, c), line });
            }
            else if (PatternUtils::isClosedFour(window, player, opponent)) {
                patterns.close_four_moves.push_back({ Move_2024180014(r, c), line });
            }
        }
    }
    
    // 4목 계열 체크 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isBlankedFour(window, player)) {
                patterns.blanked_four_moves.push_back({ Move_2024180014(r, c), line });
                return;
            }
        }
    }

    // 3목 계열 체크
    // 열린 삼 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isOpenThree(window, player)) {
                patterns.open_three_moves.push_back({ Move_2024180014(r, c), line });
            }
            else if (PatternUtils::isClosedThree(window, player, opponent)) {
                patterns.close_three_moves.push_back({ Move_2024180014(r, c), line });
            }
        }
    }

    // 한 칸 띈 열린/닫힌 삼 (window: 6)
    for (int i = 0; i <= 9 - 6; ++i) {
        if (i <= 4 && i + 5 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 6);
            if (PatternUtils::isBlankedOpenThree(window, player)) {
                patterns.blanked_open_three_moves.push_back({ Move_2024180014(r, c), line });
            }
            else if (PatternUtils::isBlankedCloseThree(window, player, opponent)) {
                patterns.blanked_close_three_moves.push_back({ Move_2024180014(r, c), line });
            }
        }
    }

    // 두 칸 띈 열린 삼 (window: 7)
    for (int i = 0; i <= 9 - 7; ++i) {
        if (i <= 4 && i + 6 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 7);
            if (PatternUtils::isDoubleBlankedOpenThree(window, player)) {
                patterns.double_blanked_open_three_moves.push_back({ Move_2024180014(r, c), line });
            }
            else if (PatternUtils::isDoubleBlankedCloseThree(window, player, opponent)) {
                patterns.double_blanked_close_three_moves.push_back({ Move_2024180014(r, c), line });
            }
        }
    }
    
    // 2목 계열 체크
    // 열린 2목 (window: 4)
    for (int i = 0; i <= 9 - 4; ++i) {
        if (i <= 4 && i + 3 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 4);
            if (PatternUtils::isOpenTwo(window, player)) {
                patterns.open_two_moves.push_back({ Move_2024180014(r, c), line });
            }
        }
    }

    // 한 칸 띈 열린/닫힌 2목 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isBlankedOpenTwo(window, player)) {
                patterns.blanked_open_two_moves.push_back({ Move_2024180014(r, c), line });
            }
            else if(PatternUtils::isBlankedCloseTwo(window, player, opponent)) {
                patterns.blanked_open_two_moves.push_back({ Move_2024180014(r, c), line });
            }
        }
    }

    // 두 칸 띈 열린/닫힌 2목 (window: 5)
    for (int i = 0; i <= 9 - 5; ++i) {
        if (i <= 4 && i + 4 >= 4) {
            std::vector<StoneType_2024180014> window(segment.begin() + i, segment.begin() + i + 5);
            if (PatternUtils::isDoubleBlankedOpenTwo(window, player)) {
                patterns.blanked_open_two_moves.push_back({ Move_2024180014(r, c), line });
            }
            else if (PatternUtils::isDoubleBlankedCloseTwo(window, player, opponent)) {
                patterns.blanked_open_two_moves.push_back({ Move_2024180014(r, c), line });
            }
        }
    }
}
vector<Move_2024180014> generate_neighborhood_moves_2024180014(const Board_2024180014& board) {
    unordered_set<Move_2024180014> candidate_set;

    for (int r = 0; r < Board_2024180014::SIZE; ++r) {
        for (int c = 0; c < Board_2024180014::SIZE; ++c) {
            if (board.getStone(r, c) == StoneType_2024180014::EMPTY) {
                continue;
            }
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    if (dx == 0 && dy == 0) continue;

                    int nr = r + dy;
                    int nc = c + dx;

                    if (board.isOnBoard(nr, nc) && board.getStone(nr, nc) == StoneType_2024180014::EMPTY) {
                        candidate_set.insert(Move_2024180014(nr, nc));
                    }
                }
            }
        }
    }

    return vector<Move_2024180014>(candidate_set.begin(), candidate_set.end());
}
inline bool isBlocker_2024180014(StoneType_2024180014 stone_to_check, StoneType_2024180014 opponent_player) {
    return stone_to_check == opponent_player || stone_to_check == StoneType_2024180014::WALL;
}
void add_sequence_to_book(const std::vector<Move_2024180014>& base_sequence) {
    if (base_sequence.empty()) return;

    constexpr int center = Board_2024180014::SIZE / 2;

    // 8개의 대칭 변환 함수 정의
    vector<function<Move_2024180014(Move_2024180014)>> transforms;

    // 1. 원본
    transforms.push_back([&](Move_2024180014 m) { return m; });
    // 2. 90도 회전
    transforms.push_back([&](Move_2024180014 m) { return Move_2024180014(center - (m.col - center), center + (m.row - center)); });
    // 3. 180도 회전
    transforms.push_back([&](Move_2024180014 m) { return Move_2024180014(center - (m.row - center), center - (m.col - center)); });
    // 4. 270도 회전
    transforms.push_back([&](Move_2024180014 m) { return Move_2024180014(center + (m.col - center), center - (m.row - center)); });

    // 5. 좌우 반전
    transforms.push_back([&](Move_2024180014 m) { return Move_2024180014(m.row, center - (m.col - center)); });
    // 6. 좌우 반전 + 90도 회전
    transforms.push_back([&](Move_2024180014 m) { m.col = center - (m.col - center); return Move_2024180014(center - (m.col - center), center + (m.row - center)); });
    // 7. 좌우 반전 + 180도 회전
    transforms.push_back([&](Move_2024180014 m) { m.col = center - (m.col - center); return Move_2024180014(center - (m.row - center), center - (m.col - center)); });
    // 8. 좌우 반전 + 270도 회전
    transforms.push_back([&](Move_2024180014 m) { m.col = center - (m.col - center); return Move_2024180014(center + (m.col - center), center - (m.row - center)); });

    for (const auto& transform : transforms) {
        Board_2024180014 temp_board;
        StoneType_2024180014 current_stone = StoneType_2024180014::BLACK_2024180014;

        for (size_t i = 0; i < base_sequence.size(); ++i) {
            Move_2024180014 transformed_move = transform(base_sequence[i]);

            string key = temp_board.toStringKey();

            if (opening_book.find(key) == opening_book.end()) {
                opening_book[key] = transformed_move;
            }

            temp_board.placeStone(transformed_move, current_stone);
            current_stone = (current_stone == StoneType_2024180014::BLACK_2024180014) ? StoneType_2024180014::WHITE_2024180014 : StoneType_2024180014::BLACK_2024180014;
        }
    }
}
void initialize_opening_book() {
    // 주형 오프닝
    add_sequence_to_book({ {9, 9}, {9, 10}, {10, 10}, {10, 9}, {8, 8} });
    add_sequence_to_book({ {9, 9}, {9, 10}, {10, 10}, {10, 9}, {11, 11} });

    // 화월 오프닝
    add_sequence_to_book({ {9, 9}, {10, 9}, {10, 10}, {11, 10}, {11, 11} });
    add_sequence_to_book({ {9, 9}, {10, 9}, {10, 10}, {11, 10}, {9, 10} });

    // 포월 오프닝
    add_sequence_to_book({ {9, 9}, {9, 8}, {8, 8}, {10, 9}, {8, 10} });

    // 명성 오프닝
    add_sequence_to_book({ {9, 9}, {9, 10}, {10, 9}, {11, 8}, {8, 11} });

    add_sequence_to_book({ {9, 9}, {9, 10}, {10, 10} });
    add_sequence_to_book({ {9, 9}, {9, 10}, {8, 10} });
    add_sequence_to_book({ {9, 9}, {9, 8}, {8, 8} });
    add_sequence_to_book({ {9, 9}, {10, 9}, {9, 10} });

    add_sequence_to_book({ {9, 9}, {9, 10}, {9, 11}, {8, 10}, {10, 10} });
    add_sequence_to_book({ {9, 9}, {9, 10}, {10, 9}, {10, 10}, {8, 9} });
    add_sequence_to_book({ {9, 9}, {9, 10}, {8, 10}, {8, 9}, {7, 10} });
    add_sequence_to_book({ {9, 9}, {9, 10}, {10, 9}, {10, 10}, {8, 10} });
}
struct OpeningBookInitializer {
    OpeningBookInitializer() {
        initialize_opening_book();
    }
};
OpeningBookInitializer book_initializer;

// Node 멤버 함수
void Node_2024180014::generate_children_2024180014(const Board_2024180014& current_board, StoneType_2024180014 player_to_move) {
    vector<Move_2024180014> candidate_moves;

    PatternAnalyzer_2024180014 analyzer;
    analyzer.analyze_2024180014(current_board, player_to_move);

    StoneType_2024180014 opponent_player = (player_to_move == StoneType_2024180014::BLACK_2024180014) ? StoneType_2024180014::WHITE_2024180014 : StoneType_2024180014::BLACK_2024180014;

    const auto& my_wins = analyzer.getAIWinMoves();
    if (!my_wins.empty()) {
        //cout << "[DEBUG] >> Priority 1: AI Win Found!" << endl;
        candidate_moves = extract_moves_2024180014(my_wins);
    }
    else if (!analyzer.getOpponentWinMoves().empty()) {
        //cout << "[DEBUG] >> Priority 2: Opponent Win Found! Blocking..." << endl;
        candidate_moves = extract_moves_2024180014(analyzer.getOpponentWinMoves());
    }
    else if (!analyzer.getAIFourThreeMoves().empty() || !analyzer.getAIOpenFourMoves().empty()) {
        //cout << "[DEBUG] >> Priority 3: AI Unstoppable Attack Found!" << endl;
        candidate_moves = extract_moves_2024180014(analyzer.getAIFourThreeMoves());
        const auto& open_fours = extract_moves_2024180014(analyzer.getAIOpenFourMoves());
        candidate_moves.insert(candidate_moves.end(), open_fours.begin(), open_fours.end());
    }
    else if (!analyzer.getOpponentFourThreeMoves().empty() || !analyzer.getOpponentOpenFourMoves().empty()) {
        //cout << "[DEBUG] >> Priority 4: Opponent Unstoppable Attack Found! Blocking..." << endl;
        candidate_moves = extract_moves_2024180014(analyzer.getOpponentFourThreeMoves());
        const auto& opponent_open_fours = extract_moves_2024180014(analyzer.getOpponentOpenFourMoves());
        candidate_moves.insert(candidate_moves.end(), opponent_open_fours.begin(), opponent_open_fours.end());
    }
    else {
        //cout << "[DEBUG] >> Priority 5: Normal Developing Moves." << endl;
        struct PrioritizedMove {
            Move_2024180014 move;
            int score;
        };
        std::vector<PrioritizedMove> scored_moves;

        constexpr int P_SCORE_MAKE_CLOSED_FOUR = 100000;
        constexpr int P_SCORE_BLOCK_OPEN_THREE = 90000;
        constexpr int P_SCORE_MAKE_OPEN_THREE = 50000;
        constexpr int P_SCORE_BLOCK_CLOSED_FOUR = 10000;
        constexpr int P_SCORE_MAKE_BLANKED_OPEN_THREE = 8000;
        constexpr int P_SCORE_BLOCK_BLANKED_OPEN_THREE = 9000;
        constexpr int P_SCORE_MAKE_CLOSED_THREE = 1500;
        constexpr int P_SCORE_BLOCK_CLOSED_THREE = 1200;
        constexpr int P_SCORE_MAKE_BLANKED_CLOSE_THREE = 1000;
        constexpr int P_SCORE_BLOCK_BLANKED_CLOSE_THREE = 900;
        constexpr int P_SCORE_BLOCK_OPEN_TWO = 600;
        constexpr int P_SCORE_MAKE_OPEN_TWO = 500;
        constexpr int P_SCORE_BLOCK_BLANKED_OPEN_TWO = 120;
        constexpr int P_SCORE_MAKE_BLANKED_OPEN_TWO = 100;
        constexpr int P_SCORE_BLOCK_CLOSED_TWO = 60;
        constexpr int P_SCORE_MAKE_CLOSED_TWO = 50;
        constexpr int P_SCORE_MAKE_DOUBLE_BLANKED_OPEN_TWO = 10;
        constexpr int P_SCORE_MAKE_DOUBLE_BLANKED_CLOSE_TWO = 5;

        // 3. 분석 결과를 바탕으로 모든 후보 수에 점수를 매겨 scored_moves에 추가
        for (const auto& info : analyzer.getAICloseFourMoves())     scored_moves.push_back({ info.move, P_SCORE_MAKE_CLOSED_FOUR });
        for (const auto& info : analyzer.getOpponentOpenThreeMoves()) scored_moves.push_back({ info.move, P_SCORE_BLOCK_OPEN_THREE });
        for (const auto& info : analyzer.getAIOpenThreeMoves())      scored_moves.push_back({ info.move, P_SCORE_MAKE_OPEN_THREE });

        // --- Tier 4-5급 공격/방어 ---
        for (const auto& info : analyzer.getOpponentCloseFourMoves())   scored_moves.push_back({ info.move, P_SCORE_BLOCK_CLOSED_FOUR });
        for (const auto& info : analyzer.getAIBlankedOpenThreeMoves())      scored_moves.push_back({ info.move, P_SCORE_MAKE_BLANKED_OPEN_THREE });
        for (const auto& info : analyzer.getOpponentBlankedOpenThreeMoves()) scored_moves.push_back({ info.move, P_SCORE_BLOCK_BLANKED_OPEN_THREE });
        for (const auto& info : analyzer.getAICloseThreeMoves())      scored_moves.push_back({ info.move, P_SCORE_MAKE_CLOSED_THREE });
        for (const auto& info : analyzer.getOpponentCloseThreeMoves())  scored_moves.push_back({ info.move, P_SCORE_BLOCK_CLOSED_THREE });
        for (const auto& info : analyzer.getAIBlankedCloseThreeMoves()) scored_moves.push_back({ info.move, P_SCORE_MAKE_BLANKED_CLOSE_THREE });

        // --- Tier 5-6급 공격/방어 ---
        for (const auto& info : analyzer.getOpponentBlankedCloseThreeMoves()) scored_moves.push_back({ info.move, P_SCORE_BLOCK_BLANKED_CLOSE_THREE }); // 빠진 부분 추가
        for (const auto& info : analyzer.getOpponentOpenTwoMoves()) scored_moves.push_back({ info.move, P_SCORE_BLOCK_OPEN_TWO });
        for (const auto& info : analyzer.getAIOpenTwoMoves())      scored_moves.push_back({ info.move, P_SCORE_MAKE_OPEN_TWO });
        for (const auto& info : analyzer.getOpponentBlankedOpenTwoMoves()) scored_moves.push_back({ info.move, P_SCORE_BLOCK_BLANKED_OPEN_TWO }); // 빠진 부분 추가
        for (const auto& info : analyzer.getAIBlankedOpenTwoMoves())    scored_moves.push_back({ info.move, P_SCORE_MAKE_BLANKED_OPEN_TWO });
        for (const auto& info : analyzer.getOpponentCloseTwoMoves())    scored_moves.push_back({ info.move, P_SCORE_BLOCK_CLOSED_TWO });
        for (const auto& info : analyzer.getAICloseTwoMoves())    scored_moves.push_back({ info.move, P_SCORE_MAKE_CLOSED_TWO });
        for (const auto& info : analyzer.getAIDoubleBlankedOpenTwoMoves())    scored_moves.push_back({ info.move, P_SCORE_MAKE_DOUBLE_BLANKED_OPEN_TWO });
        for (const auto& info : analyzer.getAIDoubleBlankedCloseTwoMoves())    scored_moves.push_back({ info.move, P_SCORE_MAKE_DOUBLE_BLANKED_CLOSE_TWO });

        sort(scored_moves.begin(), scored_moves.end(), [](const auto& a, const auto& b) {
            return a.score > b.score;
            });

        // 3. 정렬된 결과에서 Move만 추출하여 최종 후보군 생성
        std::unordered_set<Move_2024180014> added_moves;
        for (const auto& scored_move : scored_moves) {
            if (added_moves.find(scored_move.move) == added_moves.end()) {
                candidate_moves.push_back(scored_move.move);
                added_moves.insert(scored_move.move);
            }
        }
    }
    
    if (player_to_move == StoneType_2024180014::BLACK_2024180014) {
        const auto& forbidden_infos = analyzer.getAIForbiddenSpot();
        if (!forbidden_infos.empty()) {
            const auto& forbidden_spots = extract_moves_2024180014(forbidden_infos);
            candidate_moves.erase(
                std::remove_if(candidate_moves.begin(), candidate_moves.end(),
                    [&](const Move_2024180014& m) {
                        return std::find(forbidden_spots.begin(), forbidden_spots.end(), m) != forbidden_spots.end();
                    }),
                candidate_moves.end());
        }
    }

    // 6. 폴백(Fallback) 전략: 위에서 유의미한 후보 수를 하나도 찾지 못했다면
    if (candidate_moves.empty()) {
        //cout << "[DEBUG] >> Fallback: Using Neighborhood Heuristic." << endl;
        candidate_moves = generate_neighborhood_moves_2024180014(current_board);
        if (candidate_moves.empty() && current_board.isEmpty()) {
            candidate_moves.push_back(Move_2024180014(Board_2024180014::SIZE / 2, Board_2024180014::SIZE / 2));
        }
    }

    // 7. 최종 후보 목록으로 자식 노드를 생성
    createChildNodesFromMoves_2024180014(candidate_moves);
}
void Node_2024180014::createChildNodesFromMoves_2024180014(vector<Move_2024180014>& moves) {
    // 기존 자식 노드 정리
    for (Node_2024180014* child : this->children) { delete child; }
    this->children.clear();

    // 중복 제거
    if (moves.size() > 1) {
        std::sort(moves.begin(), moves.end());
        moves.erase(std::unique(moves.begin(), moves.end()), moves.end());
    }

    // 자식 노드 생성
    for (const Move_2024180014& m : moves) {
        this->children.push_back(new Node_2024180014(this, m));
    }
}
vector<Move_2024180014> Node_2024180014::extract_moves_2024180014(const vector<PatternInfo_2024180014>& infos) {
    std::vector<Move_2024180014> moves;
    moves.reserve(infos.size());
    for (const auto& info : infos) {
        moves.push_back(info.move);
    }
    return moves;
}

// PatternAnalyzer 멤버 함수
void PatternAnalyzer_2024180014::analyze_2024180014(const Board_2024180014& board, StoneType_2024180014 ai_player) {
    ai_patterns.clear();
    opponent_patterns.clear();
    StoneType_2024180014 opponent_player = (ai_player == StoneType_2024180014::BLACK_2024180014) ? StoneType_2024180014::WHITE_2024180014 : StoneType_2024180014::BLACK_2024180014;

    for (int r = 0; r < Board_2024180014::SIZE; ++r) {
        for (int c = 0; c < Board_2024180014::SIZE; ++c) {
            if (board.getStone(r, c) == StoneType_2024180014::EMPTY) {
                checkPatternsAfterMove_2024180014(board, r, c, ai_player, ai_patterns);
                checkPatternsAfterMove_2024180014(board, r, c, opponent_player, opponent_patterns);
            }
        }
    }
}
void PatternAnalyzer_2024180014::checkPatternsAfterMove_2024180014(const Board_2024180014& board, int r, int c, StoneType_2024180014 player, PlayerPatterns_2024180014& final_patterns) {

    // --- 1. 이 Move(r,c) 하나만을 위한 임시 패턴 저장소 생성 ---
    PlayerPatterns_2024180014 temp_patterns;

    Board_2024180014 temp_board = board;
    temp_board.placeStone(Move_2024180014(r, c), player);

    const int directions[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };
    const LineType_2024180014 line_types[4] = { LineType_2024180014::HORIZONTAL, LineType_2024180014::VERTICAL, LineType_2024180014::DIAGONAL_MAIN, LineType_2024180014::DIAGONAL_ANTI };

    for (int i = 0; i < 4; ++i) {
        vector<StoneType_2024180014> segment = extractLineSegment_2024180014(temp_board, r, c, directions[i][0], directions[i][1], player);

        findBestPatternInSegment_2024180014(segment, r, c, line_types[i], player, temp_patterns);
    }

    // --- 2. 임시 저장소(temp_patterns)의 내용을 보고 최종 패턴 판단 ---

    if (!temp_patterns.win_moves.empty()) {
        final_patterns.win_moves.push_back({ Move_2024180014(r, c), LineType_2024180014::DONTCARE });
        return;
    }

    int four_count = (int)temp_patterns.open_four_moves.size() +
        (int)temp_patterns.close_four_moves.size() +
        (int)temp_patterns.blanked_four_moves.size();

    int open_three_count = (int)temp_patterns.open_three_moves.size() +
        (int)temp_patterns.blanked_open_three_moves.size() +
        (int)temp_patterns.double_blanked_open_three_moves.size();

    bool is_44 = (four_count >= 2);
    bool is_33 = (open_three_count >= 2);
    bool is_43 = (four_count >= 1 && open_three_count >= 1);

    if (player == StoneType_2024180014::BLACK_2024180014 && (is_44 || is_33)) {
        final_patterns.forbidden_spot.push_back({ Move_2024180014(r, c), LineType_2024180014::DONTCARE });
    }

    if (is_43 || (player == StoneType_2024180014::WHITE_2024180014 && (is_44 || is_33))) {
        final_patterns.four_three_moves.push_back({ Move_2024180014(r, c), LineType_2024180014::DONTCARE });
        return;
    }

    // --- 3. 단일 패턴들을 진짜 final_patterns 객체에 추가 ---
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
StoneType_2024180014 Board_2024180014::checkForWin_2024180014(const Move_2024180014& last_move) const {
    int r = last_move.row;
    int c = last_move.col;
    StoneType_2024180014 stone = this->getStone(r, c);

    if (stone == StoneType_2024180014::EMPTY) {
        return StoneType_2024180014::EMPTY;
    }

    const int dr[] = { 0, 1, 1, 1 };
    const int dc[] = { 1, 0, 1, -1 };

    for (int i = 0; i < 4; ++i) {
        int consecutive_count = 1;

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
            return stone;
        }
    }

    return StoneType_2024180014::EMPTY;
}

void WhiteAttack_2024180014(int* x, int* y)
{
    Board_2024180014 tempBoard = board_2024180014;
    Move_2024180014 location = find_best_move_2024180014(tempBoard, StoneType_2024180014::WHITE_2024180014);

    *y = location.row;
    *x = location.col;

    board_2024180014.placeStone(location, StoneType_2024180014::WHITE_2024180014);
}
void WhiteDefence_2024180014(int x, int y)
{
    Move_2024180014 location(y, x);

    board_2024180014.placeStone(location, StoneType_2024180014::BLACK_2024180014);
}
void BlackAttack_2024180014(int* x, int* y)
{
    Board_2024180014 tempBoard = board_2024180014;
    Move_2024180014 location = find_best_move_2024180014(tempBoard, StoneType_2024180014::BLACK_2024180014);

    *y = location.row;
    *x = location.col;

    board_2024180014.placeStone(location, StoneType_2024180014::BLACK_2024180014);
}
void BlackDefence_2024180014(int x, int y)
{
    Move_2024180014 location(y, x);

    board_2024180014.placeStone(location, StoneType_2024180014::WHITE_2024180014);
}
