import 'dart:ffi' as ffi;
import 'dart:io';

import 'package:ffi/ffi.dart';
import 'package:reversi/reversi_bindings_generated.dart';

class Coordinates {
  int x = 0;
  int y = 0;
}

// 取得可以落子的地方
List<Coordinates> getMovableArray(int player, List<int> chessTable) {
  ffi.Pointer<IntArray> chessTableOfPointer = chessTable.toIntArrayPointer();
  ffi.Pointer<PairArray> resOfPointer = _bindings.getMovableArray(
    player,
    chessTableOfPointer,
  );
  List<Coordinates> res = resOfPointer.toListOfCoordinates();
  chessTableOfPointer.release();
  _bindings.freePairArray(resOfPointer);
  return res;
}

// 取得可以吃的棋
List<Coordinates> getAllCanFlipped(
    int player, List<int> chessTable, Coordinates findPoint) {
  ffi.Pointer<IntArray> chessTableOfPointer = chessTable.toIntArrayPointer();
  ffi.Pointer<PairStruct> findPointOfPointer = findPoint.toPairStruct();
  ffi.Pointer<PairArray> resOfPointer = _bindings.getAllCanFlipped(
      player, chessTableOfPointer, findPointOfPointer);
  List<Coordinates> res = resOfPointer.toListOfCoordinates();
  chessTableOfPointer.release();
  _bindings.freePairArray(resOfPointer);
  calloc.free(findPointOfPointer);
  return res;
}

// 落子，並回傳棋盤
List<int> makeMove(int player, List<int> chessTable, Coordinates dropPoint) {
  ffi.Pointer<IntArray> chessTableOfPointer = chessTable.toIntArrayPointer();
  ffi.Pointer<PairStruct> dropPointOfPointer = dropPoint.toPairStruct();
  ffi.Pointer<IntArray> newChessTable =
      _bindings.makeMove(player, chessTableOfPointer, dropPointOfPointer);

  var newChessTableList = List<int>.empty(growable: true);
  for (int i = 0; i < newChessTable.ref.size; i++) {
    newChessTableList.add(newChessTable.ref.array[i]);
  }

  _bindings.freeIntArray(newChessTable);
  chessTableOfPointer.release();
  calloc.free(dropPointOfPointer);
  return newChessTableList;
}

// 使用Random的AI (Level 1)，Player是AI，回傳棋盤
List<int> aiRandom(int player, List<int> chessTable) {
  ffi.Pointer<IntArray> chessTableOfPointer = chessTable.toIntArrayPointer();
  ffi.Pointer<IntArray> newChessTable =
      _bindings.aiRandom(player, chessTableOfPointer);

  var newChessTableList = List<int>.empty(growable: true);
  for (int i = 0; i < newChessTable.ref.size; i++) {
    newChessTableList.add(newChessTable.ref.array[i]);
  }

  _bindings.freeIntArray(newChessTable);
  chessTableOfPointer.release();
  return newChessTableList;
}

// 使用貪婪的AI (Level 2)，Player是AI，回傳棋盤
List<int> aiGreedy(int player, List<int> chessTable) {
  ffi.Pointer<IntArray> chessTableOfPointer = chessTable.toIntArrayPointer();
  ffi.Pointer<IntArray> newChessTable =
      _bindings.aiGreedy(player, chessTableOfPointer);

  var newChessTableList = List<int>.empty(growable: true);
  for (int i = 0; i < newChessTable.ref.size; i++) {
    newChessTableList.add(newChessTable.ref.array[i]);
  }

  _bindings.freeIntArray(newChessTable);
  chessTableOfPointer.release();
  return newChessTableList;
}

// 使用貪婪+位置權重+吃子數量的AI (Level 3)，Player是AI，回傳棋盤
List<int> aiGreedyAlphaBeta(int player, List<int> chessTable) {
  ffi.Pointer<IntArray> chessTableOfPointer = chessTable.toIntArrayPointer();
  ffi.Pointer<IntArray> newChessTable =
      _bindings.aiGreedyAlphaBeta(player, chessTableOfPointer);

  var newChessTableList = List<int>.empty(growable: true);
  for (int i = 0; i < newChessTable.ref.size; i++) {
    newChessTableList.add(newChessTable.ref.array[i]);
  }

  _bindings.freeIntArray(newChessTable);
  chessTableOfPointer.release();
  return newChessTableList;
}

const String _libName = 'reversi';

/// The dynamic library in which the symbols for [FfiPluginBindings] can be found.
final ffi.DynamicLibrary _dylib = () {
  if (Platform.isMacOS || Platform.isIOS) {
    return ffi.DynamicLibrary.open('$_libName.framework/$_libName');
  }
  if (Platform.isAndroid || Platform.isLinux) {
    return ffi.DynamicLibrary.open('lib$_libName.so');
  }
  if (Platform.isWindows) {
    return ffi.DynamicLibrary.open('$_libName.dll');
  }
  throw UnsupportedError('Unknown platform: ${Platform.operatingSystem}');
}();

/// The bindings to the native functions in [_dylib].
final ReversiBindings _bindings = ReversiBindings(_dylib);

extension ToIntArrayPointer on List<int> {
  ffi.Pointer<IntArray> toIntArrayPointer() {
    final array = calloc<ffi.Int>(length);
    for (var i = 0; i < length; i++) {
      array[i] = this[i];
    }

    final intArray = calloc<IntArray>();
    intArray.ref
      ..array = array
      ..size = length;

    return intArray;
  }
}

extension ReleasePointer on ffi.Pointer<IntArray> {
  void release() {
    calloc.free(ref.array);
    calloc.free(this);
  }
}

extension ToListOfCoordinates on ffi.Pointer<PairArray> {
  List<Coordinates> toListOfCoordinates() {
    final list = <Coordinates>[];
    ffi.Pointer<PairStruct> pairArray = ref.array;
    for (int i = 0; i < ref.size; i++) {
      Coordinates coordinates = Coordinates();
      coordinates.y = pairArray[i].first;
      coordinates.x = pairArray[i].second;
      list.add(coordinates);
    }
    return list;
  }
}

extension ToPairStruct on Coordinates {
  ffi.Pointer<PairStruct> toPairStruct() {
    final pairStruct = calloc<PairStruct>();
    pairStruct.ref
      ..first = y
      ..second = x;
    return pairStruct;
  }
}
