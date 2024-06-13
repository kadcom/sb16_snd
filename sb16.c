#include "sb16.h"
#include "platform.h"

#define SB_BASE_DETECTION 0x200

/* Default Sound Blaster settings */
#define SB_DEFAULT_PORT 0x220
#define SB_DEFAULT_IRQ  5
#define SB_DEFAULT_DMA  1

#define SB_DSP_RESET  0x06
#define SB_DSP_READ   0x0A
#define SB_DSP_WRITE  0x0C
#define SB_DSP_STATUS 0x0E

#define SB_DSP_CMD_SET_TIME_CONSTANT  0x40
#define SB_DSP_CMD_VERSION            0xE1
#define SB_DSP_CMD_SPEAKER_ON         0xD1
#define SB_DSP_CMD_SPEAKER_OFF        0xD3
#define SB_DSP_CMD_8BIT_OUTPUT        0x14

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

static int sb_dsp_reset(u16 port) {
  outp(port + SB_DSP_RESET, 1);
  delay(3);
  outp(port + SB_DSP_RESET, 0);
  delay(100);

  if (sb_dsp_read(port) != SB_DSP_ACK) {
    return -SB_E_FAIL;
  }

  return SB_SUCCESS;
}

/* Basic Commands */

void sb_speaker_on(struct sb_context_t *sb_card) {
  sb_dsp_write(sb_card->port, SB_DSP_CMD_SPEAKER_ON);
}

void sb_speaker_off(struct sb_context_t *sb_card) {
  sb_dsp_write(sb_card->port, SB_DSP_CMD_SPEAKER_OFF);
}

static INLINE u16 time_constant(u8 nchannel, u16 freq) {
  /* round up because it's supposed to be 65536 but I don't want 32 bit-bit calculations */
  return 0xFFFF - (256 * 1000000 / (freq * nchannel)) + 0x01; 
}

void sb_set_time_constant(struct sb_context_t *sb_card, u8 nchannel, u16 freq) {
  u16 tc = time_constant(nchannel, freq);

  sb_dsp_write(sb_card->port, SB_DSP_CMD_SET_TIME_CONSTANT);
  sb_dsp_write(sb_card->port, _HI(tc));
}
/* Parse a number until a whitespace is found */

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

static int sb_get_version(u16 port, struct sb_version_t *version) {
  u8 major, minor;
  sb_dsp_write(port, SB_DSP_CMD_VERSION);
  major = sb_dsp_read(port);
  minor = sb_dsp_read(port);
  version->major = major;
  version->minor = minor;
  return SB_SUCCESS;
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

  if (sb_get_version(sb_card->port, &sb_card->version) != SB_SUCCESS) {
    return -SB_E_VERSION;
  }

  return SB_SUCCESS;
}

/* Start Block Transfer */
void sb_start_block_transfer(struct sb_context_t *sb_card, struct sb_dma_buffer_t *dma_buffer) {
  sb_dsp_write(sb_card->port, SB_DSP_CMD_8BIT_OUTPUT);
  sb_dsp_write(sb_card->port, _LO(dma_buffer->size));
  sb_dsp_write(sb_card->port, _HI(dma_buffer->size));
}


/* Print the SoundBlaster 16 card information */
void sb_print(struct sb_context_t *sb_card) {
  char const*version_str;

  switch (sb_card->version.major) {
    case 1:
      version_str = "SoundBlaster 1.x";
      break;
    case 2:
      version_str = "SoundBlaster 2.0";
      break;
    case 3:
      version_str = "SoundBlaster Pro";
      break;
    case 4:
      version_str = "SoundBlaster 16";
      break;
    case 5:
      version_str = "SoundBlaster AWE32";
      break;
    default:
      version_str = "Unknown";
      break;
  }

  printf(
      "%s detected!\t" 
      "IRQ: 0x%X "
      "DMA: 0x%X "
      "Port: 0x%X ",
      version_str,
      sb_card->irq,
      sb_card->dma,
      sb_card->port);
}

