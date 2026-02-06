# 🚀 ESP32 OTA Update Template

**GitHub에서 펌웨어를 관리하고 ESP32를 무선으로 업데이트하세요!**

[![GitHub](https://img.shields.io/badge/GitHub-Public-green)](https://github.com/Fuzzyline-HAS2/TTGO_update_test)

---

## 📋 이게 뭔가요?

이 프로젝트를 사용하면:
1. ESP32에 펌웨어를 **USB로 한 번만** 업로드
2. 이후 코드 수정 시 **GitHub에 올리기만** 하면
3. ESP32가 **자동으로 새 펌웨어 다운로드** 및 업데이트!

> **USB 케이블 없이 원격 업데이트!** WiFi만 있으면 됩니다.

---

## ⚡ 5분 시작 가이드

### 1단계: 저장소 복사

```bash
git clone https://github.com/Fuzzyline-HAS2/TTGO_update_test.git
cd TTGO_update_test
```

또는 GitHub에서 **"Use this template"** 버튼으로 자신의 저장소 생성

> ⚠️ **중요:** 저장소는 반드시 **Public**이어야 합니다!

---

### 2단계: WiFi 설정

`SignedOTA/UserConfig.h` 파일 수정:

```cpp
// 1. 와이파이 설정
const char *ssid = "내_WiFi_이름";
const char *password = "WiFi_비밀번호";

// 2. GitHub 주소 (자신의 저장소로 변경)
const char *firmware_url = "https://raw.githubusercontent.com/내아이디/내저장소/main/update.bin";
const char *version_url = "https://raw.githubusercontent.com/내아이디/내저장소/main/version.txt";

// 3. 현재 버전 (나중에 변경하면 업데이트됨)
#define CURRENT_FIRMWARE_VERSION 1
```

---

### 3단계: 첫 업로드 (USB)

1. **Arduino IDE**에서 `SignedOTA/Main.ino` 열기
2. **Tools** 설정:
   | 항목 | 값 |
   |------|-----|
   | Board | `ESP32 Dev Module` 또는 `ESP32S3 Dev Module` |
   | Partition Scheme | `Minimal SPIFFS (1.9MB APP with OTA)` ⚠️ 중요! |
   | Upload Speed | `115200` |
3. **Upload** (Ctrl+U)

---

### 4단계: 동작 확인

Serial Monitor (115200 baud):

```
ESP32 모듈형 시스템 시작
[OTA 모듈] 와이파이 연결 성공!
[OTA 모듈] ✅ 서버와 동일한 버전 (v1)
```

**축하합니다! 🎉** 이제 OTA 업데이트 준비 완료!

---

## 🔄 코드 수정 후 업데이트하는 법

### 방법: deploy.py 사용 (권장) ⚡

```bash
python scripts/deploy.py
```

**자동으로 처리되는 것들:**
1. ✅ 버전 번호 +1 증가
2. ✅ "컴파일 하세요" 메시지 → **Arduino IDE에서 Verify 클릭**
3. ✅ Enter 키 입력
4. ✅ GitHub에 자동 업로드

**끝!** ESP32를 재부팅하면 새 펌웨어가 자동 설치됩니다.

---

## 📂 프로젝트 구조

```
TTGO_update_test/
├── SignedOTA/
│   ├── Main.ino           # 메인 파일 (setup/loop)
│   ├── SignedOTA.ino      # OTA 업데이트 로직 (수정 불필요)
│   └── UserConfig.h       # ⭐ WiFi/URL 설정
├── scripts/
│   └── deploy.py          # 배포 자동화 스크립트
├── version.txt            # 서버 버전 (자동 관리됨)
└── update.bin             # 펌웨어 파일 (자동 생성됨)
```

---

## ➕ 자신의 코드 추가하기

### 예시: LED 깜빡이기 추가

1. **새 파일 생성:** `SignedOTA/MyLED.ino`

```cpp
#define LED_PIN 2

void initMyLED() {
  pinMode(LED_PIN, OUTPUT);
  Serial.println("[MyLED] 초기화 완료");
}

void updateMyLED() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  delay(500);
}
```

2. **Main.ino 수정:**

```cpp
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  initOTA();      // OTA 모듈 (필수!)
  initMyLED();    // ← 추가!
}

void loop() {
  updateMyLED();  // ← 추가!
}
```

3. **배포:**
```bash
python scripts/deploy.py
```

---

## ❓ 자주 묻는 질문

### Q1. "404 Not Found" 에러

**원인:** GitHub 저장소가 Private

**해결:** Settings → Change visibility → **Make public**

---

### Q2. OTA 다운로드가 1분 넘게 걸려요

**정상입니다!** WiFi로 1MB 파일 다운로드는 1~2분 소요됩니다.

---

### Q3. 버전이 같은데 계속 업데이트해요

**원인:** `UserConfig.h`와 `version.txt`의 버전이 다름

**해결:** `python scripts/deploy.py` 실행하면 자동 동기화됨

---

### Q4. 버전을 낮출 수 있나요? (예: v5 → v1)

**네!** 버전이 "다르면" 무조건 업데이트됩니다. (높든 낮든 상관없음)

---

## 🛡️ 안전 기능

- ✅ 다운로드 실패 시 재부팅 안함
- ✅ 파일 크기 검증
- ✅ 업데이트 실패 시 자동 롤백
- ✅ WiFi 연결 실패 시 자동 재시도

---

## 💡 핵심 팁

| 항목 | 내용 |
|------|------|
| 저장소 | 반드시 **Public** |
| Partition | **Minimal SPIFFS with OTA** 필수 |
| 첫 업로드 | USB로 1회 |
| 이후 업데이트 | `deploy.py` 실행만 하면 끝 |

---

**만든이:** Fuzzyline-HAS2  
**최종 업데이트:** 2026-02-06
