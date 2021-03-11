
#include "secure_piezo_buzzer.h"
#include "piezo_buzzer.h"
#include "main.h"

CMSE_NS_ENTRY void secure_piezoBuzzer_setup(void) {
  // Do nothing here, as secure set this up.
}

CMSE_NS_ENTRY void secure_piezoBuzzer_buzz(int high_ms, int low_ms, int count) {
  piezoBuzzer_buzz(high_ms, low_ms, count);
}

CMSE_NS_ENTRY void secure_piezoBuzzer_highBuzz(int high_spin, int low_spin, int count) {
  piezoBuzzer_highBuzz(high_spin, low_spin, count);
}
