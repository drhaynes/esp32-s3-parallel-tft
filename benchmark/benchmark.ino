//Graph test
#define LGFX_USE_V1

#include <LovyanGFX.hpp>
#include <vector>

#define LCD_CS 37
#define LCD_BLK 45

#define LINE_COUNT 6

class LGFX : public lgfx::LGFX_Device
{
    // lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_Parallel16 _bus_instance; // 8ビットパラレルバスのインスタンス (ESP32のみ)

public:
    // コンストラクタを作成し、ここで各種設定を行います。
    // クラス名を変更した場合はコンストラクタも同じ名前を指定してください。
    LGFX(void)
    {
        {                                      // バス制御の設定を行います。
            auto cfg = _bus_instance.config(); // バス設定用の構造体を取得します。

            // 16位设置
            cfg.port = 0;              // 使用するI2Sポートを選択 (0 or 1) (ESP32のI2S LCDモードを使用します)
            cfg.freq_write = 20000000; // 送信クロック (最大20MHz, 80MHzを整数で割った値に丸められます)
            cfg.pin_wr = 35;           // WR を接続しているピン番号
            cfg.pin_rd = 48;           // RD を接続しているピン番号
            cfg.pin_rs = 36;           // RS(D/C)を接続しているピン番号

            cfg.pin_d0 = 47;
            cfg.pin_d1 = 21;
            cfg.pin_d2 = 14;
            cfg.pin_d3 = 13;
            cfg.pin_d4 = 12;
            cfg.pin_d5 = 11;
            cfg.pin_d6 = 10;
            cfg.pin_d7 = 9;
            cfg.pin_d8 = 3;
            cfg.pin_d9 = 8;
            cfg.pin_d10 = 16;
            cfg.pin_d11 = 15;
            cfg.pin_d12 = 7;
            cfg.pin_d13 = 6;
            cfg.pin_d14 = 5;
            cfg.pin_d15 = 4;

            _bus_instance.config(cfg);              // 設定値をバスに反映します。
            _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
        }

        {                                        // 表示パネル制御の設定を行います。
            auto cfg = _panel_instance.config(); // 表示パネル設定用の構造体を取得します。

            cfg.pin_cs = -1;   // CS要拉低
            cfg.pin_rst = -1;  // RST和开发板RST相连
            cfg.pin_busy = -1; // BUSYが接続されているピン番号 (-1 = disable)

            // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

            cfg.memory_width = 320;   // ドライバICがサポートしている最大の幅
            cfg.memory_height = 480;  // ドライバICがサポートしている最大の高さ
            cfg.panel_width = 320;    // 実際に表示可能な幅
            cfg.panel_height = 480;   // 実際に表示可能な高さ
            cfg.offset_x = 0;         // パネルのX方向オフセット量
            cfg.offset_y = 0;         // パネルのY方向オフセット量
            cfg.offset_rotation = 0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
            cfg.dummy_read_pixel = 8; // ピクセル読出し前のダミーリードのビット数
            cfg.dummy_read_bits = 1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
            cfg.readable = true;      // データ読出しが可能な場合 trueに設定
            cfg.invert = false;       // パネルの明暗が反転してしまう場合 trueに設定
            cfg.rgb_order = false;    // パネルの赤と青が入れ替わってしまう場合 trueに設定
            cfg.dlen_16bit = true;    // データ長を16bit単位で送信するパネルの場合 trueに設定
            cfg.bus_shared = true;    // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance); // 使用するパネルをセットします。
    }
};

static LGFX tft;
// static TFT_eSPI tft;

unsigned long total = 0;
unsigned long tn = 0;
void setup()
{
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_BLK, OUTPUT);

  digitalWrite(LCD_CS, LOW);
  digitalWrite(LCD_BLK, HIGH);

  Serial.begin(115200);
  Serial.println("");
  Serial.println("");
  Serial.println("Lovyan's LovyanGFX library Test!");

  tft.init();
  tft.setRotation(1);
  tft.startWrite();
}

void loop(void)
{
  Serial.println(F("Benchmark                Time (microseconds)"));

  uint32_t usecFillScreen = testFillScreen();
  Serial.print(F("Screen fill              "));
  Serial.println(usecFillScreen);
  delay(100);

  uint32_t usecText = testText();
  Serial.print(F("Text                     "));
  Serial.println(usecText);
  delay(100);

  uint32_t usecPixels = testPixels();
  Serial.print(F("Pixels                   "));
  Serial.println(usecPixels);
  delay(100);

  uint32_t usecLines = testLines(TFT_BLUE);
  Serial.print(F("Lines                    "));
  Serial.println(usecLines);
  delay(100);

  uint32_t usecFastLines = testFastLines(TFT_RED, TFT_BLUE);
  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(usecFastLines);
  delay(100);

  uint32_t usecRects = testRects(TFT_GREEN);
  Serial.print(F("Rectangles (outline)     "));
  Serial.println(usecRects);
  delay(100);

  uint32_t usecFilledRects = testFilledRects(TFT_YELLOW, TFT_MAGENTA);
  Serial.print(F("Rectangles (filled)      "));
  Serial.println(usecFilledRects);
  delay(100);

  uint32_t usecFilledCircles = testFilledCircles(10, TFT_MAGENTA);
  Serial.print(F("Circles (filled)         "));
  Serial.println(usecFilledCircles);
  delay(100);

  uint32_t usecCircles = testCircles(10, TFT_WHITE);
  Serial.print(F("Circles (outline)        "));
  Serial.println(usecCircles);
  delay(100);

  uint32_t usecTriangles = testTriangles();
  Serial.print(F("Triangles (outline)      "));
  Serial.println(usecTriangles);
  delay(100);

  uint32_t usecFilledTrangles = testFilledTriangles();
  Serial.print(F("Triangles (filled)       "));
  Serial.println(usecFilledTrangles);
  delay(100);

  uint32_t usecRoundRects = testRoundRects();
  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(usecRoundRects);
  delay(100);

  uint32_t usedFilledRoundRects = testFilledRoundRects();
  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(usedFilledRoundRects);
  delay(100);

  Serial.println(F("Done!"));

  uint16_t c = 4;
  int8_t d = 1;
  for (int32_t i = 0; i < tft.height(); i++)
  {
    tft.drawFastHLine(0, i, tft.width(), c);
    c += d;
    if (c <= 4 || c >= 11)
      d = -d;
  }


  tft.setCursor(0, 0);
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextSize(2);

  tft.println(F("  LovyanGFX test"));

  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.println(F(""));
  tft.setTextSize(1);
  tft.println(F(""));
  tft.setTextColor(tft.color565(0x80, 0x80, 0x80));

  tft.println(F(""));

  tft.setTextColor(TFT_GREEN);
  tft.println(F("Benchmark    microseconds"));
  tft.println(F(""));
  tft.setTextColor(TFT_YELLOW);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Screen fill    "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecFillScreen);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Text           "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecText);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Pixels         "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecPixels);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Lines          "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecLines);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Hor/Ve Lines   "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecFastLines);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Rectangles     "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecRects);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Rectan-filled  "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecFilledRects);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Circles        "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecCircles);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Circles-filled "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecFilledCircles);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Triangles      "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecTriangles);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Trian-filled   "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecFilledTrangles);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Rounded rects  "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usecRoundRects);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(1);
  tft.print(F("Rou rects-fill "));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  printnice(usedFilledRoundRects);

  tft.setTextSize(1);
  tft.println(F(""));
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(1);
  tft.print(F("Benchmark Complete!"));

  delay(360 * 1000L);
}

void printnice(int32_t v)
{
  char str[32] = {0};
  sprintf(str, "%d", v);
  for (char *p = (str + strlen(str)) - 3; p > str; p -= 3)
  {
    memmove(p + 1, p, strlen(p) + 1);
    *p = ',';
  }
  while (strlen(str) < 10)
  {
    memmove(str + 1, str, strlen(str) + 1);
    *str = ' ';
  }
  tft.println(str);
}

static inline uint32_t micros_start() __attribute__((always_inline));
static inline uint32_t micros_start()
{
  uint8_t oms = millis();
  while ((uint8_t)millis() == oms)
    ;
  return micros();
}

uint32_t testFillScreen()
{
  uint32_t start = micros_start();
  // Shortened this tedious test!
  tft.fillScreen(TFT_WHITE);
  tft.fillScreen(TFT_RED);
  tft.fillScreen(TFT_GREEN);
  tft.fillScreen(TFT_BLUE);
  tft.fillScreen(TFT_BLACK);

  return (micros() - start) / 5;
}

uint32_t testText()
{
  tft.fillScreen(TFT_BLACK);
  uint32_t start = micros_start();
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.println(F("Hello World!"));
  tft.setTextSize(2);
  tft.setTextColor(tft.color565(0xff, 0x00, 0x00));
  tft.print(F("RED "));
  tft.setTextColor(tft.color565(0x00, 0xff, 0x00));
  tft.print(F("GREEN "));
  tft.setTextColor(tft.color565(0x00, 0x00, 0xff));
  tft.println(F("BLUE"));
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(5);
  tft.println(F("Groop"));
  tft.setTextSize(2);
  tft.println(F("I implore thee,"));
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(1);
  tft.println(F("my foonting turlingdromes."));
  tft.println(F("And hooptiously drangle me"));
  tft.println(F("with crinkly bindlewurdles,"));
  tft.println(F("Or I will rend thee"));
  tft.println(F("in the gobberwarts"));
  tft.println(F("with my blurglecruncheon,"));
  tft.println(F("see if I don't!"));
  tft.println(F(""));
  tft.println(F(""));
  tft.setTextColor(TFT_MAGENTA);
  tft.setTextSize(6);
  tft.println(F("Woot!"));
  uint32_t t = micros() - start;
  delay(1000);
  delay(10 * 1000L);
  return t;
}

uint32_t testPixels()
{
  int32_t w = tft.width();
  int32_t h = tft.height();

  uint32_t start = micros_start();
  tft.startWrite();
  for (uint16_t y = 0; y < h; y++)
  {
    for (uint16_t x = 0; x < w; x++)
    {
      tft.drawPixel(x, y, tft.color565(x << 3, y << 3, x * y));
    }
  }
  tft.endWrite();
  return micros() - start;
}

uint32_t testLines(uint16_t color)
{
  uint32_t start, t;
  int32_t x1, y1, x2, y2;
  int32_t w = tft.width();
  int32_t h = tft.height();

  tft.fillScreen(TFT_BLACK);

  x1 = y1 = 0;
  y2 = h - 1;

  start = micros_start();

  for (x2 = 0; x2 < w; x2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }

  x2 = w - 1;

  for (y2 = 0; y2 < h; y2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }

  t = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(TFT_BLACK);

  x1 = w - 1;
  y1 = 0;
  y2 = h - 1;

  start = micros_start();

  for (x2 = 0; x2 < w; x2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }

  t += micros() - start;

  tft.fillScreen(TFT_BLACK);

  x1 = 0;
  y1 = h - 1;
  y2 = 0;

  start = micros_start();

  for (x2 = 0; x2 < w; x2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }
  x2 = w - 1;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }
  t += micros() - start;

  tft.fillScreen(TFT_BLACK);

  x1 = w - 1;
  y1 = h - 1;
  y2 = 0;

  start = micros_start();

  for (x2 = 0; x2 < w; x2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }

  x2 = 0;
  for (y2 = 0; y2 < h; y2 += 6)
  {
    tft.drawLine(x1, y1, x2, y2, color);
  }

  t += micros() - start;

  return t;
}

uint32_t testFastLines(uint16_t color1, uint16_t color2)
{
  uint32_t start;
  int32_t x, y;
  int32_t w = tft.width();
  int32_t h = tft.height();

  tft.fillScreen(TFT_BLACK);

  start = micros_start();

  for (y = 0; y < h; y += 5)
    tft.drawFastHLine(0, y, w, color1);
  for (x = 0; x < w; x += 5)
    tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

uint32_t testRects(uint16_t color)
{
  uint32_t start;
  int32_t n, i, i2;
  int32_t cx = tft.width() / 2;
  int32_t cy = tft.height() / 2;

  tft.fillScreen(TFT_BLACK);
  n = std::min(tft.width(), tft.height());
  start = micros_start();
  for (i = 2; i < n; i += 6)
  {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color);
  }

  return micros() - start;
}

uint32_t testFilledRects(uint16_t color1, uint16_t color2)
{
  uint32_t start, t = 0;
  int32_t n, i, i2;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n = std::min(tft.width(), tft.height());
  for (i = n; i > 0; i -= 6)
  {
    i2 = i / 2;

    start = micros_start();

    tft.fillRect(cx - i2, cy - i2, i, i, color1);

    t += micros() - start;

    // Outlines are not included in timing results
    tft.drawRect(cx - i2, cy - i2, i, i, color2);
  }

  return t;
}

uint32_t testFilledCircles(uint8_t radius, uint16_t color)
{
  uint32_t start;
  int32_t x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(TFT_BLACK);

  start = micros_start();

  for (x = radius; x < w; x += r2)
  {
    for (y = radius; y < h; y += r2)
    {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

uint32_t testCircles(uint8_t radius, uint16_t color)
{
  uint32_t start;
  int32_t x, y, r2 = radius * 2;
  int32_t w = tft.width() + radius;
  int32_t h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros_start();

  for (x = 0; x < w; x += r2)
  {
    for (y = 0; y < h; y += r2)
    {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

uint32_t testTriangles()
{
  uint32_t start;
  int32_t n, i;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);
  n = std::min(cx, cy);

  start = micros_start();

  for (i = 0; i < n; i += 5)
  {
    tft.drawTriangle(
        cx, cy - i,     // peak
        cx - i, cy + i, // bottom left
        cx + i, cy + i, // bottom right
        tft.color565(0, 0, i));
  }

  return micros() - start;
}

uint32_t testFilledTriangles()
{
  uint32_t start, t = 0;
  int32_t i;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);

  start = micros_start();

  for (i = std::min(cx, cy); i > 10; i -= 5)
  {
    start = micros_start();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
                     tft.color565(i, i, 0));
  }

  return t;
}

uint32_t testRoundRects()
{
  uint32_t start;
  int32_t w, i, i2;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);

  w = std::min(tft.width(), tft.height());

  start = micros_start();

  for (i = 0; i < w; i += 6)
  {
    i2 = i / 2;
    tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

uint32_t testFilledRoundRects()
{
  uint32_t start;
  int32_t i, i2;
  int32_t cx = tft.width() / 2 - 1;
  int32_t cy = tft.height() / 2 - 1;

  tft.fillScreen(TFT_BLACK);

  start = micros_start();

  for (i = std::min(tft.width(), tft.height()); i > 20; i -= 6)
  {
    i2 = i / 2;
    tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
  }

  return micros() - start;
}
