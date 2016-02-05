#include <stm32f4xx.h>

#define TRX_BUFFER_SIZE 64 // 送受信バッファサイズ
struct {
    unsigned int head, tail;
    char buff[TRX_BUFFER_SIZE];
} txb,rxb; // FIFOバッファ

DMA_InitTypeDef DMA_InitStructure_SIO_TX;

////////////////////////////////////////////////////////////////////////////////
/*
使用するペリフェラル
- USART1
- DMAC2-7 (TX)
- DMAC2-5 (RX)
使用するポート
- PA9 (USART1_TX)
- PA10 (USART1_RX)
*///////////////////////////////////////////////////////////////////////////////
#define DMAx_StreamY_SIO_TX        DMA2_Stream7
#define DMAx_StreamY_SIO_RX        DMA2_Stream5
#define DMA_Channel_x_SIO_TX       DMA_Channel_4
#define DMA_Channel_x_SIO_RX       DMA_Channel_4
void USART1_Conf(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	// Tx
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// Rx
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USARTの設定
	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	// USART用DMAの初期化
	// 送信用
	DMA_StructInit(&DMA_InitStructure_SIO_TX);
	DMA_InitStructure_SIO_TX.DMA_Channel = DMA_Channel_x_SIO_TX;
	DMA_InitStructure_SIO_TX.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->DR));
	DMA_InitStructure_SIO_TX.DMA_Memory0BaseAddr = (uint32_t)txb.buff;
	DMA_InitStructure_SIO_TX.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure_SIO_TX.DMA_BufferSize = 0;
	DMA_InitStructure_SIO_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure_SIO_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure_SIO_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure_SIO_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure_SIO_TX.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure_SIO_TX.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure_SIO_TX.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure_SIO_TX.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure_SIO_TX.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMAx_StreamY_SIO_TX, &DMA_InitStructure_SIO_TX);

	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/** @brief 送信バッファに一文字追加する
@param[in] c 追加する文字
@return なし
*///////////////////////////////////////////////////////////////////////////////
//
void outbyte(char c)
{
// headに新しく追加する
// tailは次に送信されるデータを指す
// バッファに空きが無い（headがtailに追いついた）場合は待機する

	// バッファ内データ数をカウントし，空きがない場合待機する
	// バッファフルで待機しているときには割り込みを許可するためにwhileループになっている
	volatile int remain;
	while(1){
		// この関数は多重に実行されるとまずいので割り込みを禁止する
		__disable_irq();

		// DMACを一時的に停止
		DMA_Cmd(DMAx_StreamY_SIO_TX, DISABLE);
		while(DMA_GetCmdStatus(DMAx_StreamY_SIO_TX) != DISABLE);
		
		// 転送データ残数を取得
		remain = DMA_GetCurrDataCounter(DMAx_StreamY_SIO_TX);

		// バッファに空きがあればループから抜ける
		if(remain != TRX_BUFFER_SIZE) break;
		
		// DMAC動作再開
		DMA_DeInit(DMAx_StreamY_SIO_TX);
		DMA_InitStructure_SIO_TX.DMA_Memory0BaseAddr = (uint32_t)txb.buff;
		DMA_InitStructure_SIO_TX.DMA_BufferSize = TRX_BUFFER_SIZE;
		DMA_Init(DMAx_StreamY_SIO_TX, &DMA_InitStructure_SIO_TX);
		DMA_Cmd(DMAx_StreamY_SIO_TX, ENABLE);

		// 割り込み許可
		__enable_irq();

		// バッファに空きができるまで待機（この間割り込みが発生してもよい）
		while(DMA_GetCurrDataCounter(DMAx_StreamY_SIO_TX) == TRX_BUFFER_SIZE);
	}

	// ここの時点でDMACは停止，割り込みは禁止されている

	// 次に書き込もうとする領域がバッファ終端なら前に詰める
	if(txb.head == TRX_BUFFER_SIZE){
		int blank = TRX_BUFFER_SIZE - remain; // バッファの空き数
		int i;
		for(i=0;i+blank<TRX_BUFFER_SIZE;i++){
			txb.buff[i] = txb.buff[i+blank];
		}
		txb.head = i;
	}
	
	// 1文字追加
	txb.buff[txb.head++] = c;
	remain++;
	txb.tail = txb.head - remain;
	
	// DMACの再設定
	DMA_DeInit(DMAx_StreamY_SIO_TX);
	DMA_InitStructure_SIO_TX.DMA_Memory0BaseAddr = (uint32_t)(&txb.buff[txb.tail]);
	DMA_InitStructure_SIO_TX.DMA_BufferSize = remain;
	DMA_Init(DMAx_StreamY_SIO_TX, &DMA_InitStructure_SIO_TX);
	
	// DMAC動作再開
	DMA_Cmd(DMAx_StreamY_SIO_TX, ENABLE);
	
	// 割り込み許可
	__enable_irq();
}

////////////////////////////////////////////////////////////////////////////////
/** @brief 受信バッファから一文字取得する
@retval 読みだした文字（受信バッファに入力があるとき）
@return ヌル文字（受信バッファが空のとき）
*///////////////////////////////////////////////////////////////////////////////
volatile char inkey()
{
// head（DMACが受信データを書き込む位置）に新しく受信データが蓄積される．
// tailから読み出せばいい
// headがtailに追いつくとバッファオーバーフローとなり正しく読み出せない

	// この関数は多重に実行されるとまずいので割り込みを禁止する
	__disable_irq();
	
	// headの位置を読み取る
	rxb.head = TRX_BUFFER_SIZE - DMA_GetCurrDataCounter(DMAx_StreamY_SIO_RX);
	
	// バッファが空ならヌル文字を返す
	if(rxb.head == rxb.tail){
		// 割り込み許可
		__enable_irq();

		return '\0';
	}
	
	// バッファから一文字読み取り返す
	char c;
	c = rxb.buff[rxb.tail++];
	if(rxb.tail == TRX_BUFFER_SIZE) rxb.tail = 0;
	
	// 割り込み許可
	__enable_irq();
	
	return c;
}

////////////////////////////////////////////////////////////////////////////////
/** @brief 受信バッファを空にする
@return なし
*///////////////////////////////////////////////////////////////////////////////
void clear_rxb()
{
	while(inkey()!=0);
}

////////////////////////////////////////////////////////////////////////////////
/** @brief 受信バッファから一文字取得する（入力があるまで待つ）
@return 取得した文字
*///////////////////////////////////////////////////////////////////////////////
char inbyte()
{
	volatile char c;
	while((c = inkey()) == '\0');
	return c;
}

