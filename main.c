#include "platform.h"
#include "sb16.h"

int main() {
  struct sb_context_t sb_card;
  int ret;

  ret = sb_init(&sb_card);
  if (ret != SB_SUCCESS) {
    return ret;
  }

  sb_print(&sb_card);
  return 0;
}
