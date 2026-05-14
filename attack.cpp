#include "attack.h"

void attack::attacking(const QRect& m_player,int& direction, QRect& m_attackRect){
    int playerMiddleY = m_player.top() + (m_player.height() - attackHeight) / 2;

    if (direction == 1) {
        m_attackRect = QRect(m_player.right(), playerMiddleY, attackWidth, attackHeight);
    } else {
        m_attackRect = QRect(m_player.left()-attackWidth, playerMiddleY, attackWidth, attackHeight);
    }
}

