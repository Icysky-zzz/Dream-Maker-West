# game2

一个使用 Qt (C++) 开发的 2D 游戏项目。

## 项目结构

```
├── main.cpp          # 程序入口
├── game.cpp/h        # 游戏主逻辑
├── player.cpp/h      # 玩家角色
├── enemy.cpp/h       # 敌人
├── collision.cpp/h   # 碰撞检测
├── individual.cpp/h  # 个体对象
├── paint.cpp         # 绘制逻辑
├── CMakeLists.txt    # CMake 构建配置
└── resources/        # 资源文件（图片等）
```

## 构建

使用 CMake 构建：

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## 依赖

- Qt (版本见 CMakeLists.txt)
- C++17 或更高版本
