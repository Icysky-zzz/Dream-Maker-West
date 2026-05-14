#include "game.h"
#include "individual.h"

#include <QRect>
#include <QKeyEvent>
#include <QPushButton>

Game::Game(QWidget *parent):
    QWidget(parent)
{
    is_start_interface = true;
    btn_start = new QPushButton("开始游戏", this);
    btn_start->setGeometry(400, 300, 220, 70);
    btn_start->setStyleSheet(R"(
        QPushButton{
            background-color:#ffcc00;
            color:#000;
            font-size:26px;
            font-weight:bold;
            border:none;
        }
        QPushButton:hover{
            background-color:#ffdd33;
        }
    )");
    connect(btn_start, &QPushButton::clicked, this, [=](){
        is_start_interface = false;
        btn_start->hide();
    });

    btnRestart = new QPushButton("重新开始", this);
    btnRestart->setGeometry(GAME_WIDTH - 120, 10, 110, 30);
    connect(btnRestart, &QPushButton::clicked, this, &Game::resetGame);
    btnRestart->hide();

    //按钮-"显示攻击框/隐藏攻击框"
    btnShowAtkBox = new QPushButton("显示攻击框", this);
    btnShowAtkBox->setGeometry(GAME_WIDTH - 240, 10, 110, 30);
    connect(btnShowAtkBox, &QPushButton::clicked, this,
            [=](){
                m_drawAttackBox = !m_drawAttackBox;

                if(m_drawAttackBox)
                    btnShowAtkBox->setText("隐藏攻击框");
                else
                    btnShowAtkBox->setText("显示攻击框");
                update();
            });
    btnShowAtkBox->hide(); // 初始隐藏

    //创建页面
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    setWindowTitle("造梦西游");
    creat();

    //游戏设置帧数
    m_gameTimerId = startTimer(15);

    //导入图片
    role_img = QPixmap("resources/ui/role.png");
    m_obstaclePixmap = QPixmap("resources/bg/obstacle.png");
    m_groundPixmap = QPixmap("resources/bg/ground.png");
    m_playerPixmap_stand = QPixmap("resources/player/stand.png");
    m_playerPixmap_jump = QPixmap("resources/player/jump.png");
    m_playerPixmap_skill1 = QPixmap("resources/player/skill1.png");
    m_playerPixmap_jump=QPixmap("resources/player/jump1.png");
    hurt_img=QPixmap("resources/player/hurt_img.png");
    m_playerPixmap_jump6=QPixmap("resources/player/jump6.png");
    m_evil_run=QPixmap("resources/enemy/evil_stand.png");
    m_evil_stand=QPixmap("resources/enemy/evil_stand.png");
    m_evil_jump=QPixmap("resources/enemy/evil_stand.png");
    evil_hurt_img=QPixmap("resources/enemy/evil_hurt_img.png");


}

//松开按键触发事件
void Game::keyReleaseEvent(QKeyEvent *event)
{
    if(is_start_interface) return;

    if(event->key() == Qt::Key_D ) {
        player.m_isRMove = false;
    }
    if(event->key() == Qt::Key_A ) {
        player.m_isLMove = false;
    }
}
//按压按键触发事件
void Game::keyPressEvent(QKeyEvent *event)
{
    if(is_start_interface) return;

    switch (event->key()) {
    //移动
    case Qt::Key_D:
        player.m_isRMove = true;
        player.direction = 1;
        player.m_playerHorizon = 4;
        break;
    case Qt::Key_A:
        player.m_isLMove = true;
        player.direction = 2;
        player.m_playerHorizon = 3;
        break;
    //攻击
    case Qt::Key_J:
        if(!player.m_attacked){
            // 连击：1→2→3→4，间隔<65帧（动画播完也能连）
            if (player.m_attackStage < 4
                && m_gameTick - player.last_j_tick < 65 && player.last_j_tick > 0)
            {
                player.m_attackStage++;
                if (player.m_attackStage == 2) {
                    player.attack2_index = 0;
                    player.attack2_frame_timer = 0;
                } else if (player.m_attackStage == 3) {
                    player.attack3_index = 0;
                    player.attack3_frame_timer = 0;
                } else if (player.m_attackStage == 4) {
                    player.attack4_index = 0;
                    player.attack4_frame_timer = 0;
                }
            } else {
                player.m_attackStage = 1;
            }
            player.last_j_tick = m_gameTick;
            player.m_isAttacking = true;
            player.variabletime = player.m_attackDuration;
        }
        break;
    //跳跃
    case Qt::Key_K:
        if (player.check_onground(m_obstacles) && player.m_jumptime == 0) {
            player.m_isJumping = true;
            player.m_jumptime = 1;
        }
        else if (!player.check_onground(m_obstacles) && player.m_jumptime == 1) {
            player.m_isJumping = true;
            player.m_jumptime = 2;
        }
        break;
    //技能1
    case Qt::Key_U:
        if (!player.is_skill_1 && player.skill_cd == 0 &&player.mp >= 10 &&!player.m_attacked)
        {
            player.is_skill_1 = true;
            player.time1 =player. skill_1_time;
            player.mp -= 20;
            player.skill_direction = player.direction;  // 锁定冲刺方向
        }
        break;
    default:
        break;
    }
}

void Game::timerEvent(QTimerEvent *event)
{
    if(is_start_interface) return;

    m_gameTick++;

    //玩家
    player.move(m_obstacles);
    player.attack();
    player.attacked();

    // 敌人

    for (size_t i = 0; i < m_enemies.size(); ) {
        enemy* e = m_enemies[i];

        if (e->is_dead) {
            // 1/5 概率生成掉落物
            if (rand() % 5 == 0) {
                Drop drop;
                drop.pos = QPoint(e->m_player.center().x(), e->m_player.center().y());
                drop.type = rand() % 2;
                drop.value = (drop.type == 0) ? 20 : 15;
                drop.lifetime = 600;
                drop.frame = 0;
                drop.pickupDelay = 30;
                m_drops.push_back(drop);
            }

            delete e;
            m_enemies.erase(m_enemies.begin() + i);
        }

        else {
            e->move(m_obstacles);
            e->trace(player);
            e->e_attack(player);
            e->is_attacked(player);
            e->m_drawRect = e->m_player;
            i++;
        }
    }


    // 镜头跟随

    int screenCenter = GAME_WIDTH / 2;
    int playerCenterX = player.m_player.center().x();
    int dif = playerCenterX - (m_cameraX + screenCenter);

    if (dif > GAME_WIDTH / 6)
        m_cameraX = playerCenterX - (screenCenter + GAME_WIDTH / 6);
    else if (dif < -GAME_WIDTH / 6)
        m_cameraX = playerCenterX - (screenCenter - GAME_WIDTH / 6);
    //左边界
    if (m_cameraX < 0)
        m_cameraX = 0;
    //右边界
    if (m_cameraX > 2400 - GAME_WIDTH)
        m_cameraX = 2400 - GAME_WIDTH;

    // 掉落物更新
    for (size_t i = 0; i < m_drops.size(); ) {
        Drop& d = m_drops[i];
        d.lifetime--;
        d.frame++;
        if (d.pickupDelay > 0) d.pickupDelay--;

        QRect dropRect(d.pos.x() - 10, d.pos.y() - 10, 20, 20);
        if (d.pickupDelay <= 0 && dropRect.intersects(player.m_player)) {
            if (d.type == 0)
                player.hp = qMin(player.hp + d.value, 100);
            else
                player.mp = qMin(player.mp + d.value, 100.0f);
            m_drops.erase(m_drops.begin() + i);
        }
        else if (d.lifetime <= 0) {
            m_drops.erase(m_drops.begin() + i);
        }
        else {
            i++;
        }
    }

    update();
}

void Game::resetGame()
{

    for (enemy* e : m_enemies) {
        delete e;
    }
    m_enemies.clear();
    m_drops.clear();
    m_obstacles.clear();
    creat();
    m_cameraX = 0;
    player.direction=1;
    player.hp = 100;
    player.mp = 100;
    player.m_player.moveTo(100, GAME_HEIGHT - 50 - player.PLAYER_HEIGHT);

    update();
}

void Game::creat()
{
    //障碍物
    QRect obstacle1(400,380, 150, 40);
    QRect obstacle2(800,380, 150, 40);
    QRect obstacle3(1200,380, 150, 40);
    QRect ground(0, 550, 2400, 50);

    m_obstacles.push_back(ground);
    m_obstacles.push_back(obstacle1);
    m_obstacles.push_back(obstacle2);
    m_obstacles.push_back(obstacle3);
    //敌人出生点
    m_enemySpawnPoints.clear();

    m_enemySpawnPoints.push_back(QPoint(600, 300));
    m_enemySpawnPoints.push_back(QPoint(800, 300));
    m_enemySpawnPoints.push_back(QPoint(1000, 300));
    m_enemySpawnPoints.push_back(QPoint(1200, 300));
    m_enemySpawnPoints.push_back(QPoint(1400, 300));



    //生成敌人
    for (const QPoint& pos : m_enemySpawnPoints) {
        enemy* e = new enemy();
        e->m_player.moveTo(pos.x(), pos.y());
        m_enemies.push_back(e);
    }
}
