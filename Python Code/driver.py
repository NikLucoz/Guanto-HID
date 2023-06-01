import serial
import keyboard

# Imposta la porta seriale e il baud rate
porta_seriale = 'COM3'  # Sostituisci con la tua porta seriale corretta
baud_rate = 9600

# Apre la connessione seriale
ser = serial.Serial(porta_seriale, baud_rate)
if not ser.isOpen():
    ser.open()
    ser.nonblocking()
print('com3 is open', ser.isOpen())
while True:
    # Legge una linea di dati dalla porta seriale
    linea_dati = ser.readline().decode().strip()

    match linea_dati:
        case "PAGEUP":
            keyboard.write('up', delay=0)
        case "PAGEDOWN":
            keyboard.write('down',delay=0)
        case "STOP":
            # Chiude la connessione seriale
            ser.close()
            exit()


