#ifndef SB16_H
#define SB16_H

#include "platform.h"
#include "sb_dma.h"

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

/* Turn on the speaker */
void sb_speaker_on(struct sb_context_t *sb_card);

/* Turn off the speaker */
void sb_speaker_off(struct sb_context_t *sb_card);

/* Set the time constant for the speaker */
void sb_set_time_constant(struct sb_context_t *sb_card, u8 nchannel, u16 freq);

/* Start Block Transfer */
void sb_start_block_transfer(struct sb_context_t *sb_card, struct sb_dma_buffer_t *dma_buffer);

#endif /* SB16_H */
