#include <FastLED.h>
#include <EEPROM.h>
#include <math.h>

#define LED_PIN     6
#define NUM_FANS    4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// عدّل عدد لمبات LED الفعلي لكل مروحة من المراوح الأربع
const int fanLedCounts[NUM_FANS] = {12, 18, 15, 15};
const int TOTAL_LEDS = fanLedCounts[0] + fanLedCounts[1] + fanLedCounts[2] + fanLedCounts[3];
#define NUM_LEDS TOTAL_LEDS

#define EEPROM_ADDR 0
#define MAGIC_BYTE  0xA5

struct SavedConfig {
  uint8_t magic;
  int mode;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t brightness;
  int speedVal;
};

CRGB leds[NUM_LEDS];
String cmd = "";

// الأوضاع:
// 0: ثابت, 1: قوس قزح, 2: تنفس, 3: تفاعل صوتي, 4: شهاب, 5: نجوم,
// 6: موجة, 7: إطفاء, 8: ذهاب وإياب, 9: نبض مزدوج, 10: مسح لوني, 11: ستروب,
// 12: لون مستقل لكل مروحة من الـ 4, 13: انتقال سلس عبر المراوح الأربع
int mode = 0;
CRGB currentColor = CRGB(0, 255, 255);
uint8_t hue = 0;

// ألوان المراوح الأربع (مروحة 1 إلى 4)
CRGB fanColors[NUM_FANS] = {
  CRGB(255, 0, 80),
  CRGB(0, 200, 255),
  CRGB(0, 255, 120),
  CRGB(255, 140, 0)
};

// متغيرات الصوت (FFT)
uint8_t bassLevel = 0;
uint8_t trebleLevel = 0;

// متغيرات الحركة والسرعة
int cometPos = 0;
int scannerPos = 0;
int scannerDir = 1;
int wipePos = 0;
bool strobeState = false;
float fanFlowOffset = 0.0f;

int speedVal = 50; // 1 (أبطأ) إلى 100 (أسرع)
unsigned long lastStepTime = 0;

void getFanRange(int fanIdx, int &startIdx, int &endIdx) {
  startIdx = 0;
  for (int f = 0; f < fanIdx; f++) {
    startIdx += max(0, fanLedCounts[f]);
  }
  endIdx = startIdx + max(0, fanLedCounts[fanIdx]);
  if (endIdx > NUM_LEDS) {
    endIdx = NUM_LEDS;
  }
}

void loadConfigFromEEPROM() {
  SavedConfig cfg;
  EEPROM.get(EEPROM_ADDR, cfg);

  if (cfg.magic == MAGIC_BYTE) {
    mode = constrain(cfg.mode, 0, 13);
    currentColor = CRGB(cfg.r, cfg.g, cfg.b);
    FastLED.setBrightness(cfg.brightness);
    speedVal = constrain(cfg.speedVal, 1, 100);
  } else {
    mode = 0;
    currentColor = CRGB(0, 255, 255);
    FastLED.setBrightness(200);
    speedVal = 50;
  }
}

void saveConfigToEEPROM() {
  SavedConfig cfg;
  cfg.magic = MAGIC_BYTE;
  cfg.mode = mode;
  cfg.r = currentColor.r;
  cfg.g = currentColor.g;
  cfg.b = currentColor.b;
  cfg.brightness = FastLED.getBrightness();
  cfg.speedVal = speedVal;

  EEPROM.put(EEPROM_ADDR, cfg);
}

void handleCommand(String line) {
  line.trim();
  if (line.length() == 0) return;

  if (line.startsWith("C:")) {
    int r = 0, g = 0, b = 0;
    if (sscanf(line.c_str() + 2, "%d,%d,%d", &r, &g, &b) == 3) {
      currentColor = CRGB(
        constrain(r, 0, 255),
        constrain(g, 0, 255),
        constrain(b, 0, 255)
      );
      mode = 0;
    }
  } else if (line.startsWith("F4:")) {
    int r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4;
    int parsed = sscanf(
      line.c_str() + 3,
      "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
      &r1, &g1, &b1, &r2, &g2, &b2, &r3, &g3, &b3, &r4, &g4, &b4
    );
    if (parsed == 12) {
      fanColors[0] = CRGB(constrain(r1, 0, 255), constrain(g1, 0, 255), constrain(b1, 0, 255));
      fanColors[1] = CRGB(constrain(r2, 0, 255), constrain(g2, 0, 255), constrain(b2, 0, 255));
      fanColors[2] = CRGB(constrain(r3, 0, 255), constrain(g3, 0, 255), constrain(b3, 0, 255));
      fanColors[3] = CRGB(constrain(r4, 0, 255), constrain(g4, 0, 255), constrain(b4, 0, 255));
      mode = 12;
    }
  } else if (line.startsWith("M:")) {
    mode = constrain(line.substring(2).toInt(), 0, 13);
    if (mode == 10) {
      wipePos = 0;
    }
  } else if (line.startsWith("B:")) {
    FastLED.setBrightness(constrain(line.substring(2).toInt(), 0, 255));
  } else if (line.startsWith("S:")) {
    speedVal = constrain(line.substring(2).toInt(), 1, 100);
  } else if (line.startsWith("F:")) {
    int bVal = 0, tVal = 0;
    if (sscanf(line.c_str() + 2, "%d,%d", &bVal, &tVal) == 2) {
      bassLevel = constrain(bVal, 0, 255);
      trebleLevel = constrain(tVal, 0, 255);
    }
  } else if (line.startsWith("K:")) {
    int r = 0, g = 0, b = 0;
    if (sscanf(line.c_str() + 2, "%d,%d,%d", &r, &g, &b) == 3) {
      currentColor = CRGB(constrain(r, 0, 255), constrain(g, 0, 255), constrain(b, 0, 255));
      // بدون تغيير mode
    }
  } else if (line == "SAVE") {
    saveConfigToEEPROM();
    Serial.println("SAVED_OK");
  }
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  // حماية الكمبيوتر: الحد الأقصى 5 فولت و 2000 ملي أمبير
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  loadConfigFromEEPROM();
  FastLED.show();
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      handleCommand(cmd);
      cmd = "";
    } else if (c != '\r') {
      cmd += c;
    }
  }

  unsigned long stepInterval = map(speedVal, 1, 100, 120, 8);
  unsigned long now = millis();

  if (now - lastStepTime >= stepInterval || mode == 0 || mode == 3 || mode == 7 || mode == 12) {
    lastStepTime = now;

    if (mode == 0) {
      // ثابت
      fill_solid(leds, NUM_LEDS, currentColor);
    } else if (mode == 1) {
      // قوس قزح
      fill_rainbow(leds, NUM_LEDS, hue++, 7);
    } else if (mode == 2) {
      // تنفس
      uint8_t breath = beatsin8(map(speedVal, 1, 100, 8, 55), 30, 255);
      fill_solid(leds, NUM_LEDS, currentColor);
      nscale8(leds, NUM_LEDS, breath);
    } else if (mode == 3) {
      // تفاعل صوتي (Bass + Treble)
      int activeLeds = map(bassLevel, 0, 255, 0, NUM_LEDS);
      fill_solid(leds, NUM_LEDS, CRGB::Black);

      uint8_t baseHue = hue + map(trebleLevel, 0, 255, 0, 120);
      for (int i = 0; i < activeLeds; i++) {
        uint8_t ledHue = baseHue + map(i, 0, max(1, NUM_LEDS - 1), 0, 90);
        leds[i] = CHSV(ledHue, 255, 255);
      }

      if (trebleLevel > 90) {
        uint8_t sparkles = map(trebleLevel, 90, 255, 1, 5);
        for (uint8_t s = 0; s < sparkles; s++) {
          int idx = random16(NUM_LEDS);
          leds[idx] = CRGB::White;
        }
      }
      hue++;
    } else if (mode == 4) {
      // شهاب متحرك
      fadeToBlackBy(leds, NUM_LEDS, 45);
      leds[cometPos] = currentColor;
      cometPos = (cometPos + 1) % NUM_LEDS;
    } else if (mode == 5) {
      // نجوم / وميض عشوائي
      fadeToBlackBy(leds, NUM_LEDS, 20);
      if (random8() < 35) {
        int idx = random16(NUM_LEDS);
        leds[idx] = currentColor;
      }
    } else if (mode == 6) {
      // موجة لونية متدفقة
      for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t wave = sin8((i * 8) + (hue * 2));
        CRGB c = currentColor;
        c.nscale8(wave);
        leds[i] = c;
      }
      hue++;
    } else if (mode == 7) {
      // إطفاء كامل
      fill_solid(leds, NUM_LEDS, CRGB::Black);
    } else if (mode == 8) {
      // ذهاب وإياب
      fadeToBlackBy(leds, NUM_LEDS, 55);
      leds[scannerPos] = currentColor;
      scannerPos += scannerDir;
      if (scannerPos <= 0) {
        scannerPos = 0;
        scannerDir = 1;
      } else if (scannerPos >= NUM_LEDS - 1) {
        scannerPos = NUM_LEDS - 1;
        scannerDir = -1;
      }
    } else if (mode == 9) {
      // نبض مزدوج متعاكس
      fadeToBlackBy(leds, NUM_LEDS, 50);
      int posA = scannerPos;
      int posB = (NUM_LEDS - 1) - scannerPos;
      leds[posA] = currentColor;
      leds[posB] = CHSV(hue, 255, 255);
      scannerPos = (scannerPos + 1) % NUM_LEDS;
      hue += 2;
    } else if (mode == 10) {
      // مسح لوني متدرج
      if (wipePos == 0) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
      }
      leds[wipePos] = currentColor;
      wipePos = (wipePos + 1) % NUM_LEDS;
    } else if (mode == 11) {
      // وميض / ستروب
      strobeState = !strobeState;
      fill_solid(leds, NUM_LEDS, strobeState ? currentColor : CRGB::Black);
    } else if (mode == 12) {
      // لون مستقل لكل مروحة حسب عدد لمباتها الفعلي
      for (int f = 0; f < NUM_FANS; f++) {
        int startIdx = 0, endIdx = 0;
        getFanRange(f, startIdx, endIdx);
        for (int i = startIdx; i < endIdx; i++) {
          leds[i] = fanColors[f];
        }
      }
    } else if (mode == 13) {
      // انتقال سلس للألوان عبر المراوح الأربع بتوزيع لمبات غير متساوٍ
      float speedStep = map(speedVal, 1, 100, 1, 18) * 0.015f;
      fanFlowOffset += speedStep;
      if (fanFlowOffset >= NUM_FANS) {
        fanFlowOffset -= NUM_FANS;
      }

      for (int i = 0; i < NUM_LEDS; i++) {
        int currentFan = 0;
        float fanProgress = 0.0f;
        int acc = 0;
        for (int f = 0; f < NUM_FANS; f++) {
          int cnt = max(1, fanLedCounts[f]);
          if (i < acc + cnt || f == NUM_FANS - 1) {
            currentFan = f;
            fanProgress = constrain((float)(i - acc) / (float)cnt, 0.0f, 1.0f);
            break;
          }
          acc += cnt;
        }

        float fanCoord = fmod(((float)currentFan + fanProgress) + fanFlowOffset, (float)NUM_FANS);
        if (fanCoord < 0) fanCoord += NUM_FANS;

        int idxA = (int)fanCoord % NUM_FANS;
        int idxB = (idxA + 1) % NUM_FANS;
        float frac = fanCoord - (int)fanCoord;

        uint8_t blendAmt = (uint8_t)constrain((int)(frac * 255.0f), 0, 255);
        leds[i] = blend(fanColors[idxA], fanColors[idxB], blendAmt);
      }
    }

    FastLED.show();
  }
}
