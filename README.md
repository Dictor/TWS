# 뇌우 & 기상 감지 스테이션 (TWS)
* 2019년 군장병 공개SW 역량강화 집체교육 산출물
* 공군 일병 김정현 / 팀 하늘을 보아라

## 개요
TWS는 뇌우(섬광, 전자기장)와 기상(온습도, 강수량)을 감지해 사용자에게 웹으로 알려주는 장치입니다.
정보를 수집하는 센서 모듈, 센서 모듈의 정보를 DB에 삽입하는 서버 (Router), DB 정보를 시각화하는 웹으로 구성되어 있습니다.


## 폴더 구조 
* **/Sensor** : 센서 모듈 펌웨어 (Arduino Uno)
* **/Router** : 서버 스크립트
* **/Web** : 서버 웹 

## 요구 사항 (의존성)
### **/Sensor/Sensor.ino**
* FreeRTOS 10 : https://github.com/feilipu/Arduino_FreeRTOS_Library (?)
* SimpleDHT : https://github.com/winlinvip/SimpleDHT (MIT)
### **/Router/main.py**
* Python3 (recommend 3.7 =<)
* pyserial : https://github.com/pyserial/pyserial (MIT)
### **/Web/*.php**
* PHP7 (recommend 7.3 =<)
* sqlite3
