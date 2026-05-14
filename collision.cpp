#include "collision.h"


void Collision::m_Collision(const QRect& player, QRect& newPlayer, const QRect& obstacle,
                            double& velocity, int& jumpTime, bool& isJumping)
{
    if (!newPlayer.intersects(obstacle)) return;

    // 顶部碰撞（子类实现）
    if (player.bottom() <= obstacle.top() && newPlayer.bottom() > obstacle.top()) {
        onTopCollision(newPlayer, obstacle, velocity, jumpTime, isJumping);
    }
    // 底部碰撞
    else if (player.top() >= obstacle.bottom() && newPlayer.top() < obstacle.bottom()) {
        BottomCollision(newPlayer, obstacle, velocity);
    }
    // 左侧碰撞
    else if (player.right() <= obstacle.left() && newPlayer.right() > obstacle.left()) {
        LeftCollision(newPlayer, obstacle);
    }
    // 右侧碰撞
    else if (player.left() >= obstacle.right() && newPlayer.left() < obstacle.right()) {
        RightCollision(newPlayer, obstacle);
    }
}

void Collision::BottomCollision(QRect& newPlayer, const QRect& obstacle, double& velocity) {
    newPlayer.moveTop(obstacle.bottom());
    velocity = 0;
}

void Collision::LeftCollision(QRect& newPlayer, const QRect& obstacle) {
    newPlayer.moveRight(obstacle.left());
}

void Collision::RightCollision(QRect& newPlayer, const QRect& obstacle) {
    newPlayer.moveLeft(obstacle.right());
}

void NormalCollision::onTopCollision(QRect& newPlayer, const QRect& obstacle,
                                     double& velocity, int& jumpTime, bool& isJumping)
{
    newPlayer.moveBottom(obstacle.top());
    velocity = 0;
    jumpTime = 0;
    isJumping = false;
}

void BounceCollision::onTopCollision(QRect& newPlayer, const QRect& obstacle,
                                     double & velocity, int& jumpTime, bool& isJumping)
{

    velocity = -20;
    jumpTime = 1;
    isJumping = true;
}
