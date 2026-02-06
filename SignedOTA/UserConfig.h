#ifndef USER_CONFIG_H
#define USER_CONFIG_H

// ==========================================
// 사용자 설정 (USER CONFIGURATION)
// ==========================================

// 1. 와이파이 설정
const char *ssid = "SK_DA20_2.4G";
const char *password = "GGA48@6587";

// 2. 펌웨어 다운로드 주소
// 펌웨어 파일(.bin)이 있는 인터넷 주소(URL)를 입력해주세요.
// 주의: 반드시 'raw' 주소여야 합니다. (브라우저에서 주소를 쳤을 때 파일이 바로
// 다운로드되어야 함)
const char *firmware_url = "https://raw.githubusercontent.com/Fuzzyline-HAS2/"
                           "TTGO_update_test/main/update.bin";

// 버전 정보 파일 URL (version.txt)
const char *version_url = "https://raw.githubusercontent.com/Fuzzyline-HAS2/"
                          "TTGO_update_test/main/version.txt";

// 3. 디버그 및 버전 정보
#define CURRENT_FIRMWARE_VERSION 1

// ==========================================
#endif
