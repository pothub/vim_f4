#ifndef PRM_INTERFACE_H
#define PRM_INTERFACE_H

const int TYPE_FLOAT=1;
const int TYPE_INT=2;
////////////////////////////////////////////////////////////////////////////////
// 変数名最大文字数
const int N_NAME_MAX=28; // 4の倍数でなければならない
////////////////////////////////////////////////////////////////////////////////
struct st_prm
{
	char name[N_NAME_MAX];
	void *addr;
	int type;
};
extern const struct st_prm *prm; // 初期化が必要
extern int n_prm; // 初期化が必要

extern void load_prms();
extern int save_prms();
extern void print_prms();
extern int edit_prms();

#endif // PRM_INTERFACE_H

