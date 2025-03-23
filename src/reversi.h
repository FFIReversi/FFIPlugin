#pragma once
#if _WIN32
#define FFI_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FFI_PLUGIN_EXPORT
#endif
#ifdef __cplusplus
extern "C"{
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
FFI_PLUGIN_EXPORT struct PairArray* getMovableArray(int player, struct IntArray* chessTable);

// 釋放記憶體
FFI_PLUGIN_EXPORT void freePairArray(struct PairArray* pairArray);
#ifdef __cplusplus
}
#endif