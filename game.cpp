#include "game.h"
#include "collision.h"
#include "attack.h"
#include <QRect>
#include <QPainter>
#include <QKeyEvent>

Game::Game(QWidget *parent):
    QWidget(parent)
    , m_playerVelocity(0)
    , m_playerHorizon(8)
    , m_isJumping(false)
    , m_isRMove(false)
    , m_isLMove(false)
    , m_gameOver(false)
    , m_jumptime(0)
    , m_score(0)
    , direction(1)
    , m_isAttacking(false)
    , m_attackDuration(15)
    , is_skill_1(false)
{
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    setWindowTitle("造梦西游Qt简易版");

    setStyleSheet("background-color: #f5f5f5;");

    m_player = QRect(100, GAME_HEIGHT - GROUND_HEIGHT - PLAYER_HEIGHT,
                     PLAYER_WIDTH, PLAYER_HEIGHT);

    m_attackRect = QRect(0, 0, 0, 0);
    m_gameTimerId = startTimer(15);
    m_playerPixmap_stand = QPixmap("stand.png");
    m_playerPixmap_jump = QPixmap("jump.png");
    m_playerPixmap_run = QPixmap("run.png");
    m_playerPixmap_run = QPixmap("run.png");
    m_obstaclePixmap = QPixmap("obstacle.png");
    m_playerPixmap_skill1 = QPixmap("skill1.png");
    creatobstacle();
}

void Game::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    static QPixmap bg("background.png");
    painter.drawPixmap(0, 0, GAME_WIDTH, GAME_HEIGHT, bg);

    for (const auto& obstacle : m_obstacles) {

        painter.drawPixmap(obstacle.x()- m_cameraX, obstacle.y(),
                           obstacle.width(), obstacle.height(), m_obstaclePixmap);
    }
    for (const auto& obstacle : m_bound) {
        painter.drawPixmap(obstacle.x()- m_cameraX, obstacle.y(),
                           obstacle.width(), obstacle.height(), m_obstaclePixmap);
    }
    bool isOnObstacle = false;
    for(const auto &obstacle : m_obstacles) {
        if (m_player.bottom() >= obstacle.top()
            && m_player.bottom() <= obstacle.top() + 4
            && m_player.right() > obstacle.left()
            && m_player.left() < obstacle.right())
        {
            isOnObstacle = true;
            break;
        }
    }
    QRect drawPlayer = m_player.translated(-m_cameraX, 0);
    if (direction == 1)
    {
        if(is_skill_1){
            painter.drawPixmap(drawPlayer.left()-50,drawPlayer.top()
                               ,200,80 ,m_playerPixmap_skill1);}
        else{
            if(!isOnObstacle){
                painter.drawPixmap(drawPlayer, m_playerPixmap_jump);
            }
            else{
                if(!m_isRMove){
                    painter.drawPixmap(drawPlayer, m_playerPixmap_stand);}
                else{painter.drawPixmap(drawPlayer, m_playerPixmap_run);}
            }
        }
    }
    else {
        QTransform transform;
        transform.scale(-1, 1);
        if(is_skill_1){
            QPixmap flipped = m_playerPixmap_skill1.transformed(transform);
            painter.drawPixmap(drawPlayer.left()-170,drawPlayer.top()
                               ,200,80,flipped);}
        else{
            if(!isOnObstacle){
                QPixmap flipped = m_playerPixmap_jump.transformed(transform);
                painter.drawPixmap(drawPlayer, flipped);
            }
            else{
                if(!m_isLMove){
                    QPixmap flipped = m_playerPixmap_stand.transformed(transform);
                    painter.drawPixmap(drawPlayer, flipped);
                }
                else{
                    QPixmap flipped = m_playerPixmap_run.transformed(transform);
                    painter.drawPixmap(drawPlayer, flipped);
                }
            }
        }
    }


    if (m_isAttacking)
    {
        painter.setBrush(QColor(255, 0, 0));
        painter.setPen(QPen(QColor(200, 0, 0), 2));
        QRect drawAttackRect = m_attackRect.translated(-m_cameraX, 0);
        painter.drawRect(drawAttackRect);
    }
}

void Game::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_D ) {
        m_isRMove = false;
    }
    if(event->key() == Qt::Key_A ) {
        m_isLMove = false;
    }
}

void Game::keyPressEvent(QKeyEvent *event)
{
    if (m_gameOver) {
        if (event->key() == Qt::Key_R) {
            resetGame();
        }
        return;
    }

    bool isOnObstacle = false;
    for(const auto &obstacle : m_obstacles) {
        if (m_player.bottom() >= obstacle.top()
            && m_player.bottom() <= obstacle.top() + 4
            && m_player.right() > obstacle.left()
            && m_player.left() < obstacle.right())
        {
            isOnObstacle = true;
            break;
        }
    }

    switch (event->key()) {
    case Qt::Key_D:
        m_isRMove = true;
        direction = 1;
        m_playerHorizon = 3;
        break;
    case Qt::Key_A:
        m_isLMove = true;
        direction = 2;
        m_playerHorizon = 3;
        break;
    case Qt::Key_J:
    {   m_isAttacking=true;
        variabletime=m_attackDuration;
        attack att;
        att.attacking(m_player,direction, m_attackRect);
        break;
    }
    case Qt::Key_K:
    {
        if (isOnObstacle && m_jumptime == 0) {
            m_isJumping = true;
            m_jumptime = 1;
        }
        else if (!isOnObstacle && m_jumptime == 1) {
            m_isJumping = true;
            m_jumptime = 2;
        }
        break;
    }
    case Qt::Key_U:
    {   is_skill_1=true;
        time1=m_attackDuration;
        break;
    }

    default:
        break;
    }
}

void Game::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != m_gameTimerId || m_gameOver) {
        return;
    }

    QRect newPlayerRect = m_player;

    newPlayerRect = m_player;
    m_playerVelocity += GRAVITY;

    if (m_isJumping) {
        if (m_jumptime == 1) {
            m_playerVelocity = -JUMP_FORCE1;
        }
        if (m_jumptime == 2) {
            m_playerVelocity = -JUMP_FORCE2;
        }
        m_isJumping = false;
    }

    newPlayerRect.moveTop(newPlayerRect.top() + m_playerVelocity);

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

    if (m_isAttacking) {
        variabletime--;
        if (variabletime <= 0) {
            m_isAttacking = false;
            m_attackRect = QRect(0, 0, 0, 0);
        }
    }
    if(is_skill_1)
    {
        m_playerVelocity=0;
        time1--;
        if(time1<0)
        {
            is_skill_1=false;
        }
        else{
            if(direction==1)
            {
                newPlayerRect.moveRight(newPlayerRect.right() + 20);
            }
            if(direction==2)
            {
                newPlayerRect.moveLeft(newPlayerRect.left() - 20);
            }
        }
    }
    for(const auto &obstacle : m_obstacles)
    {
        NormalCollision normal;
        normal.m_Collision(m_player,newPlayerRect, obstacle, m_playerVelocity,
                           m_jumptime,  m_isJumping);
    }

    for(const auto &obstacle : m_bound)
    {
        BounceCollision bounce;
        bounce.m_Collision(m_player,newPlayerRect, obstacle, m_playerVelocity,
                           m_jumptime,  m_isJumping);
    }

    if (newPlayerRect.left() < 0) {
        newPlayerRect.moveLeft(0);
    }

    m_player = newPlayerRect;

    int screenCenter = GAME_WIDTH / 2;
    int playerCenterX = m_player.center().x();
    int dif = playerCenterX - (m_cameraX + screenCenter);

    if (dif > GAME_WIDTH / 6) {

        m_cameraX = playerCenterX - (screenCenter + GAME_WIDTH / 6);
    }
    else if (dif < -GAME_WIDTH / 6) {
        m_cameraX = playerCenterX - (screenCenter - GAME_WIDTH / 6);
    }

    if (m_cameraX < 0) m_cameraX = 0;
    update();
}

void Game::resetGame()
{
    m_player = QRect(100, GAME_HEIGHT - GROUND_HEIGHT - PLAYER_HEIGHT,
                     PLAYER_WIDTH, PLAYER_HEIGHT);
    m_playerVelocity = 0;
    m_jumptime = 0;
    m_isJumping = false;
    m_isRMove = false;
    m_isLMove = false;

    m_isAttacking = false;
    m_attackRect = QRect(0, 0, 0, 0);

    m_gameOver = false;
    direction = 1;

    m_obstacles.clear();
    m_bound.clear();
    creatobstacle();
    m_cameraX = 0;
    update();
}

void Game::creatobstacle()
{

    QRect obstacle1(600, 400, 150, 25);
    QRect ground1(0, 550, 600, 50);
    QRect ground2(600, 550, 600, 50);
    QRect ground3(1200, 550, 600, 50);

    m_obstacles.push_back(ground1);
    m_obstacles.push_back(ground2);
    m_obstacles.push_back(ground3);
    m_obstacles.push_back(obstacle1);


}
