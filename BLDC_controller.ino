#define H_U 10
#define H_V 11
#define H_W 12

//UL-UH-VL-VH-WL-WH

byte allasok[8] = {
  0b00000000,

  0b01100000,
  0b00011000,
  0b01001000,
  0b10000100,
  0b00100100,
  0b10010000,

  0b00000000
};

byte hallRead() {
  return ((PINB & 0b00011100) >> 2);
}

void setup() {
  // put your setup code here, to run once:
  cli();

  DDRD = DDRD | 0b11111100;

  TCCR1B &= ~(1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B |= (1 << CS10);

  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);

  TCCR1A |= (1 << WGM11);
  TCCR1A |= (1 << WGM10);
  pinMode(9, OUTPUT);
  analogWrite(9, 512);

  Serial.begin(9600);

  PCICR |= 1;
  PCMSK0 |= 0b00011100;
  sei();
  lepes();
}

void lepes() {
  PORTD = PORTD & 01010100;
  byte ertek = (PINB & 0b00011100) >> 2;
  PORTD = allasok[ertek];
}

ISR(PCINT0_vect) {
  //Serial.println((PINB & 0b00011100) >> 2);
  lepes();
}


void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(9, analogRead(A0));
}
