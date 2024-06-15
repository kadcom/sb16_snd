#include "sb_dma.h"
#include "platform.h"

#define DMA_MASK_REG 0x0A
#define DMA_MODE_REG 0x0B
#define DMA_FLIP_FLOP_REG 0x0C

#define DMA_DISABLE 0x04
#define DMA_ENABLE 0x00

/* This bit field needs to be set for each DMA 
 * It's hard-coded now for read mode, single cycle
 *
 * If you want to change DMA, please create
 * a proper function
 */
#define DMA_SINGLE_CYCLE 0x48

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

static u32 sb_dma_page_boundary(struct sb_dma_buffer_t *dma_buffer);

#if __386__
u32 sb_dma_linear_address(struct sb_dma_buffer_t *dma_buffer) {
  return (u32)dma_buffer->buffer;
}

void sb_dma_print_buffer(struct sb_dma_buffer_t *dma_buffer) {
  struct sb_dma_page_t dma_page;
  void FAR *end_buffer = &dma_buffer->buffer[dma_buffer->size - 1];

  sb_dma_page_offset(dma_buffer, &dma_page);

  printf("Buffer:%08lX -> %08lX B: %08lX P:%X O:%X\n",
    sb_dma_linear_address(dma_buffer),
    (u32) end_buffer,
    sb_dma_page_boundary(dma_buffer),
    dma_page.page, dma_page.offset); 
}
#else
u32 sb_dma_linear_address(struct sb_dma_buffer_t *dma_buffer) {
  u16 seg = FP_SEG(dma_buffer->buffer);
  u16 off = FP_OFF(dma_buffer->buffer);

  return ((u32)seg << 4) + off;

}

void sb_dma_print_buffer(struct sb_dma_buffer_t *dma_buffer) {
  struct sb_dma_page_t dma_page;
  void *end_buffer = dma_buffer->buffer + dma_buffer->size;

  sb_dma_page_offset(dma_buffer, &dma_page);

  printf("Buffer: %04X:%04X -> %04X:%04X\nL:0x%08lX -> 0x%08lX  B:%08lX P:%X O:%X\n",
    FP_SEG(dma_buffer->buffer), FP_OFF(dma_buffer->buffer),
    FP_SEG(end_buffer), FP_OFF(end_buffer),

    sb_dma_linear_address(dma_buffer),
    sb_dma_linear_address(end_buffer),

    sb_dma_page_boundary(dma_buffer),
    dma_page.page, dma_page.offset);
}
#endif

static u32 sb_dma_page_boundary(struct sb_dma_buffer_t *dma_buffer) {
  return (sb_dma_linear_address(dma_buffer) + dma_buffer->size - 1) & 0xFFFF0000;
}

bool sb_dma_cross_page(struct sb_dma_buffer_t *dma_buffer, u32 *cross_page_offset /* out */) {
  u32 const linear_addr = sb_dma_linear_address(dma_buffer);
  u32 const page_boundary = sb_dma_page_boundary(dma_buffer);

  if ((linear_addr & 0xFFFF0000) == page_boundary) {
    return false;
  }
 
  if (cross_page_offset != NULL) {
    *cross_page_offset = page_boundary;
  }

  return true;
}

void sb_dma_page_offset(struct sb_dma_buffer_t *dma_buffer, struct sb_dma_page_t *dma_page) {
  
  u32 const linear_addr = sb_dma_linear_address(dma_buffer); 

  dma_page->page = (u8)(linear_addr >> 16);
  dma_page->offset = linear_addr & 0xFFFF;
}

int sb_dma_prepare(struct sb_dma_buffer_t *dma_buffer, u8 dma_channel) {
  u8 const dma_channel_port_addr = dma_channel << 1;
  u8 const dma_channel_port_count = dma_channel_port_addr + 1;
  u8 dma_page_port = 0x87;
  
  u16 const dma_count = dma_buffer->size;

  struct sb_dma_page_t dma_page;
  sb_dma_page_offset(dma_buffer, &dma_page);

  /* Disable DMA */
  outp(DMA_MASK_REG, dma_channel | DMA_DISABLE);

  /* Clear the flip-flop */
  outp(DMA_FLIP_FLOP_REG, 0x0);

  /* Set the DMA mode */
  outp(DMA_MODE_REG, dma_channel | DMA_SINGLE_CYCLE);

  /* Set the DMA address */
  outp(dma_channel_port_addr, _LO(dma_page.offset));
  outp(dma_channel_port_addr, _HI(dma_page.offset));

  /* Set the DMA count */
  outp(dma_channel_port_count, _LO(dma_count));
  outp(dma_channel_port_count, _HI(dma_count));

  /* Set the DMA page */
  switch(dma_channel) {
    case 0:
      dma_page_port = 0x87;
      break;
    case 1:
      dma_page_port = 0x83;
      break;
    case 2:
      dma_page_port = 0x81;
      break;
    case 3:
      dma_page_port = 0x82;
      break;
    default:
      return -SB_E_FAIL;
  }

  outp(dma_page_port, dma_page.page);

  /* Enable DMA */
  outp(DMA_MASK_REG, dma_channel | DMA_ENABLE);

  return SB_SUCCESS;
}
