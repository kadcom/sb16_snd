#include "sb16.h"

/* DSP Read Write */
INLINE static void sb_dsp_write(u16 port, u8 val) {
  while((inp(port + SB_DSP_WRITE) & 0x80) != 0); /* wait for ready */
  outp(port + SB_DSP_WRITE, val);
}

INLINE static u8 sb_dsp_read(u16 port) {
  while((inp(port + SB_DSP_STATUS) & 0x80) == 0); /* wait for ready */
  return inp(port + SB_DSP_READ);
}

INLINE static int sb_dsp_reset(struct sb_context_t *sb_card) {
  outp(sb_card->port + SB_DSP_RESET, 1);
  delay(3);
  outp(sb_card->port + SB_DSP_RESET, 0);
  delay(100);

  if (sb_dsp_read(sb_card->port) != SB_DSP_ACK) {
    return -SB_E_FAIL;
  }

  return SB_SUCCESS;
}

/* Initialise and detect the SoundBlaster 16 card */
int sb_init(struct sb_context_t *sb_card) {
  /* Detect the SoundBlaster 16 card */
  char const *blaster_env = getenv("BLASTER");
  char const *c, *c1, *c2;
  unsigned short port, i;
  
  if (blaster_env == NULL) {
    return -SB_E_NO_BLASTER_ENV;
  }

  memset(sb_card, 0, sizeof(struct sb_context_t));

  /* Parse the BLASTER environment variable */
  for ( c = blaster_env; *c != '\0'; ++c) {
    c1 = (c + 1);

    /* if next character is null, break */
    if (*c1 == '\0') {
      continue;
    }

    c2 = (c + 2);

    if ( 'i' == ( *c | 0x20 ) ) {
      sb_card->irq = *c1 - '0';
      if (*c2 != '\0' || *c2 != ' ') {
        if (*c2 == '0') {
          sb_card->irq = 10;
          ++c;
        } else {
          sb_card->irq = (sb_card->irq * 10) + (*c2 - '0');
          c += 2;
        }
      }
    }

    if ( 'd' == ( *c | 0x20 ) ) {
      sb_card->dma = *c1 - '0';
      if (*c2 != '\0' || *c2 != ' ') {
        if (*c2 == '0') {
          sb_card->dma = 10;
          ++c;
        } else {
          sb_card->dma = (sb_card->dma * 10) + (*c2 - '0');
          c += 2;
        }
      }
    }
  }

  /* determine the port */
  for( i = 1; i < 9; ++i) {
    if ( i == 7 ) continue;

    port = SB_BASE_DETECTION + (i << 4);

    if (sb_dsp_reset(sb_card) == SB_SUCCESS) {
      break;
    }

    if (i == 9) {
      port = SB_DEFAULT_PORT;
      break;
    }
  }

  return SB_SUCCESS;
}

void sb_print(struct sb_context_t *sb_card) {
  printf(
      "SoundBlaster 16 Card\n" 
      "IRQ: %d\n"
      "DMA: %d\n"
      "Port: %d\n",
      sb_card->irq,
      sb_card->dma,
      sb_card->port);
}

