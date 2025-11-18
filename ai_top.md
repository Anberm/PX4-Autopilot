### 基于算法
- 单路720p，基础作物识别+障碍物检测+定位（VIO/EKF）：建议整板算力 6–10 TOPS
- 1080p、1–2路视频，检测+轻量分割/深度，稳定30 FPS：20–30 TOPS
- 多路相机/多模型并发（检测+分割+深度）且需稳定30 FPS：50–100 TOPS

### 模块级算力预算（30 FPS、INT8量化，含工程冗余）
- 作物识别/目标检测（轻量YOLO/Anchor-Free，nano/small）：0.3–1 TOPS/路
- 障碍物检测（单独模型或复用检测头）：0.3–1 TOPS/路
- 轻量语义分割（Fast-SCNN/BiSeNet-lite/PP-LiteSeg）：1–3 TOPS/路
- 单目深度/光流（轻量Monodepth/RAFT-lite/SuperPoint+SuperGlue-lite）：0.5–2 TOPS/路
- 视觉惯导/多传感器融合（EKF/UKF/VIO前端）：“传统算法”占CPU/GPU少量算力，基本不计TOPS
- 路径规划（A*/RRT*/DWA 等传统算法）：CPU为主，不计TOPS
- 多目标决策优化（启发式/小规模MIP）：CPU为主，不计TOPS


### 典型整板选型建议
- 入门（成本/功耗敏感，单路720p）：RK3588 NPU（≈6 TOPS）或 Edge TPU（≈4 TOPS），15–30 FPS 基础检测可达
- 平衡（1080p、1–2路，检测+轻量分割/深度）：Hailo-8（≈26 TOPS）或 Jetson Orin Nano（≈20 TOPS）
- 进阶（多路、多模型并发，稳定30 FPS）：Jetson Orin NX（≈70–100 TOPS）或双NPU并行

### 快速预估公式（工程保守法）
- 需求TOPS ≈ Σ(模型GFLOPs × 目标FPS)/1000 × 余量系数(3–6) × 并发路数
- 例：YOLOv5s ≈7 GFLOPs，30 FPS → 0.21 TOPS；乘工程余量5 ≈ 1 TOPS/路

### 与PX4集成要点
- 视觉/AI全部在伴随计算板运行，通过 MAVLink/串口/以太网与PX4交互；确保控制回路不被AI任务阻塞
- 推理使用 INT8 量化与厂商SDK（TensorRT/HailoRT/RKNN）以达成标称TOPS
- 预留2×算力冗余应对模型升级、环境复杂度上升与并发增加
- 功耗与热设计：入门8–10W、平衡10–25W、进阶25–45W；需与续航权衡
- 内存建议：8–16GB；存储≥32GB（含模型、日志、地图）
