/*
  [통합 코드: 엔코더 밝기 제어 & 네오픽셀 타이머 애니메이션]

  1. 기능 설명:
     - 엔코더(12, 13번): 0~40 범위로 위치를 조절합니다.
     - 네오픽셀(6번): 엔코더 값에 따라 밝기(0~255)가 조절됩니다.
     - 타이머 인터럽트: 2초마다 흰색 <-> 무지개색 모드가 자동으로 전환됩니다.

  2. 주요 기술:
     - Robust State Machine (엔코더): 싱글코어/듀얼코어 모두에서 정확한 동작
  보장.
     - Timer Interrupt (애니메이션): 메인 루프 지연과 상관없이 정확한 타이밍에
  모드 전환.
*/

#include <Adafruit_NeoPixel.h>

// ---------------------------
// 1. 핀 및 상수 설정
// ---------------------------
// 네오픽셀
#define NEOPIXEL_PIN 22
#define PIXEL_COUNT 6
#define BRIGHTNESS_MAX 255

// 엔코더 (사용자 지정 핀)
const int PIN_CLK = 12;
const int PIN_DT = 13;

// 객체 생성
Adafruit_NeoPixel strip(PIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// ---------------------------
// 2. 전역 변수 (Volatile: 인터럽트용)
// ---------------------------
// [엔코더 관련]
volatile uint8_t old_AB = 0;    // 핀 상태 기록용
volatile long encoderCount = 0; // 원시 스텝 카운트 (X4)
volatile int encoderPos = 10;   // 최종 위치 (0 ~ 40)
int lastReportedPos = -1;       // 로그 출력용

// 엔코더 상태 테이블 (X4 Quadrature)
static const int8_t enc_states[] = {0,  -1, 1, 0, 1, 0, 0,  -1,
                                    -1, 0,  0, 1, 0, 1, -1, 0};

// [네오픽셀 타이머 관련]
volatile int mode = 0; // 0: 흰색, 1: 무지개
volatile bool modeChanged = false;
const uint64_t timerPeriod = 2000000; // 2초 (마이크로초)
hw_timer_t *timer = NULL;

// [애니메이션 관련]
uint8_t rainbowBrightness = 120;
long firstPixelHue = 0;
float currentBlend = 0.0;       // 0.0(흰색) ~ 1.0(무지개)
const float blendSpeed = 0.005; // 부드러운 전환 속도

// ---------------------------
// 3. 인터럽트 서비스 루틴 (ISR)
// ---------------------------

// A. 엔코더 처리 ISR (정밀 제어)
void IRAM_ATTR isr_encoder() {
  old_AB <<= 2;
  // CLK(12)와 DT(13) 상태 읽기
  uint8_t state = (digitalRead(PIN_CLK) << 1) | digitalRead(PIN_DT);
  old_AB |= state;
  old_AB &= 0x0f; // 하위 4비트 마스킹

  if (enc_states[old_AB] != 0) {
    encoderCount += enc_states[old_AB];

    // 4스텝 = 1칸 (X4 디코딩)
    int newPos = encoderCount >> 2;

    // 범위 제한 (0 ~ 40)
    if (newPos < 0) {
      encoderPos = 0;
      encoderCount = 0;
    } else if (newPos > 40) {
      encoderPos = 40;
      encoderCount = 40 * 4;
    } else {
      encoderPos = newPos;
    }
  }
}

// B. 타이머 처리 ISR (모드 전환)
void IRAM_ATTR onTimer() {
  if (mode == 0) {
    mode = 1;
  } else {
    mode = 0;
  }
  modeChanged = true;
}

// ---------------------------
// 4. 초기화 및 유틸리티
// ---------------------------
void timerSetting() {
  // ESP32 Core 3.0+ 문법 (timerBegin(주파수))
  // 만약 컴파일 에러가 나면 timerBegin(0, 80, true); 로 변경해보세요 (구버전)
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, timerPeriod, true, 0);
}

// 색상 혼합 함수 (Ratio: 0.0 ~ 1.0)
uint32_t mixColors(uint32_t color1, uint32_t color2, float ratio) {
  if (ratio <= 0.0)
    return color1;
  if (ratio >= 1.0)
    return color2;

  uint8_t r1 = (uint8_t)(color1 >> 16), g1 = (uint8_t)(color1 >> 8),
          b1 = (uint8_t)color1;
  uint8_t r2 = (uint8_t)(color2 >> 16), g2 = (uint8_t)(color2 >> 8),
          b2 = (uint8_t)color2;

  uint8_t r = (uint8_t)(r1 + (r2 - r1) * ratio);
  uint8_t g = (uint8_t)(g1 + (g2 - g1) * ratio);
  uint8_t b = (uint8_t)(b1 + (b2 - b1) * ratio);

  return strip.Color(r, g, b);
}

// ---------------------------
// 5. 메인 함수 (통합용 이름 변경)
// ---------------------------
void initNeoPixel() {
  Serial.println("[NeoPixel 모듈] 초기화 시작...");

  // 네오픽셀 초기화
  strip.begin();
  strip.show();

  // 엔코더 핀 설정
  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT, INPUT_PULLUP);

  // 엔코더 인터럽트 연결 (양방향 감지)
  attachInterrupt(digitalPinToInterrupt(PIN_CLK), isr_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_DT), isr_encoder, CHANGE);

  // 타이머 설정
  timerSetting();

  Serial.println(
      "[NeoPixel 모듈] 엔코더(12,13)로 밝기 조절, 2초마다 자동 색상 변경");
  Serial.println("[NeoPixel 모듈] 초기화 완료\n");
}

void updateNeoPixel() {
  // -------------------------
  // A. 엔코더 값으로 밝기 설정
  // -------------------------
  // 0~40 값을 0~255 범위로 변환 (Map)
  int brightness = map(encoderPos, 0, 40, 0, 255);
  strip.setBrightness(brightness);

  // 엔코더 상태 로깅
  if (encoderPos != lastReportedPos) {
    Serial.print("현재 위치: ");
    Serial.print(encoderPos);
    Serial.print(" -> 밝기: ");
    Serial.println(brightness);
    lastReportedPos = encoderPos;
  }

  // -------------------------
  // B. 모드 전환 로깅
  // -------------------------
  if (modeChanged) {
    modeChanged = false;
    if (mode == 0)
      Serial.println("Switching to WHITE...");
    else
      Serial.println("Switching to RAINBOW...");
  }

  // -------------------------
  // C. 애니메이션 블렌딩 처리
  // -------------------------
  float targetBlend = (mode == 1) ? 1.0 : 0.0;

  if (currentBlend < targetBlend) {
    currentBlend += blendSpeed;
    if (currentBlend > targetBlend)
      currentBlend = targetBlend;
  } else if (currentBlend > targetBlend) {
    currentBlend -= blendSpeed; // 감소 로직
    if (currentBlend < targetBlend)
      currentBlend = targetBlend;
  }

  // 무지개 흐름 업데이트
  firstPixelHue += 256;
  if (firstPixelHue >= 5 * 65536) {
    firstPixelHue = 0;
  }

  // -------------------------
  // D. 픽셀 색상 출력
  // -------------------------
  for (int i = 0; i < PIXEL_COUNT; i++) {
    // 1. 무지개색 계산
    int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
    uint32_t rainbowColor = strip.gamma32(strip.ColorHSV(pixelHue, 255, 255));

    // 2. 흰색 정의
    uint32_t whiteColor = strip.Color(255, 255, 255); // RGB White

    // 3. 최종 색상 혼합 (블렌딩)
    uint32_t finalColor = mixColors(whiteColor, rainbowColor, currentBlend);

    strip.setPixelColor(i, finalColor);
  }

  strip.show();
  delay(10);
}
