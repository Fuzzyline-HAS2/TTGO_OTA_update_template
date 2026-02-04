# 📘 Signed OTA 통합 사용자 가이드

이 문서는 **Signed OTA (보안 무선 업데이트)** 템플릿의 모든 기능을 다루는 통합 가이드입니다.
새 프로젝트 생성부터 설정, 배포, 문제 해결까지 모든 내용을 담고 있습니다.

---

## 📑 목차
1.  **시작하기 (Getting Started)**
    *   [옵션 A] 기존 프로젝트 사용하기
    *   [옵션 B] 새 프로젝트 만들기 (GitHub Template)
2.  **설정 (Configuration)**
    *   필수 도구 설치
    *   보안 키 생성 (필수!)
    *   코드 설정 (WiFi & URL)
3.  **배포하기 (Deployment)**
    *   자동 배포 (추천)
    *   수동 빌드 및 서명
4.  **고급 사용법**
    *   여러 대의 기기 관리
5.  **문제 해결 (Troubleshooting)**

---

## 1. 시작하기 (Getting Started)

### [옵션 A] 기존 프로젝트 사용하기
이미 만들어진 이 저장소를 그대로 사용하여 테스트하려면, 저장소를 **Clone** 또는 **Download ZIP** 하여 PC에 저장하세요.

### [옵션 B] 새 프로젝트 만들기 (GitHub Template) 🚀
이 저장소를 '틀(Template)'로 삼아 나만의 새로운 기기용 저장소를 만들 수 있습니다.

1.  GitHub 웹사이트의 이 저장소 페이지에서 **"Use this template"** 버튼 -> **"Create a new repository"**를 클릭합니다.
2.  새 저장소 이름(예: `My_Smart_Home_V1`)을 짓고 생성합니다.
3.  생성된 내 저장소를 PC로 **Clone** 해옵니다.
    > **팁**: 이렇게 하면 기존 코드를 복사/붙여넣기 할 필요 없이 바로 시작할 수 있습니다!

---

## 2. 설정 (Configuration)

### 2.1 필수 도구 설치
*   **VS Code** 및 **Arduino Extension** (또는 Arduino IDE)
*   **Python 3.x**: [공식 홈페이지](https://www.python.org/downloads/)에서 설치 (설치 시 "Add Python to PATH" 체크 필수)
*   **필수 라이브러리**: 터미널에서 아래 명령어 실행
    ```bash
    pip install cryptography
    ```

### 2.2 보안 키 생성 (⚠️ 중요)
보안을 위해 **반드시** 자신만의 키를 생성해야 합니다. (템플릿을 그대로 쓰면 키가 공개되어 위험합니다.)

터미널을 열고 다음 명령어를 실행하세요:
```bash
python scripts/generate_keys.py
```
*   실행하면 `private_key.pem` (비공개 키)과 `SignedOTA/public_key.h` (공개 키)가 새로 생성됩니다.
*   **주의**: `private_key.pem` 파일은 절대 남에게 공유하거나 GitHub에 올리지 마세요! (자동으로 `.gitignore` 처리됩니다.)

### 2.3 코드 설정 (`SignedOTA/UserConfig.h`)
`SignedOTA/UserConfig.h` 파일을 열어 다음 내용을 수정합니다.

```cpp
// 1. 와이파이 설정
const char *ssid = "내_와이파이_이름";
const char *password = "와이파이_비번";

// 2. 펌웨어 다운로드 주소
// [중요] 내 GitHub 저장소의 'Raw' 파일 주소를 적어야 합니다.
const char *firmware_url = "https://raw.githubusercontent.com/내아이디/내저장소/main/update.bin";
```

---

## 3. 배포하기 (Deployment)

### 3.1 자동 배포 (추천) ⚡
코드를 수정하고 전 세계의 기기들을 업데이트하고 싶다면, **스크립트 하나만 실행**하면 됩니다.

1.  터미널에서 아래 명령어 입력:
    ```bash
    python scripts/deploy.py
    ```
2.  스크립트가 시키는 대로 진행:
    *   자동으로 버전(`CURRENT_FIRMWARE_VERSION`)이 1 올라갑니다.
    *   **"컴파일 하세요"** 문구가 뜨면 VS Code에서 **Verify(컴파일)** 버튼을 누릅니다.
    *   컴파일이 끝나면 터미널에서 **Enter** 키를 누릅니다.
3.  **결과**: 자동으로 파일이 서명되고, 이름이 바뀌고, GitHub에 업로드됩니다. 기기들이 자동으로 업데이트됩니다.

### 3.2 수동 빌드 및 서명 (고급 사용자용)
수동으로 과정을 이해하고 싶다면 다음 순서로 진행합니다.
1.  아두이노 IDE에서 'Export Compiled Binary'로 빌드합니다.
2.  `scripts/sign_firmware.py`를 이용해 서명합니다.
    ```bash
    python scripts/sign_firmware.py <입력파일.bin> private_key.pem update.bin
    ```
3.  생성된 `update.bin`을 GitHub에 직접 커밋 & 푸시합니다.

---

## 4. 고급 사용법

### 여러 대의 기기 관리 (Multi-Device)
기기 A와 기기 B가 서로 다른 동작을 해야 한다면?

1.  **GitHub에 파일을 따로 올립니다.**
    *   예: `device_A_update.bin`, `device_B_update.bin`
2.  **각 기기의 `UserConfig.h`를 다르게 설정합니다.**
    *   기기 A: `firmware_url = ".../device_A_update.bin"`
    *   기기 B: `firmware_url = ".../device_B_update.bin"`
3.  **배포할 때**: 원하는 기기의 코드를 수정하고, 해당 파일 이름으로 빌드하여 업로드하면 **그 기기들만** 업데이트됩니다.

---

## 5. 문제 해결 (Troubleshooting)

### Q. "Connection Refused" 또는 "Timeout" 오류가 떠요.
*   **원인**: GitHub 서버와 연결하는 데 시간이 너무 오래 걸려서 ESP32가 포기한 경우입니다.
*   **해결**: `SignedOTA.ino`의 `client.setHandshakeTimeout(30000);` 부분이 있는지 확인하세요. 타임아웃을 30초 이상으로 늘려야 합니다.

### Q. "Space Not Enough" (공간 부족) 오류가 떠요.
*   **원인**: GitHub에 올라간 파일이 너무 클 때(예: 4MB 통합 바이너리) 발생합니다.
*   **해결**: `scripts/deploy.py`를 사용하여 배포하세요. 이 스크립트는 자동으로 용량이 작은(1MB 내외) OTA 전용 파일을 선택하여 올립니다.

### Q. "Signature Mismatch" (서명 불일치) 오류가 떠요.
*   **원인**: 기기에 들어있는 `public_key.h`와, 배포된 파일을 서명한 `private_key.pem`이 서로 다른 쌍일 때 발생합니다.
*   **해결**: `python scripts/generate_keys.py`를 다시 실행하고, 기기에 USB로 펌웨어를 다시 업로드(Upload)하여 키를 맞춰주세요.
