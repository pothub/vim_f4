#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prm_interface.h"
#include "config_clock_flash.h"
#include "usart.h"
#include "adc.h"


// these variables are defined in "memory.ld"
extern unsigned long _FLASH_PRM_START;
extern unsigned long _FLASH_PRM_SIZE;
extern unsigned long _FLASH_PRM_SECTOR;
// パラメータ保存用に使用するFLASHの領域開始アドレスと領域サイズ
const unsigned long FLASH_PRM_START = ((unsigned long)&_FLASH_PRM_START);
const unsigned long FLASH_PRM_SIZE  = ((unsigned long)&_FLASH_PRM_SIZE);
// パラメータ保存用に使用するFLASHのセクター番号
const unsigned FLASH_PRM_SECTOR = ((unsigned long)&_FLASH_PRM_SECTOR);

/// パラメータ構造体へのポインタ．プログラムの始めに初期化される必要がある
const struct st_prm *prm;
/// パラメータの数．プログラムの始めに初期化される必要がある
int n_prm = 0;


void load_prms()
{
	for(unsigned long addr=FLASH_PRM_START; addr<FLASH_PRM_START+FLASH_PRM_SIZE; addr+=sizeof(((st_prm*)0)->name)+sizeof(((st_prm*)0)->addr)){
		char* str = (char*)addr;
		for(int i=0;i<n_prm;i++){
			if(strncmp(str, prm[i].name, N_NAME_MAX) == 0){
				if(prm[i].type == TYPE_FLOAT) *((float*)prm[i].addr) = *((float*)(addr+sizeof(((st_prm*)0)->name)));
				else if(prm[i].type == TYPE_INT) *((int*)prm[i].addr) = *((int*)(addr+sizeof(((st_prm*)0)->name)));
				else printf("Cannot handle the data type for %s",prm[i].name);
				break;
			}
		}
	}
	printf("Loaded parameters from FLASH.\r\n");
}


int save_prms()
{
	stop_periodic_interrupt();

	// romへの書き込み回数をインクリメント
	for(int i=0;i<n_prm;i++){
		if(strncmp(prm[i].name, "write_count", N_NAME_MAX) == 0){
			(*((int*)prm[i].addr))++;
		}
	}

	// romの消去
	erase_flash_sector(FLASH_PRM_SECTOR);
	// 書き込み
	unsigned long addr = FLASH_PRM_START;
	for(int i=0;i<n_prm;i++){
		// ROM領域に空きがあるかチェック
		if(addr + sizeof(((st_prm*)0)->name) + sizeof(((st_prm*)0)->addr) > FLASH_PRM_START + FLASH_PRM_SIZE){
			printf("Struct prm is too large.\r\n");
			printf("Not all parameters were written.\r\n");
			restart_periodic_interrupt();
			return 1;
		}
		// 変数名の書き込み(リトルエンディアンを考慮)
		for(const char *str=prm[i].name; str<prm[i].name + sizeof(((st_prm*)0)->name); str+=4){
			write_4bytes_to_flash(addr, (str[3]<<24)+(str[2]<<16)+(str[1]<<8)+str[0]);
			addr += 4;
		}
		// 変数値の書き込み
		write_4bytes_to_flash(addr, *((long*)(prm[i].addr)));
		addr += 4;
	}
	// 残りの領域を0で埋める．
	for(;addr<FLASH_PRM_START+FLASH_PRM_SIZE;addr+=4){
		write_4bytes_to_flash(addr,0);
	}

	printf("Saved parameters to FLASH.\r\n");

	restart_periodic_interrupt();
	return 0;
}


int verify_prms()
{
	printf("Verifying..."); fflush(stdout);

	unsigned long addr = FLASH_PRM_START;
	for(int i=0;i<n_prm;i++){
		bool okflag = false;
		char* str = (char*)addr;
		if(strncmp(str, prm[i].name, N_NAME_MAX) == 0){
			if(prm[i].type == TYPE_FLOAT){
				if( (*((float*)prm[i].addr)) == (*((float*)(addr+sizeof(((st_prm*)0)->name)))) ) okflag = true;
			}
			else if(prm[i].type == TYPE_INT){
				if( (*((int*)prm[i].addr)) == (*((int*)(addr+sizeof(((st_prm*)0)->name)))) ) okflag = true;
			}
			else{
				printf("Cannot handle the data type for %s",prm[i].name);
				addr += sizeof(((st_prm*)0)->name)+sizeof(((st_prm*)0)->addr);
				continue;
			}
		}

		if(okflag == false){
			printf("error.\r\n");
			printf("name: %s\r\n", prm[i].name);
			return 1;
		}

		addr += sizeof(((st_prm*)0)->name)+sizeof(((st_prm*)0)->addr);
	}

	printf("ok.\r\n");
	return 0;
}


const static int n_column = 1;
void print_prms()
{
	printf("----------------------------- parameters -----------------------------------\r\n");
	const int n = (n_prm + n_column -1) / n_column; // 行数の計算
	for(int i=0;i<n;i++){ // n_column列で出力
		for(int j=0;j<n_column;j++){
			if(i+n*j < n_prm){
				if(prm[i+n*j].type == TYPE_FLOAT){
					printf("%-12s %11.5G",prm[i+n*j].name, *((float*)prm[i+n*j].addr));
				}
				else if(prm[i+n*j].type == TYPE_INT){
					printf("%-12s %11d",prm[i+n*j].name, *((int*)prm[i+n*j].addr));
				}
				else{
					printf("%-12s %11s",prm[i+n*j].name, "TYPE?");
				}
				if(j == n_column-1) printf("\r\n");
				else printf("  ");
			}
			else{
				printf("\r\n");
			}
		}
	}
	printf("----------------------------------------------------------------------------\r\n");
}


const char ESC_del_after[]  = {0x1b,'[','K'};
void put_ESC_move_right(int n)
{
	if(n < 1) return;
	printf("%c[%dC",0x1b,n);
	fflush(stdout);
}
void put_ESC_move_left(int n)
{
	if(n < 1) return;
	printf("%c[%dD",0x1b,n);
	fflush(stdout);
}

void insert_char(char *s, int *cur, char c)
{
	int end;
	for(end=0;s[end]!='\0';end++); // 文字列終端位置を探す
	for(int i=end;i>=*cur;i--) s[i+1] = s[i]; // 一文字空ける
	s[(*cur)++] = c; // 一文字挿入，カーソル位置進める
	outbyte(c); // 挿入した文字を出力
	printf("%s",&s[*cur]); // 挿入位置以降の文字列を改めて出力する
	put_ESC_move_left(strlen(&s[*cur])); // カーソル位置復帰
	fflush(stdout);
}
void delete_char(char *s, int *cur)
{
	if(s[*cur] == '\0') return; // 行末では何もしない
	for(int i=*cur;s[i]!='\0';i++) s[i] = s[i+1]; // 1文字詰める
	printf("%s",&s[*cur]); printf("%s",ESC_del_after); // 文字列を改めて出力し，最後の一文字を消去
	put_ESC_move_left(strlen(&s[*cur])); // カーソル位置復帰
	fflush(stdout);
}

void show_help_edit_prm()
{
	printf("usage1: <parameter name> <new value>\r\n");
	printf("usage2: <command>\r\n");
	printf("    ls  print parameter list\r\n");
	printf("  save  save parameters and exit\r\n");
	printf("  quit  exit without saving\r\n");
	printf("  load  reload parameters from flash\r\n");
}

int edit_prms()
{
	const int nb_max = 3; // historyバッファの個数
	const int c_max = N_NAME_MAX + 11 + 1; // 一行の文字数（名前+値+'\0'）
	char str[c_max];
	char buff[nb_max][c_max];
	int loc_buff;
	for(int i=0;i<nb_max;i++) buff[i][0] = '\0';
	int cur; // カーソル位置を示す

	// 説明の表示
	printf("---- parameter edit mode ---------------------------------------------------\r\n");
	show_help_edit_prm();
	print_prms();
	// 受信バッファをクリア
	clear_rxb();
	while(1){
		if(buff[0][0] != '\0'){
			// バッファをシフトする
			for(int i=nb_max-1;i>0;i--){
				strncpy(buff[i], buff[i-1], c_max);
			}
		}
		buff[0][0] = '\0'; // 現在の入力のバッファをクリア
		str[0] = '\0';
		loc_buff = 0; // バッファ番号を0にクリア
		cur = 0;
		printf("parameter>"); fflush(stdout);
		while(1){
			// 文字を受け取る
			char c = inbyte();
			// 'ENTER'のとき
			if(c == '\r' || c == '\n'){ // [ENTER]
				printf("\r\n");
				// 一致する変数名を探し値を更新する
				// 第一引数と第二引数の開始位置および長さを取得する
				int p1, p2, l1, l2;
				int ii=0;
				for(;str[ii]==' ';ii++); // スペースのスキップ
				p1 = ii; // 第一引数開始位置
				for(;str[ii]!=' ' && str[ii]!='\0';ii++); // スペースもしくは文字列終端が来るまでスキップ
				l1 = ii - p1; // 第一引数の長さ
				if(l1 == 0){ // 入力なしの場合
					show_help_edit_prm();
					print_prms();
					break;
				}
				for(;str[ii]==' ';ii++); // スペースのスキップ
				p2 = ii; // 第二引数開始位置
				for(;str[ii]!=' ' && str[ii]!='\0';ii++); // スペースもしくは文字列終端が来るまでスキップ
				l2 = ii - p2; // 第二引数の長さ
				if(l2 == 0){ // 第二引数がないときはコマンド
					if(l1 == 2 && strncmp(&str[p1], "ls", 2) == 0){ // リスト表示する
						print_prms();
					}
					else if(l1 == 4 && strncmp(&str[p1], "save", 4) == 0){ // 保存して終了する
						int ret = save_prms();
						if(ret != 0) return ret;
						ret = verify_prms();
						return ret;
					}
					else if(l1 == 4 && strncmp(&str[p1], "quit", 4) == 0){ // 保存せずに終了する
						printf("Parameters were not saved.\r\n");
						return 0;
					}
					else if(l1 == 4 && strncmp(&str[p1], "load", 4) == 0){ // 変数のリロード
						load_prms(); // 変数のリロード
					}
					else{
						printf("Unknown command\r\n");
					}
				}
				else{ // 第二引数があるときはパラメータ変更
					for(int j=0;j<n_prm;j++){
						if(strncmp(&str[p1], prm[j].name, l1) == 0){
							if(prm[j].type == TYPE_FLOAT) *((float*)prm[j].addr) = atof(&str[p2]);
							else if(prm[j].type == TYPE_INT) *((int*)prm[j].addr) = atoi(&str[p2]);
							else printf("Cannot handle the data type for %s",prm[j].name);
							break;
						}
						if(j == n_prm-1) printf("Unknown parameter\r\n");
					}
				}
				strncpy(buff[0], str, c_max); // 現在の入力を最新のバッファに移す
				break; // 次の行へ移行する
			}
			// Ctrl-c のとき
			else if(c == 0x03){ // [Ctrl-c]
				printf("\r\n");
				break; // 次の行へ移行する
			}
			// エスケープシーケンスのとき
			else if(c == 0x1b){ // [ESC]
				c = inbyte(); // 続きを読み込む
				if(c == '['){
					c = inbyte(); // さらに続きを読み込む
					if(c == 'C'){ // 右矢印キー
						if(str[cur] != '\0'){
							cur++; put_ESC_move_right(1); // カーソルを右に移動
						}
					}
					else if(c == 'D'){ // 左矢印キー
						if(cur != 0){
							cur--; put_ESC_move_left(1); // カーソルを左に移動
						}
					}
					else if(c == 'A'){ // 上矢印キー
						if(loc_buff+1 < nb_max){ // バッファ最大個数より小さいとき
							if(buff[loc_buff+1][0] != '\0'){ // 一つ前のバッファが空でないとき
								if(cur != 0) put_ESC_move_left(cur); // カーソルを左端に移動
								printf("%s",ESC_del_after); // 行を削除
								if(loc_buff == 0) strncpy(buff[0], str, c_max);
								strncpy(str, buff[++loc_buff], c_max);
								printf("%s",str); // 新しいバッファの中身を表示
								cur = strlen(str);
								fflush(stdout);
							}
						}
					}
					else if(c == 'B'){ // 下矢印キー
						if(loc_buff > 0){
							if(cur != 0) put_ESC_move_left(cur); // カーソルを左端に移動
							printf("%s",ESC_del_after); // 行を削除
							strncpy(str, buff[--loc_buff], c_max);
							printf("%s",str);
							cur = strlen(str);
							fflush(stdout);
						}
					}
					else if(c == '3'){
						c = inbyte(); // さらに続きを読み込む
						if(c == '~'){ // delキーが押されたときの動作をする
							delete_char(str, &cur);
						}
					}
				}
			}
			// Back Spaceキーが押されたときの動作をする
			//（ただし，端末によって送信コードが異なるため期待通りの動作をしないことがある）
			else if(c == 0x7f || c == '\b'){ // [DEL],[BS]
				if(cur != 0){ // 文字列の左端では何もしない
					cur--; put_ESC_move_left(1); // カーソルを左に移動
					delete_char(str, &cur); // 一文字消去
				}
			}
			// タブキーのとき
			else if(c == '\t' && cur != 0){ // なおかつ何か文字列が入っているとき
				int count = 0; // 補完候補数
				char s[N_NAME_MAX]; // 補完文字列
				for(int i=0;i<n_prm;i++){
					if(strncmp(str, prm[i].name, cur) == 0){ // カーソル位置まで同じ文字列のとき
						count++;
						if(count == 1){ // 一個目の候補のとき
							strncpy(s, &prm[i].name[cur], N_NAME_MAX-1); // カーソル以降の文字列を記録
							s[N_NAME_MAX-1] = '\0'; // 念のためヌル文字を最後に付加する
						}
						else{ // 二個目以降の候補のとき
							int j=0;
							for(;s[j] == prm[i].name[cur+j];j++); // 以前の候補と一致している限り進める
							s[j] = '\0'; // 以前の候補と異なる位置で文字列を切る
						}
					}
				}
				if(count == 1){ // 候補が一つのときは全部補完する
					for(int j=0;s[j]!='\0';j++) insert_char(str, &cur, s[j]);
					insert_char(str, &cur, ' '); // スペースを追加する
				}
				else if(count > 1){ // 候補が複数あるとき
					printf("\r\n");
					int k=0;
					for(int i=0;i<n_prm;i++){ // 候補を列挙する
						if(strncmp(str, prm[i].name, cur) == 0){
							printf("%-26s",prm[i].name);
							k++;
							if((k%3) == 0) printf("\r\n");
						}
					}
					if((k%3) != 0) printf("\r\n");
					printf("parameter>");
					printf(str); fflush(stdout);
					if(strlen(str) != (size_t)cur){
						put_ESC_move_left(strlen(str) - cur);
						fflush(stdout);
					}
					for(int j=0;s[j]!='\0';j++) insert_char(str, &cur, s[j]);
				}
			}
			// その他のとき
			else{
				if(strlen(str) < c_max-1){ // 文字列に余裕があるとき
					// 不正な文字が入らないようチェックする
					// 入力を認めるのは，通常の（制御文字でない）文字だけ
					if(0x20 <= c && c <= 0x7e){
						insert_char(str, &cur, c);
					}
				}
			}
		}
	}
}

