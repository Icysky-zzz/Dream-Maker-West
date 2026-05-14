#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <vector>
#include "individual.h"

#define GAME_WIDTH 800
#define GAME_HEIGHT 600
#define LEVEL_BOUNDARY 1500

class Game : public QWidget
{
    Q_OBJECT
public:
    explicit Game(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void resetGame();

private:
    void creat();
    bool is_start_interface;  // 是否显示初始界面
    QPushButton* btn_start;
    QPushButton *btnRestart;
    QPushButton *btnShowAtkBox;
    bool m_drawAttackBox = false;

    int m_gameTimerId;
    int m_cameraX = 0;
    int m_gameTick = 0;

    QPixmap role_img,m_playerPixmap_jump,m_playerPixmap_skill1,
        hurt_img,m_playerPixmap_jump6;
    QPixmap m_obstaclePixmap,m_groundPixmap;
    QPixmap m_evil_stand,evil_hurt_img;
    QPixmap initial_img;

    Player player;
    Boss* m_boss = nullptr;
    std::vector<enemy*> m_enemies;
    std::vector<QRect> m_obstacles;
    std::vector<QPoint> m_enemySpawnPoints;

    // 敌人波次生成
    bool m_waveTriggered = false;
    int m_waveSpawnCount = 0;
    int m_lastSpawnTick = 0;

    // 关卡系统
    int m_currentLevel = 1;
    bool m_level1Cleared = false;

    // 第二波敌人
    bool m_wave2Triggered = false;
    int m_wave2SpawnCount = 0;
    int m_wave2LastSpawnTick = 0;

    // 掉落物
    struct Drop {
        QPoint pos;
        int type;         // 0=HP, 1=MP
        int value;
        int lifetime;
        int frame;
        int pickupDelay;  // 拾取延迟
        double vy = 0;    // 垂直速度（重力下落）
    };
    std::vector<Drop> m_drops;

    // 设置
    QPushButton* btnSettings;
    QPushButton* btnCloseSettings;
    bool m_showSettings = false;

    // 游戏结束
    bool m_isGameOver = false;

    // 背景音乐
    QMediaPlayer* m_backgroundMusic;
    QAudioOutput* m_audioOutput;

};

#endif // GAME_H
