/*
 * SH7080 DMAコントローラー制御ヘッダーファイル
 * 7080S.Hファイルを参照したDMA制御定義
 */

#ifndef CAB48708_1F0B_46D6_A951_0E74EA86277D
#define CAB48708_1F0B_46D6_A951_0E74EA86277D

#ifndef DMA_CONTROL_H
#define DMA_CONTROL_H

#include "7080S.H"

/* DMAチャネル定義 */
#define DMA_CH0     0
#define DMA_CH1     1
#define DMA_CH2     2
#define DMA_CH3     3

/* 転送サイズ定義 */
#define DMA_TRANSFER_BYTE       0x00000000  /* バイト転送 (TS=00) */
#define DMA_TRANSFER_WORD       0x00000008  /* ワード転送 (TS=01) */
#define DMA_TRANSFER_LONG       0x00000010  /* ロングワード転送 (TS=10) */

/* アドレスモード定義 */
#define DMA_ADDR_FIXED          0x00000000  /* アドレス固定 */
#define DMA_ADDR_INCREMENT      0x00000001  /* アドレスインクリメント */
#define DMA_ADDR_DECREMENT      0x00000002  /* アドレスデクリメント */

#define DMA_SRC_ADDR_FIXED      (DMA_ADDR_FIXED << 12)
#define DMA_SRC_ADDR_INC        (DMA_ADDR_INCREMENT << 12)
#define DMA_SRC_ADDR_DEC        (DMA_ADDR_DECREMENT << 12)

#define DMA_DST_ADDR_FIXED      (DMA_ADDR_FIXED << 14)
#define DMA_DST_ADDR_INC        (DMA_ADDR_INCREMENT << 14)
#define DMA_DST_ADDR_DEC        (DMA_ADDR_DECREMENT << 14)

/* リソース選択定義 (RS[3:0]) */
#define DMA_RS_EXTERNAL         0x00000000  /* 外部要求 */
#define DMA_RS_SCI0_TX          0x00000800  /* SCI0送信 */
#define DMA_RS_SCI0_RX          0x00000C00  /* SCI0受信 */
#define DMA_RS_SCI1_TX          0x00001000  /* SCI1送信 */
#define DMA_RS_SCI1_RX          0x00001400  /* SCI1受信 */
#define DMA_RS_SCIF_TX          0x00001800  /* SCIF送信 */
#define DMA_RS_SCIF_RX          0x00001C00  /* SCIF受信 */
#define DMA_RS_MTU0_TGRA        0x00002000  /* MTU0 TGRA */
#define DMA_RS_MTU0_TGRB        0x00002400  /* MTU0 TGRB */
#define DMA_RS_MTU1_TGRA        0x00002800  /* MTU1 TGRA */
#define DMA_RS_MTU1_TGRB        0x00002C00  /* MTU1 TGRB */

/* 割り込み制御 */
#define DMA_INT_DISABLE         0x00000000  /* 割り込み無効 */
#define DMA_INT_ENABLE          0x00000004  /* 割り込み有効 */

/* 転送モード定義 */
#define DMA_MODE_NORMAL         0x00000000  /* ノーマルモード */
#define DMA_MODE_BLOCK          0x00080000  /* ブロック転送モード */

/* よく使用される設定の組み合わせ */
#define DMA_MEM_TO_MEM_BYTE     (DMA_TRANSFER_BYTE | DMA_SRC_ADDR_INC | DMA_DST_ADDR_INC | DMA_RS_EXTERNAL)
#define DMA_MEM_TO_MEM_WORD     (DMA_TRANSFER_WORD | DMA_SRC_ADDR_INC | DMA_DST_ADDR_INC | DMA_RS_EXTERNAL)
#define DMA_MEM_TO_MEM_LONG     (DMA_TRANSFER_LONG | DMA_SRC_ADDR_INC | DMA_DST_ADDR_INC | DMA_RS_EXTERNAL)

#define DMA_MEM_TO_SCI0_TX      (DMA_TRANSFER_BYTE | DMA_SRC_ADDR_INC | DMA_DST_ADDR_FIXED | DMA_RS_SCI0_TX)
#define DMA_SCI0_RX_TO_MEM      (DMA_TRANSFER_BYTE | DMA_SRC_ADDR_FIXED | DMA_DST_ADDR_INC | DMA_RS_SCI0_RX)

#define DMA_MEM_TO_SCIF_TX      (DMA_TRANSFER_BYTE | DMA_SRC_ADDR_INC | DMA_DST_ADDR_FIXED | DMA_RS_SCIF_TX)
#define DMA_SCIF_RX_TO_MEM      (DMA_TRANSFER_BYTE | DMA_SRC_ADDR_FIXED | DMA_DST_ADDR_INC | DMA_RS_SCIF_RX)

/* DMA制御構造体 */
typedef struct {
    void *src_addr;             /* 転送元アドレス */
    void *dst_addr;             /* 転送先アドレス */
    unsigned int transfer_count; /* 転送カウント */
    unsigned int mode;          /* 転送モード */
    int channel;                /* 使用チャネル */
    int int_enable;             /* 割り込み有効フラグ */
} dma_config_t;

/* 関数プロトタイプ */
// void dma_init(int channel, void *src, void *dst, unsigned int size, unsigned int mode);
void dma_start(int channel);
void dma_stop(int channel);
int dma_is_complete(int channel);
void dma_memory_copy(void *src, void *dst, unsigned int size);
void dma_sci_receive_setup(void *buffer, unsigned int size);
void dma_sci_transmit_setup(void *buffer, unsigned int size);
void dma_error_handler(void);

/* 高レベルDMA設定関数 */
int dma_setup(dma_config_t *config);
void dma_wait_complete(int channel);
unsigned int dma_get_remaining_count(int channel);

/* DMAマクロ関数 */
#define DMA_ENABLE_MASTER()     (DMAC.DMAOR.BIT.DME = 1)
#define DMA_DISABLE_MASTER()    (DMAC.DMAOR.BIT.DME = 0)
#define DMA_CLEAR_AE_FLAG()     (DMAC.DMAOR.BIT.AE = 0)
#define DMA_CLEAR_NMIF_FLAG()   (DMAC.DMAOR.BIT.NMIF = 0)

#define DMA_CH_ENABLE(ch)       ((ch == 0) ? (DMAC0.CHCR.BIT.DE = 1) : \
                                 (ch == 1) ? (DMAC1.CHCR.BIT.DE = 1) : \
                                 (ch == 2) ? (DMAC2.CHCR.BIT.DE = 1) : \
                                 (ch == 3) ? (DMAC3.CHCR.BIT.DE = 1) : 0)

#define DMA_CH_DISABLE(ch)      ((ch == 0) ? (DMAC0.CHCR.BIT.DE = 0) : \
                                 (ch == 1) ? (DMAC1.CHCR.BIT.DE = 0) : \
                                 (ch == 2) ? (DMAC2.CHCR.BIT.DE = 0) : \
                                 (ch == 3) ? (DMAC3.CHCR.BIT.DE = 0) : 0)

#define DMA_CH_IS_COMPLETE(ch)  ((ch == 0) ? DMAC0.CHCR.BIT.TE : \
                                 (ch == 1) ? DMAC1.CHCR.BIT.TE : \
                                 (ch == 2) ? DMAC2.CHCR.BIT.TE : \
                                 (ch == 3) ? DMAC3.CHCR.BIT.TE : 0)

/* エラーチェックマクロ */
#define DMA_HAS_ADDRESS_ERROR() (DMAC.DMAOR.BIT.AE)
#define DMA_HAS_NMI_FLAG()      (DMAC.DMAOR.BIT.NMIF)

/* CHCRレジスタビットフィールド直接設定マクロ */
#define DMA_SET_CHCR_BITS(ch, de, te, ie, ts, tb, ds, dl, rs, sm, dm, al, am, tl, do_bit) \
    do { \
        volatile struct st_dmac0 *dmac_ch = \
            (ch == 0) ? &DMAC0 : \
            (ch == 1) ? &DMAC1 : \
            (ch == 2) ? &DMAC2 : \
            (ch == 3) ? &DMAC3 : NULL; \
        if (dmac_ch) { \
            dmac_ch->CHCR.BIT.DE = de; \
            dmac_ch->CHCR.BIT.TE = te; \
            dmac_ch->CHCR.BIT.IE = ie; \
            dmac_ch->CHCR.BIT.TS = ts; \
            dmac_ch->CHCR.BIT.TB = tb; \
            dmac_ch->CHCR.BIT.DS = ds; \
            dmac_ch->CHCR.BIT.DL = dl; \
            dmac_ch->CHCR.BIT.RS = rs; \
            dmac_ch->CHCR.BIT.SM = sm; \
            dmac_ch->CHCR.BIT.DM = dm; \
            dmac_ch->CHCR.BIT.AL = al; \
            dmac_ch->CHCR.BIT.AM = am; \
            dmac_ch->CHCR.BIT.TL = tl; \
            dmac_ch->CHCR.BIT.DO = do_bit; \
        } \
    } while(0)

#endif /* DMA_CONTROL_H */


#endif /* CAB48708_1F0B_46D6_A951_0E74EA86277D */
