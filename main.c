#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

#if !defined(INLINE)
#define INLINE __inline 
#endif

#define KB    1024
#define KB_16 (16 * KB)
#define KB_32 (32 * KB)
#define KB_64 (64 * KB)

#define SB_TRUE 1
#define SB_FALSE 0

#define SB_BASE_DETECTION 0x200

#define SB_DSP_RESET  0x06
#define SB_DSP_READ   0x0A
#define SB_DSP_WRITE  0x0C
#define SB_DSP_STATUS 0x0E
#define SB_DSP_ACK    0xAA

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define _LO(x) ((u8)((x) & 0xFF))
#define _HI(x) ((u8)((x) >> 8))

#define _LO16(x) ((u16)((x) & 0xFFFF))
#define _HI16(x) ((u16)((x) >> 16))

struct sb_config_t {
  u16 port;
  u8 irq;
  u8 dma;
};

/* DSP Read Write */
INLINE static void sb_dsp_write(u16 port, u8 val) {
  while((inp(port + SB_DSP_WRITE) & 0x80) != 0); /* wait for ready */
  outp(port + SB_DSP_WRITE, val);
}

INLINE static u8 sb_dsp_read(u16 port) {
  while((inp(port + SB_DSP_STATUS) & 0x80) == 0); /* wait for ready */
  return inp(port + SB_DSP_READ);
}

/* ---------------------------------------------------- */

/* DSP Reset */
static int dsp_reset(u16 port) {
  outp(port + SB_DSP_RESET, 1);
  delay(3);
  outp(port + SB_DSP_RESET, 0);
  delay(100);

  if (sb_dsp_read(port) != SB_DSP_ACK) {
    return SB_FALSE;
  }

  return SB_TRUE;
}


/* Sound Blaster Commands */
static int sb_detect(struct sb_config_t *cfg) {
  u16 i, port;
  char *blaster_env, c, c1, c2;
  u16 blaster_len;

  cfg->port = 0x220;
  cfg->irq = 5;
  cfg->dma = 1;

  for (i = 0; i < 9; ++i) {
    if (i == 7) {
      continue; /* skip 0x270 */
    }

    port = SB_BASE_DETECTION + (i << 4);

    if (dsp_reset(port)) {
      cfg->port = port;
      break;
    }
  }

  if (i == 9) {
    return SB_FALSE;
  }

  blaster_env = getenv("BLASTER");

  if (blaster_env == NULL) {
    return SB_FALSE;
  }
  
  blaster_len = strlen(blaster_env);

  if ( (c | 32) == 'i' ) {
    cfg->irq = c1 - '0';

    if (c2 >= '0' && c2 <= '9') {
      cfg->irq = cfg->irq * 10 + c2 - '0';
    }
  }

  if ( (c | 32) == 'd' ) {
    cfg->dma = c1 - '0';

    if (c2 >= '0' && c2 <= '9') {
      cfg->dma = cfg->dma * 10 + c2 - '0';
    }
  }

  return SB_TRUE;
}  

static void sb_print_config(struct sb_config_t *cfg) {
  printf("Sound Blaster Configuration:\n");
  printf("  Port: 0x%03X\n", cfg->port);
  printf("  IRQ: %d\n", cfg->irq);
  printf("  DMA: %d\n", cfg->dma);
}


int main() {
  int ret;
  struct sb_config_t cfg;

  ret = sb_detect(&cfg);

  if (!ret) {
    puts("Sound Blaster not detected");
    return 1;
  }
  puts("Sound Blaster detected");
  sb_print_config(&cfg);
  return 0;
}
