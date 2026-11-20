# STM32 嵌入式项目集

基于 STM32F103 的三个嵌入式项目：光源追踪云台、PID 平衡小车、温度监控报警装置。

## 目录结构

```
.
├── light-tracking/          # 光源追踪云台（ESP32-CAM + STM32 双轴舵机）
│   ├── stm32/               #   STM32 端：Keil MDK 工程（MDK-ARM/LightServo.uvprojx）
│   └── esp32/               #   ESP32 端：Arduino 源码（LightTracker.ino）
├── balance-car/             # PID 平衡小车（STM32 + MPU6050 + TB6612）
│   └── MDK-ARM/PID_PWM.uvprojx
└── temperature-sensor/      # 温度监控报警装置（NTC + OLED + 蜂鸣器）
    ├── STM32_TEMP/          #   Keil 工程（Project.uvprojx）
    └── docs/                #   原理图 PDF、PCB 图、Gerber、汇报 PPT
```

## 1. 光源追踪云台（light-tracking）

双控制器架构：ESP32-CAM 采集灰度图像并计算光斑质心，通过 UART 将偏移量发送给 STM32F103C8T6，STM32 驱动双轴舵机做闭环跟随。

- **视觉**：OV2640 灰度图（QVGA）+ PSRAM 双帧缓冲；全画面扫描取最亮值，按相对阈值（最亮值的 75%）二值化求质心，最小像素数过滤噪点；稳定 15 FPS
- **通信**：自定义文本协议，`dx,dy\n` 表示偏移，`none` 表示无目标；STM32 端 USART1 逐字节中断接收，行缓冲 + `sscanf` 校验，异常数据整行丢弃
- **控制**：负反馈 P 控制（Kp = 0.5），增量式调节 50Hz PWM 脉宽（1000–2000µs），±15 像素死区，行程限幅

### 接线

| 连接 | 说明 |
| --- | --- |
| ESP32 GPIO13 (TX) → STM32 PA10 (RX) | 数据线路 |
| ESP32 GND ↔ STM32 GND | 必须共地 |
| 水平舵机信号 → PA0 | TIM2_CH1 |
| 垂直舵机信号 → PA1 | TIM2_CH2 |
| 舵机电源 → 5V（独立供电） | 与主控共地 |

### 构建

- STM32：Keil MDK5 打开 `light-tracking/stm32/MDK-ARM/LightServo.uvprojx`
- ESP32：Arduino IDE 打开 `light-tracking/esp32/LightTracker.ino`，开发板选 AI Thinker ESP32-CAM

## 2. PID 平衡小车（balance-car）

STM32F103C8T6 + MPU6050 + TB6612 双路电机驱动。

- 互补滤波姿态解算，直立环 PD 控制，PWM 差速驱动
- 控制周期 6ms（约 167Hz）：I²C 连续读替代 12 次单字节读取，显示刷新降频

构建：Keil MDK5 打开 `balance-car/MDK-ARM/PID_PWM.uvprojx`。

## 3. 温度监控报警装置（temperature-sensor）

NTC 热敏电阻经 ADC 采样，分压电路 + 温度特性公式换算；ADC 模拟看门狗硬件告警，WFI 低功耗；I²C OLED 实时显示。

- 硬件设计：嘉立创 EDA 原理图与两层 PCB（见 `temperature-sensor/docs/`）

构建：Keil MDK5 打开 `temperature-sensor/STM32_TEMP/Project.uvprojx`。

## 开发环境

- STM32：Keil MDK 5 + STM32Cube HAL / 标准外设库
- ESP32：Arduino 框架（arduino-cli / Arduino IDE，esp32 core）
- 烧录：ST-Link（SWD）/ USB-TTL
