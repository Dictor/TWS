#include <Arduino_FreeRTOS.h>
#include "pin_config.h"

/* Task declaration */
void tskSendStaticSensor(void *pvParameters);
void tskMonitorDynamicSensor(void *pvParameters);

/* Value struct declaration */
typedef struct _CdsValue {
  long time;
  int vertical;
  int horizon1;
  int horizon2;
  int horizon3;
} CdsValue;

typedef struct _StaticSensorValue {
  long time;
  int water;
  int sound;
  int temp;
  int humi;
} StaticSensorValue;

enum DynamicSensorKind {
  KIND_GLARE,
  KIND_ANTENNA
};

typedef struct _DynamicSensorEvent {
  DynamicSensorKind kind;
  char data;
} DynamicSensorEvent;

/*Global variable declaration*/
QueueHandle_t CdsDerivative; //Cds 변화율 적재 큐
CdsValue LastCdsValue; //가장 최근 Cds 값
byte LastGlareCount; //가장 최근 주기 섬광 감시 횟수
QueueHandle_t DynamicSensorEvents; //동적 센서 이벤트 적재 큐 
StaticSensorValue LastStaticSensorValue; //가장 최근 정적 센서 값

void setup() {

}

void loop() {

}