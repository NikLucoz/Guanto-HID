# Progetto

Realizzare un programma che legge dati dal MCU 6050 interfacciandosi direttamente (senza usare librerie specifiche) tramite interfaccia I2C ed utilizzarli per controllare delle gesture specifiche sudi un dispositivo Windows.

## Codice e simulazione:

La simulazione del dispositivo si trovano su Wokwi a questo indirizzo:

[Progetto Trojani (Arduino Nano) - Wokwi ESP32, STM32, Arduino Simulator](https://wokwi.com/projects/365074812174370817)

Il codice dello script in Python per eseguire le gesture è disponibile qui:

# Analisi del problema:

Il dispositivo dovrà essere in grado di leggere i dati del giroscopio (utilizzando un interrupt per la lettura) e di inviarli tramite interfaccia seriale USB ad un computer dove uno script Python eseguirà le gesture.

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled.png)

## **Funzionamento protocollo I2C**

Il protocollo **I2C** (Inter-Integrated Circuit) specifica le regole di comunicazione fra più dispositivi collegati ad un bus formato da due linee.

Le due linee, chiamate “**scl**” e “**sda**” rispettivamente, trasportano la tempistica di sincronizzazione (chiamata anche “clock”) e i dati.

Le modalità di trasferimento dati possono essere riassunte come segue:

**A trasmette dati a B**

1. A(master) spedisce l’indirizzo di B(slave) sul bus
2. A trasmette i dati a B
3. A termina il trasferimento

**A vuole ricevere dati da B**

1. A (master) spedisce l’indirizzo di B(slave) sul bus
2. B spedisce i dati ad A
3. A termina il trasferimento

## Funzionamento e descrizione MPU6050

L'**MPU6050** è un componente elettronico che integra un accelerometro a 3 assi e un giroscopio a 3 assi in un singolo chip. 

È ampiamente utilizzato in applicazioni che richiedono la misurazione dell'orientamento, del movimento e dell'accelerazione.

L'accelerometro nel **MPU6050** misura l'accelerazione lungo gli assi X, Y e Z, fornendo dati sulla forza di gravità e sull'accelerazione lineare. Questi dati possono essere utilizzati per determinare l'orientamento dell'unità e rilevare i movimenti del dispositivo.

Il giroscopio nel **MPU6050** misura la velocità angolare di rotazione attorno agli assi X, Y e Z.

Questi dati consentono di rilevare i cambiamenti di angolo o la velocità di rotazione del dispositivo.

Il **MPU6050** comunica con un microcontrollore o un microprocessore tramite un'interfaccia di comunicazione seriale come **I2C**. Utilizzando questo protocollo, il microcontrollore può richiedere dati all'**MPU6050** e configurare le sue impostazioni.

Per questo progetto verrà utilizzato il microcontrollore **Arduino Nano**.

## FASE 1 (Connessione Arduino Nano con MPU6050)

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled%201.png)

La connessione del dispositivo **MPU6050** ad un **Arduino Nano** è molto semplice.

I cavi da collegare sono i seguenti:

- Collega il pin VCC dell'MPU6050 al pin 3.3V dell'Arduino Nano per l'alimentazione.
- Collega il pin GND dell'MPU6050 al pin GND dell'Arduino Nano per il collegamento a terra.
- Collega il pin SDA dell'MPU6050 al pin A4 dell'Arduino Nano. Questo è il pin di dati seriale per la comunicazione I2C.
- Collega il pin SCL dell'MPU6050 al pin A5 dell'Arduino Nano. Questo è il pin di clock seriale per la comunicazione I2C.

## FASE 2 (Configurazione MPU6050)

### Inviare il segnale di reset al MCU6050

L'indirizzo **I2C** del **MPU6050** è definito come **0x68** quindi bisognerà contattare il dispositivo tramite questo indirizzo utilizzando la libreria `Wire.h`.

Successivamente tramite l’indirizzo **0x6B** andremo a configuare il dispositivo per renderlo utilizzabile**.**

Questo registro (**0x6B)** consente all'utente di configurare la modalità di alimentazione e la sorgente del clock. Fornisce anche un bit per ripristinare l'intero dispositivo e un bit per disabilitare il sensore di temperatura.

Questo è lo schema del registro **0x6B:**

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled%202.png)

Per inviare un segnale di reset bisogna mettere il **Bit7** ad **1**, inoltre mettendo il **Bit6** a **0** è possibile disattivare il low power mode del dispositivo.

Quindi il valore da inviare è **10000000** o **0x80** in Hex.

# INSERIRE FLOWCHART

- **[ESTRATTO DI CODICE E SPIEGAZIONE]**
    
    Questo è un estratto del codice integrale che mostra il processo di reset:
    
    ```arduino
    #include <Wire.h>
    
    #define MPU_ADDRESS 0x68  // Indirizzo I2C del MPU6050
    #define MPU_PWR_MGT 0x6B  // Registro per il controllo dell'alimentazione
    
    void setup() {
      Wire.begin();  // Inizializza la comunicazione I2C
    	Serial.begin(115200);
    
      // Invia il segnale di reset al MPU6050
      Wire.beginTransmission(MPU_ADDRESS);  // Inizia la trasmissione verso l'indirizzo del MPU6050
      Wire.write(MPU_PWR_MGT);              // Seleziona il registro per il controllo dell'alimentazione
      Wire.write(0x80);                     // Invia il valore 0x80 (0b10000000) per impostare il bit di reset
    																				// 
      Wire.endTransmission(true);           // Termina la trasmissione e invia il segnale di reset
    }
    
    void loop() {
      // RESTO DEL CODICE
    }
    ```
    
    Nella funzione **`setup()`**, viene inizializzata la comunicazione I2C utilizzando **`Wire.begin()`**. Successivamente, viene avviata una trasmissione verso l'indirizzo del MPU6050 utilizzando **`Wire.beginTransmission(MPU_ADDRESS)`**. 
    
    Viene selezionato il registro per il controllo dell'alimentazione utilizzando **`Wire.write(MPU_PWR_MGT)`**. 
    
    Infine, viene inviato il valore **`0x80`** (**10000000**) per impostare il bit di reset nel registro di controllo dell'alimentazione. 
    
    La trasmissione viene terminata utilizzando **`Wire.endTransmission(true)`**.
    

### Configurazione giroscopio e accelerometro

Tramite l’indirizzo **0x1B** e **0x1C** andremo a configuare rispettivamente il sensore del giroscopio e accelerometro**.**

Per configurare i due sensori andia ad inviare ai due registri una serie di bit per impostare le modalità **Full Scale Range**:

Le modalità **Full Scale Range** del MPU6050 si riferiscono alla gamma completa di valori di misurazione supportati dagli accelerometri e giroscopi presenti nel componente. 

In altre parole, la **Full Scale Range** indica l'intervallo massimo di misurazione per ciascun asse dei sensori.

Nel caso dell'accelerometro, il **Full Scale Range** determina la massima accelerazione lineare che può essere misurata lungo ciascun asse (X, Y, Z). Ad esempio, se il **Full Scale Range** dell'accelerometro è impostato su ±2g, significa che può rilevare accelerazioni nell'intervallo compreso tra -2g e +2g.

Per quanto riguarda il giroscopio, il **Full Scale Range** indica la massima velocità angolare misurabile attorno a ciascun asse. Se il **Full Scale Range** del giroscopio è impostato su ±250°/s, significa che può rilevare velocità angolari nell'intervallo compreso tra -250°/s e +250°/s.

Schema registro **1B** ovvero Giroscopio:

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled%203.png)

Schema registro **1C** ovvero Accelerometro:

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled%204.png)

Per andare a configurare entrambi i **Range** al massimo bisogna modificare i **Bit4** e **Bit3** e impostarli secondo queste due tabelle:

**Tabella per il Giroscopio**

**Tabella per l’Accelerometro**

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled%205.png)

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled%206.png)

Quindi per impostare entrambi i **Full Scale Range** al massimo dobbiamo inviare al registro questa serie di bit **00011000** o ****0x18**** in Hex.

In questo modo entrambi abbiamo impostato entrambi i full scale a 3, ovvero il massimo.

# INSERIRE FLOWCHART

- **[ESTRATTO DI CODICE]**
    
    Questo è un estratto del codice integrale che mostra quanto detto in precedenza con l’aggiunta di questa nuova configurazione:
    
    ```arduino
    #include <Wire.h>
    
    #define MPU_ADDRESS 0x68  // Indirizzo I2C del MPU6050
    #define MPU_PWR_MGT 0x6B  // Registro per il controllo dell'alimentazione
    #define MPU_GYRO_CONFIG 0x1B   // Registro di configurazione del giroscopio
    #define MPU_ACCEL_CONFIG 0x1C  // Registro di configurazione dell'accelerometro
    
    // Resto del codice
    
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
    
      // Resto del codice
    }
    
    void loop() {
      // Resto del codice
    }
    ```
    

## FASE 3 (Configurazione dell’Interrupt)

Per effettuare la lettura cadenzata dal **MPU6050** per ottenere i dati dei sensori possiamo utilizzare il sistema di Interrupt fornito da **Arduino**.

Quando si configura un interrupt su **Arduino**, è necessario seguire alcuni passaggi fondamentali. 

In primo luogo, è necessario selezionare il timer appropriato, ad esempio **Timer0**, **Timer1** o **Timer2**, a seconda delle esigenze specifiche. In questo caso utilizzeremo il **Timer1.**

Successivamente, è necessario impostare il **prescaler**, che determina la frequenza di clock del timer. Il **prescaler** è un divisore di frequenza che riduce la velocità del **clock** del timer rispetto alla velocità del **clock** del microcontrollore. È possibile scegliere un valore adeguato per il **prescaler**, in modo da ottenere la frequenza di **clock** desiderata per il timer.

Il prescaler che utilizzeremo è **1024**, quindi verrà aumentato il valore del timer ogni 1024 cicli di **clock** (il **Clock** di 1024 **Arduino** è **16Mhz**).

Una volta configurato il **prescaler**, è necessario impostare il valore di confronto per l'interrupt **Compare Match**. 

L'interrupt **Compare Match** su **Arduino** è una funzionalità che consente di generare un'interruzione quando un valore di confronto specificato corrisponde al valore attuale di un timer hardware. 

Per utilizzare l'interrupt **Compare Match** su **Arduino**, è necessario configurare i registri del timer appropriati per impostare il valore di confronto desiderato. 

I registri da impostare per il **Compare Match** bisogna configurare i seguenti registri:

- TCCR1A → il registro va impostato a 0
- TCCR1B → il **Bit3 (WGM12)**  va impostato a 1
- 

Ora bisogna impostare il registro i confronto **(OCR1A).**

Il registro OCR1A determina il valore di confronto del timer. Quando il valore del timer raggiunge questo valore, viene generato un interrupt.

Per calcolare il valore approrpriato per effetuare l’interrupt possiamo usare questa formula:

$OCR1A = (\dfrac{frequenzaTimer * TempoDesideratoInMillisecondi}{prescaler}) - 1$

Ora è necessario impostare il **prescaler** a 1024, per far ciò andiamo a modificare il registro TCCR1B.

Schema dei bit necessari per il prescaler del registro **TCCR1B**

![Untitled](Progetto%20aba06e50264e4eca85b2195337a1ab8c/Untitled%207.png)

Per impostarlo a 1024 basterà cambiare i bit **CS12** e **CS10** ad **1**

Quando il timer raggiunge il valore di confronto, verrà generato un interrupt, che interromperà il flusso di esecuzione del programma principale e avvierà una funzione di interrupt specifica definita dall'utente, chiamata **interrupt handler**.

Tutta la configurazione dell’interrupt viene effettuata dentro una funzione dedicata chiamata `setupReadingInterrupt()`.

- **[ESTRATTO DI CODICE]**
    
    Questo è un estratto del codice integrale che mostra quanto detto in precedenza con l’aggiunta di questa nuova configurazione:
    
    ```arduino
    #include <Wire.h>
    
    #define MPU_ADDRESS 0x68  // Indirizzo I2C del MPU6050
    #define MPU_PWR_MGT 0x6B  // Registro per il controllo dell'alimentazione
    #define MPU_GYRO_CONFIG 0x1B   // Registro di configurazione del giroscopio
    #define MPU_ACCEL_CONFIG 0x1C  // Registro di configurazione dell'accelerometro
    
    // Resto del codice
    
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
      OCR1A = 15624 * readingInterval;  // con prescaler 1024 ((16 MHz * intervalloInMs) / 1024) - 1) 
    
      TCCR1B |= (1 << CS12) | (1 << CS10); // Imposta il prescaler a 1024
      sei(); // Abilita gli interrupt globali
    }
    
    void loop() {
      // Resto del codice
    }
    ```
    

## FASE 4 (Raccolta dati dal MPU6050 e invio segnali)

# INSERIRE FLOWCHART

- **[ESTRATTO DI CODICE]**
    
    Questo è il codice finale che mostra quanto detto in precedenza con l’aggiunta della raccolta dei dati e l’invio dei segnali tramite seriale.
    
    ```arduino
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
    
    bool canRead = true;
    bool canDisplayData = false;
    
    // VARIABILI DATI GLOBALI
    float XAxisData = 0;
    float YAxisData = 0;
    float ZAxisData = 0;
    
    float XGyroData = 0;
    float YGyroData = 0;
    float ZGyroData = 0;
    
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
      canDisplayData = true;
    }
    
    ISR(TIMER1_COMPA_vect){ // Interrupt che viene attivato ogni x millisecondi (x = readInterval)
      canRead = true; // Abilitare il flag canRead per permettere al codice di leggere dal MPU6050
    }
    
    void loop() {
      if(canRead) {
        handleInterrupt();
        canRead = false;
      }
    
      if(canDisplayData) {
        printDebug(XAxisData, YAxisData, ZAxisData, XGyroData, YGyroData, ZGyroData);
        canDisplayData = false;
      }
    }
    ```
