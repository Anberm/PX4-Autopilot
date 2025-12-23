# PX4 Autopilot 多专家协作规范

## 概述

本项目采用多专家 AI Agent 协作模式，由六个专家角色共同完成 PX4 飞控固件开发任务。

## 专家角色架构

```
                        ┌─────────────────┐
                        │   Coordinator   │
                        │   (协调者)       │
                        └────────┬────────┘
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
        ▼                        ▼                        ▼
┌───────────────┐       ┌───────────────┐       ┌───────────────┐
│  PX4 Firmware │       │ Flight Control│       │ Driver Expert │
│    Expert     │       │    Expert     │       │  (驱动专家)    │
│ (固件专家)     │       │ (飞控算法专家) │       └───────────────┘
└───────────────┘       └───────────────┘
        │                        │                        │
        └────────────────────────┼────────────────────────┘
                                 │
                ┌────────────────┼────────────────┐
                ▼                                 ▼
        ┌───────────────┐                 ┌───────────────┐
        │  Test Expert  │                 │ Project Mgr   │
        │  (测试专家)    │                 │ (项目管理)     │
        └───────────────┘                 └───────────────┘
```

## 专家角色详解

### 1. Coordinator (协调者)
- **职责**: 任务分析、分发、进度跟踪、专家协调
- **模型**: sonnet
- **调用**: `/coordinator`
- **决策权**: 优先级分歧仲裁

### 2. PX4 Firmware Expert (固件专家)
- **职责**: 核心模块开发 (commander, navigator, sensors, mavlink)
- **模型**: opus
- **调用**: `/px4-expert`
- **技术决策权**: 系统架构最终裁决
- **专注领域**:
  - Commander 状态机
  - Navigator 任务执行
  - Sensors 模块
  - MAVLink 通信
  - uORB 消息系统

### 3. Flight Control Expert (飞控算法专家)
- **职责**: 飞控算法、状态估计、控制系统
- **模型**: opus
- **调用**: `/fc-expert`
- **技术决策权**: 算法方案最终裁决
- **专注领域**:
  - EKF2 状态估计
  - 姿态控制 (mc_att_control, fw_att_control)
  - 位置控制 (mc_pos_control)
  - Control Allocator
  - Land Detector

### 4. Driver Expert (驱动专家)
- **职责**: 硬件驱动开发、板级支持
- **模型**: sonnet
- **调用**: `/driver-expert`
- **技术决策权**: 硬件接口最终裁决
- **专注领域**:
  - 传感器驱动 (IMU, 气压计, 磁力计, GPS)
  - PWM 输出
  - RC 输入
  - 板级配置 (boards/)
  - NuttX 适配

### 5. Test Expert (测试专家)
- **职责**: 测试编写、SITL 仿真、CI/CD
- **模型**: sonnet
- **调用**: `/test-expert`
- **质量决策权**: 质量标准最终裁决
- **专注领域**:
  - 单元测试
  - SITL 仿真测试
  - HIL 硬件在环测试
  - CI/CD 流水线
  - 日志分析

### 6. Project Manager (项目管理)
- **职责**: 任务规划、Git 工作流、发布管理
- **模型**: sonnet
- **调用**: `/pm`
- **流程决策权**: 流程规范最终裁决

## 协作流程

### 任务分发流程
```
用户需求 → Coordinator 分析 → 识别任务类型 → 分发给专家 → 专家执行 → 结果汇总
```

### 任务类型识别

| 任务类型 | 主要专家 | 辅助专家 |
|---------|---------|---------|
| 飞行模式开发 | PX4 Firmware | Flight Control |
| 控制算法优化 | Flight Control | Test |
| EKF 调优 | Flight Control | Test |
| 新传感器支持 | Driver | PX4 Firmware |
| 新板卡适配 | Driver | Test |
| MAVLink 扩展 | PX4 Firmware | Test |
| SITL 仿真 | Test | Flight Control |
| Bug 修复 | 根据模块分配 | Test |
| 性能优化 | 根据模块分配 | Test |

### 典型协作场景

#### 场景 1: 新飞行模式开发
```
1. PM: 需求分析，任务分解
2. PX4: Commander 状态机修改
3. FC: 控制逻辑实现
4. TEST: SITL 仿真验证
5. PM: 代码审查，合并发布
```

#### 场景 2: 新传感器驱动开发
```
1. Coordinator: 分析需求，分配任务
2. Driver: 驱动实现，硬件接口
3. PX4: Sensors 模块集成
4. TEST: 驱动测试验证
5. PM: 审查合并
```

#### 场景 3: EKF 算法优化
```
1. Coordinator: 分析问题，定义目标
2. FC: 算法分析，方案设计
3. FC: 算法实现
4. TEST: SITL/日志分析验证
5. PM: 合并发布
```

## 通信协议

### 任务传递格式
```markdown
## 任务传递单

**From:** [发起者]
**To:** [接收者]
**Task ID:** [唯一标识]
**Priority:** P0/P1/P2/P3

### 任务描述
[详细描述任务内容]

### 上下文
[相关背景信息和前置工作成果]

### 涉及模块
[相关的 src/modules/ 或 src/drivers/ 模块]

### 依赖
[依赖的其他任务或资源]

### 验收标准
[完成标准和测试要求]
```

### 任务完成报告格式
```markdown
## 任务完成报告

**Task ID:** [任务标识]
**Expert:** [完成专家]
**Status:** 完成/部分完成/阻塞

### 完成内容
[详细描述完成的工作]

### 产出物
- [修改的文件列表]
- [新增的测试]
- [更新的文档]

### 测试结果
- [单元测试结果]
- [SITL 测试结果]

### 遗留问题
[如有未完成或需要后续处理的问题]

### 建议
[对后续工作的建议]
```

## 记忆系统

### 目录结构
```
.claude/memory/
├── tasks/          # 任务记录
├── decisions/      # 决策记录 (ADR)
├── context/        # 项目上下文
└── README.md
```

### 使用规范
- **任务开始**: 检查 `context/` 和 `decisions/`
- **任务完成**: 更新相关记忆
- **重要决策**: 记录到 `decisions/`

## 工作流

### 标准工作流
- `workflows/feature-development.md` - 新功能开发
- `workflows/bug-fix.md` - Bug 修复
- `workflows/driver-development.md` - 驱动开发
- `workflows/algorithm-optimization.md` - 算法优化
- `workflows/release.md` - 版本发布

### 质量门禁
每个阶段必须满足定义的条件才能进入下一阶段。

## 冲突解决

| 冲突类型 | 仲裁者 |
|---------|--------|
| 系统架构分歧 | PX4 Firmware Expert |
| 算法设计分歧 | Flight Control Expert |
| 硬件接口分歧 | Driver Expert |
| 流程规范分歧 | Project Manager |
| 质量标准分歧 | Test Expert |
| 优先级分歧 | Coordinator |

## 快速参考

### 调用专家
```
/coordinator [任务]     # 协调复杂任务
/px4-expert [任务]      # 固件核心模块开发
/fc-expert [任务]       # 飞控算法开发
/driver-expert [任务]   # 驱动开发
/test-expert [任务]     # 测试相关任务
/pm [任务]              # 项目管理任务
```

### 专家能力矩阵

| 任务类型 | PX4 | FC | DRV | TEST | PM |
|---------|:---:|:--:|:---:|:----:|:--:|
| Commander 开发 | ★ | ○ | - | ○ | - |
| Navigator 开发 | ★ | ○ | - | ○ | - |
| 姿态控制 | ○ | ★ | - | ○ | - |
| 位置控制 | ○ | ★ | - | ○ | - |
| EKF 状态估计 | - | ★ | - | ○ | - |
| 传感器驱动 | ○ | - | ★ | ○ | - |
| 板卡适配 | - | - | ★ | ○ | - |
| MAVLink 通信 | ★ | - | - | ○ | - |
| uORB 消息 | ★ | ○ | ○ | - | - |
| SITL 仿真 | - | ○ | - | ★ | - |
| 单元测试 | - | - | - | ★ | - |
| CI/CD | - | - | - | ★ | ○ |
| 版本发布 | - | - | - | ○ | ★ |
| 任务规划 | - | - | - | - | ★ |

★ = 主要负责, ○ = 辅助参与, - = 不参与

## 最佳实践

1. **模块边界清晰**: 明确任务属于哪个模块
2. **uORB 优先**: 模块间通信使用 uORB 消息
3. **参数化设计**: 可调参数使用 PX4 参数系统
4. **SITL 先行**: 功能开发先在 SITL 中验证
5. **日志完善**: 添加适当的日志输出
6. **文档同步**: 代码变更同步更新文档
7. **持续集成**: 确保 CI 测试通过
