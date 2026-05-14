#include "individual.h"

Player::Player():
    skill_direction(1)
{
    m_player = QRect(200, GAME_HEIGHT - GROUND_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT);
    m_drawRect = m_player;
    m_attackRect = QRect(0, 0, 0, 0);
    attackWidth = 110;
    attackHeight = 100;

    //精灵表加载
    loadSheet(stand_frames,  "resources/player/player_stand.png",  512, 356, 7);
    loadSheet(run_frames,      "resources/player/player_run.png",   199, 96, 4);
    loadSheet(djump_frames,  "resources/player/player_jump2.png",  340, 190, 6);
    loadSheet(attack_frames, "resources/player/player_attack1.png", 512, 309, 7);
    loadSheet(attack2_frames, "resources/player/player_attack2.png", 512, 309, 7);
    loadSheet(attack3_frames, "resources/player/player_attack3.png", 512, 309, 12);
    loadSheet(attack4_frames, "resources/player/player_attack4.png", 512, 309, 9);
    loadSheet(skill2_frames, "resources/player/skill2.png", 512, 560, 7, 0.35f);

    // 攻击动画设定
    attack_index = 0;
    frame_timer1 = 0;
    frame_speed1 = 3;
    frame_speed2 = 7;
    // 跳跃动画设定
    djump_frame_count = 6;
    djump_index = 0;
    djump_frame_timer = 0;
    djump_frame_speed = 4;
}

static void advanceAttackAnim(int& timer, int speed, int& index, const QVector<QPixmap>& frames, bool& isAttacking)
{
    timer++;
    if (timer >= speed) {
        timer = 0;
        index++;
        if (index >= frames.size()) {
            index = 0;
            isAttacking = false;
        }
    }
}

void Player::attack()
{
    //攻击动画效果
    if (m_isAttacking)
    {
        if (m_attackStage == 1)
            advanceAttackAnim(frame_timer1, frame_speed1, attack_index, attack_frames, m_isAttacking);
        else if (m_attackStage == 2)
            advanceAttackAnim(attack2_frame_timer, attack2_frame_speed, attack2_index, attack2_frames, m_isAttacking);
        else if (m_attackStage == 3)
            advanceAttackAnim(attack3_frame_timer, attack3_frame_speed, attack3_index, attack3_frames, m_isAttacking);
        else if (m_attackStage == 4)
            advanceAttackAnim(attack4_frame_timer, attack4_frame_speed, attack4_index, attack4_frames, m_isAttacking);
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

    }
    if (m_isAttacking) {
        switch (m_attackStage) {
        case 1:  attackWidth = 80;  break;
        case 2:  attackWidth = 90; break;
        case 3:  attackWidth = 100; break;
        case 4:  attackWidth = 120; break;
        default: attackWidth = 80;  break;
        }
    } else {
        attackWidth = 80;
    }

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

    if (is_skill_2)
    {
        skill_2_time--;
        if (skill_2_time <= 0)
            is_skill_2 = false;
        // 技能2动画帧
        if (skill2_frames.isEmpty()) {
            skill2_index = 0;
        } else {
            skill2_timer++;
            if (skill2_timer >= skill2_speed) {
                skill2_timer = 0;
                skill2_index = (skill2_index + 1) % skill2_frames.size();
            }
        }
    }
    else
    {
        skill2_index = 0;
        skill2_timer = 0;
    }

    if (skill_2_cd > 0)
        skill_2_cd--;
    if (is_skill_1 && time1 > 0)
    {

        if (skill_direction == 1) // 向右冲刺
            m_player.moveRight(m_player.right() + 25);
        if (skill_direction == 2) // 向左冲刺
            m_player.moveLeft(m_player.left() - 25);
    }
}

void Player::move(const std::vector<QRect>& m_obstacles)
{

    bool savedR = m_isRMove, savedL = m_isLMove;
    if (m_isAttacking) {
        m_isRMove = false;
        m_isLMove = false;
    }

    individual::move(m_obstacles);

    m_isRMove = savedR;
    m_isLMove = savedL;

    //待机动画
    if (!m_isRMove && !m_isLMove && m_jumptime == 0 && !m_isAttacking)
    {
        if (!stand_frames.isEmpty())
        {
            advanceFrame(stand_frame_timer, stand_frame_speed, stand_index, stand_frames.size());
        }
    }
    else
    {
        stand_index = 0;
        stand_frame_timer = 0;
    }

    //跑步动画
    if (m_isRMove || m_isLMove)
    {
        advanceFrame(frame_timer2, frame_speed2, run_index, run_frames.size());
    }
    else
    {
        run_index = 0;
        frame_timer2 = 0;

    }

    // 二段跳动画
    if (m_jumptime == 2 && djump_frames.size() > 0)
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
                    djump_anim_complete = true;
                }
            }
        }
    }
    else
    {
        djump_index = 0;
        djump_frame_timer = 0;
        djump_anim_complete = false;
    }
    m_drawRect = m_player;

    //回蓝
    if(mp<100)
    {mp+=0.05;}
}
