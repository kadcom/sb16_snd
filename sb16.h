#ifndef SB16_H
#define SB16_H

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
#define SB_DSP_ACK    0xAA

struct sb_context_t {
  u16 port;
  u8 irq;
  u8 dma;
};

/* Initialise Sound Blaster */
int sb_init(struct sb_context_t *sb_card /* out */);

/* print sound blaster settings */
void sb_print(struct sb_context_t *sb_card);

#endif /* SB16_H */
