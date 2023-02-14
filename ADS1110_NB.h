#pragma once

#include <Arduino.h>

namespace ADS1110_NB {

// SPS (samples per second)
enum DataRate { DR_15 = 0, DR_30 = 1, DR_60 = 2, DR_240 = 3 };

// PGA (programmable gain amplifier)
enum PGA { PGA_1 = 0, PGA_2 = 1, PGA_4 = 2, PGA_8 = 4 };

// SC (single or continuous)
enum Mode { Mode_Continuous = 0, Mode_Single = 1 };

struct Config {
  DataRate dataRate;
  PGA gainAmp;
  Mode mode;
  bool newData;  // set on read
  bool start;  // used on write
  String dataRateString();
  String gainAmpString();
  String modeString();
};

class ADS1110 {
 public:
  ADS1110(uint8_t addr) : m_addr(addr) {}
  ~ADS1110() {}
  bool begin();
  bool read(Config& config, float& data);
  bool writeConfig(Config& config);
  bool readData(int& output);

 private:
  uint8_t m_addr;
};

}  // namespace ADS1110_NB
