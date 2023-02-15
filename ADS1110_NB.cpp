#include "ADS1110_NB.h"

#include <Wire.h>
#include <trace.h>

#define TRACE_PRE "ads1110 | "
#define READ_SIZE 3

// TI datasheet page 11:
// write 1 in single mode to start, read 0 indicates new data
#define CONFIG_BIT_ST_DRDY 7  // write: start, read: ready
#define CONFIG_BIT_SC 4       // continuous or single mode
#define CONFIG_BIT_DR1 3
#define CONFIG_BIT_DR0 2
#define CONFIG_BIT_PGA1 1
#define CONFIG_BIT_PGA0 0

namespace ADS1110_NB {

void intToBin(int d, u_int8_t* buff, u_int8_t start, u_int8_t len) {
  for (int i = start + len - 1; i >= start; i--) {
    buff[i] = d & 1;
    d >>= 1;
  }
}

int binToInt(u_int8_t* buff, u_int8_t start, u_int8_t len) {
  int d = 0;
  for (int i = start; i < start + len; i++) {
    d <<= 1;
    if (buff[i]) {
      d |= 1;
    }
  }
  return d;
}

bool ADS1110::begin() {
  Wire.begin();
  Wire.beginTransmission(m_addr);
  uint8_t r = Wire.endTransmission();
  TRACE_VA(TRACE_DEBUG2, TRACE_PRE "init response: %d", r);
  return r == 0;
}

bool ADS1110::read(Config& config, float& output) {
  uint8_t request = Wire.requestFrom(m_addr, (uint8_t)READ_SIZE);
  TRACE_VA(TRACE_DEBUG2, TRACE_PRE "request result: %d", request);

  if (request != READ_SIZE) {
    TRACE(TRACE_DEBUG2, TRACE_PRE "unexpected reqest result");
    return false;
  }

  uint8_t buffer[READ_SIZE];
  uint8_t read = Wire.readBytes(buffer, READ_SIZE);
  TRACE_VA(TRACE_DEBUG2, TRACE_PRE "data read result: %d", read);

  if (read != READ_SIZE) {
    TRACE(TRACE_DEBUG2, TRACE_PRE "unexpected read result");
    return false;
  }

  const uint8_t a = buffer[0];
  const uint8_t b = buffer[1];
  const uint8_t c = buffer[2];

  TRACE_VA(  //
      TRACE_DEBUG2,
      TRACE_PRE "registers: output[a]=%d, output[b]=%d, config=%d",  //
      a, b, c);

  output = a * 16.0 + b / 16.0;

  config.newData = (bool)(((c >> CONFIG_BIT_ST_DRDY) & 1) == 0);
  config.mode = (Mode)((c >> CONFIG_BIT_SC) & 1);

  uint8_t dr[2];
  dr[0] = (c >> CONFIG_BIT_DR0) & 1;
  dr[1] = (c >> CONFIG_BIT_DR1) & 1;
  config.dataRate = (DataRate)binToInt(dr, 0, 2);

  uint8_t pga[2];
  pga[0] = (c >> CONFIG_BIT_PGA0) & 1;
  pga[1] = (c >> CONFIG_BIT_PGA1) & 1;
  config.gainAmp = (PGA)binToInt(pga, 0, 2);

  return true;
}

bool ADS1110::writeConfig(Config& config) { return false; }

String Config::dataRateString() {
  switch (dataRate) {
    case DR_15:
      return String(F("15"));
    case DR_30:
      return String(F("30"));
    case DR_60:
      return String(F("60"));
    case DR_240:
      return String(F("240"));
    default:
      return String(F("?"));
  }
}

String Config::gainAmpString() {
  switch (gainAmp) {
    case PGA_1:
      return String(F("1x"));
    case PGA_2:
      return String(F("2x"));
    case PGA_4:
      return String(F("4x"));
    case PGA_8:
      return String(F("8x"));
    default:
      return String(F("?"));
  }
}

String Config::modeString() {
  switch (mode) {
    case Mode_Single:
      return String(F("single"));
    case Mode_Continuous:
      return String(F("continuous"));
    default:
      return String(F("?"));
  }
}

}  // namespace ADS1110_NB
