# LTspice_M ver. 0.0.4
Library for automatic simulation in LTspice. Includes some usage examples.

## RAWtoCSV
This is a console application that converts .raw files of .dc, .tran, and .ac to .csv.

## auto
This is an example of automatic simulation.<br>
![img](/auto/test.bmp)

## Dependency
- C++ 14
- LTspice 24.1.6

## include
```cpp
#include "LTspice_M.hpp"
```

## Class
- dib_M
  - create .bmp file
- raw_M
  - read .raw file
  - output .csv file
- graph_M
  - create image of .raw data
- LTspice_M
  - create txt(wsring) file
  - .asc -> .net
  - .net -> .raw

## Vertion
| DATA | VERSION | INFO |
| :---: | :---: | :---: |
| 2025-04-29 | 0.0.0 | .raw(.tran)に対応。 |
| 2025-04-30 | 0.0.1 | .rawファイルの扱いをraw_Mクラスに。<br>.raw(.dc)に対応。<br>.raw(.ac)に対応。 |
| 2025-05-02 | 0.0.2 | ファイル書き込みをストリームではなくfs.write()に変更。<br>出力.csvファイルをutf-8に変更。<br>拡張子の自動付与を停止。<br>グラフ描画の補助機能と.bmp出力機能を追加。 |
| 2025-05-03 | 0.0.3 | LTspice_M.makeFile()でのファイル出力先を他メソッドと同じ場所に。 |
| 2025-05-03 | 0.0.4 | コンパイラの構造体最適化を停止するマクロを追加。 |

## License
WTFPL

## Developer
Ryoichi Tsukamoto<br>
contact : r03e24@hachinohe.kosen-ac.jp
