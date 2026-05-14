#ifndef COLLISION_H
#define COLLISION_H

#include <QRect>

// 碰撞基类
class Collision
{
public:
    virtual ~Collision() = default;

    void m_Collision(const QRect& player, QRect& newPlayer, const QRect& obstacle,
                     double& velocity, int& jumpTime, bool& isJumping);

protected:

    virtual void onTopCollision(QRect& newPlayer, const QRect& obstacle,
                                double& velocity, int& jumpTime, bool& isJumping) = 0;

private:

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


