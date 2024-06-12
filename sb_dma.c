#include "sb_dma.h"
#include "platform.h"

#define SB_DMA_BUFFER_SIZE KB_16

int sb_dma_init(struct sb_dma_buffer_t *dma_buffer /* out */) {
  memset(dma_buffer, 0, sizeof(struct sb_dma_buffer_t));
  dma_buffer->buffer = malloc(SB_DMA_BUFFER_SIZE * 2);
  if (dma_buffer->buffer == NULL) {
    return -SB_E_NO_MEMORY;
  }

  dma_buffer->capacity = SB_DMA_BUFFER_SIZE * 2;
  return SB_SUCCESS;
}

void sb_dma_free(struct sb_dma_buffer_t *dma_buffer) {
  if (dma_buffer->buffer != NULL) {
    free(dma_buffer->buffer);
    dma_buffer->buffer = NULL;
  }

  dma_buffer->capacity = 0;
  dma_buffer->size = 0;
}

u32 sb_dma_linear_address(struct sb_dma_buffer_t *dma_buffer) {
  return ((u32)FP_SEG(dma_buffer->buffer) << 4) + FP_OFF(dma_buffer->buffer);
}
