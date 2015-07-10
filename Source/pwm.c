#include <pwm.h>



extern afAddrType_t Point_To_Point_DstAddr;//��Ե�ͨ�Ŷ���
extern endPointDesc_t SampleApp_epDesc;
extern uint8 SampleApp_TransID;  // This is the unique message ID (counter)


/**
* @brief ��ʼ����ʱ��3
*/
void Init_Timer3(void)
{
    P1DIR |=  (1<<4);//P1_4����Ϊ���
    P1SEL |=  (1<<4);
    P2SEL |=  (1<<5);//timer3����ռ��P1_4
    P1_4 = 0;

    SetSystemClock32MHz(); //����ϵͳ����Ƶ��32MHz
    
    T3IF = 0;         //IRCON bit3   ��ʱ��3�жϱ�־
    T3IE = 0;         //��ʱ��3�ж�ʹ��
    TIMIF &= ~0x00;   //�嶨ʱ��3����жϱ�־��0�����ж�δ��   1���ж�δ����
    TIMIF &= ~0x02;   //�嶨ʱ��ͨ��0����жϱ�־��0�����ж�δ��   1���ж�δ����
    
    T3CTL |= (4<<5);  //128��Ƶ
    
    T3CC0 = 0xff;     //װ����ֵ���������ֵΪPWMһ�����ڵĳ���
    T3CC1 = 0x1f;     //װ����ֵ���������ֵΪPWMΪ�ߵ�ƽ�ĳ���

    T3CCTL1 |= (5<<3); //�Ƚ�ģʽ5 ������ֵΪ0ʱ�л�IO
    T3CCTL1 |= 0x04;   //ͨ��1����Ƚ�ģʽ
    
    T3CTL = 0x00;
    T3CTL |= 0x08;    //����ж�����λ��0���жϽ�ֹ   1���ж�ʹ�ܣ�   
    
    T3CTL  |=  0x03;  //��������
    
    T3CTL |= 0x10;    // ������ʱ��
}
