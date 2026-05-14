#include "game.h"
#include <QRect>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QtMath>

// 水平翻转精灵图（方向向左时翻转）
static QPixmap flipH(const QPixmap& src, int dir)
{
    if (dir == 2)
        return src.transformed(QTransform().scale(-1, 1));
    return src;
}

void Game::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);


    if(is_start_interface){
        painter.drawPixmap(0, 0, GAME_WIDTH, GAME_HEIGHT, initial_img);
        return;
    }

    //背景
    static QPixmap bg("resources/bg/background.png");
    painter.drawPixmap(0, 0, GAME_WIDTH, GAME_HEIGHT, bg);


    int barW = 120, barH = 14;
    painter.setPen(QPen(QColor(50,50,50), 2));
    painter.setBrush(QColor(30,30,30));
    QRect hpbar = QRect(75, 25, barW, barH);
    painter.drawRect(hpbar); // 直角

    painter.setPen(Qt::NoPen);
    QLinearGradient hpGrad(hpbar.left(), 0, hpbar.right(), 0);
    hpGrad.setColorAt(0, QColor(255, 100, 100));
    hpGrad.setColorAt(1, QColor(220, 40, 40));
    painter.setBrush(hpGrad);
    QRect hprect = QRect(77, 27, player.hp * 1.2, barH-4);
    painter.drawRect(hprect);

    // 血量数字
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
    painter.drawText(hpbar, Qt::AlignCenter, QString::number(player.hp) + "/100");


    painter.setPen(QPen(QColor(50,50,50), 2));
    painter.setBrush(QColor(30,30,30));
    QRect mpbar = QRect(75, 50, barW, barH);
    painter.drawRect(mpbar);

    painter.setPen(Qt::NoPen);
    QLinearGradient mpGrad(mpbar.left(), 0, mpbar.right(), 0);
    mpGrad.setColorAt(0, QColor(80, 180, 255));
    mpGrad.setColorAt(1, QColor(30, 120, 220));
    painter.setBrush(mpGrad);
    QRect mprect = QRect(77, 52, player.mp * 1.2, barH-4);
    painter.drawRect(mprect);

    // 蓝量数字
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
    painter.drawText(mpbar, Qt::AlignCenter, QString::number((int)player.mp) + "/100");


    if (m_boss && !m_boss->is_dead) {
        int bossBarW = 300, bossBarH = 22;
        int bossBarX = (GAME_WIDTH - bossBarW) / 2;
        int bossBarY = 12;

        // 背景
        painter.setPen(QPen(QColor(80, 40, 40), 2));
        painter.setBrush(QColor(30, 10, 10));
        painter.drawRect(bossBarX, bossBarY, bossBarW, bossBarH);

        // 血条
        painter.setPen(Qt::NoPen);
        QLinearGradient bGrad(bossBarX, 0, bossBarX + bossBarW, 0);
        bGrad.setColorAt(0, QColor(255, 60, 60));
        bGrad.setColorAt(1, QColor(200, 20, 20));
        painter.setBrush(bGrad);
        int hpW = (bossBarW - 4) * m_boss->hp / m_boss->max_hp;
        painter.drawRect(bossBarX + 2, bossBarY + 2, hpW, bossBarH - 4);

        // Boss 
        painter.setPen(QColor(255, 210, 0));
        painter.setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
        painter.drawText(bossBarX + 8, bossBarY, bossBarW, bossBarH,
                         Qt::AlignVCenter, "BOSS");

        painter.setPen(Qt::white);
        painter.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        QString hpText = QString::number(m_boss->hp) + "/" + QString::number(m_boss->max_hp);
        painter.drawText(bossBarX, bossBarY, bossBarW - 8, bossBarH,
                         Qt::AlignVCenter | Qt::AlignRight, hpText);
    }


    for (enemy* e : m_enemies) {
        if (e->is_dead) continue;
        if (e == m_boss) continue;  

        QRect drawevil = e->m_player.translated(-m_cameraX, 0);

        int hpBarW = 55;
        int hpBarH = 7;
        QRect barBg(
            drawevil.center().x() - hpBarW/2,
            drawevil.top() - 22,
            hpBarW,
            hpBarH
            );

        painter.setPen(QColor(40,40,40));
        painter.setBrush(QColor(25,25,25));
        painter.drawRect(barBg);

        painter.setBrush(QColor(230, 50, 50));
        painter.setPen(Qt::NoPen);
        QRect hpRect = barBg.adjusted(1,1,-1,-1);
        int hpRatioMax = (e->max_hp > 0) ? e->max_hp : 100;
        hpRect.setWidth( (hpBarW-2) * e->hp / hpRatioMax );
        painter.drawRect(hpRect);

        // 敌人血量
        painter.setPen(Qt::white);
        painter.setFont(QFont("Microsoft YaHei", 7, QFont::Bold));
        painter.drawText(barBg, Qt::AlignCenter, QString::number(e->hp));
    }

    //画障碍物
    for (const auto& obstacle : m_obstacles) {
        if(obstacle.y()>500)
        {
            painter.drawPixmap(obstacle.x()- m_cameraX, obstacle.y(),
                               obstacle.width(), obstacle.height(), m_groundPixmap);
        }
        else{
            painter.drawPixmap(obstacle.x()- m_cameraX, obstacle.y(),
                               obstacle.width(), obstacle.height(), m_obstaclePixmap);
        }
    }

    //头像
    QRect role=QRect(0,0,75,75);
    painter.setPen(QPen(QColor(255, 210, 0), 3));
    painter.setBrush(QColor(40,40,40));
    painter.drawRect(role.adjusted(-2,-2,2,2)); 
    painter.drawPixmap(role, role_img);

    QRect drawPlayer = player.m_drawRect.translated(-m_cameraX, 0);

    //攻击动画
    if (player.m_attacked)
    {
        painter.drawPixmap(drawPlayer, flipH(hurt_img, player.direction));
    }
    // 2. 攻击动画
    else if(player.m_isAttacking)
    {
        // 选帧
        QVector<QPixmap>* frames = nullptr;
        int idx = 0, h = 300, yOff = 25;
        if (player.m_attackStage == 1) {
            frames = &player.attack_frames;
            idx = player.attack_index;
        } else if (player.m_attackStage == 2) {
            frames = &player.attack2_frames;
            idx = player.attack2_index;
            h = 200; yOff = 5;
        } else if (player.m_attackStage == 3) {
            frames = &player.attack3_frames;
            idx = player.attack3_index;
            h = 200; yOff = 5;
        } else if (player.m_attackStage == 4) {
            frames = &player.attack4_frames;
            idx = player.attack4_index;
            h = 200; yOff = 5;
        }
        if (frames && !frames->isEmpty() && idx < frames->size()) {
            QPixmap frame = flipH((*frames)[idx], player.direction).scaledToHeight(h, Qt::SmoothTransformation);
            int sx = drawPlayer.center().x() - frame.width() / 2;
            if (player.direction == 1) sx += 50;
            else sx -= 50;
            painter.drawPixmap(sx, drawPlayer.bottom() - frame.height() + yOff, frame);
        }
    }

    else if (player.is_skill_2 && !player.skill2_frames.isEmpty())
    {
        int idx = qMin(player.skill2_index, player.skill2_frames.size() - 1);
        QPixmap frame = flipH(player.skill2_frames[idx], player.direction).scaledToHeight(200, Qt::SmoothTransformation);
        int sx = drawPlayer.center().x() - frame.width() / 2;
        painter.drawPixmap(sx, drawPlayer.bottom() - frame.height() + 10, frame);
    }

    //其他动作（技能，移动，跳跃）
    else
    {
        bool moving = (player.direction == 1) ? player.m_isRMove : player.m_isLMove;

        if (player.is_skill_1) {
            int xOff = (player.direction == 2) ? 170 : 50;
            painter.drawPixmap(drawPlayer.left() - xOff, drawPlayer.top(), 200, 80,
                               flipH(m_playerPixmap_skill1, player.direction));
        }
        else if (!player.check_onground(m_obstacles)) {
            if (player.m_jumptime == 2 && !player.djump_frames.isEmpty()) {
                const QPixmap& src = player.djump_anim_complete
                    ? m_playerPixmap_jump6 : player.djump_frames[player.djump_index];
                int tH = player.djump_anim_complete ? 120 : 150;
                QPixmap frame = flipH(src, player.direction).scaledToHeight(tH, Qt::SmoothTransformation);
                painter.drawPixmap(drawPlayer.center().x() - frame.width() / 2,
                    drawPlayer.bottom() - frame.height() + 25, frame);
            } else {
                painter.drawPixmap(drawPlayer, flipH(m_playerPixmap_jump, player.direction));
            }
        }
        else if (!moving) {
            if (!player.stand_frames.isEmpty()) {
                QPixmap frame = flipH(player.stand_frames[player.stand_index], player.direction)
                    .scaledToHeight(200, Qt::SmoothTransformation);
                painter.drawPixmap(drawPlayer.center().x() - frame.width() / 2,
                    drawPlayer.bottom() - frame.height() + 25, frame);
            }
        }
        else if (!player.run_frames.isEmpty()) {
            QPixmap frame = flipH(player.run_frames[player.run_index], player.direction)
                .scaledToHeight(player.PLAYER_HEIGHT, Qt::SmoothTransformation);
            if (player.direction == 2)
                painter.drawPixmap(drawPlayer.right() - frame.width(), drawPlayer.bottom() - frame.height(), frame);
            else
                painter.drawPixmap(drawPlayer.left(), drawPlayer.bottom() - frame.height(), frame);
        }
    }

    // 玩家攻击框
    if (player.m_isAttacking && m_drawAttackBox)
    {
        QRect drawAttackRect = player.m_attackRect.translated(-m_cameraX, 0);
        QLinearGradient atkGrad(drawAttackRect.topLeft(), drawAttackRect.bottomRight());
        atkGrad.setColorAt(0, QColor(255, 80, 0, 100));
        atkGrad.setColorAt(1, QColor(255, 0, 0, 140));
        painter.setBrush(atkGrad);
        painter.setPen(QPen(QColor(255, 200, 0), 3));
        painter.drawRect(drawAttackRect); // 直角
    }

    // 绘制敌人
    for (enemy* e : m_enemies) {
        if (e->is_dead) continue;

        QRect drawevil = e->m_player.translated(-m_cameraX, 0);
        if (e->isAttackingAnim) {
            QPixmap frame;
            if (e->m_attackType == 2 && !e->attack2Frames.isEmpty()) {
                frame = e->attack2Frames[e->attack2Index];
            } else {
                frame = e->attackFrames[e->attackIndex];
            }

            frame = flipH(frame, e->direction);

            if (e == m_boss) {
                int sx = drawevil.center().x() - frame.width() / 2;
                painter.drawPixmap(sx, drawevil.bottom() - frame.height(), frame);
            } else {
                painter.drawPixmap(
                    e->m_player.x()-90 - m_cameraX,
                    e->m_player.y()-10,
                    frame
                );
            }
        }
        else if (e->m_attacked)
        {
            QPixmap frame;
            if (e == m_boss) {
                frame = flipH(static_cast<Boss*>(e)->bossAttackedPixmap, e->direction);
                int sx = drawevil.center().x() - frame.width() / 2;
                painter.drawPixmap(sx, drawevil.bottom() - frame.height(), frame);
            } else {
                frame = flipH(evil_hurt_img, e->direction);
                painter.drawPixmap(drawevil, frame);
            }
        }
        else{
            if (e == m_boss) {
                Boss* boss = static_cast<Boss*>(e);
                QPixmap frame = flipH(boss->walkFrames[boss->walkIndex], boss->direction);
                int sx = drawevil.center().x() - frame.width() / 2;
                painter.drawPixmap(sx, drawevil.bottom() - frame.height(), frame);
            }
            else {
                bool moving = (e->direction == 1) ? e->m_isRMove : e->m_isLMove;
                if (!moving) {
                    painter.drawPixmap(drawevil, flipH(m_evil_stand, e->direction));
                } else {
                    QPixmap frame = flipH(e->walkFrames[e->walkIndex], e->direction);
                    int sx = drawevil.center().x() - frame.width() / 2;
                    painter.drawPixmap(sx, drawevil.bottom() - frame.height(), frame);
                }
            }
        }

        // 敌人攻击框
        if (e->m_isAttacking && m_drawAttackBox)
        {
            QRect drawAttackRect = e->m_attackRect.translated(-m_cameraX, 0);
            QLinearGradient eAtkGrad(drawAttackRect.topLeft(), drawAttackRect.bottomRight());
            eAtkGrad.setColorAt(0, QColor(0, 100, 255, 100));
            eAtkGrad.setColorAt(1, QColor(0, 0, 255, 140));
            painter.setBrush(eAtkGrad);
            painter.setPen(QPen(QColor(0, 200, 255), 3));
            painter.drawRect(drawAttackRect);
        }

        if (e == m_boss && m_boss->m_isBeamActive)
        {
            QRect beamScreen = m_boss->m_beamRect.translated(-m_cameraX, 0);
            QPixmap beamFrame = m_boss->m_beamSprite;
            if (m_boss->m_beamDirection == -1) {
                QTransform tf;
                tf.scale(-1, 1);
                beamFrame = beamFrame.transformed(tf);
            }
            painter.drawPixmap(beamScreen, beamFrame);
        }
    }

    // 掉落物
    for (const auto& d : m_drops) {
        QPoint screenPos(d.pos.x() - m_cameraX, d.pos.y());

        int dropSize = 18;
        QRect dropRect(screenPos.x() - dropSize/2, screenPos.y() - dropSize/2, dropSize, dropSize);

        QRadialGradient glow(dropRect.center(), dropSize * 1.0);
        if (d.type == 0) {
            glow.setColorAt(0, QColor(60, 255, 60, 80));
            glow.setColorAt(1, QColor(60, 255, 60, 0));
        } else {
            glow.setColorAt(0, QColor(60, 160, 255, 80));
            glow.setColorAt(1, QColor(60, 160, 255, 0));
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(glow);
        painter.drawEllipse(dropRect.adjusted(-dropSize/2, -dropSize/2, dropSize/2, dropSize/2));

        painter.setBrush(d.type == 0 ? QColor(60, 255, 60) : QColor(60, 160, 255));
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(dropRect);

        painter.setPen(Qt::white);
        painter.setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        painter.drawText(dropRect, Qt::AlignCenter, d.type == 0 ? "H" : "M");
    }

    int skillSize = 60;
    int skillX = 20;
    int skillY = GAME_HEIGHT - skillSize - 20;

    QRect skillRect(skillX, skillY, skillSize, skillSize);

    painter.setPen(QPen(QColor(255, 200, 0), 2));
    painter.setBrush(QColor(60, 60, 60));
    painter.drawRect(skillRect);

    // 技能1（U键）
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 24, QFont::Bold));
    painter.drawText(skillRect, Qt::AlignCenter, "U");
    if (player.skill_cd > 0)
    {
        qreal ratio = player.skill_cd / 60.0;
        int coverHeight = skillSize * ratio;

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 180));
        painter.drawRect(
            skillX,
            skillY + skillSize - coverHeight,
            skillSize,
            coverHeight
            );
    }

    // 技能2（Y键）
    int skill2X = skillX + skillSize + 10;
    QRect skill2Rect(skill2X, skillY, skillSize, skillSize);
    painter.setPen(QPen(QColor(100, 200, 255), 2));
    painter.setBrush(QColor(60, 60, 60));
    painter.drawRect(skill2Rect);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 24, QFont::Bold));
    painter.drawText(skill2Rect, Qt::AlignCenter, "Y");

    if (player.skill_2_cd > 0) {
        qreal ratio = player.skill_2_cd / 90.0;
        int coverHeight = skillSize * ratio;
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 180));
        painter.drawRect(skill2X, skillY + skillSize - coverHeight, skillSize, coverHeight);
    }

    // 半透明遮罩
    if (player.mp < 15) {
        painter.setBrush(QColor(0, 0, 0, 120));
        painter.setPen(Qt::NoPen);
        painter.drawRect(skill2Rect);
    }

    //设置面板
    if (m_showSettings) {
        // 半透明遮罩
        painter.fillRect(rect(), QColor(0, 0, 0, 160));

        // 设置面板背景
        int panelW = 280, panelH = 220;
        int panelX = (GAME_WIDTH - panelW) / 2;
        int panelY = (GAME_HEIGHT - panelH) / 2;

        painter.setPen(QPen(QColor(255, 200, 30), 2));
        painter.setBrush(QColor(40, 40, 50));
        painter.drawRect(panelX, panelY, panelW, panelH);

        // 标题
        painter.setPen(QColor(255, 200, 30));
        painter.setFont(QFont("Microsoft YaHei", 18, QFont::Bold));
        painter.drawText(panelX, panelY + 15, panelW, 40, Qt::AlignCenter, "设  置");

        // 分隔线
        painter.setPen(QPen(QColor(100, 100, 100), 1));
        painter.drawLine(panelX + 20, panelY + 55, panelX + panelW - 20, panelY + 55);
    }

    //游戏结束
    if (m_isGameOver) {
        painter.fillRect(rect(), QColor(0, 0, 0, 180));

        QFont failFont("Microsoft YaHei", 64, QFont::Bold);
        painter.setFont(failFont);
        painter.setPen(QColor(220, 40, 40));
        painter.drawText(rect(), Qt::AlignCenter, "失  败");

        QFont tipFont("Microsoft YaHei", 18);
        painter.setFont(tipFont);
        painter.setPen(QColor(200, 200, 200));
        QRect tipRect(0, GAME_HEIGHT / 2 + 60, GAME_WIDTH, 40);
        painter.drawText(tipRect, Qt::AlignCenter, "按任意键重新开始");
    }
}
