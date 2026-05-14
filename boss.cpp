#include "individual.h"

Boss::Boss()
{
    hp = 500;
    max_hp = 500;
    QPixmap walkSheet("resources/boss/boss_walk.png");
    int frameW = 120, frameH = 210;
    int targetH = 230;
    walkFrames.clear();
    for (int i = 0; i < 5; i++) {
        QPixmap frame = walkSheet.copy(i * frameW, 0, frameW, frameH)
                            .scaledToHeight(targetH, Qt::SmoothTransformation);
        walkFrames.push_back(frame);
    }
    walkFrameCount = 5;
    walkSpeed = 10;

    // 待机
    bossStandPixmap = QPixmap("resources/boss/boss_stand.png")
                      .scaledToHeight(targetH, Qt::SmoothTransformation);

    int walkFrameW = walkFrames.isEmpty() ? 120 : walkFrames[0].width();
    bossAttackedPixmap = QPixmap("resources/boss/boss_attacked.png")
                         .scaledToWidth(walkFrameW, Qt::SmoothTransformation);

    {
        static bool s_atkLoaded = false;
        static QVector<QPixmap> s_atkFrames;
        static int s_atkCount = 0;

        if (!s_atkLoaded) {
            QPixmap attackSheet("resources/boss/boss_attack.png");
            s_atkCount = 33;
            int atkFrameW = attackSheet.width() / s_atkCount;
            int atkFrameH = attackSheet.height();
            s_atkFrames.clear();
            for (int i = 0; i < s_atkCount; i++) {
                s_atkFrames.push_back(
                    attackSheet.copy(i * atkFrameW, 0, atkFrameW, atkFrameH)
                        .scaledToHeight(180, Qt::SmoothTransformation)
                );
            }
            s_atkLoaded = true;
        }
        attackFrames = s_atkFrames;
        attackFrameCount = s_atkCount;
        attackIndex = 0;
        attackTimer = 0;
        attackSpeed = 3;
        isAttackingAnim = false;
    }


    m_beamBeamSprite = QPixmap("resources/boss/boss_attack1.png")
                       .scaledToHeight(160, Qt::SmoothTransformation);
    m_beamSprite = QPixmap("resources/boss/boss_attack2.png")
                   .scaledToHeight(50, Qt::SmoothTransformation);

    m_isBeamActive = false;
    m_beamDirection = 1;
    m_beamSpeed = 8;
    m_beamCooldown = 180;      // 冷却约3秒
    m_beamTimer = 0;
    m_beamDamage = 15;         // 每次命中造成20伤害
    m_beamLife = 0;
    m_beamWidth = m_beamBeamSprite.width();
    m_beamHeight = m_beamBeamSprite.height();
    m_beamFired = false;
    m_bossAttackType = 1;       // 默认为攻击1（光箭）

    // 攻击框大小
    attackWidth = 80;
    attackHeight = 100;
}

void Boss::fire_beam()
{
    if (m_isBeamActive) return;  // 已有光束/弓箭时不再发射

    if (m_bossAttackType == 1) {
        m_beamSprite = m_beamBeamSprite;
    }
    m_beamWidth = m_beamSprite.width();
    m_beamHeight = m_beamSprite.height();

    m_isBeamActive = true;
    m_beamLife = 0;
    m_beamDirection = (direction == 1) ? 1 : -1;

    int beamY = m_player.top() + (m_player.height() - m_beamHeight) / 2;

    if (m_beamDirection == 1)
    {
        m_beamRect = QRect(m_player.right()-50, beamY, m_beamWidth, m_beamHeight);
    }
    else
    {

        m_beamRect = QRect(m_player.left() - m_beamWidth+50, beamY, m_beamWidth, m_beamHeight);
    }
}

void Boss::update_beam()
{
    if (!m_isBeamActive) return;
    m_beamRect.moveLeft(m_beamRect.left() + m_beamDirection * m_beamSpeed);

    m_beamLife++;

    if (m_beamRect.left() > 2400 || m_beamRect.right() < 0 || m_beamLife >= 120)
    {
        m_isBeamActive = false;
    }
}

bool Boss::check_beam_hit(individual &target)
{
    if (!m_isBeamActive) return false;
    if (m_beamRect.intersects(target.m_player) && !target.m_attacked && !target.once_attacked)
    {
        target.m_attacked = true;
        target.once_attacked = true;
        target.hp -= m_beamDamage;
        if (target.hp < 0) target.hp = 0;
        playPlayerAttackedSound();

        m_isBeamActive = false;  // 命中后光箭消失
        return true;
    }
    return false;
}

void Boss::move(const std::vector<QRect>& m_obstacles)
{
    enemy::move(m_obstacles);
    update_beam();

    if (isAttackingAnim && !m_beamFired)
    {
        int fireFrame = (m_bossAttackType == 2) ? 3 : 19;
        if (attackIndex == fireFrame) {
            fire_beam();
            m_beamFired = true;
            if (m_bossAttackType == 2) {
                isAttackingAnim = false;
                m_isAttacking = false;
                attackIndex = 0;
            }
        }
    }
}

void Boss::boss_trace(individual &player)
{
    if (m_beamTimer > 0)
    {
        m_beamTimer--;
    }

    if (isAttackingAnim || m_isBeamActive)
    {
        m_isRMove = false;
        m_isLMove = false;
        return;
    }

    trace(player);

    int dis = abs(m_player.center().x() - player.m_player.center().x());
    if (dis < 400 && m_beamTimer == 0)
    {
        // 随机选择攻击方式：1=光束, 2=弓箭
        m_bossAttackType = (rand() % 2) + 1;
        m_attackType = 1;  
        attackFrameCount = 33;
        isAttackingAnim = true;
        attackIndex = 0;
        attackTimer = 0;
        m_beamFired = false;
        m_beamTimer = m_beamCooldown;
    }
}
