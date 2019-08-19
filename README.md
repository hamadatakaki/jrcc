# Jellyfish Rumble C Compiler
Backend: LLVM

## メモ書き

### STEP5までにおけるLLVM IRの構成

* push/popらしき命令を見つけられなかったのでレジスタマシン的なやつそのもので実装した。
* 具体的には、
  * 数字のNodeを評価するときはメモリ割り当てをする命令を吐き出し、メモリ番号をコンパイラ内で利用するスタックにpushするようにした。
  * 演算子のNodeを評価するときは前述のスタックから上2つの変数のメモリ番号をpopし、その番号を用いて計算するIRを吐き、計算結果を新たなメモリ番号に割り当て、それをpushするようにした。

## 参考文献
* [低レイヤを知りたい人のためのCコンパイラ作成入門 - Rui Ueyama](https://www.sigbus.info/compilerbook)
* [LLVM Language Reference Manual](https://releases.llvm.org/6.0.0/docs/LangRef.html#add-instruction)
* [LLVMについて調べたことまとめ](https://kotetuco.hatenablog.com/entry/2017/12/14/235019)
* [LLVMを始めよう！ 〜 LLVM IRの基礎はclangが教えてくれた・Brainf**kコンパイラを作ってみよう 〜](https://itchyny.hatenablog.com/entry/2017/02/27/100000)
