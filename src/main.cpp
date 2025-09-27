#include <Arduino.h>

const int audioPin = A0;
const int ledAmarelo = 11;   // PB3 – Pino físico 17
const int ledVermelho = 12;  // PB4 – Pino físico 18
const int ledVerde = 13;     // PB5 – Pino físico 19
const int buzzerPin = 5;     // PD5 – Pino físico 11
const int buttonPin = 10;    // PB2 – Pino físico 16

const int threshold = 515;
const unsigned long timeout = 20000;           // 20 segundos
const unsigned long longPressDuration = 2000;  // 2 segundos

unsigned long lastSignalTime = 0;
unsigned long buttonPressTime = 0;
bool alarmActive = false;
bool systemOn = true;
bool buttonWasPressed = false;
bool longPressHandled = false;

void setup() {
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);  // Com debounce externo
  Serial.begin(9600);
  lastSignalTime = millis();
  digitalWrite(ledVerde, HIGH);  // Sistema ligado
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  unsigned long currentTime = millis();

  // Verifica botão pressionado
  if (buttonState == HIGH) {
    if (!buttonWasPressed) {
      buttonPressTime = currentTime;
      buttonWasPressed = true;
      longPressHandled = false;  // nova flag
    } else {
      // Se já está pressionado e passou de 3s
      if (!longPressHandled && currentTime - buttonPressTime >= longPressDuration) {
        systemOn = !systemOn;
        digitalWrite(ledVerde, systemOn ? HIGH : LOW);
        alarmActive = false;
        digitalWrite(ledVermelho, LOW);
        digitalWrite(buzzerPin, LOW);
        longPressHandled = true;  // evita repetição
      }
    }
  }

  // Botão liberado
  if (buttonState == LOW && buttonWasPressed) {
    unsigned long pressDuration = currentTime - buttonPressTime;
    buttonWasPressed = false;

    if (pressDuration < longPressDuration) {
      // Toque curto: desliga alarme
      alarmActive = false;
      digitalWrite(ledVermelho, LOW);
      digitalWrite(buzzerPin, LOW);
      lastSignalTime = currentTime;
    }
  }

  if (systemOn) {
    int audioValue = analogRead(audioPin);
    Serial.println(audioValue);

    // LED amarelo pisca com sinal
    if (audioValue > threshold) {
      digitalWrite(ledAmarelo, HIGH);
      lastSignalTime = currentTime;
    } else {
      digitalWrite(ledAmarelo, LOW);
    }

    // Verifica ausência de sinal
    if (!alarmActive && currentTime - lastSignalTime > timeout) {
      alarmActive = true;
      digitalWrite(ledVermelho, HIGH);
      digitalWrite(buzzerPin, HIGH);
    }
  } else {
    // Sistema desligado: tudo apagado
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  delay(1);  // Amostragem rápida
}
