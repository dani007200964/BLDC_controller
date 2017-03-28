//BLDC vezérlő HALL cellás visszacsatolású motorokhoz
//készítette:         Hajnal Dániel
//projekt kezdete:    2017.03.28
//legutóbb frissítve: 2017.03.28

//HALL cellák
#define H_U 10
#define H_V 11
#define H_W 12


//A motor 6db állásához tartozó kimeneti értékek
//A 0. és 7. elem azért 0, hogy rendszerhiba esetén ne legyen tápzárlat
//Ha kettes csoportokra osztjuk a biteket, akkor az 11 kombináció tiltott, zárlatot okoz!!!
//PL: jó= 01 | 10 | 00 | 00; rossz: 11 | 00 | 01 | 00;
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

void setup() {
  //Interruptok kikapcsolása
  cli();

  //PORTD 2:7 kimenet
  DDRD = DDRD | 0b11111100;

  //TIMER1 konfigurálása a 10 bites PWM-hez
  TCCR1B &= ~(1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B |= (1 << CS10);

  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);

  TCCR1A |= (1 << WGM11);
  TCCR1A |= (1 << WGM10);

  //9-es láb kimenet és motor kikapcsolva
  pinMode(9, OUTPUT);
  analogWrite(9, 0);

  //Soros port inicializálása
  Serial.begin(9600);

   //Pin Change Interrupt engedélyezése PORTB-n
  PCICR |= 1;
  //Pin Change Interrupt maszkolása PORTB 2:4-re
  PCMSK0 |= 0b00011100;

  //Interruptok engedélyezése
  sei();

  //Az első léptetést meg kell hívni manuálisan, különben meg kell forgatni a tengelyt,
  //hogy a motor szinkronban legyen.
  //Nem szabad kihagyni, különben magától nem indul el a motor!!!!!
  lepes();
}

//A függvény a motort a következő pozícióba lépteti
//Akkor kell meghívni, ha a HALL szenzorok által adott jel megváltozik
void lepes() {
  //N-csatornás FET-ek kikapcsolása, hogyn e legyen tápzárlat
  PORTD = PORTD & 01010100;
  
  //HALL cellák beolvasása(egész számot ad 1-6 között)
  //Ha 0, vagy 7, akkor probléma van a jeladóval!!!
  byte ertek = (PINB & 0b00011100) >> 2;

  //PORTD léptetése a következő fázisba
  //      UL-UH-VL-VH-WL-WH
  //PORTD: 7--6--5--4--3--2
  PORTD = allasok[ertek];
}

//A HALL cellák változásakor történik interrupt
ISR(PCINT0_vect) {
  //Motor léptetése
  lepes();
}


void loop() {
  //A 9-es lábon lévő PWM jel szabályozza közvetlen a motor fordulatszámát
  //Itt az A0 bemenetre egy potméter van kötve, és annak a jele kerül a PWM kimenetre
  analogWrite(9, analogRead(A0));
}
