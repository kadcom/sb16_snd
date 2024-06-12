#include "sb16.h"

#define SB_BASE_DETECTION 0x200

/* Default Sound Blaster settings */
#define SB_DEFAULT_PORT 0x220
#define SB_DEFAULT_IRQ  5
#define SB_DEFAULT_DMA  1

#define SB_DSP_RESET  0x06
#define SB_DSP_READ   0x0A
#define SB_DSP_WRITE  0x0C
#define SB_DSP_STATUS 0x0E
#define SB_DSP_ACK    0xAA


/* DSP Read Write */
INLINE static void sb_dsp_write(u16 port, u8 val) {
  while((inp(port + SB_DSP_WRITE) & 0x80) != 0); /* wait for ready */
  outp(port + SB_DSP_WRITE, val);
}

INLINE static u8 sb_dsp_read(u16 port) {
  while((inp(port + SB_DSP_STATUS) & 0x80) == 0); /* wait for ready */
  return inp(port + SB_DSP_READ);
}

INLINE static int sb_dsp_reset(u16 port) {
  u8 read_val;
  outp(port + SB_DSP_RESET, 1);
  delay(3);
  outp(port + SB_DSP_RESET, 0);
  delay(100);

  read_val = sb_dsp_read(port);


  if (read_val != SB_DSP_ACK) {
    return -SB_E_FAIL;
  }

  return SB_SUCCESS;
}

INLINE static u16 parse_until_wspace(char const *ptr, char const **end_ptr) {
  u16 val = 0;
  while (*ptr != ' ' && *ptr != '\0' && *ptr != '\n' && *ptr != '\r' && *ptr != '\t') {
    val = (val * 10) + (*ptr - '0');
    ++ptr;
  }

  *end_ptr = ptr;
  return val;
}

static u16 sb_dsp_port_detect(void) {
  /* determine the port */
  u8 i;
  u16 port;

  for( i = 1; i < 9; ++i) {
    if ( i == 7 ) continue;

    port = SB_BASE_DETECTION + (i << 4);

    if (sb_dsp_reset(SB_BASE_DETECTION + (i << 4))
          == SB_SUCCESS) {
      break;
    }

    if (i == 9) {
      port = SB_DEFAULT_PORT;
      break;
    }
  }
  
  return port;
}
/* Initialise and detect the SoundBlaster 16 card */
int sb_init(struct sb_context_t *sb_card) {
  /* Detect the SoundBlaster 16 card */
  char const *blaster_env = getenv("BLASTER");
  char const *c, *cnext;
  
  if (blaster_env == NULL) {
    return -SB_E_NO_BLASTER_ENV;
  }

  memset(sb_card, 0, sizeof(struct sb_context_t));

  sb_card->port = sb_dsp_port_detect();
  /* Parse the BLASTER environment variable */
  for ( c = blaster_env; *c != '\0'; ++c) {
    switch (*c | 0x20) {
      case 'i':
        sb_card->irq = parse_until_wspace(c + 1, &cnext);
        c = cnext;
        break;
      case 'd':
        sb_card->dma = parse_until_wspace(c + 1, &cnext);
        c = cnext;
        break;
      default:
        continue;
    }
  }

   
  return SB_SUCCESS;
}

void sb_print(struct sb_context_t *sb_card) {
  printf(
      "SoundBlaster 16 Card\t" 
      "IRQ: 0x%X "
      "DMA: 0x%X "
      "Port: 0x%X ",
      sb_card->irq,
      sb_card->dma,
      sb_card->port);
}

