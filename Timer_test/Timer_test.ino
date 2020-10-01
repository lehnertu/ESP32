#include "FreeRTOS.h"
#include "heltec.h"

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t isrCounter = 0;
volatile uint32_t last = micros();
volatile uint32_t minMicros = 1000;
volatile uint32_t maxMicros = 1000;

// the timer interrupt routine
void IRAM_ATTR onTimer()
{
    // FreeRTOS : disable ISR and other tasks
    portENTER_CRITICAL_ISR(&timerMux);
    // Increment the counter
    isrCounter++;
    uint32_t now = micros();
    uint32_t elapsed = now - last;
    last = now;
    if (elapsed<minMicros) minMicros=elapsed;
    if (elapsed>maxMicros) maxMicros=elapsed;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
    Serial.begin(115200);
    // use timer 0, prescaler 80, counting up from zero
    timer = timerBegin(0, 80, true);
    // attach onTimer function to our timer, edge type
    timerAttachInterrupt(timer, &onTimer, true);
    // set alarm, time in microseconds, repeat
    timerAlarmWrite(timer, 1000, true);
    // Start an alarm
    timerAlarmEnable(timer);
}

void loop() {
    // FreeRTOS : disable ISR and other tasks
    portENTER_CRITICAL(&timerMux);
    uint32_t cnt = isrCounter;
    isrCounter = 0;
    uint32_t mn = minMicros;
    uint32_t mx = maxMicros;
    minMicros = 1001;
    maxMicros =  999;
    portEXIT_CRITICAL(&timerMux);
    Serial.print("counting ");
    Serial.print(cnt);
    Serial.print("  jitter ");
    Serial.print(mn);
    Serial.print("...");
    Serial.print(mx);
    Serial.println(" µs");
    delay(1000);
}
