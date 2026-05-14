#include "individual.h"
#include"collision.h"

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
    , mp(100)
    , stand_index(0)
    , stand_frame_timer(0)
    , stand_frame_speed(6)
    , walk_index(0)
    , walk_frame_timer(0)
    , walk_frame_speed(5)
    , jump_frame_count(0)
    , jump_index(0)
    , jump_frame_timer(0)
    , jump_frame_speed(4)
    , djump_frame_count(0)
    , djump_index(0)
    , djump_frame_timer(0)
    , djump_frame_speed(2)
    , djump_anim_complete(false)
    , attack2_index(0)
    , attack2_frame_timer(0)
    , attack2_frame_speed(3)
    , attack2_finished(false)
    , attack3_index(0)
    , attack3_frame_timer(0)
    , attack3_frame_speed(3)
    , attack3_finished(false)
    , attack4_index(0)
    , attack4_frame_timer(0)
    , attack4_frame_speed(3)
    , attack4_finished(false)
    , m_attackStage(1)
    , last_j_tick(-100)
{}

bool individual::check_onground(const std::vector<QRect> m_obstacles)
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

void individual::move(const std::vector<QRect> m_obstacles)
{

    QRect newPlayerRect = m_player;

//垂直速度
    if (is_skill_1)
    {
        m_playerVelocity = 0;
    }
    else
    {
        m_playerVelocity += GRAVITY;
    }
//防止超出左屏幕
    if (newPlayerRect.left() < 0) {
        newPlayerRect.moveLeft(0);
    }
    if (newPlayerRect.right() > 2400) {
        newPlayerRect.moveRight(2400);
    }
//跳&二段跳
    if (m_isJumping) {
        if (m_jumptime == 1) {
            m_playerVelocity = -JUMP_FORCE1;
        }
        if (m_jumptime == 2) {
            m_playerVelocity = -JUMP_FORCE2;
        }
        m_isJumping = false;
    }
//垂直移动
    newPlayerRect.moveTop(newPlayerRect.top() + m_playerVelocity);
//水平移动&攻击方块移动
    if(m_isRMove) {
        newPlayerRect.moveLeft(newPlayerRect.left() + m_playerHorizon);
        if (m_isAttacking) {
            m_attackRect.moveLeft(m_attackRect.left() + m_playerHorizon);
        }
    }
    if(m_isLMove) {
        newPlayerRect.moveLeft(newPlayerRect.left() - m_playerHorizon);
        if (m_isAttacking) {
            m_attackRect.moveLeft(m_attackRect.left() - m_playerHorizon);
        }
    }

     //攻击设置
    int playerMiddleY = m_player.top() + (m_player.height() - attackHeight) / 2;

    if (direction == 1) {
        m_attackRect = QRect(m_player.right(), playerMiddleY, attackWidth, attackHeight);
    } else {
        m_attackRect = QRect(m_player.left()-attackWidth, playerMiddleY, attackWidth, attackHeight);
    }

    //障碍物碰撞
    for(const auto &obstacle : m_obstacles)
    {
        NormalCollision normal;
        normal.m_Collision(m_player,newPlayerRect, obstacle, m_playerVelocity,
                           m_jumptime,  m_isJumping);
    }
    //攻击动画效果 — 各段播完立即结束，连击靠 last_j_tick 时间窗口
    if (m_isAttacking)
    {
        if (m_attackStage == 1)
        {
            frame_timer1++;
            if (frame_timer1 >= frame_speed1)
            {
                frame_timer1 = 0;
                attack_index++;
                if (attack_index >= attack_frames.size())
                    { attack_index = 0; m_isAttacking = false; is_finished1 = true; }
            }
        }
        else if (m_attackStage == 2)
        {
            attack2_frame_timer++;
            if (attack2_frame_timer >= attack2_frame_speed)
            {
                attack2_frame_timer = 0;
                attack2_index++;
                if (attack2_index >= attack2_frames.size())
                    { attack2_index = 0; m_isAttacking = false; attack2_finished = true; }
            }
        }
        else if (m_attackStage == 3)
        {
            attack3_frame_timer++;
            if (attack3_frame_timer >= attack3_frame_speed)
            {
                attack3_frame_timer = 0;
                attack3_index++;
                if (attack3_index >= attack3_frames.size())
                    { attack3_index = 0; m_isAttacking = false; attack3_finished = true; }
            }
        }
        else if (m_attackStage == 4)
        {
            attack4_frame_timer++;
            if (attack4_frame_timer >= attack4_frame_speed)
            {
                attack4_frame_timer = 0;
                attack4_index++;
                if (attack4_index >= attack4_frames.size())
                    { attack4_index = 0; m_isAttacking = false; attack4_finished = true; }
            }
        }
    }
    else
    {
        attack_index = 0;
        frame_timer1 = 0;
        attack2_index = 0;
        attack2_frame_timer = 0;
        attack3_index = 0;
        attack3_frame_timer = 0;
        attack4_index = 0;
        attack4_frame_timer = 0;
        is_finished2 = false;
    }

    //待机动画效果
    if (!m_isRMove && !m_isLMove && m_jumptime == 0 && !m_isAttacking)
    {
        if (!stand_frames.isEmpty())
        {
            stand_frame_timer++;
            if (stand_frame_timer >= stand_frame_speed)
            {
                stand_frame_timer = 0;
                stand_index++;
                if (stand_index >= (int)stand_frames.size())
                    stand_index = 0;
            }
        }
    }
    else
    {
        stand_index = 0;
        stand_frame_timer = 0;
    }

    //移动动画效果 — 直接用跑
    if (m_isRMove || m_isLMove)
    {
        if (!run_frames.isEmpty())
        {
            frame_timer2++;
            if (frame_timer2 >= frame_speed2)
            {
                frame_timer2 = 0;
                run_index++;
                if (run_index >= (int)run_frames.size())
                    run_index = 0;
            }
        }
    }
    else
    {
        run_index = 0;
        walk_index = 0;
        frame_timer2 = 0;
        walk_frame_timer = 0;
        is_finished2 = false;
    }

    // 跳跃动画
    if (m_jumptime == 1 && jump_frames.size() > 0)
    {
        jump_frame_timer++;
        if (jump_frame_timer >= jump_frame_speed)
        {
            jump_frame_timer = 0;
            jump_index++;
            if (jump_index >= (int)jump_frames.size())
                jump_index = jump_frames.size() - 1;
        }
    }
    else if (m_jumptime == 2 && djump_frames.size() > 0)
    {
        if (!djump_anim_complete) {
            djump_frame_timer++;
            if (djump_frame_timer >= djump_frame_speed)
            {
                djump_frame_timer = 0;
                djump_index++;
                if (djump_index >= (int)djump_frames.size())
                {
                    djump_index = djump_frames.size() - 1;
                    djump_anim_complete = true;   // 播完 → 之后画 jump6
                }
            }
        }
    }
    else
    {
        jump_index = 0;
        jump_frame_timer = 0;
        djump_index = 0;
        djump_frame_timer = 0;
        djump_anim_complete = false;
    }

    m_drawRect = m_player;

    //回蓝
    if(mp<100)
    {mp+=0.05;}

    m_player = newPlayerRect;
}
