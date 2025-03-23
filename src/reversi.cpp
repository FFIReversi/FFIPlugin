#include "reversi.h"
#include <vector>
#include <iostream>
#include <set>

using namespace std;


// 8個方向
int dx[8] = {1, -1, 0, 0, 1, 1, -1, -1};
int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};

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

FFI_PLUGIN_EXPORT struct IntArray *makeMove(int player, struct IntArray *chessTable, struct PairStruct* dropPoint) {
    int x = dropPoint->first;
    int y = dropPoint->second;
    if (chessTable->size != 64 || x>7 || y>7 || x<0 || y<0) {
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
        for (int j = 0; j < 8; j++) {
            row.emplace_back(chessTable->array[8 * i + j]);
        }
        chessTableVector.emplace_back(row);
    }

    set<pair<int, int> > theChessCanFlip;
    // 取得被吃的棋子
    for (int i = 0; i < 8; i++) {
        vector<pair<int,int> > flipChess =  getFlipChess(player, chessTableVector, make_pair(dropPoint->first, dropPoint->second), dx[i], dy[i]);
        for (auto it:flipChess) {
            theChessCanFlip.insert(it);
        }
    }

    for (auto it:theChessCanFlip) {
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

FFI_PLUGIN_EXPORT void freePairArray(PairArray *pairArray) {
    delete[] pairArray->array;
    delete[] pairArray;
}

FFI_PLUGIN_EXPORT void freeIntArray(IntArray *IntArray) {
    delete[] IntArray->array;
    delete[] IntArray;
}

int main() {
    int chessTable[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 2, 1, 0, 0, 0,
        0, 0, 0, 1, 2, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    IntArray ia;
    ia.array = chessTable;
    ia.size = 64;
    PairStruct pos = {2, 3};
    IntArray *res = makeMove(1, &ia, &pos);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << res->array[i * 8 + j];
        }
        cout << endl;
    }
    freeIntArray(res);
}
