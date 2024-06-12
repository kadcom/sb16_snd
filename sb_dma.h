#ifndef SB_DMA_H
#define SB_DMA_H

#include "platform.h"

/* This file contains the prototypes for the low level DMA service
 * routines that are used to handle the hardware DMA. These are used
 * to handle the sound blaster DMA.
 */

struct sb_dma_buffer_t {
  u8 *buffer;
  u32 size;
  u32 capacity;
};

int sb_dma_init(struct sb_dma_buffer_t *dma_buffer /* out */);
void sb_dma_free(struct sb_dma_buffer_t *dma_buffer);

u32 sb_dma_linear_address(struct sb_dma_buffer_t *dma_buffer);

#endif /* SB_DMA_H */
