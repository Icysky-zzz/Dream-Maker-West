
#include "individual.h"

Player::Player():
    skill_direction(1)
{
    m_player = QRect(300, GAME_HEIGHT - GROUND_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT);
    m_drawRect = m_player;
    m_attackRect = QRect(0, 0, 0, 0);

    // === 精灵表加载 ===
    loadSheet(stand_frames,  "resources/player/player_stand.png",  512, 356, 7);
    loadSheet(walk_frames,   "resources/player/player_walk.png",   64, 77, 14);
    loadSheet(run_frames,    "resources/player/player_run.png",    199, 96, 4);
    loadSheet(jump_frames,   "resources/player/player_jump.png",   90, 120, 6);
    // 格式: player_djump.png 是一张 540x120 的长图，包含6帧，每帧 90x120（可选，没有则用 jump）
    loadSheet(djump_frames,  "resources/player/player_jump2.png",  340, 190, 6);
    loadSheet(attack_frames, "resources/player/player_attack1.png", 512, 309, 7);
    loadSheet(attack2_frames, "resources/player/player_attack2.png", 512, 309, 7);
    loadSheet(attack3_frames, "resources/player/player_attack3.png", 512, 309, 12);
    loadSheet(attack4_frames, "resources/player/player_attack4.png", 512, 309, 9);

    // 攻击动画设定
    attack_index = 0;
    frame_timer1 = 0;
    frame_speed1 = 3;
    is_finished1 = false;
    // 移动动画设定
    run_index = 0;
    frame_timer2 = 0;
    frame_speed2 = 7;
    is_finished2 = false;
    // 跳跃动画设定
    jump_frame_count = 6;
    jump_index = 0;
    jump_frame_timer = 0;
    jump_frame_speed = 4;
    djump_frame_count = 6;
    djump_index = 0;
    djump_frame_timer = 0;
    djump_frame_speed = 4;
}

void Player::move(const std::vector<QRect> m_obstacles)
{

    if (is_skill_1)
    {
        m_playerVelocity = 0;
        time1--;

        if (time1 < 0)
        {
            is_skill_1 = false;
            skill_cd = 60;
        }
    }


    if (skill_cd > 0)
        skill_cd--;


    individual::move(m_obstacles);

    if (is_skill_1 && time1 > 0)
    {
        QRect newPlayerRect = m_player;

        if (skill_direction == 1) // 向右冲刺（锁定方向）
            newPlayerRect.moveRight(newPlayerRect.right() + 25);
        if (skill_direction == 2) // 向左冲刺（锁定方向）
            newPlayerRect.moveLeft(newPlayerRect.left() - 25);

        m_player = newPlayerRect; // 应用冲刺位置
    }

    // 攻击加宽动画

    m_drawRect = m_player; // 重置正常大小

    if (m_isAttacking && !m_attacked)
    {
        if (direction == 1)
        {
            // 右攻击：向右加长显示
            m_drawRect.setWidth(PLAYER_WIDTH + 100);
        }
        else
        {
            // 左攻击：向左加长显示
            m_drawRect.setWidth(PLAYER_WIDTH + 100);
            m_drawRect.moveLeft(m_drawRect.left() - 100);
        }
    }
}
