#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H
#include<QRect>
#include <QPixmap>
#include <QVector>

void playPlayerAttackedSound();

inline void advanceFrame(int& timer, int speed, int& index, int frameCount)
{
    timer++;
    if (timer >= speed) {
        timer = 0;
        index++;
        if (index >= frameCount)
            index = 0;
    }
}

class individual{
public:
    static constexpr float GRAVITY=0.6f;        //重力大小
    static constexpr int GROUND_HEIGHT=50;       //地面高度
    static constexpr int GAME_HEIGHT=600;        //游戏画面高度

    QRect m_player;
    QRect m_drawRect;
    QRect m_attackRect;

    // 攻击动画帧（一段攻击）
    QVector<QPixmap> attack_frames;
    int attack_index;         // 当前播放到第几帧
    int frame_timer1;         // 帧间隔计时器
    int frame_speed1;         // 动画速度

    // 二段攻击动画帧
    QVector<QPixmap> attack2_frames;
    int attack2_index;
    int attack2_frame_timer;
    int attack2_frame_speed;

    // 三段攻击动画帧
    QVector<QPixmap> attack3_frames;
    int attack3_index;
    int attack3_frame_timer;
    int attack3_frame_speed;

    // 四段攻击动画帧
    QVector<QPixmap> attack4_frames;
    int attack4_index;
    int attack4_frame_timer;
    int attack4_frame_speed;


    int m_attackStage;        // 1=一段, 2=二段, 3=三段, 4=四段
    int last_j_tick;          // 上次按J时的tick数
    // 待机动画帧
    QVector<QPixmap> stand_frames;
    int stand_index;
    int stand_frame_timer;
    int stand_frame_speed;



    // 跑步动画帧
    QVector<QPixmap> run_frames;
    int run_index;
    int frame_timer2;
    int frame_speed2;




    // 二段跳动画帧
    QVector<QPixmap> djump_frames;
    int djump_frame_count;
    int djump_index;
    int djump_frame_timer;
    int djump_frame_speed;
    bool djump_anim_complete;   // 二段跳动画是否播完



    //玩家设置
    int hp;     //血量
    int max_hp; //最大血量
    float mp;     //蓝量
    int direction;  //方向
    bool is_dead;   //是否死亡

    const int PLAYER_WIDTH = 90;      // 玩家宽度
    const int PLAYER_HEIGHT = 120;     // 玩家高度
    bool m_isRMove;                 // 右移标记
    bool m_isLMove;                 // 左移标记
    double m_playerVelocity;        // 垂直速度
    int m_playerHorizon;            // 水平速度
    const int JUMP_FORCE1 = 15;     //一段跳力度
    const int JUMP_FORCE2 = 12;     //二段跳力度

    bool m_isJumping;               //是否在跳
    int m_jumptime;                 //跳跃次数

    int attackWidth = 60;           //攻击框宽度
    int attackHeight = 80;          //攻击框高度
    bool m_isAttacking;         // 是否正在攻击
    int m_attackDuration = 40;         // 攻击持续时间
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

    bool is_skill_2 = false;        //是否使用技能2
    int skill_2_time = 0;           //技能2持续时间
    int skill_2_cd = 0;             //技能2冷却

    individual();
    virtual ~individual() = default;
    virtual void move(const std::vector<QRect>& m_obstacles);
    bool check_onground(const std::vector<QRect>& m_obstacles);
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

    QVector<QPixmap> walkFrames;
    int walkIndex = 0;
    int walkTimer = 0;
    int walkSpeed = 10;
    int walkFrameCount = 4;
    int lastHitStage = 0;        // 记录被哪段攻击打中

    int m_attackType;         // 1=普通攻击, 2=攻击2
    QVector<QPixmap> attack2Frames;
    int attack2FrameCount;
    int attack2Index;
    int attack2Timer;
    int attack2Speed;

    enemy();
    void move(const std::vector<QRect>& m_obstacles);
    void e_attack(individual &Me);
    void is_attacked(individual &Me);
    void trace(individual &myplayer);

};
class Boss : public enemy
{
public:
    // === 光束技能 ===
    QRect m_beamRect;           // 光束的矩形区域
    bool m_isBeamActive;        // 光束是否激活
    int m_beamDirection;        // 光束方向：1=右，-1=左
    int m_beamSpeed;            // 光束移动速度
    int m_beamCooldown;         // 光束冷却时间（帧数）
    int m_beamTimer;            // 冷却计时器
    int m_beamDamage;           // 光束伤害
    int m_beamMaxLife;          // 光束最大存活帧数
    int m_beamLife;             // 光束当前存活帧数
    int m_beamWidth;            // 光束宽度
    int m_beamHeight;           // 光束高度

    QPixmap bossStandPixmap;       
    QPixmap bossAttackedPixmap;       

    bool m_beamFired;                   // 当前攻击是否已发射
    QPixmap m_beamSprite;               // 当前使用的精灵贴图（光束或弓箭）
    QPixmap m_beamBeamSprite;           // 光束精灵贴图 (attack_rect.png)
    int m_bossAttackType;               // 1=光束(33帧), 2=弓箭(前4帧)

    Boss();
    void move(const std::vector<QRect>& m_obstacles) override;
    void fire_beam();                     // 发射光束
    void update_beam();                   // 每帧更新光束位置
    bool check_beam_hit(individual &target);  // 检测光束命中
    void boss_trace(individual &player);   
};
class Player : public individual
{
public:
    int skill_direction = 1;     // 技能冲刺锁定方向

    // 技能2精灵帧
    QVector<QPixmap> skill2_frames;
    int skill2_index = 0;
    int skill2_timer = 0;
    int skill2_speed = 4;

    Player();
    void attack();
    void move(const std::vector<QRect>& m_obstacles) override;

};

#endif // INDIVIDUAL_H
