#pragma once
#if _WIN32
#define FFI_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FFI_PLUGIN_EXPORT
#endif
#ifdef __cplusplus
extern "C" {
#endif

struct IntArray {
    int *array;
    int size;
};

struct PairStruct {
    int first;
    int second;
};

struct PairArray {
    struct PairStruct *array;
    int size;
};

enum Player {
    None,
    Black,
    White
};

// 取得可以下的位置
FFI_PLUGIN_EXPORT struct PairArray *getMovableArray(int player, struct IntArray *chessTable);

// 落子
FFI_PLUGIN_EXPORT struct IntArray *makeMove(int player, struct IntArray *chessTable, struct PairStruct *dropPoint);

// 使用Random的AI (Level 1)
FFI_PLUGIN_EXPORT struct IntArray *aiRandom(int player, struct IntArray *chessTable);

// 使用貪婪演算法的AI (Level 2)
FFI_PLUGIN_EXPORT struct IntArray *aiGreedy(int player, struct IntArray *chessTable);

// 使用貪婪+位置權重+吃子數量的AI (Level 3)
FFI_PLUGIN_EXPORT struct IntArray *aiGreedyAlphaBeta(int player, struct IntArray *chessTable);

// 釋放記憶體
FFI_PLUGIN_EXPORT void freePairArray(struct PairArray *pairArray);

FFI_PLUGIN_EXPORT void freeIntArray(struct IntArray *intArray);
#ifdef __cplusplus
}
#endif
