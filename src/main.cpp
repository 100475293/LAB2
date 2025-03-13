#include <Arduino.h>

// Definición de pines
#define LED_GREEN 25
#define LED_YELLOW 26
#define LED_RED 27
#define BUZZER 32
#define TOUCH_BUTTON 33  // Botón táctil en GPIO 33

// Definición de canal y frecuencia para el buzzer
#define BUZZER_CHANNEL 0  
#define BUZZER_FREQUENCY_GREEN 2000  // Hz en verde
#define BUZZER_FREQUENCY_RED 2500    // Hz en rojo
#define BUZZER_RESOLUTION 8  // Resolución del PWM (8 bits)

// Variables de estado
bool pedestrianRequest = false;
unsigned long lastGreenTime = 0;

void setup() {
    Serial.begin(115200);

    // Configurar pines como salida digital
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(TOUCH_BUTTON, INPUT);  // Leer el botón táctil como entrada

    // Apagar LEDs al inicio y forzar su configuración
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    // Prueba inicial: Parpadeo de los LEDs para verificar que funcionan
    Serial.println("Prueba inicial de LEDs...");
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_GREEN, HIGH);
        delay(500);
        digitalWrite(LED_GREEN, LOW);
        
        digitalWrite(LED_YELLOW, HIGH);
        delay(500);
        digitalWrite(LED_YELLOW, LOW);
        
        digitalWrite(LED_RED, HIGH);
        delay(500);
        digitalWrite(LED_RED, LOW);
    }

    // Configuración del buzzer con LEDC (PWM en ESP32)
    ledcDetachPin(BUZZER); // Asegurar que no haya conflictos en el pin del buzzer
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY_GREEN, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER, BUZZER_CHANNEL);

    Serial.println("Sistema listo.");
}

void loop() {
    // Estado inicial: Verde
    Serial.println("Encendiendo LED Verde");
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    Serial.println("Semáforo en VERDE. Esperando peatón...");
    unsigned long greenStartTime = millis();
    lastGreenTime = greenStartTime;

    while ((millis() - greenStartTime < 5000) || digitalRead(TOUCH_BUTTON) == LOW) {
        // Patrón de buzzer en verde
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQUENCY_GREEN);  
        delay(500);
        ledcWriteTone(BUZZER_CHANNEL, 0);  // Apagar buzzer
        delay(1500);

        if (digitalRead(TOUCH_BUTTON) == LOW) {  
            pedestrianRequest = true;
            Serial.println("¡Botón de peatón presionado!");
            break;
        }
    }

    // Amarillo (2s)
    Serial.println("Encendiendo LED Amarillo");
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
    delay(2000);

    // Rojo (10s)
    Serial.println("Encendiendo LED Rojo");
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);

    unsigned long redStartTime = millis();
    while (millis() - redStartTime < 10000) {
        // Patrón de buzzer en rojo
        ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQUENCY_RED);  
        delay(250);
        ledcWriteTone(BUZZER_CHANNEL, 0);  // Apagar buzzer
        delay(250);
    }

    // Rojo-amarillo (2s)
    Serial.println("Cambio a ROJO-AMARILLO...");
    digitalWrite(LED_YELLOW, HIGH);
    delay(2000);

    // Vuelve a verde
    Serial.println("Regresando a VERDE...");
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, HIGH);

    pedestrianRequest = false;  // Resetear la petición
}