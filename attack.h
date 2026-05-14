#ifndef ATTACK_H
#define ATTACK_H

#include<QRect>

class attack
{
public:
    int attackWidth = 40;
    int attackHeight = 60;
    void attacking(const QRect& m_player,int& direction, QRect& m_attackRect);

};
#endif
