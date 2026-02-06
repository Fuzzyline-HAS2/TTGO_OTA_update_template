/*
  Main.ino - 모듈형 ESP32 시스템의 진입점

  이 파일은 프로젝트의 유일한 setup()과 loop() 함수를 포함합니다.
  모든 모듈은 init/update 패턴을 따르며, 여기서 호출됩니다.

  새 모듈 추가 방법:
  1. NewModule.ino 파일 생성
  2. initNewModule() 함수 정의
  3. updateNewModule() 함수 정의
  4. 아래 setup()에 initNewModule() 추가
  5. 아래 loop()에 updateNewModule() 추가
*/

void setup() {
  // 시리얼 통신 초기화
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n\n====================================");
  Serial.println("   ESP32 모듈형 시스템 시작");
  Serial.println("====================================\n");

  // 모듈 초기화 (순서 중요!)
  // OTA 모듈: WiFi 연결 및 펌웨어 업데이트 체크
  initOTA();

  // 여기에 새 모듈 초기화 추가
  // initNewModule();

  Serial.println("\n====================================");
  Serial.println("   모든 모듈 초기화 완료!");
  Serial.println("====================================\n");
}

void loop() {
  // 각 모듈의 업데이트 함수 호출

  // 여기에 새 모듈 업데이트 추가
  // updateNewModule();

  // 필요시 주기적으로 OTA 체크 (선택 사항)
  // static unsigned long lastOTACheck = 0;
  // if (millis() - lastOTACheck > 3600000) { // 1시간마다
  //   checkOTA();
  //   lastOTACheck = millis();
  // }
}
