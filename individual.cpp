#include "individual.h"
#include"collision.h"
#include <QMediaPlayer>
#include <QAudioOutput>

void playPlayerAttackedSound()
{
    static QMediaPlayer* player = nullptr;
    static QAudioOutput* output = nullptr;
    if (!player) {
        output = new QAudioOutput();
        output->setVolume(1.0);
        player = new QMediaPlayer();
        player->setAudioOutput(output);
        player->setSource(QUrl::fromLocalFile("resources/audio/player_attacked.mp3"));
    }
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->setPosition(0);
    } else {
        player->play();
    }
}

void individual::loadSheet(QVector<QPixmap>& frames, const QString& path,
                            int frameW, int frameH, int count, float scale)
{
    QPixmap sheet(path);
    frames.clear();
    for (int i = 0; i < count; i++) {
        QRect rect(i * frameW, 0, frameW, frameH);
        QPixmap frame = sheet.copy(rect);
        if (scale != 1.0f) {
            frame = frame.scaled(frameW * scale, frameH * scale,
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        frames.push_back(frame);
    }
}

individual::individual() :
      m_playerVelocity(0)
    , m_playerHorizon(2)
    , m_isRMove(false)
    , m_isLMove(false)
    , m_isJumping(false)
    , m_jumptime(0)
    , direction(1)
    , m_isAttacking(false)
    , m_attacked(false)
    , m_attackDuration(40)
    , attacked_time(15)
    , variabletime(15)
    , is_skill_1(false)
    , skill_1_time(20)
    , hp(100)
    , max_hp(100)
    , mp(100)
    , stand_index(0)
    , stand_frame_timer(0)
    , stand_frame_speed(6)

    , djump_frame_count(0)
    , djump_index(0)
    , djump_frame_timer(0)
    , djump_frame_speed(2)
    , djump_anim_complete(false)
    , attack2_index(0)
    , attack2_frame_timer(0)
    , attack2_frame_speed(3)

    , attack3_index(0)
    , attack3_frame_timer(0)
    , attack3_frame_speed(3)

    , attack4_index(0)
    , attack4_frame_timer(0)
    , attack4_frame_speed(3)

    , m_attackStage(1)
    , last_j_tick(-100)
{}

bool individual::check_onground(const std::vector<QRect>& m_obstacles)
{
    for(const auto &obstacle : m_obstacles) {
        if (m_player.bottom() >= obstacle.top()
            && m_player.right() > obstacle.left()
            && m_player.left() < obstacle.right())
        {
            return true;
        }
    }
    return false;
}

void individual::attack()
{
    if (m_isAttacking)
    {
        variabletime--;
        if (variabletime <= 0) {
            m_isAttacking = false;
        }
    }

}

void individual::attacked()
{
    if(m_attacked)
    {
        attacked_time--;
        if(attacked_time < 0)
        {
            m_attacked = false;
            once_attacked=false;
            attacked_time = 10;
            knockbackDir = 0;
        }
        else
        {m_player.moveLeft(m_player.left() + knockbackDir * knockbackPower);}
    }
}

void individual::move(const std::vector<QRect>& m_obstacles)
{
    QRect newPlayerRect = m_player;

//垂直速度
    m_playerVelocity += GRAVITY;
    newPlayerRect.moveTop(newPlayerRect.top() + m_playerVelocity);

//保存跳跃状态
    bool wantsJump = m_isJumping;
    int wantedJumptime = m_jumptime;

    //水平移动
    if (!m_attacked) {
        if(m_isRMove)
            newPlayerRect.moveLeft(newPlayerRect.left() + m_playerHorizon);
        if(m_isLMove)
            newPlayerRect.moveLeft(newPlayerRect.left() - m_playerHorizon);
    }
    //防止超出左右屏幕
    if (newPlayerRect.left() < 0) {
        newPlayerRect.moveLeft(0);
    }
    if (newPlayerRect.right() > 2400) {
        newPlayerRect.moveRight(2400);
    }
     //攻击方块位置
    int playerMiddleY = newPlayerRect.top() + (newPlayerRect.height() - attackHeight) / 2;
    if (direction == 1) {
        m_attackRect = QRect(newPlayerRect.right(), playerMiddleY, attackWidth, attackHeight);
    } else {
        m_attackRect = QRect(newPlayerRect.left() - attackWidth, playerMiddleY, attackWidth, attackHeight);
    }

    //障碍物碰撞
    for(const auto &obstacle : m_obstacles)
    {
        NormalCollision normal;
        normal.m_Collision(m_player, newPlayerRect, obstacle, m_playerVelocity,
                           m_jumptime,  m_isJumping);
    }

    // 跳&二段跳
    if (wantsJump) {
        m_jumptime = wantedJumptime;
        if (wantedJumptime == 1)
            m_playerVelocity = -JUMP_FORCE1;
        else if (wantedJumptime == 2)
            m_playerVelocity = -JUMP_FORCE2;
        m_isJumping = false;
        newPlayerRect.moveTop(newPlayerRect.top() + m_playerVelocity);
    }

    m_player = newPlayerRect;
}
