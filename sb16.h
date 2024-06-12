#ifndef SB16_H
#define SB16_H

#include "platform.h"

struct sb_version_t {
  u8 major;
  u8 minor;
};

/* Sound Blaster context */
struct sb_context_t {
  u16 port;
  u8 irq;
  u8 dma;

  struct sb_version_t version;
};

/* Initialise Sound Blaster */
int sb_init(struct sb_context_t *sb_card /* out */);

/* print sound blaster settings */
void sb_print(struct sb_context_t *sb_card);

#endif /* SB16_H */
