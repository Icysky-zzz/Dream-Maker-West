#include "individual.h"
#include "collision.h"

enemy::enemy()
{
    m_player = QRect(200, GAME_HEIGHT - GROUND_HEIGHT - PLAYER_HEIGHT,
                     PLAYER_WIDTH, PLAYER_HEIGHT);
    rest_time=200;
    attacked_time = 60;
    is_dead = false;
    hp=100;


    attackSheet.load("resources/enemy/e_attack1.png");
    frameWidth = 750;
    frameHeight = 368;
    attackFrameCount = 13;
    attackIndex = 0;
    attackTimer = 0;
    attackSpeed = 3;
    isAttackingAnim = false;

    attackFrames.clear();
    for (int i = 0; i < attackFrameCount; i++) {
        QRect rect(i * frameWidth, 0, frameWidth, frameHeight);
        attackFrames.push_back(
            attackSheet.copy(rect).scaled(
                750 * 0.3,
                368 * 0.3,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                )
            );
    }
}

void enemy::move(const std::vector<QRect> m_obstacles)
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
    if (isAttackingAnim) {
        attackTimer++;
        if (attackTimer >= attackSpeed) {
            attackTimer = 0;
            attackIndex++;

            if (attackIndex >= attackFrameCount) {
                attackIndex = 0;        // 重置帧
                isAttackingAnim = false;// 关闭攻击动画
                m_isAttacking = false;  // 关闭攻击状态
            }
        }
    }
    m_drawRect = m_player;

    m_player = newPlayerRect;

}

void enemy::e_attack(individual &Me)
{
    if (rest_time > 0) {
        rest_time--;
    }

    if (m_attackRect.intersects(Me.m_player) && !m_attacked)
    {
        if (rest_time == 0 && !Me.m_attacked) {

            m_isAttacking = true;
            Me.m_attacked=true;
            Me.once_attacked=true;
            variabletime = m_attackDuration;
            rest_time = 200;
            if (Me.hp > 0) {
                Me.hp -= 3;
            }
        }
    }
    if(m_attacked || !m_attackRect.intersects(Me.m_player)){
        m_isAttacking = false;
    }

    if (m_isAttacking) {
        isAttackingAnim = true;  // 攻击开始 → 开启动画

        attack();
    }
}

void enemy::is_attacked(individual &Me)
{
    // 玩家技能1冲刺碰撞伤害
    if (Me.is_skill_1 && Me.m_player.intersects(m_player) && !once_attacked)
    {
        m_attacked = true;
        once_attacked = true;
        m_isAttacking = false;
        hp -= 8;
        if (hp <= 0) {
            hp = 0;
            is_dead = true;
        }
    }
    // 玩家普攻
    else if(Me.m_attackRect.intersects(m_player) && Me.m_isAttacking && !once_attacked)
    {
        m_attacked = true;
        once_attacked = true;
        m_isAttacking=false;
        hp -= 10;
        if(hp <= 0) {
            hp = 0;
            is_dead = true;
        }
    }
    if(!Me.m_isAttacking && !Me.is_skill_1)
    {
        once_attacked = false;
    }
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
            m_player.moveLeft(m_player.left() + knockbackDir * knockbackPower);
        }
    }
}

void enemy::trace(individual &myplayer)
{
    // 只设置敌人自身被击退的方向，不再击退玩家
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
