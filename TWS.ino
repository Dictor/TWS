#include <Arduino_FreeRTOS.h>
#include <SimpleDHT.h>
#include "config.h"

/* Task declaration */
void tskSendStaticSensor(void *pvParameters);
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
  char data;
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

}

void loop() {

}

void tskSendStaticSensor(void *pvParameters) {
  (void) pvParameters;
  int dhterr;

  for (;;) {
    /* Read static sensors value */
    LastStaticSensorValue.time = millis();
    dhterr = SimpleDHTErrSuccess;
    if ((dhterr = dht.read(&(LastStaticSensorValue.temp), &(LastStaticSensorValue.humi), NULL)) != SimpleDHTErrSuccess) {
      LastStaticSensorValue.humi = -1; // -1 of humi indicates dht sensor has error
    }
    LastStaticSensorValue.sound = analogRead(PIN_SOUND);
    LastStaticSensorValue.water = analogRead(PIN_WATER); //ADC의 입력 임피던스에 (10k<) 따라 첫번쨰 값을 버려야 할 수도 있다

    /* Send Data through BT */
    SendAllData();
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
        AddDynamicSensorEvent(KIND_GLARE, LastAntennaCount);
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
    LastGlareValue += abs((((lastestCds.vertical - lastestCds.vertical) +
                          (lastestCds.horizon1 - lastestCds.horizon1) +
                          (lastestCds.horizon2 - lastestCds.horizon2) +
                          (lastestCds.horizon3 - lastestCds.horizon3)) / timeDeriv) / 4);
    if (millis() - LastGlareValueResetTime > CALIB_CDS_RESET_MS) {
      if (LastGlareValue >= CALIB_CDS_DERIV_LIMIT) {
        AddDynamicSensorEvent(KIND_ANTENNA, LastGlareValue);
      }
      LastGlareValueResetTime = millis();
      LastGlareValue = 0;
    }
    LastCdsValue = lastestCds;
  }
}

void AddDynamicSensorEvent(DynamicSensorKind evtkind, int data) {
  DynamicSensorEvent addevt;
  addevt.kind = evtkind;
  addevt.time = millis();
  addevt.data = data;
  xQueueSendToBack(DynamicSensorEvents, &addevt, 0);
}

void SendAllData() {
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

  /* Send dynamic sensor event data*/
  DynamicSensorEvent nowevent;
  while(xQueueReceive(DynamicSensorEvents, &nowevent, 0) == pdPASS)) {
    Serial.print(F('EVENT,'));
    Serial.print(nowevent->time);
    Serial.print(F(","));
    Serial.print(nowevent->kind);
    Serial.print(F(","));
    Serial.println(nowevent->data);
  }
}