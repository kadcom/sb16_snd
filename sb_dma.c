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
  u16 seg = FP_SEG(dma_buffer->buffer);
  u16 off = FP_OFF(dma_buffer->buffer);

  return ((u32)seg << 4) + off;

}

void sb_dma_page_offset(struct sb_dma_buffer_t *dma_buffer, struct sb_dma_page_t *dma_page) {
  
  u32 linear_addr = sb_dma_linear_address(dma_buffer); 

  dma_page->page = (u8)(linear_addr >> 16);
  dma_page->offset = linear_addr & 0xFFFF;
}
