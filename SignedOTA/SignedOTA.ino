/*
 * SimpleOTA: 초간단 클라우드 업데이트 (Cloud OTA) 예제
 *
 * 이 코드는 인터넷상의 URL에서 펌웨어 파일(.bin)을 다운로드하고
 * 장치를 자동으로 업데이트하는 기능을 보여줍니다.
 *
 * [핵심 기능]
 * 1. 지정된 URL(Github 등)에서 펌웨어를 다운로드합니다.
 * 2. 복잡한 인증 절차 없이 바로 업데이트를 진행합니다.
 * 3. UserConfig.h 파일 설정만으로 누구나 쉽게 사용할 수 있습니다.
 */

#include "public_key.h"
#include <HTTPClient.h>
#include <Update.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "UserConfig.h"

// 해시 알고리즘 선택
#define USE_SHA256

// 서명 알고리즘 선택
#define USE_RSA

// =======================================================

// URL에서 펌웨어를 다운로드하고 OTA 업데이트를 실행하는 함수
void execOTA() {
  // URL 유효성 검사
  if (String(firmware_url).indexOf("http") < 0 ||
      String(firmware_url).indexOf("REPLACE") >= 0) {
    Serial.println("❌ 오류: 유효한 firmware_url을 설정해주세요!");
    return;
  }

  Serial.println("클라우드 OTA 업데이트를 시작합니다...");
  Serial.println("대상 URL: " + String(firmware_url));

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure(); // GitHub와 같이 HTTPS를 사용하는 경우 SSL 인증서 검증
  client.setHandshakeTimeout(30000); // [중요] 핸드쉐이크 타임아웃 30초 설정

  http.begin(client, String(firmware_url));
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setTimeout(30000); // [중요] HTTP 타임아웃 30초 설정

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      int contentLength = http.getSize();
      Serial.printf("Download Size: %d bytes\n", contentLength);

      if (contentLength <= 0) {
        Serial.println("❌ Error: Content-Length is invalid (0 or missing)");
        http.end();
        return;
      }

      // OTA 업데이트 시작 가능 여부 확인
      // Update.begin()은 파티션 크기를 확인하고 업데이트 준비를 합니다.
      bool canBegin = Update.begin(contentLength);

      if (canBegin) {
        Serial.println("OTA 업데이트를 시작합니다. 잠시만 기다려주세요...");

        // 다운로드된 스트림 데이터를 업데이트 파티션에 직접 기록합니다.
        size_t written = Update.writeStream(http.getStream());

        if (written == contentLength) {
          Serial.println("Written : " + String(written) + " successfully");
        } else {
          Serial.println("Written only : " + String(written) + "/" +
                         String(contentLength) + ". Retry?");
        }

        if (Update.end()) {
          Serial.println("✅ OTA 완료!");
          if (Update.isFinished()) {
            Serial.println(
                "업데이트가 성공적으로 완료되었습니다. 재부팅합니다...");
            ESP.restart();
          } else {
            Serial.println(
                "업데이트가 완전히 종료되지 않았습니다. 문제가 발생했습니다.");
          }
        } else {
          Serial.println("❌ 오류 발생. 오류 코드: " +
                         String(Update.getError()));
        }
      } else {
        Serial.println("❌ OTA를 시작할 공간이 부족합니다.");
      }
    } else {
      Serial.printf("❌ HTTP connect failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
    }
  } else {
    Serial.printf("❌ GET failed, error: %s\n",
                  http.errorToString(httpCode).c_str());
  }
  http.end();
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=================================");
  Serial.println("SimpleOTA - 초간단 URL 업데이트 예제");
  Serial.println("=================================\n");
  Serial.println("시스템 부팅 중...");

  // 와이파이 연결 시작
  Serial.print("와이파이 연결 중: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // 연결될 때까지 최대 20초간 대기합니다.
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 10) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n와이파이 연결 성공!");
    Serial.print("할당된 IP 주소: ");
    Serial.println(WiFi.localIP());

    // [테스트용] 와이파이 연결 후 5초 뒤에 자동으로 OTA를 시도합니다.
    // 실제 제품에서는 버튼을 누르거나 특정 MQTT 명령을 받았을 때 실행하도록
    // 수정할 수 있습니다.
    Serial.println("5초 후 OTA 업데이트를 시도합니다...");
    delay(5000);
    execOTA();
  } else {
    Serial.println("\n❌ 와이파이 연결 실패! 아이디와 비밀번호를 확인하세요.");
  }
}

void loop() {
  // 아무것도 하지 않음
  Serial.println("야호");
  delay(1000);
}
