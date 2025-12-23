# PX4 Autopilot 项目上下文

## 项目简介
PX4 是一个开源的无人机飞控固件，支持多种飞行器类型，包括多旋翼、固定翼、VTOL、直升机、地面车辆和水下无人机。

## 技术栈
- **语言**: C/C++17
- **RTOS**: NuttX (硬件平台) / POSIX (仿真)
- **构建**: CMake + Make
- **通信**: MAVLink 2.0
- **消息系统**: uORB (微对象请求代理)

## 核心模块

| 模块 | 路径 | 职责 |
|-----|------|-----|
| Commander | src/modules/commander/ | 状态机、解锁、模式切换 |
| Navigator | src/modules/navigator/ | 任务执行、航点导航 |
| Sensors | src/modules/sensors/ | 传感器数据聚合 |
| MAVLink | src/modules/mavlink/ | 地面站通信 |
| EKF2 | src/modules/ekf2/ | 状态估计 |
| mc_att_control | src/modules/mc_att_control/ | 多旋翼姿态控制 |
| mc_pos_control | src/modules/mc_pos_control/ | 多旋翼位置控制 |
| control_allocator | src/modules/control_allocator/ | 控制分配 |
| land_detector | src/modules/land_detector/ | 着陆检测 |

## 目录结构

```
PX4-Autopilot/
├── src/
│   ├── modules/        # 核心模块
│   ├── drivers/        # 硬件驱动
│   ├── lib/            # 库函数
│   ├── include/        # 公共头文件
│   ├── systemcmds/     # 系统命令
│   └── templates/      # 模板
├── boards/             # 板级支持包
├── platforms/          # 平台抽象层
├── msg/                # uORB 消息定义
├── Tools/              # 构建工具
├── ROMFS/              # 文件系统
└── test/               # 测试代码
```

## 编码规范
- 遵循 PX4 编码规范
- 使用 C++17 标准特性
- 类成员变量以 `_` 前缀
- 常量使用大写下划线命名
- 使用 `px4_` 前缀的平台抽象函数

## 通信架构

### uORB 消息系统
- 发布/订阅模式
- 消息定义在 `msg/*.msg`
- 支持多实例消息
- 模块间解耦通信

### MAVLink 协议
- MAVLink 2.0
- 心跳、参数、命令、任务协议
- 支持自定义消息扩展

## 构建系统

### 基本构建命令
```bash
# 构建 SITL
make px4_sitl_default

# 构建硬件固件
make px4_fmu-v6x_default

# 构建并启动仿真
make px4_sitl gazebo-classic
```

### 构建目标
- `px4_sitl_default` - SITL 仿真
- `px4_fmu-v5_default` - Pixhawk 4
- `px4_fmu-v6x_default` - Pixhawk 6X
- `px4_fmu-v6c_default` - Pixhawk 6C

## 分支策略
- `main`: 开发分支，接受新功能
- `release/x.xx`: 发布分支，仅接受 Bug 修复
- 特性开发在 Fork 仓库进行

## 测试要求
- 新功能必须通过 SITL 测试
- 核心模块需要单元测试
- PR 必须通过 CI 测试

## 已知技术债务
- [ ] 部分旧驱动需要重构
- [ ] 测试覆盖率需要提升
- [ ] 文档需要更新

## 关键链接
- 官网: https://px4.io
- 文档: https://docs.px4.io
- GitHub: https://github.com/PX4/PX4-Autopilot
- 论坛: https://discuss.px4.io
- Discord: https://discord.gg/dronecode

## 更新记录
- 2024-12-23: 初始创建
