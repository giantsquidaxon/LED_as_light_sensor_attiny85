#define ANODE_PIN 4
#define CATHODE_PIN 5
#define threshold

#define LOGGING

void setup() {
  // put your setup code here, to run once:
#ifdef LOGGING
  Serial.begin(9600);
#endif
  DDRD |= 0b11000000;
  PORTD &= 0b00001111;
  PORTD |= 0b01000000;
}

inline void do_cycle() __attribute__((always_inline));
void do_cycle(uint16_t duty) {
  PORTD = 0b00010000;
  for (int i = 0; i < duty; i++) {
    __asm__("nop\n\t");
  }

  PORTD = 0b00000000;
  for (int i = duty; i <= 4096; i++) {
    __asm__("nop\n\t");
  }
  return;
}


long int last_count = 2000000;

void loop() {
  // put your main code here, to run repeatedly:
  //DDRD |= 0b00110000;
  // Set cathode to 1, anode to 0.
  //PORTD &= 0b11001111;
  //PORTD |= 0b00010000;
  //delay(1000);


  // Charge LED capacitance
  // Set cathode pin to OUT
  DDRD |= 0b00110000;
  // Set cathode to 1, anode to 0.
  PORTD &= 0b10001111;
  PORTD |= 0b01100000;
  // Wait
  delay(10);
  // Set cathode pin to IN / HIGH Z
  DDRD &= 0b11011111;
  // Disable pullup
  PORTD &= 0b11001111;
  // Read from cathode pin until 0
  long int count = 0;
  while (PIND & 0b00100000) {
    count++;
  }
  PORTD &= 0b10111111;
#ifdef LOGGING
  // Write count out to serial
  Serial.print(count);
  Serial.print('\n');
#endif

  // Flash if shadow
  long int t = last_count + 3 * (last_count / 20);
  if (count > t) {
    DDRD |= 0b00110000;

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
    delay(2000);
  }else{
    last_count = count;
  }
}
