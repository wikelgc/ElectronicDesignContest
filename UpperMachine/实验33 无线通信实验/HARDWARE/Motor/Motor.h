#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"

#define PWM_FREQ 			 24         // 24000000
#define PWM_PSC              0        //时钟预分频数
#define PWM_ARR              1000      //自动重装值
/***
!!!!!移植时注意修改PWM_Update函数
**/


/****
设置pwm为定时器5
***/
#define PWM_TIMER						TIM3
#define PWM_TIMER_CLK					RCC_APB1Periph_TIM3


/****
IN1A PA0
IN2A PA1

IN1B PA2
IN2B PA3
****/
#define	PWM_OUT_PORTA					GPIOA
#define	PWM_OUT_PORTB					GPIOB
#define PWM_OUT_PORT_CLK		    	RCC_APB2Periph_GPIOB
					
#define PWM_OUT_PIN1					GPIO_Pin_6    //PA6
#define PWM_OUT_PIN2					GPIO_Pin_7	  //PA7
#define PWM_OUT_PIN3					GPIO_Pin_0    //pB0
#define PWM_OUT_PIN4                    GPIO_Pin_1    //PB1

/*****
EN1 EN2   --PB5
*****/
#define MOTOR_EN_PORT					GPIOB
#define	MOTOR_EN_PIN					GPIO_Pin_5
#define MOTOR_EN_PORT_CLK				RCC_APB2Periph_GPIOB


void TIM_PWM_Init(void);
void TIM5_PWM_Init(void);

//void MotorA_Forward_PWM_Update(uint16_t CCR3_Value );
//void MotorA_Backward_PWM_Update(uint16_t CCR4_Value);
//void MotorA_PWM_Update(s16 CCR_Value);
//void MotorA_Forward(uint16_t CCR3_Value);
//void MotorA_Backward(uint16_t CCR4_Value);
//void MotorA_Brk(void);

//void MotorB_Forward_PWM_Update(uint16_t CCR3_Value );
//void MotorB_Backward_PWM_Update(uint16_t CCR4_Value);
//void MotorB_PWM_Update(s16 CCR_Value);
//void MotorB_Forward(uint16_t CCR3_Value);
//void MotorB_Backward(uint16_t CCR4_Value);
//void MotorB_Brk(void);

void MotorInit(void);


void MotorDisable(void);
void MotorEnable(void );


//轴流风机
void FanA_Brk(void);
void FanA_Forward(uint16_t CCR1_Value);

void FanAB_Forward(uint16_t CCR1_Value);
void FanAB_Backward(uint16_t CCR2_Value);
void FanAB_Brk(void);
void FanCD_Forward(uint16_t CCR3_Value);
void FanCD_Backward(uint16_t CCR4_Value);
void FanCD_Brk(void);
void FanABCD_Brk(void);

void MotorAB_PWM_Update(s16 CCR_Value);
void MotorCD_PWM_Update(s16 CCR_Value);



#endif
