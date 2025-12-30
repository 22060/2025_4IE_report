/*
 * 成功していたDMAコードのヘッダーファイル
 */

#ifndef C0DF380E_9006_440A_A29A_CADA7612C9F5
#define C0DF380E_9006_440A_A29A_CADA7612C9F5

#ifndef DMA_WORKING_H
#define DMA_WORKING_H

/* 関数プロトタイプ */
void dma_init(void);
int dma_tft_send(const unsigned short *framebuffer, unsigned int pixel_count);

#endif /* DMA_WORKING_H */


#endif /* C0DF380E_9006_440A_A29A_CADA7612C9F5 */
