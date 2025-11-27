# Kite F427 Flight Controller Board

## 硬件规格

### 主控MCU
- **MCU**: STM32F427VIT6 (LQFP-100)
  - 32位 ARM Cortex-M4F @ 168 MHz
  - 2MB Flash, 256KB RAM
  - 硬件浮点单元(FPU)

### 传感器

#### IMU (惯性测量单元)
- **BMI088** (SPI1)
  - 6轴IMU (3轴陀螺仪 + 3轴加速度计)
  - 片选: PC2
  - 数据就绪: PD15
  - 高性能低噪声传感器

#### 磁力计
- **QMC5883L** (I2C1)
  - 3轴数字罗盘
  - 连接到外部I2C总线 (PB8-SCL, PB9-SDA)

#### 气压计
- **SPL06-001** (SPI2)
  - 高精度数字气压计
  - 片选: PD7
  - SPI时钟: 最高10 MHz

### 存储

#### FRAM (非易失性参数存储)
- **FM25V02A-GTR** (SPI2)
  - 256 Kbit (32KB) FRAM
  - 片选: PD10
  - 用于参数存储(快速、无限写入次数)

#### eMMC (数据记录)
- **CSNP32GCR01-AOW** (SDIO)
  - 32GB eMMC闪存存储
  - 通过SDIO接口连接
  - 用于高速日志和数据存储

### 时钟源
- **24MHz晶振** (X322524MSB4SI)
  - 高稳定性外部振荡器
  - 系统时钟: 168 MHz (通过PLL)

### 电源

- 电池电压检测: ADC1_IN2
- 电池电流检测: ADC1_IN3  
- 5V电源检测: ADC1_IN4
- 电源砖有效检测: PB5
- USB电源检测: PC0

### 通信接口

#### 串口
- **USART1** (PA9/PA10): ESP8266 WiFi模块
- **USART2** (PD5/PD6): 数传1 (带RTS/CTS)
- **USART3** (PD8/PD9): 数传2 (带RTS/CTS)
- **UART4** (PA0/PA1): GPS 1
- **USART6** (PC6/PC7): 遥控输入
- **UART7** (PE7/PE8): 调试控制台
- **UART8** (PE0/PE1): 可用

#### CAN总线
- **CAN1** (PD0-RX, PD1-TX)
  - 用于UAVCAN通信
  - 板载收发器

#### USB
- **USB Type-C** 接口 (J4)
  - USB OTG全速
  - VBUS检测: PA9

### PWM输出 (6通道)
- **Timer1**: PE14 (CH4), PE13 (CH3), PE11 (CH2), PE9 (CH1)
- **Timer4**: PD13 (CH2), PD14 (CH3)
- 支持DShot、OneShot和标准PWM

### LED指示灯
- **红色LED**: PB11 (状态)
- **绿色LED**: PB1 (解锁状态)
- **蓝色LED**: PB3 (活动)
- **安全LED**: PC3

### 其他功能
- **安全按钮**: PC4 (带上拉)
- **RSSI输入**: PC1 (ADC1_IN11)
- **蜂鸣器/音调报警**: PA15 (定时器2, 通道1)
- **遥控输入反相**: PC13 (用于SBUS)
- **Spektrum电源控制**: PE4

## PX4固件配置

### 板子文件
- `board_config.h`: GPIO定义和硬件配置
- `spi.cpp`: SPI总线和设备配置
- `i2c.cpp`: I2C总线配置
- `timer_config.cpp`: PWM定时器配置
- `init.c`: 板子初始化代码
- `mtd.cpp`: FRAM参数存储配置
- `board.h`: 时钟和外设定义

### 构建配置
- 板子ID: 107
- 架构: ARM Cortex-M4
- 工具链: arm-none-eabi-gcc
- Flash大小: 2MB
- 最大固件大小: 2080768字节

### 传感器启动
传感器通过`rc.board_sensors`自动启动:
- BMI088 IMU (陀螺仪 + 加速度计) SPI1
- SPL06-001 气压计 SPI2
- QMC5883L 磁力计 I2C1

### 参数默认值
- 电池电压分压器: 23.13
- 电池电流比例: 36.367515152 A/V
- 遥控输入映射: 横滚(1), 俯仰(2), 油门(3), 偏航(4)

## 编译固件

```bash
make kite_f427_default
```

## 上传固件

### 通过USB
```bash
make kite_f427_default upload
```

### 通过调试端口
连接调试探针(ST-Link, J-Link等)到SWD接口并使用:
```bash
make kite_f427_default debug
```

## 硬件版本

本板基于IFCS-MAIN V1.1设计，具有以下主要特性:
- 针对多旋翼和VTOL应用优化
- 紧凑外形集成传感器
- 冗余电源监控
- 大容量板载存储(32GB eMMC)

## 开发说明

### 主要更新内容
1. **传感器驱动**: 添加了BMI088 IMU、QMC5883L磁力计、SPL06-001气压计的支持
2. **存储配置**: 配置了FM25V02A FRAM和32GB eMMC存储
3. **GPIO映射**: 根据硬件原理图更新了所有GPIO引脚定义
4. **时钟配置**: 确认24MHz晶振配置正确
5. **初始化流程**: 优化了板子初始化流程，添加eMMC支持

### 与ArduPilot的兼容性
本板原为ArduPilot设计，现已移植到PX4。主要差异:
- 参数存储: ArduPilot使用内部Flash，PX4使用FRAM
- 传感器驱动: 使用PX4原生驱动
- 电源管理: 适配PX4的电源监控框架
