const int PIN_A = 8;
const int PIN_B = 9;
const int PIN_C = 10;
const int PIN_D = 11;

const int MAX = 255;
const int DELAY_MS = 4;

const int paso[4][4] = {
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {1, 0, 0, 1}
};

const int paso2[4][4] = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

const int PINES[4] = { PIN_A, PIN_B, PIN_C, PIN_D };

void setup() {
  /*
    pinMode(numeroPin, OUTPUT|INPUT);
  */

  pinMode(1, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(PINES[i], OUTPUT);
  }
}

void loop() {
  /*
    delay(milisegundos);
    digitalWrite(numeroPin, LOW|HIGH);
    analogWrite(numeroPin, valor: int);
    analogWriteResolution(numeroDeBits); 
  */

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(PINES[j], HIGH * paso[i][j]); 
    }
    delay(DELAY_MS);
  }
}
