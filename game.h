#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QPushButton>
#include <QPixmap>
#include <vector>
#include "individual.h"

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

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
    bool m_drawAttackBox = true;

    int m_gameTimerId;
    int m_cameraX = 0;
    int m_gameTick = 0;         // 游戏帧计数器

    QPixmap role_img,m_playerPixmap_stand,m_playerPixmap_jump,m_playerPixmap_skill1,hurt_img,m_playerPixmap_jump6;
    QPixmap m_obstaclePixmap,m_groundPixmap;
    QPixmap m_evil_run,m_evil_stand,m_evil_jump,evil_hurt_img;

    Player player;
    std::vector<enemy*> m_enemies;
    std::vector<QRect> m_obstacles;
    std::vector<QPoint> m_enemySpawnPoints;

    // 掉落物
    struct Drop {
        QPoint pos;
        int type;         // 0=HP, 1=MP
        int value;
        int lifetime;
        int frame;
        int pickupDelay;  // 拾取延迟
    };
    std::vector<Drop> m_drops;


};

#endif // GAME_H
