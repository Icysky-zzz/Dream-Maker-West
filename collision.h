#ifndef COLLISION_H
#define COLLISION_H

#include <QRect>

// 碰撞基类
class Collision
{
public:
    virtual ~Collision() = default;

    // 核心碰撞处理（模板方法，固定流程）
    void m_Collision(const QRect& player, QRect& newPlayer, const QRect& obstacle,
                     double& velocity, int& jumpTime, bool& isJumping);

protected:
    // 子类只需要重写这个：顶部碰撞行为
    virtual void onTopCollision(QRect& newPlayer, const QRect& obstacle,
                                double& velocity, int& jumpTime, bool& isJumping) = 0;

private:
    // 其他方向碰撞（完全公共，不改动）
    void BottomCollision(QRect& newPlayer, const QRect& obstacle, double& velocity);
    void LeftCollision(QRect& newPlayer, const QRect& obstacle);
    void RightCollision(QRect& newPlayer, const QRect& obstacle);
};

class NormalCollision : public Collision
{
protected:
    void onTopCollision(QRect& newPlayer, const QRect& obstacle,
                        double& velocity, int& jumpTime, bool& isJumping) override;
};


class BounceCollision : public Collision
{
protected:
    void onTopCollision(QRect& newPlayer, const QRect& obstacle,
                        double& velocity, int& jumpTime, bool& isJumping) override;
};

#endif


