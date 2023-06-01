#include <Wire.h>
#include <avr/interrupt.h>

// DEVICE CONFIGURATION
#define MPU_ADDRESS 0x68  // Indirizzo I2C del MPU6050
#define MPU_PWR_MGT 0x6B  // Registro per il controllo dell'alimentazione
#define MPU_GYRO_CONFIG 0x1B   // Registro di configurazione del giroscopio
#define MPU_ACCEL_CONFIG 0x1C  // Registro di configurazione dell'accelerometro
#define LBS_SENSITIVITY_ACCEL 2048.0
#define LBS_SENSITIVITY_GYRO 16.4
#define readingInterval 0.5 // Intervallo di lettura in secondi

// ACCELEROMETER DATA REGISTERS
#define XAccelH 0x3B
#define XAccelL 0x3C
#define YAccelH 0x3D
#define YAccelL 0x3E
#define ZAccelH 0x3F
#define ZAccelL 0x40

// GYROSCOPE DATA REGISTERS
#define XGyroH 0x43
#define XGyroL 0x44
#define YGyroH 0x45
#define YGyroL 0x46
#define ZGyroH 0x47
#define ZGyroL 0x48

bool canReadNewSet = true;
bool canReadFromMPU = false;
bool ready = false;

// VARIABILI DATI GLOBALI
float XAxisData = 0;
float YAxisData = 0;
float ZAxisData = 0;

float XGyroData = 0;
float YGyroData = 0;
float ZGyroData = 0;

// THRESHOLD 
float xHighThreshold = 30;
float xLowThreshold = -30;
float zHighThreshold = 30;
float zLowThreshold = -30;

void setup() {
  Wire.begin();  // Inizializza la comunicazione I2C
  Serial.begin(115200);
  
	// Invia il segnale di reset al MPU6050
  Wire.beginTransmission(MPU_ADDRESS);  // Inizia la trasmissione verso l'indirizzo del MPU6050
  Wire.write(MPU_PWR_MGT);              // Seleziona il registro per il controllo dell'alimentazione
  Wire.write(0x80);                     // Invia il valore 0x80 (0b10000000) per impostare il bit di reset
  Wire.endTransmission(true);           // Termina la trasmissione e invia il segnale di reset

  // Configura il registro di configurazione del giroscopio
  Wire.beginTransmission(MPU_ADDRESS);   // Inizia la trasmissione all'indirizzo del dispositivo
  Wire.write(MPU_GYRO_CONFIG);           // Seleziona il registro di configurazione del giroscopio
  Wire.write(0x18);                      // Imposta il valore 0x18 (00011000 in binario)
  Wire.endTransmission(true);            // Termina la trasmissione

  // Configura il registro di configurazione dell'accelerometro
  Wire.beginTransmission(MPU_ADDRESS);   // Inizia una nuova trasmissione all'indirizzo del dispositivo
  Wire.write(MPU_ACCEL_CONFIG);          // Seleziona il registro di configurazione dell'accelerometro
  Wire.write(0x18);                      // Imposta il valore 0x18 (00011000 in binario)
  Wire.endTransmission(true);            // Termina la trasmissione

  setupReadingInterrupt();               // Viene impostato l'interrupt globale per leggere i dati dal MPU6050 
}

void setupReadingInterrupt() {
  // Disabilita gli interrupt globali
  cli();

  // Imposta il timer 1 in modalità CTC
  TCCR1A = 0;
  TCCR1B = (1 << WGM12);
  TIMSK1 = (1 << OCIE1A);

  // Imposta il registro di confronto A per generare un interrupt ogni 500 ms
  OCR1A = 15624 * readingInterval;  // con prescaler 1024 ((16 MHz / 1024 = 15625 Hz) - 1) * intervalloInMs 

  
  TCCR1B |= (1 << CS12) | (1 << CS10); // Imposta il prescaler a 1024
  sei(); // Abilita gli interrupt globali
}

ISR(TIMER1_COMPA_vect){ // Interrupt che viene attivato ogni x millisecondi (x = readInterval)
  if(canReadNewSet) {
    canReadFromMPU = true;
    canReadNewSet = false; // Abilitare il flag canRead per permettere al codice di leggere dal MPU6050
  }
}

int16_t getDataFromRegister(char registerAddressHigh, char registerAddressLow) {
  // Il protocollo I2C permette la trasmissione di chuck di dati da 8bit mentre i valori del componente MCU6050 sono a 16 bit, il componente divide i valori in 2 registri da 8bit
  // Per ottenere i valori compelti dobbiamo unire i dati dei due registri

  byte registerH = 0;
  byte registerL = 0;

  // Prendo i dati dal registro High
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(registerAddressHigh);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 1, true); // Richiede dal MPU6050 un registro e chiude la connessione
  registerH = Wire.read();
  // Prendo i dati dal registro Low
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(registerAddressLow);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 1, true); // Richiede dal MPU6050 un registro e chiude la connessione
  registerL = Wire.read();

  // Compone una variabile a 16 bit contenente il valore dell'accelerometro X
  int16_t registerFull = registerH << 8 | registerL;

  return registerFull;
}

void getAccelerometerData(float &XAxisData, float &YAxisData, float &ZAxisData) { // Vegnono passati come parametri gli indirizzi di memoria delle varibili globali così da inseririre lì i valori
  // I valori vanno divisi per 2048 perchè ho impostato la sensibilità massima del dispositivo
  XAxisData = getDataFromRegister(XAccelH, XAccelL);
  XAxisData = (float) XAxisData / LBS_SENSITIVITY_ACCEL; 
  YAxisData = getDataFromRegister(YAccelH, YAccelL);
  YAxisData = (float) YAxisData / LBS_SENSITIVITY_ACCEL;
  ZAxisData = getDataFromRegister(ZAccelH, ZAccelL);
  ZAxisData = (float) ZAxisData / LBS_SENSITIVITY_ACCEL;
}

void getGyroscopeData(float &XGyroData, float &YGyroData, float &ZGyroData) { // Vegnono passati come parametri gli indirizzi di memoria delle varibili globali così da inseririre lì i valori
  // I valori vanno divisi per 2048 perchè ho impostato la sensibilità massima del dispositivo
  XGyroData = getDataFromRegister(XGyroH, XGyroL);
  XGyroData = (float) XGyroData / LBS_SENSITIVITY_GYRO; 
  YGyroData = getDataFromRegister(YGyroH, YGyroL);
  YGyroData = (float) YGyroData / LBS_SENSITIVITY_GYRO;
  ZGyroData = getDataFromRegister(ZGyroH, ZGyroL);
  ZGyroData = (float) ZGyroData / LBS_SENSITIVITY_GYRO;
}

void printDebug(float &XAxisData, float &YAxisData, float &ZAxisData, float &XGyroData, float &YGyroData, float &ZGyroData) {
  Serial.print("XAxisAccelerometer = ");
  Serial.print(XAxisData, DEC);
  Serial.print(" ");
  Serial.print("YAxisAccelerometer = ");
  Serial.print(YAxisData, DEC);
  Serial.print(" ");
  Serial.print("ZAxisAccelerometer = ");
  Serial.print(ZAxisData, DEC);
  Serial.println();

  Serial.print("XGyroAccelerometer = ");
  Serial.print(XGyroData, DEC);
  Serial.print(" ");
  Serial.print("YGyroAccelerometer = ");
  Serial.print(YGyroData, DEC);
  Serial.print(" ");
  Serial.print("ZGyroAccelerometer = ");
  Serial.print(ZGyroData, DEC);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
}

void handleInterrupt() {
  getAccelerometerData(XAxisData, YAxisData, ZAxisData); // Ottiene i dati dall'accelerometro e li mette nelle variabili globali
  getGyroscopeData(XGyroData, YGyroData, ZGyroData); // Ottiene i dati dal giroscopio e li mette nelle variabili globali
  ready = true;
}

void sendDataToDevice() {
    if(XGyroData > xHighThreshold) {
      Serial.println("PAGE UP");
    }else if(XGyroData < xLowThreshold) {
      Serial.println("PAGE DOWN");
    }

    if(ZGyroData < zLowThreshold) {
      Serial.println("DESTRA");      
    }else if (ZGyroData > zHighThreshold) {
      Serial.println("SINISTRA");
    }
}

void loop() {
  if(canReadFromMPU) {
    handleInterrupt();
    canReadNewSet = true;
  }

  if(ready) {
    //printDebug(XAxisData, YAxisData, ZAxisData, XGyroData, YGyroData, ZGyroData);
    sendDataToDevice();
    ready = false;
  }
}
