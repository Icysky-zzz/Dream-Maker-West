#include "game.h"
#include "individual.h"

#include <QRect>
#include <QKeyEvent>
#include <QPushButton>
#include <QUrl>
#include <cstdlib>

Game::Game(QWidget *parent):
    QWidget(parent)
{
    is_start_interface = true;
    btn_start = new QPushButton("开始游戏", this);
    btn_start->setGeometry(290, 280, 220, 60);
    btn_start->setStyleSheet(R"(
        QPushButton{
            background-color:#ffffff;
            color:#333;
            font-size:24px;
            font-weight:bold;
            border:none;
            border-radius:8px;
        }
        QPushButton:hover{
            background-color:#eeeeee;
        }
    )");
    connect(btn_start, &QPushButton::clicked, this, [=](){
        is_start_interface = false;
        btn_start->hide();
        // 切换为游戏音乐
        m_backgroundMusic->stop();
        m_backgroundMusic->setSource(QUrl::fromLocalFile("resources/audio/jiuchongtian .mp3"));
        m_backgroundMusic->play();
    });

    // 设置按钮
    btnSettings = new QPushButton("⚙", this);
    btnSettings->setGeometry(GAME_WIDTH - 40, 10, 30, 30);
    btnSettings->setStyleSheet(R"(
        QPushButton{
            background:#555; color:#fff; font-size:20px;
            border:none; border-radius:4px;
        }
        QPushButton:hover{ background:#777; }
    )");
    connect(btnSettings, &QPushButton::clicked, this, [=](){
        m_showSettings = true;
        btnSettings->hide();
        // 将按钮放在设置面板
        btnShowAtkBox->setGeometry(GAME_WIDTH/2 - 80, GAME_HEIGHT/2 - 10, 160, 40);
        btnRestart->setGeometry(GAME_WIDTH/2 - 80, GAME_HEIGHT/2 + 50, 160, 40);
        btnCloseSettings->setGeometry(GAME_WIDTH/2 + 110, GAME_HEIGHT/2 - 60, 30, 30);
        btnShowAtkBox->show();
        btnRestart->show();
        btnCloseSettings->show();
    });

    // 关闭 × 按钮
    btnCloseSettings = new QPushButton("×", this);
    btnCloseSettings->setGeometry(0, 0, 30, 30); // 临时位置
    btnCloseSettings->setStyleSheet(R"(
        QPushButton{
            background:transparent; color:#fff; font-size:24px;
            border:none; font-weight:bold;
        }
        QPushButton:hover{ color:#ff5555; }
    )");
    btnCloseSettings->hide();
    connect(btnCloseSettings, &QPushButton::clicked, this, [=](){
        m_showSettings = false;
        btnSettings->show();
        btnCloseSettings->hide();
        btnShowAtkBox->hide();
        btnRestart->hide();
    });
    //按钮-"重新开始"
    btnRestart = new QPushButton("重新开始", this);
    btnRestart->setGeometry(0, 0, 160, 40);
    connect(btnRestart, &QPushButton::clicked, this, &Game::resetGame);
    btnRestart->hide();

    //按钮-"显示攻击框/隐藏攻击框"
    btnShowAtkBox = new QPushButton("显示攻击框", this);
    btnShowAtkBox->setGeometry(0, 0, 160, 40);
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
    m_playerPixmap_jump = QPixmap("resources/player/jump1.png");
    m_playerPixmap_skill1 = QPixmap("resources/player/skill1.png");
    hurt_img=QPixmap("resources/player/hurt_img.png");
    m_playerPixmap_jump6=QPixmap("resources/player/jump6.png");
    m_evil_stand=QPixmap("resources/enemy/evil_stand.png");
    evil_hurt_img=QPixmap("resources/enemy/evil_hurt_img.png");
    initial_img = QPixmap("resources/bg/initial_img.png");

    //背景音量
    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.1);
    //背景音乐
    m_backgroundMusic = new QMediaPlayer(this);
    m_backgroundMusic->setAudioOutput(m_audioOutput);
    m_backgroundMusic->setSource(QUrl::fromLocalFile("resources/audio/background.mp3"));
    m_backgroundMusic->play();

    // 音频播放循环
    connect(m_backgroundMusic, &QMediaPlayer::mediaStatusChanged, this,
            [=](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia) {
                    m_backgroundMusic->play();
                }
            });

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

    // 游戏结束时任意键重新开始
    if (m_isGameOver) {
        resetGame();
        return;
    }

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
        if (player.m_isAttacking) break;
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
        if (player.check_onground(m_obstacles)) {
            player.m_isJumping = true;
            player.m_jumptime = 1;
        }
        else if (player.m_jumptime == 1) {
            player.m_isJumping = true;
            player.m_jumptime = 2;
        }
        break;
    //技能1（冲刺）
    case Qt::Key_U:
        if (!player.is_skill_2 && !player.is_skill_1
            && player.skill_cd == 0 &&player.mp >= 10 &&!player.m_attacked)
        {
            player.is_skill_1 = true;
            player.time1 =player. skill_1_time;
            player.mp -= 20;
            player.skill_direction = player.direction;  // 锁定冲刺方向
        }
        break;
    //技能2（击飞）
    case Qt::Key_Y:
        if (!player.is_skill_2 && !player.is_skill_1 &&
            player.skill_2_cd == 0 && player.mp >= 15 &&!player.m_attacked) {
            player.is_skill_2 = true;
            player.skill_2_time = 35;
            player.mp -= 15;
            player.skill_2_cd = 90;
        }
        break;
    default:
        break;
    }
}

void Game::timerEvent(QTimerEvent *event)
{
    if(is_start_interface) return;
    // 设置中，暂停游戏
    if (m_showSettings) {
        update();
        return;
    }
    // 游戏结束，不再更新逻辑
    if (m_isGameOver) {
        update();
        return;
    }
    m_gameTick++;

    //玩家
    player.move(m_obstacles);
    player.attack();
    player.attacked();

    // 关卡1边界：清完敌人后才能向右越过1500
    if (m_currentLevel == 1 && !m_level1Cleared) {
        if (player.m_player.right() > LEVEL_BOUNDARY)
            player.m_player.moveRight(LEVEL_BOUNDARY);
    }

    // 第一波敌人生成
    if (!m_waveTriggered && player.m_player.x() >= 1000) {
        m_waveTriggered = true;
        m_waveSpawnCount = 0;
        m_lastSpawnTick = m_gameTick;
    }

    if (m_waveTriggered && m_waveSpawnCount < 5) {
        bool shouldSpawn = (m_waveSpawnCount == 0) ||
                            (m_gameTick - m_lastSpawnTick >= 133);
        if (shouldSpawn) {
            int spawnY = 200;

            // 左边（x≈500）生成
            enemy* e1 = new enemy();
            e1->m_player.moveTo(470 + m_waveSpawnCount * 30, spawnY);
            m_enemies.push_back(e1);

            // 右边（x≈1200）生成
            enemy* e2 = new enemy();
            e2->m_player.moveTo(1120 + m_waveSpawnCount * 30, spawnY);
            m_enemies.push_back(e2);

            m_waveSpawnCount++;
            m_lastSpawnTick = m_gameTick;
        }
    }

    // 敌人

    for (size_t i = 0; i < m_enemies.size(); ) {
        enemy* e = m_enemies[i];

        if (e->is_dead) {
            // 1/3 概率生成掉落物
            if (rand() % 3 == 0) {
                Drop drop;
                drop.pos = QPoint(e->m_player.center().x(), e->m_player.center().y());
                drop.type = rand() % 2;
                drop.value = (drop.type == 0) ? 20 : 15;
                drop.lifetime = 600;
                drop.frame = 0;
                drop.pickupDelay = 30;
                drop.vy = 0;
                m_drops.push_back(drop);
            }

            if (e == m_boss) m_boss = nullptr;
            delete e;
            m_enemies.erase(m_enemies.begin() + i);
        }

        else {
            if (e == m_boss) {
                m_boss->boss_trace(player);
                m_boss->check_beam_hit(player);
            } else if (!e->m_attacked) {
                e->trace(player);
                e->e_attack(player);
            }

            e->move(m_obstacles);
            e->is_attacked(player);
            e->m_drawRect = e->m_player;
            i++;
        }
    }

    // 关卡1清完检测
    if (m_currentLevel == 1 && !m_level1Cleared && m_waveSpawnCount >= 5 && m_enemies.empty()) {
        m_level1Cleared = true;
    }

    // 关卡1→关卡2过渡
    if (m_currentLevel == 1 && m_level1Cleared && player.m_player.x() >= LEVEL_BOUNDARY) {
        m_currentLevel = 2;
        m_wave2Triggered = true;
        m_wave2SpawnCount = 0;
        m_wave2LastSpawnTick = m_gameTick;
    }

    // 第二关：生成Boss
    if (m_wave2Triggered && m_wave2SpawnCount < 1) {
        if (!m_boss) {
            m_boss = new Boss();
            m_boss->m_player.moveTo(2000, 200);
            m_enemies.push_back(m_boss);
            m_wave2SpawnCount = 1;
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

    // 关卡1镜头边界：不超过1500
    if (m_currentLevel == 1 && !m_level1Cleared) {
        if (m_cameraX > LEVEL_BOUNDARY - GAME_WIDTH)
            m_cameraX = LEVEL_BOUNDARY - GAME_WIDTH;
    }

    // 掉落物更新
    for (size_t i = 0; i < m_drops.size(); ) {
        Drop& d = m_drops[i];
        d.lifetime--;
        d.frame++;
        if (d.pickupDelay > 0) d.pickupDelay--;

        // 重力下落
        d.vy += 0.5;
        d.pos.ry() += d.vy;

        // 与障碍物（地面/平台）碰撞
        for (const auto& obstacle : m_obstacles) {
            if (d.pos.x() + 8 > obstacle.left() && d.pos.x() - 8 < obstacle.right()
                && d.pos.y() + 10 >= obstacle.top() && d.pos.ry() - 10 < obstacle.top()
                && d.vy > 0)
            {
                d.pos.ry() = obstacle.top() - 10;
                d.vy = 0;
                break;
            }
        }

        // 掉出地图消失
        if (d.pos.y() > GAME_HEIGHT + 50) {
            m_drops.erase(m_drops.begin() + i);
            continue;
        }

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

    // 检测玩家死亡
    if (player.hp <= 0) {
        player.hp = 0;
        m_isGameOver = true;
    }

    update();
}

void Game::resetGame()
{

    for (enemy* e : m_enemies) {
        delete e;
    }
    m_enemies.clear();
    m_boss = nullptr;
    m_drops.clear();
    m_obstacles.clear();
    creat();
    m_cameraX = 0;
    m_waveTriggered = false;
    m_waveSpawnCount = 0;
    m_lastSpawnTick = 0;
    m_currentLevel = 1;
    m_level1Cleared = false;
    m_wave2Triggered = false;
    m_wave2SpawnCount = 0;
    m_wave2LastSpawnTick = 0;
    m_isGameOver = false;
    player.direction=1;
    player.hp = 100;
    player.mp = 100;
    player.m_player.moveTo(100, GAME_HEIGHT - 50 - player.PLAYER_HEIGHT);
    player.is_skill_2 = false;
    player.skill_2_cd = 0;
    player.skill_2_time = 0;
    // 重置时关闭设置
    m_showSettings = false;
    btnSettings->show();
    btnCloseSettings->hide();
    btnShowAtkBox->hide();
    btnRestart->hide();

    // 重置时重播背景音乐
    m_backgroundMusic->stop();
    m_backgroundMusic->play();

    update();
}

void Game::creat()
{
    //障碍物
    QRect obstacle1(0,320, 200, 50);



    QRect ground(0, 550, 2400, 50);

    m_obstacles.push_back(ground);
    m_obstacles.push_back(obstacle1);

    m_waveTriggered = false;
    m_waveSpawnCount = 0;
    m_lastSpawnTick = 0;
}
