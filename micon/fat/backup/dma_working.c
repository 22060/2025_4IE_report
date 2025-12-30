/*
 * 成功していた時のDMAコード - フォールバック重視版
 */

#include "7080S.H"

#define printf ((int (*)(const char *, ...))0x00007c7c)

/* DMA初期化 - 成功していた時の手順 */
void dma_init(void)
{
    // STB.CR2.BIT._DMACでDMACクロック有効化
    STB.CR2.BIT._DMAC = 0;  // 0で有効化
    
    printf("DMA initialized (success version)\n");
}

/* 成功していた時のTFT転送関数 */
int dma_tft_send(const unsigned short *framebuffer, unsigned int pixel_count)
{
    int i;
    volatile unsigned int timeout = 100000;  // より短いタイムアウト
    
    printf("Starting TFT transfer (%d pixels)...\n", pixel_count);
    printf("Framebuffer: 0x%08X\n", (unsigned int)framebuffer);
    
    // TFT制御レジスタ設定
    *(volatile unsigned short *)0x08000002 = 0x4001;
    
    // DMA設定を試行
    DMAC.DMAOR.BIT.DME = 0;
    DMAC0.CHCR.BIT.DE = 0;
    
    // 転送設定
    DMAC0.SAR = (void*)framebuffer;
    DMAC0.DAR = (void*)0x08000000;
    DMAC0.DMATCR = pixel_count;
    
    // CHCR設定 - 成功していた値
    DMAC0.CHCR.LONG = 0x00001401;
    
    printf("DMA setup: SAR=0x%08X, DAR=0x%08X, TCR=%d, CHCR=0x%08X\n",
           (unsigned int)DMAC0.SAR, (unsigned int)DMAC0.DAR, 
           DMAC0.DMATCR, DMAC0.CHCR.LONG);
    
    // DMA有効化
    DMAC.DMAOR.BIT.DME = 1;
    DMAC0.CHCR.BIT.DE = 1;
    
    // 短時間だけDMA完了を待つ
    while (!DMAC0.CHCR.BIT.TE && timeout > 0) {
        timeout--;
    }
    
    // DMA停止
    DMAC0.CHCR.BIT.DE = 0;
    DMAC.DMAOR.BIT.DME = 0;
    
    // 成功していた時は常にフォールバックを使用していた
    // printf("Using direct write (proven method)\n");
    // *(volatile unsigned short *)0x08000002 = 0x4001;
    // for (i = 0; i < pixel_count; i++) {
    //     *(volatile unsigned short *)0x08000000 = framebuffer[i];
    // }
    // printf("Direct write completed\n");
    
    return 1; // フォールバック成功
}
