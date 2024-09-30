#include <Servo.h>

Servo servo;

const int PIN_SENIAL = 19;
const int DELAY = 100;

const int MAX = 180;
int angulo = 0;
int incremento = 20;

void setup() {
  // put your setup code here, to run once:
  /*
    pinMode(numeroPin, OUTPUT|INPUT);
  */
  // pinMode(PIN_SENIAL, OUTPUT);
  servo.attach(PIN_SENIAL);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    /*
    delay(milisegundos);
    digitalWrite(numeroPin, LOW|HIGH);
    analogWrite(numeroPin, valor: int);
    analogWriteResolution(numeroDeBits); 
  */

  servo.write(angulo);
  if (angulo < 0)
    angulo = 0;
  else if (angulo > MAX)
    angulo = MAX;
  else
    angulo += incremento;
  
  if (angulo >= MAX || angulo <= 0) {
    incremento *= -1;
  }

  Serial.println(angulo);
  delay(DELAY);
}
