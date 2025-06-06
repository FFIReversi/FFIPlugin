#include "reversi.h"
#include <vector>
#include <iostream>
#include <set>

using namespace std;


// 8個方向
int dx[8] = {1, -1, 0, 0, 1, 1, -1, -1};
int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};

// 確認該點是否可落子
FFI_PLUGIN_EXPORT bool isMovable(int player, vector<vector<int> > chessTable, pair<int, int> position, int dx, int dy) {
    int y = position.first;
    int x = position.second;
    int opponent = player == Black ? White : Black;

    if (y + dy < 0 || y + dy >= 8) return false;
    if (x + dx < 0 || x + dx >= 8) return false;
    if (chessTable[y][x] != None) return false;

    if (chessTable[y + dy][x + dx] != opponent) return false;

    while (x + dx < 8 && y + dy < 8 && x + dx >= 0 && y + dy >= 0) {
        x += dx;
        y += dy;
        if (chessTable[y][x] == None) return false;
        if (chessTable[y][x] == player) return true;
    }
    return false;
}

// 取得可以翻的棋子
FFI_PLUGIN_EXPORT vector<pair<int, int> > getFlipChess(int player, vector<vector<int> > chessTable,
                                                       pair<int, int> position, int dx, int dy) {
    int y = position.first;
    int x = position.second;
    int opponent = player == Black ? White : Black;

    vector<pair<int, int> > theChessCanFlip;

    if (y + dy < 0 || y + dy >= 8) return theChessCanFlip;
    if (x + dx < 0 || x + dx >= 8) return theChessCanFlip;
    if (chessTable[y][x] != None) return theChessCanFlip;

    if (chessTable[y + dy][x + dx] != opponent) return theChessCanFlip;

    bool isValid = false;
    while (x + dx < 8 && y + dy < 8 && x + dx >= 0 && y + dy >= 0) {
        x += dx;
        y += dy;
        theChessCanFlip.emplace_back(make_pair(y, x));
        if (chessTable[y][x] == None) {
            theChessCanFlip.clear();
            return theChessCanFlip;
        }
        if (chessTable[y][x] == player) {
            isValid = true;
            break;
        }
    }
    if (!isValid) {
        theChessCanFlip.clear();
    }
    return theChessCanFlip;
}

// 取得所有可以下的點
FFI_PLUGIN_EXPORT PairArray *getMovableArray(int player, IntArray *chessTable) {
    if (chessTable->size != 64) {
        auto *result = new PairArray;
        result->array = new PairStruct[0];
        result->size = 0;
        return result;
    }

    // 棋盤vector
    vector<vector<int> > chessTableVector;
    // 立體化陣列
    for (int i = 0; i < 8; i++) {
        vector<int> row;
        row.reserve(8);
        for (int j = 0; j < 8; j++) {
            row.emplace_back(chessTable->array[8 * i + j]);
        }
        chessTableVector.emplace_back(row);
    }


    vector<pair<int, int> > moves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
                if (isMovable(player, chessTableVector, pair<int, int>(i, j), dx[k], dy[k])) {
                    moves.emplace_back(make_pair(i, j));
                    break;
                }
            }
        }
    }

    // 扁平化陣列
    auto *result = new PairArray;
    result->array = new PairStruct[moves.size()];
    result->size = static_cast<int>(moves.size());
    for (int i = 0; i < moves.size(); i++) {
        result->array[i].first = moves[i].first;
        result->array[i].second = moves[i].second;
    }
    return result;
}

// 可以吃的棋
FFI_PLUGIN_EXPORT struct PairArray *getAllCanFlipped(int player, struct IntArray *chessTable,
                                                     struct PairStruct *findPoint) {
    if (chessTable->size != 64) {
        auto *result = new PairArray;
        result->array = new PairStruct[0];
        result->size = 0;
        return result;
    }

    // 棋盤vector
    vector<vector<int> > chessTableVector;
    // 立體化陣列
    for (int i = 0; i < 8; i++) {
        vector<int> row;
        row.reserve(8);
        for (int j = 0; j < 8; j++) {
            row.emplace_back(chessTable->array[8 * i + j]);
        }
        chessTableVector.emplace_back(row);
    }

    set<pair<int, int> > theChessCanFlip;
    for (int i = 0; i < 8; i++) {
        vector<pair<int, int> > flipChess = getFlipChess(player, chessTableVector,
                                                         make_pair(findPoint->first, findPoint->second), dx[i], dy[i]);
        for (auto it: flipChess) {
            if (chessTableVector[it.first][it.second] == player) {
                continue;
            }
            theChessCanFlip.insert(it);
        }
    }


    vector<pair<int, int> > flipVector;
    flipVector.reserve(theChessCanFlip.size());
    for (auto it: theChessCanFlip) {
        flipVector.emplace_back(it);
    }

    // 扁平化陣列
    auto *result = new PairArray;
    result->array = new PairStruct[theChessCanFlip.size()];
    result->size = static_cast<int>(theChessCanFlip.size());
    for (int i = 0; i < theChessCanFlip.size(); i++) {
        result->array[i].first = flipVector[i].first;
        result->array[i].second = flipVector[i].second;
    }
    return result;
}

// 落子
FFI_PLUGIN_EXPORT struct IntArray *makeMove(int player, struct IntArray *chessTable, struct PairStruct *dropPoint) {
    int x = dropPoint->first;
    int y = dropPoint->second;
    if (chessTable->size != 64 || x > 7 || y > 7 || x < 0 || y < 0) {
        auto *result = new IntArray;
        result->array = new int[0];
        result->size = 0;
        return result;
    }

    // 棋盤vector
    vector<vector<int> > chessTableVector;
    // 立體化陣列
    for (int i = 0; i < 8; i++) {
        vector<int> row;
        row.reserve(8);
        for (int j = 0; j < 8; j++) {
            row.emplace_back(chessTable->array[8 * i + j]);
        }
        chessTableVector.emplace_back(row);
    }

    set<pair<int, int> > theChessCanFlip;
    // 取得被吃的棋子
    for (int i = 0; i < 8; i++) {
        vector<pair<int, int> > flipChess = getFlipChess(player, chessTableVector,
                                                         make_pair(dropPoint->first, dropPoint->second), dx[i], dy[i]);
        for (auto it: flipChess) {
            theChessCanFlip.insert(it);
        }
    }

    for (auto it: theChessCanFlip) {
        chessTableVector[it.first][it.second] = player;
    }
    if (!theChessCanFlip.empty()) {
        chessTableVector[dropPoint->first][dropPoint->second] = player;
    }

    // 扁平化陣列
    auto *result = new IntArray;
    result->array = new int[64];
    result->size = 64;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            result->array[i * 8 + j] = chessTableVector[i][j];
        }
    }
    return result;
}

// 釋放記憶體
FFI_PLUGIN_EXPORT void freePairArray(PairArray *pairArray) {
    delete[] pairArray->array;
    delete[] pairArray;
}

FFI_PLUGIN_EXPORT void freeIntArray(IntArray *IntArray) {
    delete[] IntArray->array;
    delete[] IntArray;
}

// 利用Random的AI
FFI_PLUGIN_EXPORT struct IntArray *aiRandom(int player, struct IntArray *chessTable) {
    PairArray *allMovableArray = getMovableArray(player, chessTable);
    if (allMovableArray->size == 0) {
        freePairArray(allMovableArray);
        auto *result = new IntArray;
        result->array = new int[64];
        result->size = 64;
        for (int i = 0; i < 64; i++) {
            result->array[i] = chessTable->array[i];
        }
        return result;
    }
    srand(static_cast<unsigned int>(time(NULL)));
    int dropIndex = rand() % allMovableArray->size;
    IntArray *result = makeMove(player, chessTable, &allMovableArray->array[dropIndex]);
    freePairArray(allMovableArray);
    return result;
}

// 單純貪婪的AI
FFI_PLUGIN_EXPORT struct IntArray *aiGreedy(int player, struct IntArray *chessTable) {
    PairArray *allMovableArray = getMovableArray(player, chessTable);

    // 棋盤vector
    vector<vector<int> > chessTableVector;
    // 立體化陣列
    for (int i = 0; i < 8; i++) {
        vector<int> row;
        row.reserve(8);
        for (int j = 0; j < 8; j++) {
            row.emplace_back(chessTable->array[8 * i + j]);
        }
        chessTableVector.emplace_back(row);
    }

    int maxFlipCount = 0;
    pair<int, int> movePoint;
    if (allMovableArray->size == 0) {
        freePairArray(allMovableArray);
        auto *result = new IntArray;
        result->array = new int[64];
        result->size = 64;
        for (int i = 0; i < 64; i++) {
            result->array[i] = chessTable->array[i];
        }
        return result;
    }
    for (int i = 0; i < allMovableArray->size; i++) {
        // 可以下的點
        PairStruct point = allMovableArray->array[i];
        set<pair<int, int> > theChessCanFlip;

        // 取得吃子
        for (int k = 0; k < 8; k++) {
            vector<pair<int, int> > chessCanFlipInThisDirection = getFlipChess(player, chessTableVector,
                                                                               make_pair(point.first, point.second),
                                                                               dx[k], dy[k]);

            for (auto it: chessCanFlipInThisDirection) {
                theChessCanFlip.insert(it);
            }
        }

        if (theChessCanFlip.size() > maxFlipCount) {
            maxFlipCount = static_cast<int>(theChessCanFlip.size());
            movePoint.first = point.first;
            movePoint.second = point.second;
        }
    }
    freePairArray(allMovableArray);
    PairStruct movePointPairStruct = {};
    movePointPairStruct.first = movePoint.first;
    movePointPairStruct.second = movePoint.second;
    return makeMove(player, chessTable, &movePointPairStruct);
}

// 加上 α β 的貪婪的AI
FFI_PLUGIN_EXPORT struct IntArray *aiGreedyAlphaBeta(int player, struct IntArray *chessTable) {
    PairArray *allMovableArray = getMovableArray(player, chessTable);

    // 棋盤vector
    vector<vector<int> > chessTableVector;
    // 立體化陣列
    for (int i = 0; i < 8; i++) {
        vector<int> row;
        row.reserve(8);
        for (int j = 0; j < 8; j++) {
            row.emplace_back(chessTable->array[8 * i + j]);
        }
        chessTableVector.emplace_back(row);
    }

    int chessCount = 0;
    for (auto i: chessTableVector) {
        for (auto j: i) {
            if (j != 0) {
                chessCount++;
            }
        }
    }

    // a = 位置權重α, b = 吃子數β
    int a = 1, b;
    if (chessCount < 20) {
        b = 10;
    } else if (chessCount < 45) {
        b = 15;
    } else {
        b = 25;
    }

    // 位置權重
    int bonusPointOfPosition[8][8] = {
        {100, -20, 10, 5, 5, 10, -20, 100},
        {-20, -50, -2, -2, -2, -2, -50, -20},
        {10, -2, 0, 0, 0, 0, -2, 10},
        {5, -2, 0, 0, 0, 0, -2, 5},
        {5, -2, 0, 0, 0, 0, -2, 5},
        {10, -2, 0, 0, 0, 0, -2, 10},
        {-20, -50, -2, -2, -2, -2, -50, -20},
        {100, -20, 10, 5, 5, 10, -20, 100}
    };

    int maxScore = 0;
    pair<int, int> movePoint;
    if (allMovableArray->size == 0) {
        freePairArray(allMovableArray);
        auto *result = new IntArray;
        result->array = new int[64];
        result->size = 64;
        for (int i = 0; i < 64; i++) {
            result->array[i] = chessTable->array[i];
        }
        return result;
    }
    for (int i = 0; i < allMovableArray->size; i++) {
        // 可以下的點
        PairStruct point = allMovableArray->array[i];
        set<pair<int, int> > theChessCanFlip;

        // 取得吃子
        for (int k = 0; k < 8; k++) {
            vector<pair<int, int> > chessCanFlipInThisDirection = getFlipChess(player, chessTableVector,
                                                                               make_pair(point.first, point.second),
                                                                               dx[k], dy[k]);

            for (auto it: chessCanFlipInThisDirection) {
                theChessCanFlip.insert(it);
            }
        }

        int thisScore = bonusPointOfPosition[point.first][point.second] * a + static_cast<int>(theChessCanFlip.size()) *
                        b;
        if (thisScore > maxScore) {
            maxScore = static_cast<int>(theChessCanFlip.size());
            movePoint.first = point.first;
            movePoint.second = point.second;
        }
    }
    freePairArray(allMovableArray);
    PairStruct movePointPairStruct = {};
    movePointPairStruct.first = movePoint.first;
    movePointPairStruct.second = movePoint.second;
    return makeMove(player, chessTable, &movePointPairStruct);
}

// int main() {
//     int chessTable[64] = {
//         2, 2, 2, 2, 2, 2, 2, 2,
//         2, 2, 1, 1, 1, 2, 2, 2,
//         2, 2, 2, 1, 1, 1, 1, 2,
//         0, 2, 1, 1, 1, 2, 1, 2,
//         2, 2, 1, 1, 1, 2, 1, 2,
//         2, 1, 2, 2, 1, 2, 1, 2,
//         2, 2, 1, 1, 1, 1, 2, 2,
//         2, 2, 2, 2, 1, 2, 2, 2,
//     };
//
//     IntArray ia = {};
//     ia.array = chessTable;
//     ia.size = 64;
//     PairStruct pos = {2, 4};
//     PairArray *res = getMovableArray(1, &ia);
//     // IntArray *res = makeMove(1, &ia, &pos);
//     for (int i = 0; i < res->size; i++) {
//         cout << res->array[i].first << " " << res->array[i].second << endl;
//     }
//     // freeIntArray(res);
//     // IntArray *res = aiGreedyAlphaBeta(2, &ia);
//
//     // for (int i = 0; i < 8; i++) {
//     //     for (int j = 0; j < 8; j++) {
//     //         cout << res->array[i * 8 + j];
//     //     }
//     //     cout << endl;
//     // }
//     freePairArray(res);
// }
