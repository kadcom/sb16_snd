#include "sb_irq.h"
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

static int install_irq_handler(struct sb_irq_param_t *param) {

  _disable();
  /* get teh old vector */
  param->old_handler = _dos_getvect(param->vector);

  /* Set the new vector */
  _dos_setvect(param->vector, param->handler);

  /* unmask the IRQ */
  outp(param->port + 1, inp(param->port + 1) & ~(1 << (param->irq & 0x07)));
  _enable();

  return SB_SUCCESS;
}

static int remove_irq_handler(struct sb_irq_param_t *param) {

  _disable();
  /* Set the old vector */
  _dos_setvect(param->vector, param->old_handler);

  _enable();

  return SB_SUCCESS;
}

extern struct sb_irq_param_t *_irq_param;

int sb_irq_init(u8 irq, sb_irq_handler_t handler, struct sb_irq_param_t *param) {

  if (build_irq_param(param, irq, handler) != SB_SUCCESS) {
    return -SB_E_FAIL;
  }

  if (install_irq_handler(param) != SB_SUCCESS) {
    return -SB_E_FAIL;
  }

  _irq_param = param;

  return SB_SUCCESS;

}

void sb_irq_shutdown(struct sb_irq_param_t *param) {
  if (param == NULL) {
    return;
  }

  remove_irq_handler(param);

  _irq_param = NULL;
}
