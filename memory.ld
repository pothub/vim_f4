/* メモリ定義
    STM32F405RGT6のように以下のメモリを有することを想定している
    <NAME>        <ORIGIN>    <LENGTH>
    SRAM1         0x20000000  112k
    SRAM2         0x2001C000   16k
    CCM data RAM  0x10000000   64k
    sector0       0x08000000   16k
    sector1       0x08004000   16k
    sector2       0x08008000   16k
    sector3       0x0800C000   16k
    sector4       0x08010000   64k
    sector5       0x08020000  128k
    sector6       0x08040000  128k
    sector7       0x08060000  128k
    sector8       0x08080000  128k
    sector9       0x080A0000  128k
    sector10      0x080C0000  128k
    sector11      0x080E0000  128k

  RAM         static用RAM領域 + main用RAM領域 + ヒープ領域 + スタック領域
  FLASHst     staticプログラム用FLASH領域
  FLASH       mainプログラム用FLASH領域
  VECTOR      割り込みベクタテーブル用FLASH領域
  MAZE        迷路データ保存用FLASH領域
  PARAMETER   パラメータ保存用FLASH領域
  FLOG        FLASHへのログデータ用FLASH領域
  RLOG_FLASH  RAMログをROMに保存する領域
*/

MEMORY
{
  RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 128k /* SRAM */
  RLOG(rwx): ORIGIN = 0x10000000, LENGTH =  64k /* CCM data RAM */
  VECTOR         : ORIGIN = 0x08000000, LENGTH =  16k /* sector0 */
  MAZE(r)        : ORIGIN = 0x08004000, LENGTH =  16k /* sector1 */
  PARAMETER(r)   : ORIGIN = 0x08008000, LENGTH =  16k /* sector2 */
  FLASH(rx)      : ORIGIN = 0x0800C000, LENGTH =  16k + 64k + 128k /* sector3 + 4 + 5 */
  FLASHst(rx)    : ORIGIN = 0x08040000, LENGTH = 128k /* sector6 */
  RLOG_FLASH1(r) : ORIGIN = 0x08060000, LENGTH = 128k /* sector7 */
  RLOG_FLASH2(r) : ORIGIN = 0x08080000, LENGTH = 128k /* sector8 */
  RLOG_FLASH3(r) : ORIGIN = 0x080A0000, LENGTH = 128k /* sector9 */
  FLOG1(r)       : ORIGIN = 0x080C0000, LENGTH = 128k /* sector10 */
  FLOG2(r)       : ORIGIN = 0x080E0000, LENGTH = 128k /* sector11 */
}

/* ヒープとスタック用に最低限確保しておく容量 */
/* この容量を下回るとリンクエラーとなる */
_Minimum_Stack_Size = 6k;

/* スタック終端位置 = RAM終端位置*/
/* スタートアップルーチン内でスタックポインタにこの値がセットさせる */
_RAM_SIZE = 128k;
_estack = 0x20000000 + _RAM_SIZE;

/* 下記変数の値はソース中で
   extern unsigned int _FLASH_LOG_START;
   unsigned int address = (unsigned int)&_FLASH_LOG_START;
   などとすれば利用できる．
*/
_FLASH_MAZE_START = ORIGIN(MAZE);
_FLASH_MAZE_SIZE  = LENGTH(MAZE);
_FLASH_PRM_START = ORIGIN(PARAMETER);
_FLASH_PRM_SIZE  = LENGTH(PARAMETER);
_RLOG_FLASH1_START = ORIGIN(RLOG_FLASH1);
_RLOG_FLASH1_SIZE  = LENGTH(RLOG_FLASH1);
_RLOG_FLASH2_START = ORIGIN(RLOG_FLASH2);
_RLOG_FLASH2_SIZE  = LENGTH(RLOG_FLASH2);
_RLOG_FLASH3_START = ORIGIN(RLOG_FLASH3);
_RLOG_FLASH3_SIZE  = LENGTH(RLOG_FLASH3);
_FLOG1_START = ORIGIN(FLOG1);
_FLOG1_SIZE  = LENGTH(FLOG1);
_FLOG2_START = ORIGIN(FLOG2);
_FLOG2_SIZE  = LENGTH(FLOG2);
_FLASH_MAZE_SECTOR  = 0x0008; /* sector1 */
_FLASH_PRM_SECTOR   = 0x0010; /* sector2 */
_RLOG_FLASH1_SECTOR = 0x0038; /* sector7 */
_RLOG_FLASH2_SECTOR = 0x0040; /* sector8 */
_RLOG_FLASH3_SECTOR = 0x0048; /* sector9 */
_FLOG1_SECTOR       = 0x0050; /* sector10 */
_FLOG2_SECTOR       = 0x0058; /* sector11 */

