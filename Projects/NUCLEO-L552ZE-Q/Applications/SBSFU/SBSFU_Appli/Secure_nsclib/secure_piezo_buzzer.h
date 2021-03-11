
#ifndef SECURE_PIEZO_BUZZER
#define SECURE_PIEZO_BUZZER

void secure_piezoBuzzer_setup(void);

void secure_piezoBuzzer_buzz(int high_ms, int low_ms, int count);

void secure_piezoBuzzer_highBuzz(int high_spin, int low_spin, int count);

#endif
