#include <Arduino.h>

const int audioPin = A0;
const int signalLedPin = 13;    // LED que pisca com o sinal
const int alarmLedPin = 12;     // LED de alarme (alto brilho)
const int buttonPin = 7;        // Botão com debounce externo
const int threshold = 515;
const unsigned long timeout = 20000; // 20 segundos

unsigned long lastSignalTime = 0;
bool alarmActive = false;

void setup() {
  pinMode(signalLedPin, OUTPUT);
  pinMode(alarmLedPin, OUTPUT);
  pinMode(buttonPin, INPUT); // Botão com resistor externo
  Serial.begin(9600);
  lastSignalTime = millis();
}

void loop() {
  int audioValue = analogRead(audioPin);
  Serial.println(audioValue);

  // Pisca o LED do pino 13 conforme o sinal
  if (audioValue > threshold) {
    digitalWrite(signalLedPin, HIGH);
    lastSignalTime = millis(); // Reinicia temporizador
  } else {
    digitalWrite(signalLedPin, LOW);
  }

  // Verifica se passou 60 segundos sem sinal forte
  if (!alarmActive && millis() - lastSignalTime > timeout) {
    alarmActive = true;
    digitalWrite(alarmLedPin, HIGH); // Acende LED de alarme
  }

  // Verifica se botão foi pressionado para resetar alarme
  if (alarmActive && digitalRead(buttonPin) == HIGH) { // HIGH = botão pressionado
    alarmActive = false;
    digitalWrite(alarmLedPin, LOW); // Apaga LED de alarme
    lastSignalTime = millis();      // Reinicia temporizador
  }

  delay(1); // Amostragem rápida
}
