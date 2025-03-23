import 'dart:ffi' as ffi;
import 'dart:io';

import 'package:ffi/ffi.dart';
import 'package:reversi/reversi_bindings_generated.dart';

class Coordinates {
  int x = 0;
  int y = 0;
}

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
      coordinates.x = pairArray[i].first;
      coordinates.y = pairArray[i].second;
      list.add(coordinates);
    }
    return list;
  }
}
