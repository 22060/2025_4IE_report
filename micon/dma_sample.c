/*
 * SH7080 DMAコントローラーサンプルコード
 * 7080S.Hファイルを参照したDMA実装
 */

#include "7080S.H"

/*
 * DMA初期化関数
 * チャネル: 使用するDMAチャネル (0-3)
 * src: 転送元アドレス
 * dst: 転送先アドレス
 * size: 転送サイズ（バイト数）
 * mode: 転送モード設定
 */
void dma_init(int channel, void *src, void *dst, unsigned int size, unsigned int mode)
{
    volatile struct st_dmac0 *dmac_ch;
    
    /* チャネル選択 */
    switch(channel) {
        case 0: dmac_ch = &DMAC0; break;
        case 1: dmac_ch = &DMAC1; break;
        case 2: dmac_ch = &DMAC2; break;
        case 3: dmac_ch = &DMAC3; break;
        default: return; /* 無効なチャネル */
    }
    
    /* DMAコントローラーを無効化 */
    dmac_ch->CHCR.BIT.DE = 0;
    
    /* 転送元アドレス設定 */
    dmac_ch->SAR = src;
    
    /* 転送先アドレス設定 */
    dmac_ch->DAR = dst;
    
    /* 転送カウント設定 */
    dmac_ch->DMATCR = size;
    
    /* チャネル制御レジスタ設定 */
    dmac_ch->CHCR.LONG = mode;
}

/*
 * DMA転送開始
 */
void dma_start(int channel)
{
    volatile struct st_dmac0 *dmac_ch;
    
    switch(channel) {
        case 0: dmac_ch = &DMAC0; break;
        case 1: dmac_ch = &DMAC1; break;
        case 2: dmac_ch = &DMAC2; break;
        case 3: dmac_ch = &DMAC3; break;
        default: return;
    }
    
    /* DMAマスターイネーブル */
    DMAC.DMAOR.BIT.DME = 1;
    
    /* チャネルイネーブル */
    dmac_ch->CHCR.BIT.DE = 1;
}

/*
 * DMA転送停止
 */
void dma_stop(int channel)
{
    volatile struct st_dmac0 *dmac_ch;
    
    switch(channel) {
        case 0: dmac_ch = &DMAC0; break;
        case 1: dmac_ch = &DMAC1; break;
        case 2: dmac_ch = &DMAC2; break;
        case 3: dmac_ch = &DMAC3; break;
        default: return;
    }
    
    /* チャネル無効化 */
    dmac_ch->CHCR.BIT.DE = 0;
}

/*
 * DMA転送完了チェック
 * 戻り値: 1=転送完了, 0=転送中
 */
int dma_is_complete(int channel)
{
    volatile struct st_dmac0 *dmac_ch;
    
    switch(channel) {
        case 0: dmac_ch = &DMAC0; break;
        case 1: dmac_ch = &DMAC1; break;
        case 2: dmac_ch = &DMAC2; break;
        case 3: dmac_ch = &DMAC3; break;
        default: return -1;
    }
    
    /* 転送終了フラグチェック */
    return dmac_ch->CHCR.BIT.TE;
}

/*
 * メモリ間DMA転送の設定例
 */
#define DMA_MEMORY_TO_MEMORY_MODE  0x00001001  /* メモリ→メモリ転送設定 */
#define DMA_BYTE_TRANSFER         0x00000000  /* バイト転送 */
#define DMA_WORD_TRANSFER         0x00000008  /* ワード転送 */
#define DMA_LONG_TRANSFER         0x00000010  /* ロングワード転送 */

/*
 * メモリ間DMA転送実行関数
 */
void dma_memory_copy(void *src, void *dst, unsigned int size)
{
    unsigned int mode;
    
    /* 転送モード設定 */
    mode = DMA_MEMORY_TO_MEMORY_MODE;
    mode |= DMA_BYTE_TRANSFER;  /* バイト転送 */
    mode |= 0x00000001;         /* 割り込み有効 */
    
    /* DMA初期化 */
    dma_init(0, src, dst, size, mode);
    
    /* 転送開始 */
    dma_start(0);
    
    /* 転送完了待ち */
    while(!dma_is_complete(0)) {
        /* 転送完了まで待機 */
    }
    
    /* 転送停止 */
    dma_stop(0);
}

/*
 * ペリフェラル→メモリDMA転送設定例（SCI受信）
 */
void dma_sci_receive_setup(void *buffer, unsigned int size)
{
    unsigned int mode;
    
    /* SCI→メモリ転送モード設定 */
    mode = 0x00000000;          /* 基本設定 */
    mode |= (0x0C << 8);        /* SCI受信要求 (RS=1100) */
    mode |= (0x01 << 14);       /* 転送先アドレスインクリメント */
    mode |= (0x00 << 12);       /* 転送元アドレス固定 */
    mode |= DMA_BYTE_TRANSFER;  /* バイト転送 */
    mode |= 0x00000001;         /* 割り込み有効 */
    
    /* DMA初期化（SCI0受信データレジスタから） */
    dma_init(1, (void*)&SCI0.SCRDR, buffer, size, mode);
    
    /* 転送開始 */
    dma_start(1);
}

/*
 * メモリ→ペリフェラルDMA転送設定例（SCI送信）
 */
void dma_sci_transmit_setup(void *buffer, unsigned int size)
{
    unsigned int mode;
    
    /* メモリ→SCI転送モード設定 */
    mode = 0x00000000;          /* 基本設定 */
    mode |= (0x08 << 8);        /* SCI送信要求 (RS=1000) */
    mode |= (0x00 << 14);       /* 転送先アドレス固定 */
    mode |= (0x01 << 12);       /* 転送元アドレスインクリメント */
    mode |= DMA_BYTE_TRANSFER;  /* バイト転送 */
    mode |= 0x00000001;         /* 割り込み有効 */
    
    /* DMA初期化（SCI0送信データレジスタへ） */
    dma_init(2, buffer, (void*)&SCI0.SCTDR, size, mode);
    
    /* 転送開始 */
    dma_start(2);
}

/*
 * DMA使用例
 */
void dma_example(void)
{
    unsigned char src_buffer[256];
    unsigned char dst_buffer[256];
    int i;
    
    /* 送信データ準備 */
    for(i = 0; i < 256; i++) {
        src_buffer[i] = i;
    }
    
    /* メモリ間DMA転送実行 */
    dma_memory_copy(src_buffer, dst_buffer, 256);
    
    /* 結果確認 */
    for(i = 0; i < 256; i++) {
        if(src_buffer[i] != dst_buffer[i]) {
            /* 転送エラー */
            break;
        }
    }
}

/*
 * DMAエラー処理関数
 */
void dma_error_handler(void)
{
    /* アドレスエラーチェック */
    if(DMAC.DMAOR.BIT.AE) {
        /* アドレスエラー発生 */
        DMAC.DMAOR.BIT.AE = 0;  /* フラグクリア */
        
        /* 全DMAチャネル停止 */
        dma_stop(0);
        dma_stop(1);
        dma_stop(2);
        dma_stop(3);
        
        /* DMAマスター無効化 */
        DMAC.DMAOR.BIT.DME = 0;
    }
}

/*
 * CHCRレジスタビット定義補足：
 * 
 * DE (DMA Enable): DMA転送許可
 * TE (Transfer End): 転送終了フラグ
 * IE (Interrupt Enable): 割り込み許可
 * TS[1:0] (Transfer Size): 転送サイズ (00:バイト, 01:ワード, 10:ロング)
 * TB (Transfer Bus): 転送バス選択
 * DS (DREQ Select): DREQ選択
 * DL (DACK Level): DACK極性
 * RS[3:0] (Resource Select): リソース選択
 * SM[1:0] (Source Address Mode): 転送元アドレスモード
 * DM[1:0] (Destination Address Mode): 転送先アドレスモード
 * AL (Address Latch): アドレスラッチ
 * AM (Address Mode): アドレスモード
 * TL (Transfer Level): 転送レベル
 * DO (DMA Operation): DMA動作
 * 
 * DMACORレジスタビット定義：
 * 
 * DME (DMA Master Enable): DMAマスター許可
 * NMIF (NMI Flag): NMIフラグ
 * AE (Address Error): アドレスエラー
 * PR[1:0] (Priority): 優先度
 * CMS[1:0] (Cycle Mode Select): サイクルモード選択
 */
