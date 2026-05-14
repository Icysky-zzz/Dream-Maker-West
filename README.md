<div align="center">

<br>

# 高级语言程序设计大作业实验报告
<br>

| |  |
|------|------|
| **姓名** | 王涵 |
| **学号** | 2513036 |
| **班级** | 1010（殷爱茹） |
| **项目** | 造梦西游3之天宫道——Qt动作游戏 |
| **开发环境**| Qt 6.10 、 C++17 、 CMake 3.16+ 、 MinGW/GCC |

</div>

---

## 目录

- [1. 作业题目](#1-作业题目)
- [2. 开发软件](#2-开发软件)
- [3. 课题要求](#3-课题要求)
- [4. 项目总体设计](#4-项目总体设计)
  - [4.1 文件结构](#41-文件结构)
  - [4.2 类继承体系](#42-类继承体系)
  - [4.3 游戏循环](#43-游戏循环timerevent)
- [5. 算法介绍](#5-算法介绍)
  - [5.1 重力与跳跃算法](#51-重力与跳跃算法)
  - [5.2 碰撞检测](#52-碰撞检测)
  - [5.3 四段连击算法](#53-四段连击算法)
  - [5.4 镜头跟随算法](#54-镜头跟随算法)
  - [5.5 掉落物系统](#55-掉落物系统)
- [6. 亮点与细节](#6-亮点与细节)
  - [6.1 精灵帧系统](#61-精灵帧系统)
  - [6.2 静态缓存减少卡顿](#62-静态缓存减少卡顿)
  - [6.3 flipH左右反转绘制](#63-fliph左右反转绘制)
  - [6.4 设置面板与攻击可视化](#65-设置面板与攻击可视化)
  - [6.5 关卡与波次系统](#67-关卡与波次系统)
- [7. 单元测试](#7-单元测试)
  - [7.1 测试案例](#71-测试案例)
  - [7.2 开发过程遇到并解决的问题](#72-开发过程遇到并解决的问题)
- [8. 收获](#8-收获)
- [附：AI工具使用披露](#附ai工具使用披露)

---

## 1. 作业题目

> **造梦西游之天宫道——Qt动作游戏**

基于C++ Qt框架开发的2D横板动作游戏，灵感来源于经典网页游戏《造梦西游》，实现角色控制、战斗系统、关卡推进等核心玩法。

---

## 2. 开发软件

| 类别 | 工具 |
|------|------|
| 开发环境 | Qt Creator |
| 图形引擎 | QPainter 2D绘制 |
| 音频引擎 | QMediaPlayer + QAudioOutput |
| 构建系统 | CMake 3.16+ |
| 编译器 | MinGW/GCC (C++17) |

---

## 3. 课题要求

### 3.1 类的继承与多态

使用C++类的继承体系：基类`individual`派生出`enemy`（敌人类）、`Player`（玩家类），`enemy`进一步派生出`Boss`类。基类定义虚函数`move()`，派生类各自重写实现不同行为，通过基类指针统一调用实现多态。



### 3.2 实现游戏画面

使用`QWidget::timerEvent`每15ms驱动一帧（约66.7 FPS），包含输入键处理、决策、碰撞检测、掉落物更新、绘制等。

---

## 4. 项目总体设计

### 4.1 项目文件结构

整个项目按功能模块划分为以下文件：

| # | 文件 | 功能职责 |
|---|------|----------|
| 1 | **main.cpp** | 程序入口，进入事件循环 |
| 2 | **CMakeLists.txt** | CMake构建配置 |
| 3 | **game.h / game.cpp** | 游戏主窗口：游戏循环、按键输入、关卡系统、镜头跟随、掉落物、设置面板、重新开始 |
| 4 | **individual.h / individual.cpp** | 角色基类：物理属性、碰撞检测、物理更新、攻击系统、动画帧容器 |
| 5 | **player.h / player.cpp** | 玩家类：四段连击、两个技能（冲刺/击飞）、自动回蓝 |
| 6 | **enemy.h / enemy.cpp** | 敌人类：追踪玩家、攻击决策、受击检测、静态缓存 |
| 7 | **boss.h / boss.cpp** | Boss类：光箭技能、阶段攻击动画 |
| 8 | **collision.h / collision.cpp** | 碰撞策略：NormalCollision（普通站立）、BounceCollision（弹跳） |
| 9 | **paint.cpp** | 全部绘制逻辑：QPainter 2D渲染 |

### 4.2 类继承体系
![类继承体系关系图](resources/%E5%85%B3%E7%B3%BB%E5%9B%BE.png)
```
individual（基类）
  ├── move() 虚函数
  ├── QRect m_player / m_attackRect
  ├── 重力/跳跃物理
  ├── 动画帧容器
  ├── 攻击状态 / 受击系统 / 技能冷却
  │
  ├── enemy（敌人类）
  │   ├── trace() 追踪
  │   ├── e_attack() 攻击
  │   ├── is_attacked() 受击
  │   ├── 静态缓存
  │   │
  │   └── Boss（Boss类）
  │       ├── 光箭系统
  │       ├── boss_trace() 重写追踪
  │       └── 33帧蓄力 → 第19帧发射
  │
  └── Player（玩家类）
      ├── 四段连击系统
      ├── 技能1（冲刺）/ 技能2（击飞）
      ├── 自动回蓝
      └── skill2_frames 动画
```

##### 4.2.1 individual（基类）

核心成员：`QRect m_player`（位置碰撞）、`QRect m_attackRect`（攻击框）、重力/跳跃物理、动画帧容器、攻击状态管理（`m_isAttacking`/`m_attackStage`/`variabletime`）、受击系统（`m_attacked`/`knockbackDir`/`attacked_time`）、技能冷却（`is_skill_1~2`/`skill_cd`）

核心虚函数：`virtual void move()`

##### 4.2.2 enemy（敌人类）← 继承 individual

增加：`rest_time`、攻击/行走动画帧、`m_attackType`、静态缓存、`trace()` 追踪、`e_attack()` 攻击、`is_attacked()` 受击

##### 4.2.3 Boss（Boss类）← 继承 enemy

增加：光箭系统、`boss_trace()` 重写追踪逻辑

##### 4.2.4 Player（玩家类）← 继承 individual

增加：技能2动画帧（`skill2_frames`）、冲刺方向锁定（`skill_direction`）、重写 `move()`（攻击锁定移动 + 状态动画切换 + 自动回蓝）

### 4.3 游戏循环（timerEvent）

游戏核心驱动是`QWidget::timerEvent`，**每15ms触发一次（约66 FPS）**。每帧按以下步骤顺序执行：

| 操作 | 说明 |
|------|------|
| **输入处理** | 根据keyPressEvent / keyReleaseEvent设置的布尔标记，决定玩家行为 |
| **玩家物理更新** | `Player::move()` → `individual::move()`：重力累加、水平移动、障碍物碰撞、跳跃处理 |
| **攻击系统更新** | `Player::attack()`推进四段攻击，技能冷却递减 |
| **敌人波次生成** | 玩家 x ≥ 1000 触发第一波（5波，每波左右各1敌，间隔2s） |
| **敌人AI更新** | `enemy::trace()`追踪、`e_attack()`攻击；Boss调用`boss_trace()` |
| **敌人受击检测** | `enemy::is_attacked()`遍历玩家攻击框，`lastHitStage`避免重复伤害 |
| **碰撞检测** | `NormalCollision::m_Collision()`处理障碍物碰撞 |
| **镜头跟随** | 玩家偏离屏幕中心 1/6 时移动摄像机，范围 [0, 2400 − GAME_WIDTH] |
| **掉落物更新** | 击杀 1/3 概率生成 HP/MP 掉落物，含重力/碰撞/拾取/超时逻辑 |
| **死亡检测** | `player.hp ≤ 0` 时显示 Game Over，任意键重新开始 |
| **重绘** | `update()` 触发 `paintEvent()` 绘制所有图形 |

---

## 5. 算法介绍

### 5.1 重力与跳跃算法

```
重力加速度：GRAVITY = 0.6 px/帧
       ↓
每帧: m_playerVelocity += GRAVITY
       ↓
跳转时: m_playerVelocity = -JUMP_FORCE
```

| 跳跃类型 | 初始速度 | 说明 |
|----------|----------|------|
| 一段跳 | `-15` | 按 K 触发 |
| 二段跳 | `-12` | 空中再按 K 触发 |

角色位置更新：`newPlayerRect.moveTop(top + m_playerVelocity)`  
地面检测：`check_onground()` 遍历所有障碍物，检测 `m_player.bottom()` 是否与障碍物 `top()` 接触。

### 5.2 碰撞检测

碰撞检测在 `individual::move()` 中实现。先计算 `newPlayerRect`（应用重力和水平移动后的新位置），然后对每个障碍物进行四方向检测：

| 碰撞方向 | 判定条件 | 处理方式 |
|:--------:|----------|----------|
| **顶部碰撞** | `player.bottom() ≤ obstacle.top()` 且 `newPlayer.bottom() > obstacle.top()` | 置于障碍物顶部，`velocity = 0` |
| **底部碰撞** | `player.top() ≥ obstacle.bottom()` 且 `newPlayer.top() < obstacle.bottom()` | 置于障碍物底部，`velocity = 0` |
| **左侧碰撞** | 穿透障碍物左侧 | 挤出至障碍物右侧 |
| **右侧碰撞** | 穿透障碍物右侧 | 挤出至障碍物左侧 |

> **关键技巧：** 跳跃状态在碰撞前保存，碰撞处理完之后再根据保存的状态执行跳跃。避免"重力累积导致碰撞吞跳跃输入"的bug。

### 5.3 四段连击算法

按 **J** 键触发攻击时，检测当前tick与上次按J的tick差值：

```
if (m_gameTick - player.last_j_tick < 65 && player.m_attackStage < 4)
    → 连击段数 +1（1 → 2 → 3 → 4），重置对应段数的动画帧索引
else
    → 重置为段数1（从头开始连击）
```

一段攻击框宽80px、二段90px、三段100px、四段120px，每段伤害均为10。

每段攻击的伤害只触发一次（通过 `enemy` 中的 `lastHitStage` 记录已触发的段号来避免重复扣血）。

### 5.4 镜头跟随算法

```cpp
int screenCenter = GAME_WIDTH / 2;
int playerCenterX = player.m_player.center().x();
int dif = playerCenterX - (m_cameraX + screenCenter);

if (dif > GAME_WIDTH/6)
    m_cameraX = playerCenterX - (screenCenter + GAME_WIDTH/6);
else if (dif < -GAME_WIDTH/6)
    m_cameraX = playerCenterX - (screenCenter - GAME_WIDTH/6);
```

- **阈值：** `GAME_WIDTH / 6`（约133px）
- **效果：** 玩家偏离屏幕中心超过阈值才开始跟随
- **范围：** 限制在 `[0, 2400 - GAME_WIDTH]` 内

### 5.5 掉落物系统

击杀敌人时 **1/3 概率** 生成掉落物，使用 `Drop` 结构体管理：

| 属性 | 类型 | 说明 |
|------|:----:|------|
| `pos` | QPointF | 位置坐标 |
| `type` | int | `0` = HP回20 / `1` = MP回15 |
| `lifetime` | int | 600帧超时消失 |
| `vy` | double | 垂直速度，实现重力下落 |
| `pickupDelay` | int | 30帧拾取延迟，防止击杀瞬间误捡 |

**每帧更新流程：**

```
vy += 0.5（重力加速）
  → pos.ry() += vy（垂直位移）
    → 障碍物碰撞检测（落在平台上停止）
      → 拾取检测（与玩家 m_player 矩形相交）
        → 超时检测（lifetime ≤ 0 时消失）
```

---

## 6. 亮点与细节

### 6.1 精灵帧系统

所有角色动画使用**水平精灵表**（一张大图上水平排列所有帧），通过 `QRect` 按帧切割：

```cpp
static void loadSheet(QVector<QPixmap>& frames, const QString& path,
int frameW, int frameH, int count, float scale = 1.0f);
```

帧推进函数——timer 计数到 `speed` 时推进一帧，index 循环：

```cpp
void advanceFrame(int& timer,int speed,int& index,int frameCount) {
    if (++timer >= speed) {
        timer = 0;
        index = (index + 1) % frameCount;
    }
}
```

> Boss 添加了 **"攻击蓄力前摇"** 机制：33帧攻击动画中，第19帧才发射光束。

### 6.2 静态缓存减少卡顿

在 `enemy` 构造函数中，精灵表的加载使用了 `static` 局部变量缓存：

```cpp
static bool s_loaded = false;
static QVector<QPixmap> s_attackFrames;
static QVector<QPixmap> s_walkFrames;
```

- 首次构造时：加载精灵表并切割为帧序列存入 `static` 变量
- 后续实例：直接从 `static` 缓存拷贝（`QVector<QPixmap>` 是浅拷贝，开销很小）

### 6.3 flipH左右反转绘制

```cpp
static QPixmap flipH(const QPixmap& src, int dir) {
    if (dir == 2) return src.transformed(QTransform().scale(-1, 1));
    return src;
}
```

所有精灵表只存储面向**右侧**的帧，当角色方向向左时通过 `QTransform().scale(-1, 1)` 镜像翻转，节省了一半的精灵表资源。


### 6.4 设置面板与攻击可视化

点击齿轮按钮 **⚙** 打开设置面板，提供两个功能：

- **显示/隐藏攻击框：** 将攻击矩形用半透明彩色绘制（玩家为橙红色，敌人为蓝色），辅助调试与可视化
- **重新开始：** 调用 `resetGame()` 重置所有状态到初始值


### 6.5 关卡与波次系统

| 关卡 | 触发条件 | 内容 |
|:----:|----------|------|
| **关卡1** | 玩家 x ≥ 1000 | 5波敌人，每波左右各1个，间隔约2秒 |
| **过渡** | 清完所有敌人 | `m_level1Cleared = true`，解锁边界限制 |
| **关卡2** | 玩家越过 x = 1500 | 生成 Boss（位置 2000, 200），HP = 500 |

---

## 7. 单元测试

### 7.1 测试案例

| # | 测试输入 | 预期输出 | 测试目的 |
|:-:|----------|----------|----------|
| 1 | 启动游戏，检查初始状态 | 玩家位置 (200, 430)，HP = 100，MP = 100 | 验证初始状态正确 |
| 2 | 按 J 攻击1次 | `m_attackStage = 1`，attackWidth = 80，播放攻击动画7帧 | 验证一段攻击逻辑 |
| 3 | 65帧内依次按 J 共4次 | 段数 1 → 2 → 3 → 4，攻击框宽 80 → 90 → 100 → 120 | 验证四段连击与攻击框递增 |
| 4 | 按 K 跳跃 | 垂直速度 = -15，角色上升后受重力回落 | 验证一段跳跃物理 |
| 5 | 空中按 K（二段跳） | 垂直速度 = -12，播放二段跳6帧动画后切 `jump6` 贴图 | 验证二段跳逻辑 |
| 6 | 按 U 键 | 消耗 20 MP，沿锁定方向冲刺 25 px/帧，冷却60帧 | 验证冲刺技能 |
| 7 | 按 Y 键 | 消耗 15 MP，击飞动画，冷却90帧 | 验证击飞技能 |
| 8 | 玩家 x ≥ 1000 | 第一波敌人生成（左右各1个），后续每133帧再生成 | 验证关卡1波次触发 |

> 上述所有案例均通过实际运行验证。

### 7.2 开发过程遇到并解决的问题

**问题1：碰撞吞跳跃输入**

- **现象：** 角色站在平台上按 K 跳跃，由于重力累积 `m_playerVelocity` 不断增大，碰撞处理中 `NormalCollision::onTopCollision()` 将 `m_isJumping` 置为 `false`，导致跳跃输入丢失。
- **解决：** 采用 **"先保存后应用"** 技巧——在碰撞前用局部变量保存 `wantsJump` 和 `wantedJumptime`，碰撞处理完后再根据保存的状态执行跳跃，彻底解决了这个bug。

**问题2：Boss光束冷却过短**

- **现象：** 初始 Boss 光束冷却设为 120帧（约2秒），Boss攻击频率过高，玩家体验不佳。
- **解决：** 将 `m_beamCooldown` 从 120 调整为 **180（约3秒）**，增加Boss攻击间隔，使战斗节奏更合理。同时 Boss 攻击时停止追踪，给玩家反应和走位的时间。

**问题3：精灵帧制作与对齐**

所有精灵帧均从原游戏《造梦西游》中截屏取得。将原游戏画面录屏或逐帧截取后，在图片编辑软件中手动抠出每一帧的角色轮廓，去除背景杂色，再拼接为水平精灵表。

由于原游戏中的人物在持续运动，角色的中心位置会在不同帧之间发生变化，抠图后各帧的基准位置不一致。制作精灵表时需**逐帧手动对齐角色中心**，确保动画播放时不会出现画面跳动。部分复杂动画（如四段攻击和二段跳）涉及人物的大范围移动和形变，一帧往往需要多次微调才能与其他帧平滑衔接，制作过程十分不易。

为了解决手动抠图效率低的问题，使用了以下在线工具辅助制作精灵表：
- [Video to Spritesheet](https://video2timesheet.dawnwindstudio.top/) —— 将录制的游戏视频直接转换为精灵表序列
- [Tensor Sprite Tool](https://tensor-mxsw.upma.site/) —— 利用AI辅助抠图和帧对齐，减少手动调整的工作量

---

## 8. 收获

### 8.1 虚函数与多态的实际应用

基类 `individual` 定义 `virtual void move()`，`Player` 和 `Boss` 各自重写。`game.cpp` 中通过 `individual*` 指针统一调用 `move()`，实际执行具体子类的版本。深入理解了**虚析构函数**的重要性——基类定义 `virtual ~individual() = default`，确保 `delete` 派生类对象时正确调用析构链。

### 8.2 "先保存后应用"——解决碰撞吞输入问题

碰撞处理中用局部变量在碰撞前保存状态，碰撞后恢复。这个模式不仅适用于跳跃，也可推广到其他"碰撞可能重置状态但需要保留输入"的场景。

### 8.3 策略模式实现可扩展碰撞系统

设计 `Collision` 基类定义碰撞流程，子类重写 `onTopCollision()` 实现不同行为。`NormalCollision`（普通站立）和 `BounceCollision`（弹跳）两种实现已展示出良好的扩展性。

### 8.4 静态缓存优化多实例资源加载

`enemy` 构造函数中使用 `static` 局部变量缓存精灵表帧序列，首次加载后所有实例共享。`QVector<QPixmap>` 的拷贝是浅拷贝（引用计数），多实例共享缓存的开销很小。

### 8.5 精灵表动画系统的工程实现

从 `loadSheet` 切割 → `advanceFrame` 帧推进 → 状态切换，建立了一套完整的 **2D动画管线**。每段攻击独立管理动画帧和攻击框的设计使四段连击系统清晰可维护。

### 8.6 QPainter 2D绘制技巧

`flipH()` 实现精灵水平翻转节省资源。

### 8.7 关卡设计

两关制 + 波次触发 + 镜头边界限制实现了完整的关卡流程。Boss和敌人追踪提供了有挑战性的游戏体验。`rand()%2` 随机选择攻击方式增加了战斗的不可预测性。

---

## 附：AI工具使用披露

本报告及项目开发过程中使用了以下AI工具：

| AI工具名称 | 版本 | 具体用途 |
|------------|:----:|----------|
| **Claude** | Claude Code 桌面端 | 音频导入与播放控制、界面排版布局、文字格式/大小/颜色等辅助性工作；本实验报告的格式整理 |

**使用位置：** 项目中音频相关代码的编写、`paint.cpp` 中绘制界面的布局（左下角的技能键的透明遮罩、文字格式）、本实验报告的格式整理。
