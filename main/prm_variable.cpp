#include "prm_interface.h"

// examples
int var1;
int var2;
float P_gain;
float D_gain;
float I_gain;


// flashへのパラメータ書き込みのたびにインクリメントされる特殊変数
int write_count;

#define FLOAT_ELEMENT(x) {#x, &(x), TYPE_FLOAT}
#define INT_ELEMENT(x) {#x, &(x), TYPE_INT}
const struct st_prm prm_array[]={
	INT_ELEMENT(write_count),
	
	INT_ELEMENT(var1),
	INT_ELEMENT(var2),
	FLOAT_ELEMENT(P_gain),
	FLOAT_ELEMENT(D_gain),
	FLOAT_ELEMENT(I_gain)
};
#undef FLOAT_ELEMENT
#undef INT_ELEMENT

// パラメータ配列アドレスとパラメータ数の初期化
void init_prm()
{
	prm = prm_array;
	n_prm = sizeof(prm_array) / sizeof(st_prm);
}

