#ifndef IRQ_H
#define IRQ_H

#include "platform.h"

/* This file contains the prototypes for the low level interrupt service
 * routines that are used to handle the hardware interrupts. These are used
 * to handle the sound blaster IRQ and the timer IRQ.
 */

typedef void interrupt far (*sb_irq_handler_t)(void);

struct sb_irq_param_t {
  sb_irq_handler_t handler; /* IRQ handler */
  sb_irq_handler_t old_handler; /* Old IRQ handler */


  u8 irq; /* IRQ number */
  u8 port; /* IRQ port */
  u8 vector; /* IRQ vector */
};

int sb_irq_init(u8 irq, sb_irq_handler_t handler, struct sb_irq_param_t *param);
void sb_irq_shutdown(struct sb_irq_param_t *param);

#endif /* IRQ_H */
