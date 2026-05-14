#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimerEvent>
#include <vector>
#include <QFont>
#include <QPixmap>
#include <QTransform>

const int GAME_WIDTH = 800;       // 游戏窗口宽度
const int GAME_HEIGHT = 600;      // 游戏窗口高度
const int GROUND_HEIGHT = 50;     // 地面高度
const int PLAYER_WIDTH = 90;      // 玩家宽度
const int PLAYER_HEIGHT = 120;     // 玩家高度
const int JUMP_FORCE1 = 15;
const int JUMP_FORCE2 = 12;
const double GRAVITY = 0.8;       // 重力加速度

class Game : public QWidget
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);

protected:

    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    // 游戏核心函数
    void resetGame();             // 重置游戏
    void creatobstacle();         // 创建障碍物
    int m_cameraX = 0;
    // 玩家相关变量
    QRect m_player;               // 玩家矩形
    double m_playerVelocity;      // 垂直速度
    int m_playerHorizon;          // 水平速度
    bool m_isJumping;
    bool m_isRMove;               // 右移标记
    bool m_isLMove;               // 左移标记
    int m_jumptime;               // 跳跃次数
    bool is_skill_1;
    QPixmap m_playerPixmap_stand;
    QPixmap m_playerPixmap_jump;
    QPixmap m_playerPixmap_run;

    // 游戏状态变量
    int m_gameTimerId;            // 游戏定时器ID
    bool m_gameOver;              // 游戏结束标记
    int m_score;                  // 分数
    int direction;                // 玩家方向（1=右，2=左）

    // 攻击相关变量
    QRect m_attackRect;           // 攻击矩形
    bool m_isAttacking;           // 是否正在攻击
    int m_attackDuration;         // 攻击持续时间（帧数）
    int variabletime;
    int time1;
    QPixmap m_playerPixmap_skill1;
    // 障碍物相关
    QPixmap m_obstaclePixmap;
    std::vector<QRect> m_obstacles; // 普通障碍物列表
    std::vector<QRect> m_bound;    // 特殊边界列表
};

#endif // GAME_H
