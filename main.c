#include "platform.h"
#include "sb16.h"
#include "sb_irq.h"
#include "sb_dma.h"

bool verbose_debug = true;
struct sb_irq_param_t *_irq_param = NULL;
static u16 sb_base_port = 0x220;

volatile bool played = false;
volatile u16 count = 0, ticks = 0;

static void interrupt FAR sb_test_irq_handler(void) {
  played = true;
  outp(sb_base_port + 0x0C, 0x20);
  SB_IRQ_ACK(_irq_param->port); /* Change to High DMAC */
}

void interrupt far (*old_timer_isr)(void);

static void interrupt FAR timer_isr(void) {
    count++;

    if (count == 4) {
      count = 0;
      ticks = 1;
    }

    // Call the old timer ISR
    (*old_timer_isr)();

    outp(0x20, 0x20);
}

static void install_timer_isr(void) {
  unsigned char mask = inp(0x21);
  printf("Current Timer IRQ mask: 0x%02x\n", mask);
 
  old_timer_isr = _dos_getvect(0x08);
  _dos_setvect(0x08, &timer_isr);

  /* unmask timer interrupt */
  outp(0x21, mask & ~0x1);
}

static void restore_timer_isr(void) {
  unsigned char mask = inp(0x21);
  printf("Current Timer IRQ mask: 0x%02x\n", mask);

  _dos_setvect(0x08, old_timer_isr);

  /* unmask timer interrupt */  
  outp(0x21, mask & ~0x1);
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

struct playback_param_t {
  char const *filename;
  u16  freq;
};

static void usage(void) {
  puts("Usage: sb16 -8|-11|-22 <filename>");
}

static int parse_cmd_line(int argc, char **argv, struct playback_param_t *pb ) {
  char const*argv1;
  u16 freq_arg;

  if (argc != 3) {
    return -SB_E_INVALID_PARAM;
  }

  pb->filename = argv[2];

  if (argv[1][0] != '-') {
    return -SB_E_INVALID_PARAM;
  }

  /* default pbp */
  pb->freq = HZ_8K;
  
  freq_arg = parse_until_wspace(argv[1] + 1, &argv1);

  switch (freq_arg) {
    case 8:
      pb->freq = HZ_8K;
      break;
    case 11:
      pb->freq = HZ_11K;
      break;
    case 22:
      pb->freq = HZ_22K;
      break;
    default:
      return -SB_E_INVALID_PARAM;
  }

  return SB_SUCCESS;
}

int main(int argc, char **argv) {
  struct sb_context_t sb_card;
  struct sb_irq_param_t sb_irq_param;
  struct sb_dma_buffer_t sb_dma_buffer;
  struct sb_dma_page_t sb_dma_page;
  struct playback_param_t pbp;

  u16 mixer_irq_port;
  u8 mixer_irq;
  u8 irq_mask;
  
  int ret;


  if (parse_cmd_line(argc, argv, &pbp) != SB_SUCCESS) {
    usage();
    return -SB_E_INVALID_PARAM;
  }

  
  ret = sb_init(&sb_card);

  if (ret != SB_SUCCESS) {
    puts("Failed to initialise SoundBlaster 16 card");
    return ret;
  }

  sb_print(&sb_card);

  /* Getting IRQ from mixer */

  mixer_irq_port = sb_card.port + 0x04;
  outp(mixer_irq_port, 0x80);
  mixer_irq = inp(mixer_irq_port + 1) & 0x0F;
  irq_mask = inp(0x21);

  printf("Mixer 0x%X IRQ Flags: 0x%X. System IRQ Mask: 0x%X\n", mixer_irq_port, mixer_irq, irq_mask);

  ret = sb_irq_init(sb_card.irq, sb_test_irq_handler, &sb_irq_param);

  puts("Allocating DMA buffer...");
  ret = sb_dma_init(&sb_dma_buffer);

  if (ret != SB_SUCCESS) {
    puts("Failed to allocate DMA buffer");
    sb_irq_shutdown(&sb_irq_param);
    return ret;
  }

  sb_dma_page_offset(&sb_dma_buffer, &sb_dma_page);
  
  puts("Loading file to DMA buffer...");

  ret = load_file_to_dma_buffer(&sb_dma_buffer, pbp.filename);

  if (ret != 0) {
    puts("Failed to load file to DMA buffer");
    sb_dma_free(&sb_dma_buffer);
    sb_irq_shutdown(&sb_irq_param);
    return ret;
  }

  printf("Loaded file to DMA buffer: %lu (0x%lX) bytes\n", sb_dma_buffer.size, sb_dma_buffer.size);

  printf("Setting up DMA channel %d...\n", sb_card.dma);
  sb_dma_prepare(&sb_dma_buffer, sb_card.dma);

  sb_dma_print_buffer(&sb_dma_buffer);
  
  puts("Playing sound...");

  sb_speaker_on(&sb_card);
  sb_set_time_constant(&sb_card, 1, pbp.freq);
  sb_start_block_transfer(&sb_card, &sb_dma_buffer);
  sb_speaker_off(&sb_card);



  install_timer_isr();
  while(!kbhit()) {
    if (ticks) {
      ticks = 0;
      printf("Tick\n");
    }

    if (played) {
      puts("IRQ received");
      break;
    }
  }
  restore_timer_isr();
  sb_irq_shutdown(&sb_irq_param);
  sb_dma_free(&sb_dma_buffer);
#if 0
  ret = sb_init(&sb_card);
  if (ret != SB_SUCCESS) {
    puts("Failed to initialise SoundBlaster 16 card");
    return ret;
  }

  sb_base_port = sb_card.port;

  sb_print(&sb_card);

  ret = sb_irq_init(sb_card.irq, sb_test_irq_handler, &sb_irq_param);

  if (ret != SB_SUCCESS) {
    puts("Failed to initialise IRQ");
    return ret;
  }

  _irq_param = &sb_irq_param;
 
  played = false; /* Reset IRQ flag */

  puts("Allocating DMA buffer...");
  ret = sb_dma_init(&sb_dma_buffer);

  if (ret != SB_SUCCESS) {
    puts("Failed to allocate DMA buffer");
    sb_irq_shutdown(&sb_irq_param);
    return ret;
  }

  sb_dma_page_offset(&sb_dma_buffer, &sb_dma_page);
  
  puts("Loading file to DMA buffer...");

  ret = load_file_to_dma_buffer(&sb_dma_buffer, pbp.filename);

  if (ret != 0) {
    puts("Failed to load file to DMA buffer");
    sb_dma_free(&sb_dma_buffer);
    sb_irq_shutdown(&sb_irq_param);
    return ret;
  }

  printf("Loaded file to DMA buffer: %lu (0x%lX) bytes\n", sb_dma_buffer.size, sb_dma_buffer.size);

  printf("Setting up DMA channel %d...\n", sb_card.dma);
  sb_dma_prepare(&sb_dma_buffer, sb_card.dma);

  sb_dma_print_buffer(&sb_dma_buffer);

  if (sb_dma_cross_page(&sb_dma_buffer, &page_boundary)) {
    printf("Crossing page boundary at address 0x%08lX\n", page_boundary);
  }

  install_timer_isr();

  puts("Playing sound...");

  sb_speaker_on(&sb_card);
  sb_set_time_constant(&sb_card, 1, pbp.freq);
  sb_start_block_transfer(&sb_card, &sb_dma_buffer);
  sb_speaker_off(&sb_card);

  while (!kbhit()) {
    if (!played) {
      continue;
    }

    if (ticks) {
      ticks = 0;
      printf("Tick\n");
    }

    puts("IRQ received");
    break;
  }

  restore_timer_isr();

  sb_dma_free(&sb_dma_buffer);
  sb_irq_shutdown(&sb_irq_param);
#endif
  return 0;
}
