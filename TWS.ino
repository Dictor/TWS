#include <Arduino_FreeRTOS.h>
#include "pin_config.h"

/* Task declaration */
void tskReadAnalog(void *pvParameters);
void tskReadDHT(void *pvParameters);
void tskSendData(void *pvParameters);
void tskCalculateGlare(void *pvParameters);

/* Value struct declaration */
typedef struct {
  int vertical;
  int horizon1;
  int horizon2;
  int horizon3;
} CdsValue;

typedef struct {
  long time;
  int value;
} SensorValue;

void setup() {

}

void loop() {

}
