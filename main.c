#include "platform.h"
#include "sb16.h"
#include "irq.h"
#include "sb_dma.h"

static struct sb_irq_param_t *_irq_param = NULL;

volatile int c = 0;
static void __interrupt __far sb_test_irq_handler(void) {
  c = 1;
  outp(_irq_param->port, 0x20);
}

static int load_file_to_dma_buffer(struct sb_dma_buffer_t *dma_buffer, char const *filename) {
  FILE *file = fopen(filename, "rb");
  size_t file_size;
  if (file == NULL) {
    return -1;
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);

  if (file_size > dma_buffer->capacity) {
    fclose(file);
    return -2;
  }

  fread(dma_buffer->buffer, 1, file_size, file);
  dma_buffer->size = file_size;

  fclose(file);
  return 0;
}

static void usage(void) {
  puts("Usage: sb16 <filename>");
}

int main(int argc, char **argv) {
  struct sb_context_t sb_card;
  struct sb_irq_param_t sb_irq_param;
  struct sb_dma_buffer_t sb_dma_buffer;
  struct sb_dma_page_t sb_dma_page;
  
  int ret;

  union REGS regs;

  if (argc != 2) {
    usage();
    return -SB_E_FAIL;
  }

  ret = sb_init(&sb_card);
  if (ret != SB_SUCCESS) {
    puts("Failed to initialise SoundBlaster 16 card");
    return ret;
  }

  sb_print(&sb_card);

  ret = sb_irq_init(sb_card.irq, sb_test_irq_handler, &sb_irq_param);

  if (ret != SB_SUCCESS) {
    puts("Failed to initialise IRQ");
    return ret;
  }

  _irq_param = &sb_irq_param;

  regs.h.ah = 0x00;

  fputs("\nTesting IRQ... ", stdout);
  int86(sb_irq_param.vector, &regs, &regs);
  delay(15);
  puts(c == 1 ? "[TRIGGERED]" : "[FAIL]");

  puts("Allocating DMA buffer...");
  ret = sb_dma_init(&sb_dma_buffer);

  if (ret != SB_SUCCESS) {
    puts("Failed to allocate DMA buffer");
    sb_irq_shutdown(&sb_irq_param);
    return ret;
  }

  sb_dma_page_offset(&sb_dma_buffer, &sb_dma_page);

  printf("Buffer: %04X:%04X L:0x%08lX P:%X O:%X\n",
    FP_SEG(sb_dma_buffer.buffer), FP_OFF(sb_dma_buffer.buffer),
    sb_dma_linear_address(&sb_dma_buffer),
    sb_dma_page.page, sb_dma_page.offset);

  puts("Loading file to DMA buffer...");

  ret = load_file_to_dma_buffer(&sb_dma_buffer, argv[1]);

  if (ret != 0) {
    puts("Failed to load file to DMA buffer");
    sb_dma_free(&sb_dma_buffer);
    sb_irq_shutdown(&sb_irq_param);
    return ret;
  }

  printf("Loaded file to DMA buffer: %d bytes\n", sb_dma_buffer.size);

  printf("Setting up DMA channel %d...\n", sb_card.dma);
  sb_dma_prepare(&sb_dma_buffer, sb_card.dma);

  puts("Playing sound...");

  sb_speaker_on(&sb_card);
  sb_set_time_constant(&sb_card, 1, HZ_8K);
  sb_start_block_transfer(&sb_card, &sb_dma_buffer);
  sb_speaker_off(&sb_card);

  getch();

  sb_dma_free(&sb_dma_buffer);
  sb_irq_shutdown(&sb_irq_param);
  return 0;
}
