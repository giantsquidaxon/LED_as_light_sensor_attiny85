#define ANODE_PIN_A 3
#define CATHODE_PIN_A 4
#define ANODE_PIN_B 1
#define CATHODE_PIN_B 2
#define MONITOR 0
#define LED_TIMEOUT 100000
#define SAMPLES_TO_AVERAGE 5

void setup() {
  // put your setup code here, to run once:
#ifdef LOGGING
  Serial.begin(9600);
#endif
}

/** Perform a single on off cycle with the specified duty cycle, between 0 and 4095. */
inline void do_cycle() __attribute__((always_inline));
void do_cycle(uint16_t duty) {
  PORTB = 1 << ANODE_PIN_A | 1 << ANODE_PIN_B;
  for (int i = 0; i < duty; i++) {
    __asm__("nop\n\t");
  }

  PORTB = 0b00000000;
  for (int i = duty; i <= 4096; i++) {
    __asm__("nop\n\t");
  }
  return;
}

/** log a sample to be averaged. return true is a shadow has been detected **/
bool log_sample(long int sample) {
  static long int previous_count = LED_TIMEOUT;
  static long int current_count = 0;
  static int n = 0;

  current_count += sample;
  if (++n < SAMPLES_TO_AVERAGE) {
    return false;
  }

  current_count /= n;
  long int t = previous_count + 2 * (previous_count / 20);
  previous_count = current_count;
  n = 0;

  return (current_count > t);

}

/** Flash the bug's eyes */
void eyeflash() {
  DDRB =  1 << ANODE_PIN_A | 1 << CATHODE_PIN_A |
          1 << ANODE_PIN_B | 1 << CATHODE_PIN_B;

  for (uint16_t k = 0; k < 256; k++) {
    uint16_t duty = (k * k) >> 4 ;
    do_cycle(duty);
  }

  for (uint16_t k = 255; k != 0; k--) {
    uint16_t duty = (k * k) >> 4 ;
    for (int i = 0; i < 3; i++) {
      do_cycle(duty);
    }
  }
}


void loop() {

  // Charge LED capacitance
  // Set cathode pin to OUT
#ifdef MONITOR
  DDRB = 1 << ANODE_PIN_A | 1 << CATHODE_PIN_A | 1 << MONITOR;
#else
  DDRB = 1 << ANODE_PIN_A | 1 << CATHODE_PIN_A;
#endif
  // Set cathode to 1, anode to 0.
  PORTB = 1 << CATHODE_PIN_A;
  // Wait
  delay(10);
  // Set cathode pin to IN / HIGH Z
  // Disable internal pullup
#ifdef MONITOR
  DDRB = 1 << ANODE_PIN_A | 1 << MONITOR;
  PORTB = 1 << MONITOR;
#else
  DDRB = 1 << ANODE_PIN_A;
  PORTB = 0x00;
#endif

  // Read from cathode pin until 0
  long int count = 0;
  while (PINB & 1 << CATHODE_PIN_A && count < LED_TIMEOUT) {
    count++;
  }

#ifdef MONITOR
  PORTB = 0x00;
#endif

#ifdef LOGGING
  // Write count out to serial
  Serial.print(count);
  Serial.print('\n');
#endif

  // Flash if shadow
  if (log_sample(count)) {
    eyeflash();
    delay(2000);
  }
}
