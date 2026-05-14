#include "individual.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <cstdlib>

// 攻击音效
static QMediaPlayer* attackSound() {
    static QMediaPlayer* player = nullptr;
    static QAudioOutput* output = nullptr;
    if (!player) {
        output = new QAudioOutput();
        output->setVolume(1);
        player = new QMediaPlayer();
        player->setAudioOutput(output);
        player->setSource(QUrl::fromLocalFile("resources/audio/attack.mp3"));
    }
    return player;
}

enemy::enemy()
{
    m_player = QRect(200, GAME_HEIGHT - GROUND_HEIGHT - PLAYER_HEIGHT,
                     PLAYER_WIDTH, PLAYER_HEIGHT);
    rest_time=200;
    attacked_time = 60;
    is_dead = false;
    hp=100;
    // 静态缓存
    static bool s_loaded = false;
    static int s_frameWidth, s_frameHeight, s_attackFrameCount;
    static QPixmap s_attackSheet;
    static QVector<QPixmap> s_attackFrames;
    static QVector<QPixmap> s_walkFrames;
    static int s_walkFrameCount = 4;

    if (!s_loaded) {
        s_frameWidth = 750;
        s_frameHeight = 368;
        s_attackFrameCount = 13;

        s_attackSheet.load("resources/enemy/e_attack1.png");
        s_attackFrames.clear();
        for (int i = 0; i < s_attackFrameCount; i++) {
            QRect rect(i * s_frameWidth, 0, s_frameWidth, s_frameHeight);
            s_attackFrames.push_back(
                s_attackSheet.copy(rect).scaled(
                    750 * 0.35, 368 * 0.35,
                    Qt::KeepAspectRatio, Qt::SmoothTransformation
                )
            );
        }

        QPixmap walkSheet("resources/enemy/enemy_walk.png");
        int walkFrameW = walkSheet.width() / s_walkFrameCount;
        int walkFrameH = walkSheet.height();
        s_walkFrames.clear();
        for (int i = 0; i < s_walkFrameCount; i++) {
            QRect rect(i * walkFrameW, 0, walkFrameW, walkFrameH);
            s_walkFrames.push_back(
                walkSheet.copy(rect).scaled(
                    walkFrameH * 0.24 * 3 / 4,
                    walkFrameH * 0.24,
                    Qt::IgnoreAspectRatio, Qt::SmoothTransformation
                )
            );
        }
        s_loaded = true;
    }

    // 攻击2
    static QVector<QPixmap> s_attack2Frames;
    static int s_attack2FrameCount = 10;
    if (s_attack2Frames.isEmpty()) {
        QPixmap atk2Sheet("resources/enemy/enemy_attack2.png");
        int atk2FW = atk2Sheet.width() / s_attack2FrameCount;
        int atk2FH = atk2Sheet.height();
        for (int i = 0; i < s_attack2FrameCount; i++) {
            s_attack2Frames.push_back(
                atk2Sheet.copy(i * atk2FW, 0, atk2FW, atk2FH)
                    .scaledToHeight(130, Qt::SmoothTransformation)
            );
        }
    }

    attackSheet = s_attackSheet;
    frameWidth = s_frameWidth;
    frameHeight = s_frameHeight;
    attackFrameCount = s_attackFrameCount;
    attackFrames = s_attackFrames;
    walkFrames = s_walkFrames;
    walkFrameCount = s_walkFrameCount;

    attackIndex = 0;
    attackTimer = 0;
    attackSpeed = 3;
    isAttackingAnim = false;

    attack2Frames = s_attack2Frames;
    attack2FrameCount = s_attack2FrameCount;
    attack2Index = 0;
    attack2Timer = 0;
    attack2Speed = 3;
    m_attackType = 1;

    walkIndex = 0;
    walkTimer = 0;
}

void enemy::move(const std::vector<QRect>& m_obstacles)
{
    // 利用基类处理
    individual::move(m_obstacles);

    if (isAttackingAnim) {
        int& idx = (m_attackType == 2) ? attack2Index : attackIndex;
        int& tmr = (m_attackType == 2) ? attack2Timer : attackTimer;
        int spd = (m_attackType == 2) ? attack2Speed : attackSpeed;
        int cnt = (m_attackType == 2) ? attack2FrameCount : attackFrameCount;

        tmr++;
        if (tmr >= spd) {
            tmr = 0;
            idx++;
            if (idx >= cnt) {
                idx = 0;
                isAttackingAnim = false;
                m_isAttacking = false;
            }
        }
    }

    // 行走动画
    if (!isAttackingAnim && !m_attacked && (m_isRMove || m_isLMove)) {
        advanceFrame(walkTimer, walkSpeed, walkIndex, walkFrameCount);
    } else {
        walkIndex = 0;
        walkTimer = 0;
    }
    m_drawRect = m_player;
}

void enemy::e_attack(individual &Me)
{
    if (rest_time > 0) {
        rest_time--;
    }

    if (m_attackRect.intersects(Me.m_player) && !m_attacked)
    {
        if (rest_time == 0 && !Me.m_attacked) {

            m_attackType = (rand() % 2) + 1;  // 随机1/2选择攻击方式
            m_isAttacking = true;
            Me.m_attacked=true;
            Me.once_attacked=true;
            variabletime = m_attackDuration;
            rest_time = 200;
            if (Me.hp > 0) {
                Me.hp -= 3;
                playPlayerAttackedSound();
            }
        }
    }
    if(m_attacked || !m_attackRect.intersects(Me.m_player)){
        m_isAttacking = false;
    }

    if (m_isAttacking) {
        isAttackingAnim = true; 

        attack();
    }
}

void enemy::is_attacked(individual &Me)
{

    if (!Me.m_isAttacking && !Me.is_skill_1 && !Me.is_skill_2) {
        lastHitStage = 0;
    }

    if (Me.is_skill_1 && Me.m_player.intersects(m_player))
    {
        if (lastHitStage == -1) goto knockback;
        lastHitStage = -1;
        attackSound()->play();
        m_attacked = true;
        m_isAttacking = false;
        attacked_time = 30;
        hp -= 8;
        m_playerVelocity = -3;
        knockbackDir = (Me.m_player.x() > m_player.x()) ? -1 : 1;
        if (hp <= 0) {
            hp = 0;
            is_dead = true;
        }
    }
    // 玩家技能2（击飞）
    else if (Me.is_skill_2
             && Me.m_attackRect.intersects(m_player) && lastHitStage != -2)
    {
        lastHitStage = -2;
        m_attacked = true;
        m_isAttacking = false;
        attacked_time = 15;
        hp -= 20;
        m_playerVelocity = -20;
        knockbackDir = 0;
        if (hp <= 0) {
            hp = 0;
            is_dead = true;
        }
    }
    // 玩家普攻
    else if(Me.m_attackRect.intersects(m_player) && Me.m_isAttacking)
    {
        if (Me.m_attackStage == lastHitStage) goto knockback;
        lastHitStage = Me.m_attackStage;
        attackSound()->play();
        m_attacked = true;
        m_isAttacking=false;
        attacked_time = 30;
        hp -= 10;
        m_playerVelocity = -4;   // 轻微击飞
        knockbackDir = (Me.m_player.x() > m_player.x()) ? -1 : 1;
        if(hp <= 0) {
            hp = 0;
            is_dead = true;
        }
    }

knockback:
    if(m_attacked)
    {
        m_isAttacking=false;
        attacked_time--;
        if(attacked_time < 0)
        {
            m_attacked = false;
            attacked_time = 10;
            knockbackDir = 0;
        }
        else
        {
            m_player.moveLeft(m_player.left() + knockbackDir * 4);
        }
    }
}

void enemy::trace(individual &myplayer)
{

    if (myplayer.m_player.x() > m_player.x()) {
        knockbackDir = -1;
    } else {
        knockbackDir = 1;
    }
    if(m_player.right()<=myplayer.m_player.left()-10)
    {
        m_isLMove=false;
        direction=1;
        m_isRMove=true;
    }
    if(m_player.left()>=myplayer.m_player.right()+10)
    {
        m_isRMove=false;
        direction=2;
        m_isLMove=true;
    }
    if(m_player.right()>myplayer.m_player.left()-10 &&
        m_player.left()<myplayer.m_player.right()+10)
    {
        m_isRMove=false;
        m_isLMove=false;
    }
}
