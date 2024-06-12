#include "irq.h"
#include "platform.h"

static int build_irq_param(struct sb_irq_param_t *param, u8 irq, sb_irq_handler_t handler) {
  memset(param, 0, sizeof(struct sb_irq_param_t));

  param->irq = irq;

  switch(irq) {
    case 2: /* mirrors irq 9 */
    case 9:
      param->port = 0xA0;
      param->vector = 0x71;
      break;
    case 3:
    case 5:
    case 7:
      param->port = 0x20;
      param->vector = 0x08 + irq;
      break;
    case 10:
    case 11:
      param->port = 0xA0;
      param->vector = 0x72 + (irq - 10);
      break;
    default:
      return -SB_E_FAIL;
  }

  param->handler = handler;
  param->old_handler = NULL;

  return SB_SUCCESS;
}

static struct sb_irq_param_t *_irq_param = NULL;

int sb_irq_init(u8 irq, sb_irq_handler_t handler, struct sb_irq_param_t *param) {
  u8 const imr_port = param->port + 1;
  u8 const irq_mask = 1 << (param->irq & 0x07);

  if (SB_FAIL(build_irq_param(param, irq, handler))) {
    return -SB_E_FAIL;
  }

  /* get teh old vector */
  param->old_handler = _dos_getvect(param->vector);

  /* Turn off IRQ on that specific irq line */
  outp(imr_port, inp(imr_port) | irq_mask);

  /* Set the new vector */
  _dos_setvect(param->vector, param->handler);

  /* Turn on IRQ on that specific irq line */
  outp(imr_port, inp(imr_port) & ~irq_mask);

  _irq_param = param;

  return SB_SUCCESS;

}

void sb_irq_shutdown(struct sb_irq_param_t *param) {
  u8 const imr_port = param->port + 1;
  u8 const irq_mask = 1 << (param->irq & 0x07);

  /* Turn off IRQ on that specific irq line */
  outp(imr_port, inp(imr_port) | irq_mask);

  /* Set the old vector */
  _dos_setvect(param->vector, param->old_handler);

  /* Turn on IRQ on that specific irq line */
  outp(imr_port, inp(imr_port) & ~irq_mask);

  _irq_param = NULL;
}
