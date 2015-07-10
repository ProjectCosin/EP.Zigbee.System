#include <pwm.h>



extern afAddrType_t Point_To_Point_DstAddr;//点对点通信定义
extern endPointDesc_t SampleApp_epDesc;
extern uint8 SampleApp_TransID;  // This is the unique message ID (counter)


/**
* @brief 初始化定时器3
*/
void Init_Timer3(void)
{
    P1DIR |=  (1<<4);//P1_4定义为输出
    P1SEL |=  (1<<4);
    P2SEL |=  (1<<5);//timer3优先占用P1_4
    P1_4 = 0;

    SetSystemClock32MHz(); //设置系统工作频率32MHz
    
    T3IF = 0;         //IRCON bit3   定时器3中断标志
    T3IE = 0;         //定时器3中断使能
    TIMIF &= ~0x00;   //清定时器3溢出中断标志（0：无中断未决   1：中断未决）
    TIMIF &= ~0x02;   //清定时器通道0溢出中断标志（0：无中断未决   1：中断未决）
    
    T3CTL |= (4<<5);  //128分频
    
    T3CC0 = 0xff;     //装计数值，这个计数值为PWM一个周期的长度
    T3CC1 = 0x1f;     //装计数值，这个计数值为PWM为高电平的长度

    T3CCTL1 |= (5<<3); //比较模式5 计数器值为0时切换IO
    T3CCTL1 |= 0x04;   //通道1输出比较模式
    
    T3CTL = 0x00;
    T3CTL |= 0x08;    //溢出中断屏蔽位（0：中断禁止   1：中断使能）   
    
    T3CTL  |=  0x03;  //正倒计数
    
    T3CTL |= 0x10;    // 启动定时器
}
