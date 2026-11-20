#include "esp_camera.h"
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
//Y2-Y9为数据总线,每个时钟周期传一个像素的 8 位
//XCLK为摄像头时钟，由ESP提供
//PCLK像素时钟
//VSYNC帧同步 一帧开始
//HREF行同步 一行开始
//SIOD SIOC IIC用来配置摄像头寄存器
//PWDN/RESET 电源控制和复位
#define USART_TX_PIN 13
#define USART_RX_PIN 14
#define USART_BAUD 115200
#define LIGHT_MIN_BRIGHTNESS 120  //120为分界 >120为光源
#define RELATIVE_THRESHOLD 0.75f  //不写死阈值,而是取"当前画面最亮值的 75%"当作门槛。
#define MIN_PIXELS 30             //如果"光斑"只有十几二十个像素,那大概率是传感器噪点,不是真光源,直接丢掉。
void Camera_Init() {
  camera_config_t camera = { 0 };
  camera.ledc_channel = LEDC_CHANNEL_0;
  camera.ledc_timer = LEDC_TIMER_0;
  camera.pin_d0 = Y2_GPIO_NUM;
  camera.pin_d1 = Y3_GPIO_NUM;
  camera.pin_d2 = Y4_GPIO_NUM;
  camera.pin_d3 = Y5_GPIO_NUM;
  camera.pin_d4 = Y6_GPIO_NUM;
  camera.pin_d5 = Y7_GPIO_NUM;
  camera.pin_d6 = Y8_GPIO_NUM;
  camera.pin_d7 = Y9_GPIO_NUM;

  camera.pin_xclk = XCLK_GPIO_NUM;
  camera.pin_pclk = PCLK_GPIO_NUM;
  camera.pin_vsync = VSYNC_GPIO_NUM;
  camera.pin_href = HREF_GPIO_NUM;
  camera.pin_sccb_sda = SIOD_GPIO_NUM;
  camera.pin_sccb_scl = SIOC_GPIO_NUM;
  camera.pin_pwdn = PWDN_GPIO_NUM;
  camera.pin_reset = RESET_GPIO_NUM;
  camera.xclk_freq_hz = 24000000;
  camera.pixel_format = PIXFORMAT_GRAYSCALE;
  camera.frame_size = FRAMESIZE_QVGA;
  camera.fb_location = CAMERA_FB_IN_PSRAM;
  camera.fb_count = 2;
  camera.grab_mode = CAMERA_GRAB_LATEST;


  if (esp_camera_init(&camera) != ESP_OK) {
    Serial.println("摄像头初始化失败!检查排线");
  }
  ////C++语法 class HardwareSerial {
  //public:
  // ① 成员函数
  //void begin(int baud);
  //void println(const char *s);

  //private:
  // ② 数据成员(变量)
  //int  uart_num;      // 用哪个 UART(0/1/2)
  //int  baudrate;      // 当前波特率
  // bool initialized;   // 有没有初始化过
}
void setup() {
  Serial.begin(115200);                                               //Serial操作USART0
  Serial1.begin(USART_BAUD, SERIAL_8N1, USART_RX_PIN, USART_TX_PIN);  //Serial1操作USART1
  Camera_Init();
  // put your setup code here, to run once:
}

void loop() {
  static unsigned long frameCount = 0;    //帧数量
  static unsigned long lastReport = 0;    //记录上一次打印调试信息的时间。用来做到"每秒只打印一次",而不是每帧都刷屏。
  camera_fb_t *fb = esp_camera_fb_get();  //定义结构体指针，存放取来的帧结构体地址
  if (!fb)                                //如果fb为空指针
  {
    Serial.println("取帧失败啦");
    return;
  }

  const uint8_t *buf = fb->buf;  //fb到结构体中取出buff字段的地址 赋值给*buf
  const int w = fb->width;
  const int h = fb->height;
  const size_t n = (size_t)w * h;
  //camera_fb_t 结构体(约 40 字节)---------------------------------------------------
  //┌────────────────────────┐
  //│ buf    = 0x3F400020    │──┐
  //│ len    = 76800         │  │
  //│ width  = 320           │  │
  //│ height = 240           │  │
  //│ format = 2             │  │
  //│ timestamp = ...        │  │
  //└────────────────────────┘  │
  //│
  // ▼
  ///// 0x3F400020
  ///┌───────────────────────┐
  /// │ 像素数据 76800 字节    │
  //// │ ┌───┬───┬───┬───┬─    │
  ///│ │178│ 5 │230│ 12│ ... │
  //│ └───┴───┴───┴───┴─    │
  //└───────────────────────┘
  //-------------------------------------------------------------------------------------
  uint8_t maxv = 0;
  for (size_t i = 0; i < n; i++) {
    if (buf[i] > maxv) {
      maxv = buf[i];
    }
  }
  long cx = -1, cy = -1;
  if (maxv >= LIGHT_MIN_BRIGHTNESS) {
    const uint8_t thr = (uint8_t)(maxv * RELATIVE_THRESHOLD);
    uint32_t sumx = 0, sumy = 0, Count = 0;
    for (int y = 0; y < h; y++) {
      const uint8_t *row = buf + (size_t)y * w;
      for (int x = 0; x < w; x++) {
        if (row[x] >= thr) {
          sumx += x;
          sumy += y;
          Count++;
        }
      }
    }
    if (Count >= MIN_PIXELS) {
      cx = sumx / Count;
      cy = sumy / Count;

      int dx = (int)cx - w / 2;
      int dy = (int)cy - h / 2;

      Serial1.printf("%d,%d\n", dx, dy);
    }
  }
  if (cx < 0) {
    Serial1.println("none");
  }
  esp_camera_fb_return(fb);
  frameCount++;                  //每处理完一帧 自加
  unsigned long now = millis();  //读取开机以来的时间
  if (now - lastReport > 1000)   //如果比上次打印超过1s了
  {
    float fps = frameCount * 1000.0f / (now - lastReport);
    frameCount = 0;
    lastReport = now;
    if (cx < 0) {
      Serial.printf("FPS %.1f  maxv=%d  未找到光源\n", fps, maxv);
    } else {
      Serial.printf("FPS %.1f  maxv=%d  light(%ld,%ld)  offset(%ld,%ld)\n",
                    fps, maxv, cx, cy, cx - w / 2, cy - h / 2);
    }
  }
  // put your main code here, to run repeatedly:
}
