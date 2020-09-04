#define ANODE_PIN 4
#define CATHODE_PIN 5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  DDRD |= 0b11000000;
  PORTD &= 0b00001111;
  PORTD |= 0b01000000;
}

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
  //Serial.print("Measuring charge cycle:");
  // Set cathode pin to IN / HIGH Z
  DDRD &= 0b11011111;
  // Disable pullup
  PORTD &= 0b11001111;
  // Read from cathode pin until 0
  long int count = 0;
  while (PIND & 0b00100000){
    count++;
  }
  PORTD &= 0b10111111;
  // Write count out to serial
  Serial.print(count);
  Serial.print('\n');
}
