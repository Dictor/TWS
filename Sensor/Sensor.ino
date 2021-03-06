#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <SimpleDHT.h>
#include "config.h"

/* Task declaration */
void tskSendStaticSensor(void *pvParameters);
void tskSendEvent(void *pvParameters);
void tskMonitorDynamicSensor(void *pvParameters);


/* Value struct pre-declaration */
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
  long time;
  int data;
} DynamicSensorEvent;


/*Global variable declaration*/
CdsValue LastCdsValue; //가장 최근 Cds 값
int LastGlareValue; //가장 최근 주기 Cds 누적 변화값
long LastGlareValueResetTime; //가장 최근 Cds 주기 재설정 시간
byte LastAntennaCount; //가장 최근 주기 섬광 감시 횟수
long LastAntennaCountResetTime; //가장 최근 섬광 주기 재설정 시간
QueueHandle_t DynamicSensorEvents; //동적 센서 이벤트 적재 큐 
StaticSensorValue LastStaticSensorValue; //가장 최근 정적 센서 값

SimpleDHT11 dht(PIN_DHT);


void setup() {
  Serial.begin(9600);
  DynamicSensorEvents = xQueueCreate(10, sizeof(DynamicSensorEvent));
  if (DynamicSensorEvents != NULL) {
    xTaskCreate(tskSendStaticSensor, "SendStaticSensor", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(tskMonitorDynamicSensor, "MonitorDynamicSensor", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(tskSendEvent, "SendEvent", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    Serial.println(F("ERROR,OS_INIT_SUCCESS"));
  } else {
    Serial.println(F("ERROR,OS_INIT_FAIL"));
  }
}

void loop() {
  vTaskStartScheduler();
  for(;;) Serial.println(F("ERROR,SCHEDULER_FAIL"));
}

void tskSendStaticSensor(void *pvParameters) {
  (void) pvParameters;
  int dhterr;

  for (;;) {
    /* Read static sensors value */
    LastStaticSensorValue.time = millis();
    dhterr = SimpleDHTErrSuccess;
    byte tmpTemp, tmpHumi;
    if ((dhterr = dht.read(&tmpTemp, &tmpHumi, NULL)) != SimpleDHTErrSuccess) {
      LastStaticSensorValue.humi = -1; // -1 of humi indicates dht sensor has error
    } else {
      LastStaticSensorValue.temp = (int)tmpTemp;
      LastStaticSensorValue.humi = (int)tmpHumi;
    }
    LastStaticSensorValue.sound = analogRead(PIN_SOUND);
    LastStaticSensorValue.water = analogRead(PIN_WATER); //ADC의 입력 임피던스에 (10k<) 따라 첫번쨰 값을 버려야 할 수도 있다

    /* Send Data through BT */
    SendStaticData();
    vTaskDelay(TERM_STATICSENSOR_SEND_MS / portTICK_PERIOD_MS);
  }
}

void tskMonitorDynamicSensor(void *pvParameters) { 
  (void) pvParameters;

  for (;;) {
    /* Monitor Glare Antenna */
    if (abs(CALIB_ANT_MIDDLE - analogRead(PIN_ANTTENA)) >= CALIB_ANT_DERIV_LIMIT) LastAntennaCount++;
    if (millis() - LastAntennaCountResetTime > CALIB_ANT_RESET_MS) {
      if (LastAntennaCount >= CALIB_ANT_DERIV_LIMIT) {
        AddDynamicSensorEvent(KIND_ANTENNA, LastAntennaCount);
      }
      LastAntennaCountResetTime = millis();
      LastAntennaCount = 0;
    }

    CdsValue lastestCds;
    lastestCds.time = millis();
    lastestCds.vertical = analogRead(PIN_CDS_VERTICAL);
    lastestCds.horizon1 = analogRead(PIN_CDS_HORIZON1);
    lastestCds.horizon2 = analogRead(PIN_CDS_HORIZON2);
    lastestCds.horizon3 = analogRead(PIN_CDS_HORIZON3);
    int timeDeriv = lastestCds.time - LastCdsValue.time;
    LastGlareValue += abs((((lastestCds.vertical - LastCdsValue.vertical) +
                          (lastestCds.horizon1 - LastCdsValue.horizon1) +
                          (lastestCds.horizon2 - LastCdsValue.horizon2) +
                          (lastestCds.horizon3 - LastCdsValue.horizon3)) / timeDeriv) / 4);
    if (millis() - LastGlareValueResetTime > CALIB_CDS_RESET_MS) {
      if (LastGlareValue >= CALIB_CDS_DERIV_LIMIT) {
        AddDynamicSensorEvent(KIND_GLARE, LastGlareValue);
      }
      LastGlareValueResetTime = millis();
      LastGlareValue = 0;
    }
    LastCdsValue = lastestCds;
  }
}

void tskSendEvent(void *pvParameters) {
  (void) pvParameters;

  for (;;) {
    /* Send Data through BT */
    SendEventData();
    vTaskDelay(TERM_EVENT_SEND_MS / portTICK_PERIOD_MS);
  }
}

void AddDynamicSensorEvent(DynamicSensorKind evtkind, int data) {
  DynamicSensorEvent addevt;
  addevt.kind = evtkind;
  addevt.time = millis();
  addevt.data = data;
  if (xQueueSendToBack(DynamicSensorEvents, &addevt, 0) == errQUEUE_FULL) Serial.println("ERROR,OS_QUEUE_FULL");
}

void SendStaticData() {
  /* Send static sensor data */
  Serial.print(F("STATIC,"));
  Serial.print(LastStaticSensorValue.temp);
  Serial.print(F(","));
  Serial.print(LastStaticSensorValue.humi);
  Serial.print(F(","));
  Serial.print(LastStaticSensorValue.sound);
  Serial.print(F(","));
  Serial.print(LastStaticSensorValue.water);
  Serial.print(F(","));
  Serial.println(LastStaticSensorValue.time);
}

void SendEventData() {
  /* Send dynamic sensor event data*/
  DynamicSensorEvent nowevent;
  while(xQueueReceive(DynamicSensorEvents, &nowevent, 0) == pdPASS) {
    Serial.print(F("EVENT,"));
    Serial.print(nowevent.time);
    Serial.print(F(","));
    Serial.print(nowevent.kind == KIND_ANTENNA ? F("ANT") : F("CDS"));
    Serial.print(F(","));
    Serial.println(nowevent.data, DEC);
  }
}
