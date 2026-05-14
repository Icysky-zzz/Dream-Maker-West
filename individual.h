#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H
#include<QRect>
#include <QPixmap>
#include <QVector>

class individual{
public:
    const float GRAVITY=0.6;        //重力大小
    int GROUND_HEIGHT=50;           //地面高度
    int GAME_HEIGHT=600;            //游戏画面高度

    QRect m_player;
    QRect m_drawRect;
    QRect m_attackRect;

    // 攻击动画帧（一段攻击）
    QVector<QPixmap> attack_frames;
    int attack_index;         // 当前播放到第几帧
    int frame_timer1;         // 帧间隔计时器
    int frame_speed1;         // 动画速度
    bool is_finished1;        // 动画是否播放完毕
    // 二段攻击动画帧
    QVector<QPixmap> attack2_frames;
    int attack2_index;
    int attack2_frame_timer;
    int attack2_frame_speed;
    bool attack2_finished;
    // 三段攻击动画帧
    QVector<QPixmap> attack3_frames;
    int attack3_index;
    int attack3_frame_timer;
    int attack3_frame_speed;
    bool attack3_finished;
    // 四段攻击动画帧
    QVector<QPixmap> attack4_frames;
    int attack4_index;
    int attack4_frame_timer;
    int attack4_frame_speed;
    bool attack4_finished;

    int m_attackStage;        // 1=一段, 2=二段, 3=三段, 4=四段
    int last_j_tick;          // 上次按J时的tick数（连击检测）
    // 待机动画帧
    QVector<QPixmap> stand_frames;
    int stand_index;
    int stand_frame_timer;
    int stand_frame_speed;

    // 走路动画帧
    QVector<QPixmap> walk_frames;
    int walk_index;
    int walk_frame_timer;
    int walk_frame_speed;

    // 跑步动画帧
    QVector<QPixmap> run_frames;
    int run_index;
    int frame_timer2;
    int frame_speed2;
    bool is_finished2;

    // 跳跃动画帧
    QVector<QPixmap> jump_frames;
    int jump_frame_count;
    int jump_index;
    int jump_frame_timer;
    int jump_frame_speed;

    // 二段跳动画帧
    QVector<QPixmap> djump_frames;
    int djump_frame_count;
    int djump_index;
    int djump_frame_timer;
    int djump_frame_speed;
    bool djump_anim_complete;   // 二段跳动画是否播完（之后画 jump6）



    //玩家设置
    int hp;     //血量
    float mp;     //蓝量
    int direction;  //方向
    bool is_dead;   //是否死亡
    bool visible;
    const int PLAYER_WIDTH = 90;      // 玩家宽度
    const int PLAYER_HEIGHT = 120;     // 玩家高度
    bool m_isRMove;                 // 右移标记
    bool m_isLMove;                 // 左移标记
    double m_playerVelocity;        // 垂直速度
    int m_playerHorizon;            // 水平速度
    const int JUMP_FORCE1 = 15;     //一段跳力度
    const int JUMP_FORCE2 = 12;     //二段跳力度
    float jumpRotate = 0;
    bool m_isJumping;               //是否在跳
    int m_jumptime;                 //跳跃次数

    int attackWidth = 80;           //攻击框宽度
    int attackHeight = 100;          //攻击框高度
    bool m_isAttacking;         // 是否正在攻击
    int m_attackDuration = 40;         // 攻击持续时间（动画播完即止，此为备用）
    int knockbackPower = 2;       // 击退力度
    int variabletime;               //攻击计时

    bool m_attacked;              //是否被攻击
    int knockbackDir = 0;         // 击退方向：+1 右，-1 左
    int attacked_time;            //被攻击时间
    bool once_attacked=false;           //是否已经受到攻击

    bool is_skill_1=false;         //是否使用技能1
    int skill_1_time;               //技能1时间
    int time1=0;                    //技能定时器
    int skill_cd = 0;               //技能冷却时间

    individual();
    virtual ~individual() = default;
    virtual void move(const std::vector<QRect> m_obstacles);
    bool check_onground(const std::vector<QRect> m_obstacles);
    void attack();
    void attacked();

    // 从水平精灵表加载帧
    static void loadSheet(QVector<QPixmap>& frames, const QString& path,
                          int frameW, int frameH, int count, float scale = 1.0f);

};

class enemy : public individual
{
public:
    int rest_time;
    QVector<QPixmap> attackFrames;
    QPixmap attackSheet;
    int frameWidth;
    int frameHeight;
    int attackFrameCount;
    int attackIndex;
    int attackTimer;
    int attackSpeed;
    bool isAttackingAnim;
    enemy();
    void move(const std::vector<QRect> m_obstacles);
    void e_attack(individual &Me);
    void is_attacked(individual &Me);
    void trace(individual &myplayer);

};
class Player : public individual
{
public:
    int skill_direction = 1;     // 技能冲刺锁定方向
    Player();
    void move(const std::vector<QRect> m_obstacles) override;

};

#endif // INDIVIDUAL_H
