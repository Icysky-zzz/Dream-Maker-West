#include "game.h"
#include <QRect>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QtMath>

void Game::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);


    if(is_start_interface){
        painter.fillRect(rect(), QColor(20,25,45));
        painter.setPen(QColor(255,210,0));
        painter.setFont(QFont("Microsoft YaHei",60,QFont::Bold));
        painter.drawText(0,150,GAME_WIDTH,100,Qt::AlignCenter,"造梦西游");
        return;
    }

    // 显示游戏内按钮
    btnRestart->show();
    btnShowAtkBox->show();


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
    painter.drawRect(hprect); // 直角

    // 血量数字
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
    painter.drawText(hpbar, Qt::AlignCenter, QString::number(player.hp) + "/100");


    painter.setPen(QPen(QColor(50,50,50), 2));
    painter.setBrush(QColor(30,30,30));
    QRect mpbar = QRect(75, 50, barW, barH);
    painter.drawRect(mpbar); // 直角

    painter.setPen(Qt::NoPen);
    QLinearGradient mpGrad(mpbar.left(), 0, mpbar.right(), 0);
    mpGrad.setColorAt(0, QColor(80, 180, 255));
    mpGrad.setColorAt(1, QColor(30, 120, 220));
    painter.setBrush(mpGrad);
    QRect mprect = QRect(77, 52, player.mp * 1.2, barH-4);
    painter.drawRect(mprect); // 直角

    // 蓝量数字
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 9, QFont::Bold));
    painter.drawText(mpbar, Qt::AlignCenter, QString::number((int)player.mp) + "/100");


    for (enemy* e : m_enemies) {
        if (e->is_dead) continue;

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
        hpRect.setWidth( (hpBarW-2) * e->hp / 100 );
        painter.drawRect(hpRect);

        // 敌人血量数字
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
    painter.drawRect(role.adjusted(-2,-2,2,2)); // 直角
    painter.drawPixmap(role, role_img);

    QRect drawPlayer = player.m_drawRect.translated(-m_cameraX, 0);

    //攻击动画
    // 1. 被攻击
    if (player.m_attacked)
    {
        QPixmap frame=hurt_img;
        if (player.direction == 2) {
            QTransform transform;
            transform.scale(-1, 1);
            frame= frame.transformed(transform);
        }
        painter.drawPixmap(drawPlayer, frame);
    }
    // 2. 攻击动画（一段/二段/三段）
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
            QPixmap frame = (*frames)[idx].scaledToHeight(h, Qt::SmoothTransformation);
            if(player.direction == 2)
            {
                QTransform transform;
                transform.scale(-1, 1);
                frame = frame.transformed(transform);
            }
            int sx = drawPlayer.center().x() - frame.width() / 2;
            painter.drawPixmap(sx, drawPlayer.bottom() - frame.height() + yOff, frame);
        }
    }

    //其他动作（技能，移动，跳跃）
    else
    {
        //方向向右
        if (player.direction == 1)
        {
            if(player.is_skill_1){
                painter.drawPixmap(drawPlayer.left()-50,drawPlayer.top(),200,80 ,m_playerPixmap_skill1);
            }
            else{
                if(!player.check_onground(m_obstacles)){
                    if (player.m_jumptime == 2 && !player.djump_frames.isEmpty()) {
                        if (player.djump_anim_complete) {
                            QPixmap frame = m_playerPixmap_jump6.scaledToHeight(120, Qt::SmoothTransformation);
                            int sx = drawPlayer.center().x() - frame.width() / 2;
                            painter.drawPixmap(sx, drawPlayer.bottom() - frame.height() + 25, frame);
                        } else {
                            QPixmap frame = player.djump_frames[player.djump_index].scaledToHeight(150, Qt::SmoothTransformation);
                            int sx = drawPlayer.center().x() - frame.width() / 2;
                            painter.drawPixmap(sx, drawPlayer.bottom() - frame.height() + 25, frame);
                        }
                    } else {
                        painter.drawPixmap(drawPlayer, m_playerPixmap_jump);
                    }
                }
                else{
                    if(!player.m_isRMove){
                        if (!player.stand_frames.isEmpty()) {
                            QPixmap standFrame = player.stand_frames[player.stand_index].scaledToHeight(200, Qt::SmoothTransformation);
                            int sx = drawPlayer.center().x() - standFrame.width() / 2;
                            painter.drawPixmap(sx, drawPlayer.bottom() - standFrame.height() + 25, standFrame);
                        }
                    }
                    else{
                        if (!player.run_frames.isEmpty()) {
                            QPixmap runFrame = player.run_frames[player.run_index].scaledToHeight(player.PLAYER_HEIGHT, Qt::SmoothTransformation);
                            painter.drawPixmap(drawPlayer.left(), drawPlayer.bottom() - runFrame.height(), runFrame);
                        }
                    }
                }
            }
        }
        //方向向左
        else {
            QTransform transform;
            transform.scale(-1, 1);
            if(player.is_skill_1){
                QPixmap flipped = m_playerPixmap_skill1.transformed(transform);
                painter.drawPixmap(drawPlayer.left()-170,drawPlayer.top(),200,80,flipped);
            }
            else{
                if(!player.check_onground(m_obstacles)){
                    if (player.m_jumptime == 2 && !player.djump_frames.isEmpty()) {
                        QPixmap frame;
                        if (player.djump_anim_complete) {
                            frame = m_playerPixmap_jump6.scaledToHeight(120, Qt::SmoothTransformation);
                        } else {
                            frame = player.djump_frames[player.djump_index].scaledToHeight(150, Qt::SmoothTransformation);
                        }
                        frame = frame.transformed(transform);
                        int sx = drawPlayer.center().x() - frame.width() / 2;
                        painter.drawPixmap(sx, drawPlayer.bottom() - frame.height() + 25, frame);
                    } else {
                        painter.drawPixmap(drawPlayer, m_playerPixmap_jump.transformed(transform));
                    }
                }
                else{
                    if(!player.m_isLMove){
                        if (!player.stand_frames.isEmpty()) {
                            QPixmap standFrame = player.stand_frames[player.stand_index].scaledToHeight(200, Qt::SmoothTransformation);
                            standFrame = standFrame.transformed(transform);
                            int sx = drawPlayer.center().x() - standFrame.width() / 2;
                            painter.drawPixmap(sx, drawPlayer.bottom() - standFrame.height() + 25, standFrame);
                        }
                    }
                    else{
                        if (!player.run_frames.isEmpty()) {
                            QPixmap frame = player.run_frames[player.run_index].scaledToHeight(player.PLAYER_HEIGHT, Qt::SmoothTransformation);
                            frame = frame.transformed(transform);
                            painter.drawPixmap(drawPlayer.right() - frame.width(), drawPlayer.bottom() - frame.height(), frame);
                        }
                    }
                }
            }
        }
    }

    // 玩家攻击框（直角渐变）
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
            QPixmap frame = e->attackFrames[e->attackIndex];

            if (e->direction == 2) {
                QTransform transform;
                transform.scale(-1, 1);
                frame = frame.transformed(transform);
            }

            painter.drawPixmap(
                e->m_player.x()-120 - m_cameraX,
                e->m_player.y()+10,
                frame
                );
        }
        else if (e->m_attacked)
        {
            QPixmap frame=evil_hurt_img;
            if (e->direction == 2) {
                QTransform transform;
                transform.scale(-1, 1);
                frame= frame.transformed(transform);
            }
            painter.drawPixmap(drawevil, frame);
        }
        else{
            if (e->direction == 1) {
                if (!e->m_isRMove)
                    painter.drawPixmap(drawevil, m_evil_stand);
                else
                    painter.drawPixmap(drawevil, m_evil_run);

            }
            else {
                QTransform transform;
                transform.scale(-1, 1);
                QPixmap flipped;

                if (!e->m_isLMove)
                    flipped = m_evil_stand.transformed(transform);
                else
                    flipped = m_evil_run.transformed(transform);

                painter.drawPixmap(drawevil, flipped);
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
    }

    // ===================== 掉落物 =====================
    for (const auto& d : m_drops) {
        float bob = qSin(d.frame * 0.1) * 5;
        QPoint screenPos(d.pos.x() - m_cameraX, d.pos.y() + bob);

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
    // 直角 + 实心背景 + 金色边框
    painter.setPen(QPen(QColor(255, 200, 0), 2));
    painter.setBrush(QColor(60, 60, 60));
    painter.drawRect(skillRect);

    // 显示 U
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 24, QFont::Bold));
    painter.drawText(skillRect, Qt::AlignCenter, "U");

    // 冷却阴影（从下往上减少，纯显示）
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
}
