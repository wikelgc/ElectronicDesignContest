#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "voice"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PF1~8 设置成输入，默认上拉 
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF 1~8
	
	//PE
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PE4 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PEA设置成输入，默认上拉
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE 4
	
	//PA0 WE_UP
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PA0 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入,默认下拉
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化pA0
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 

//注意此函数有响应优先级,KEY1>KEY2>KEY3>  ... >KEY8   > KEY0  >WK_UP
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)
		key_up=1;  //支持连按		  
	if(key_up && (KEY0==0||KEY1==0||KEY2==0||KEY3==0||KEY4==0||KEY5==0||KEY6==0||KEY7==0||WK_UP==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		
		if(KEY1==0)
			return KEY1_PRES;
		else if(KEY2==0)
			return KEY2_PRES;
		else if(KEY3==0)
			return KEY3_PRES;
		else if(KEY4==0)
			return KEY4_PRES;
		else if(KEY5==0)
			return KEY5_PRES;
		else if(KEY6==0)
			return KEY6_PRES;
		else if(KEY7==0)
			return KEY7_PRES;
//		else if(KEY8==0) 
//			return KEY8_PRES;
		else if(WK_UP==1)
			return WKUP_PRES;
		else if(KEY0==0)
			return KEY0_PRES;
	}
	else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1&& KEY4==1&&KEY5==1&&KEY6==1&&KEY7==1&&WK_UP==0)
	{
		key_up=1;
	} 	    
 	return 0;// 无按键按下
}
