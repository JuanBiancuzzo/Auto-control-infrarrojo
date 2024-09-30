#include <IRremote.hpp>

const int PIN_1A = 6;
const int PIN_1B = 5;
const int PIN_1C = 4;
const int PIN_1D = 3;

const int PIN_2A = 8;
const int PIN_2B = 9;
const int PIN_2C = 10;
const int PIN_2D = 11;

const int PIN_VDD = 17;
const int PIN_GND = 18;
const int PIN_DATA = 19;

const int PINES_IZQ[4] = { PIN_1A, PIN_1B, PIN_1C, PIN_1D };
const int PINES_DER[4] = { PIN_2A, PIN_2B, PIN_2C, PIN_2D };

enum BotonIR {
  CH_MENOS = 0xBA45FF00,
  CH = 0xB946FF00,
  CH_MAS = 0xB847FF00,
  BACK = 0xBB44FF00,
  FOWARD = 0xBF40FF00,
  PAUSE = 0xBC43FF00,
  MENOS = 0xF807FF00,
  MAS = 0xEA15FF00,
  EQ = 0xF609FF00,
  CERO = 0xE916FF00,
  CIEN_MAS = 0xE619FF00,
  DOSCIENTOS_MAS = 0xF20DFF00,
  UNO = 0xF30CFF00,
  DOS = 0xE718FF00,
  TRES = 0xA15EFF00,
  CUATRO = 0xF708FF00,
  CINCO = 0xE31CFF00,
  SEIS = 0xA55AFF00,
  SIETE = 0xBD42FF00,
  OCHO = 0xAD52FF00,
  NUEVE = 0xB54AFF00,
};

const size_t DELAY_TOTAL_MS = 200;
const int MAX = 3;
const int DELAY_MINIMO = 4;
const int DELAY_VALOCIDAD[MAX + 1] = {0, 4* DELAY_MINIMO, 2 * DELAY_MINIMO, DELAY_MINIMO};

const BotonIR ADELANTE = DOS;
const BotonIR ATRAS = OCHO;

const BotonIR IZQUIERDA = CUATRO;
const BotonIR DERECHA = SEIS;

const BotonIR FRENAR = CINCO;

const int PASO[4][4] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

void settearPines(int* pines, int* paso) {
  for (int i = 0; i < 4; i++) 
    digitalWrite(pines[i], paso[i]);
}

struct Dato {
  public:
    size_t Delay;
    bool Direccion_positiva;
  private:
    size_t _delay_parcial;
    size_t _pos;
    int* _pines;

  public:
  Dato(int delay_actual, int pines[4]) {
    Delay = delay_actual;
    _delay_parcial = delay_actual;
    Direccion_positiva = delay_actual >= 0;
    _pos = 0;
    _pines = pines;
  }

  void nuevaDelay(int delay_actual) {
    Direccion_positiva = delay_actual >= 0;
    Delay = (size_t)(Direccion_positiva ? delay_actual : -delay_actual);
  }
  
  void avanzar(size_t delay_avanzar) {
    if (delay_avanzar < _delay_parcial) {
      _delay_parcial -= delay_avanzar;
      return;
    }
    _delay_parcial = Delay;
    settearPines(_pines, PASO[_pos]);
    _pos = Direccion_positiva 
      ? (_pos + 1) % 4 
      : (_pos + 3) % 4;
  }
};

Dato dato_izq = Dato(0, PINES_IZQ);
Dato dato_der = Dato(0, PINES_DER);

int velocidad_izq = 0;
int velocidad_der = 0;

void setup() {
  Serial.begin(9600);
  
  IrReceiver.begin(PIN_DATA, ENABLE_LED_FEEDBACK);
  Serial.println("Ready to receive IR signals of protocols: ");
  printActiveIRProtocols(&Serial);
  
  for (int i = 0; i < 4; i++) {
    pinMode(PINES_IZQ[i], OUTPUT);
    pinMode(PINES_DER[i], OUTPUT);
  }

  pinMode(PIN_VDD, OUTPUT); 
  pinMode(PIN_DATA, INPUT);  
  pinMode(PIN_GND, OUTPUT);  

  digitalWrite(PIN_VDD, HIGH);
  digitalWrite(PIN_GND, LOW);
}

void settear_stepper(size_t delay_total) {
  if (dato_izq.Delay == 0 && dato_der.Delay == 0) {
    delay(delay_total);
    return;
  }

  if (dato_izq.Delay == 0 || dato_der.Delay == 0) {
    Dato dato = dato_izq.Delay == 0 ? dato_der : dato_izq;

    while (delay_total > dato.Delay) {
      dato.avanzar(dato.Delay);
      delay_total -= dato.Delay;
    }
    dato.avanzar(delay_total);
    return;
  }

  size_t menor_delay = dato_izq.Delay < dato_der.Delay ? dato_izq.Delay : dato_der.Delay;  

  while (delay_total > menor_delay) {
    dato_izq.avanzar(menor_delay);
    dato_der.avanzar(menor_delay);

    delay_total -= menor_delay;

    menor_delay = dato_izq.Delay < dato_der.Delay ? dato_izq.Delay : dato_der.Delay;
  }

  dato_izq.avanzar(delay_total);
  dato_der.avanzar(delay_total);
}

int delay_para_velocidad(int velocidad, size_t delay_maximo) {
  if (velocidad > MAX)
    return DELAY_MINIMO;
  return (velocidad > 0 ? 1 : -1) * DELAY_VALOCIDAD[abs(velocidad)];
}

void loop() {
  if (IrReceiver.decode()) {
    // Modificamos el dato para obtener otros valores
    IRRawDataType info = IrReceiver.decodedIRData.decodedRawData;
    
    switch (info) {
      case ADELANTE: 
        if (velocidad_izq < MAX && velocidad_der < MAX) {
          velocidad_izq++;
          velocidad_der++; 
        }
        Serial.println("Adelante");
        break;
      case ATRAS: 
        if (-velocidad_izq < MAX && -velocidad_der < MAX) {
          velocidad_izq--;
          velocidad_der--; 
        }
        Serial.println("atras");
        break;
      case IZQUIERDA: 
        if (velocidad_der < MAX) {
          velocidad_der++;          
        } else if (-velocidad_izq < MAX) {
          velocidad_izq--;
        }
        Serial.println("izquierda");
        break;
      case DERECHA: 
        if (velocidad_izq < MAX) {
          velocidad_izq++;
        } else if (-velocidad_der < MAX) {
          velocidad_der--;
        }
        Serial.println("derecha");
        break;
      case FRENAR: 
        velocidad_izq = 0;
        velocidad_der = 0;
        Serial.println("frenar");
        break;
    }
    
    Serial.print("izq: ");
    Serial.print(delay_para_velocidad(velocidad_izq, DELAY_TOTAL_MS));
    Serial.print(" der: ");
    Serial.println(delay_para_velocidad(velocidad_der, DELAY_TOTAL_MS));
    
    IrReceiver.resume();
  }  

  dato_izq.nuevaDelay(delay_para_velocidad(velocidad_izq, DELAY_TOTAL_MS));
  dato_der.nuevaDelay(delay_para_velocidad(velocidad_der, DELAY_TOTAL_MS));

  settear_stepper(DELAY_TOTAL_MS);
  
}
