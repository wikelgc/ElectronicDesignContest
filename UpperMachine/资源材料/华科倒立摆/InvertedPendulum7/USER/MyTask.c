#include "mytask.h"
// #include "math.h"


void INIT_Task()
{
	short temp;
	if(TB_DisplayDelay_IsElapsed()==TRUE)
	{
		TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
		clear_screen();
		display_GB2312_string(1,1,"旋转倒立摆系统");  
		display_GB2312_string(3,1,"请回到原点..."); 
		display_GB2312_string(5,1,"");    
		display_GB2312_string(7,1,"Play键进入菜单"); 
	}
	if(Remote_Rdy)
	{
		Remote_Rdy=0;
		ir_key=Remote_Process();
		switch(ir_key)
		{
			case IR_PLAY://初始化完成
			Controller_Init(); //PID变量初始化
			ENC_Init();					//编码器初始化
			ENC1_Init();        //
			State=MENU;//进入菜单状态
			break;
		}
	}
}
void MENU_Task()
{
	if(TB_DisplayDelay_IsElapsed()==TRUE)
		{
			TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
			clear_screen();
            display_GB2312_string(1,1,"1,往复运动"); 
            display_GB2312_string(3,1,"2固定位置启动");    
            display_GB2312_string(5,1,"3,下垂启动"); 
			display_GB2312_string(7,1,"4倒立运动5其他");  
		}
		if(Remote_Rdy)
		{
			Remote_Rdy=0;
			ir_key=Remote_Process();
			switch(ir_key)
			{
				case IR_KEY1://1往复运动输入
					State = RECIPROCATE_INPUT;
					break;
				case IR_KEY2://松手起振
					clear_screen();
					display_GB2312_string(1,1,"2固定位置启动");    
					display_GB2312_string(1,1,"请转动到165度"); 
					display_GB2312_string(5,1,"3,然后松手"); 
					display_GB2312_string(7,1,"请注意安全");  
					State = FREE_LQR;
					break;
				case IR_KEY3:
					clear_screen();	
					display_GB2312_string(1,1,"3,下垂启动");  
					display_GB2312_string(3,1,""); 
					display_GB2312_string(5,1,"");    
					display_GB2312_string(7,1,""); 
// // 				MotorA_Backward(1000);
// // 				delay_ms(10);
					MotorA_Forward(200);//促使扰动，触发起振
					delay_ms(4);
					MotorA_Backward(1000);
					delay_ms(4);
// 					MotorA_Brk();
					State = LQR_CONTROL;//
					break;
				case IR_KEY4:
					clear_screen();	
					display_GB2312_string(1,1,"4,倒立运动");  
					display_GB2312_string(3,1,""); 
					display_GB2312_string(5,1,"");    
					display_GB2312_string(7,1,""); 
// 					MotorA_Backward(500);
// 					delay_ms(20);
// 					MotorA_Forward(500);//促使扰动，触发起振
// 					delay_ms(5);
// 					MotorA_Brk();
					State = LQR_CIRCULAR_INPUT;//
					break;
				case IR_KEY5:
					State = OTHER;
					break;
				case IR_KEY6:
					State =LQR_TEST;
				break;
				case IR_PREV://回到初始化状态
					State=INIT;
				break;
				case IR_KEY7:
					State =TEST_MOTOR;
				break;
				default:
					break;
						}
				}
}

/***
往复运动输入摆动幅度状态，PLAY键完成
****/
void RECIPROCATE_INPUT_Task(void)
{
	//判断是否为0
	if(TB_DisplayDelay_IsElapsed()==TRUE)
		{
			//设置LCD的显示延时时间
			TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
			clear_screen();	
            display_GB2312_string(1,1,"1,往复运动"); 
            display_GB2312_string(3,1,"请输入角度");   
						sprintf(str,"目标：%d",AimAngle);
            display_GB2312_string(5,1,str); 
						display_GB2312_string(7,1,"PLAY键确认");  
		}
		if(Remote_Rdy)
		{
			Remote_Rdy=0;
			ir_key=Remote_Process();
			switch(ir_key)
			{
				case IR_VOL_UP:
					AimAngle += 10;
//				if(AimAngle > 100)
//				{
//					AimAngle =10;
//				}
//				else if(AimAngle < 10)
//				{
//					AimAngle = 90;
//				}
					break;
				case IR_VOL_DOWN:
					AimAngle -=10;
//				if(AimAngle > 100)
//				{
//					AimAngle =10;
//				}
//				else if(AimAngle < 10)
//				{
//					AimAngle = 90;
//				}
					break;
				case IR_PREV:
					State = MENU;//prev键返回到菜单
					break;
				case IR_PLAY://PLAY键确认，进入执行状态
					clear_screen();	
					display_GB2312_string(1,1,"1,往复运动"); 
					display_GB2312_string(3,1,"请输入角度");   
					sprintf(str,"目标：%d",AimAngle);
					display_GB2312_string(5,1,str); 
					display_GB2312_string(7,1,"执行中");  
					MotorA_Forward(200);//促使扰动，触发起振
					delay_ms(6);
					MotorA_Backward(200);
					delay_ms(6);
					MotorA_Brk();
					State = RECIPROCATE;
				break;
			}
	}
}
/****
往复运动状态，上一次往复运动的输入角度
****/
void RECIPROCATE_Task(void)
{
	double temp;
		
	if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//调节周期1ms
		{
				TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
					alpha = ENC1_Get_AnglularPosition()+180;
				/****角度限定在-180，180范围之内*/
					while(abs(alpha)>180)
					{
						  if(alpha>180)
							{
								  alpha=alpha-360;
							}
							else
								  alpha=alpha+360;
					}
					v_alpha = 	0.6*ENC1_Calc_Average_Speed();//度每秒
					theta 	= 	ENC_Get_AnglularPosition();
					/****角度限定在-180，180范围之内*/
					while(abs(theta)>180)
					{
						  if(theta>180)
							{
								  theta=theta-360;
							}
							else
								  theta=theta+360;
					}
				v_theta =  0.6*ENC_Calc_Average_Speed();//度每秒
				alpha_pi = alpha *PI /180;
				v_alpha_pi = v_alpha * PI/180;
				E = 3*v_alpha_pi*v_alpha_pi+490*(cos(alpha_pi)-1);//放大了490
				temp = ( 180-AimAngle ) *PI /180;
				E0 = 490*(cos(temp)-1);
				if(v_alpha_pi*cos(alpha_pi)>0)
				{
					MotorA.PWM = -(E-E0)*10;//根据输入角度大小，改变倍数
				}
				else if(v_alpha_pi*cos(alpha_pi)<0)
				{
						MotorA.PWM =(E-E0)*10;
				}
				else if(v_alpha_pi*cos(alpha_pi)==0)
				{
					MotorA.PWM = 0;
				}
				MotorA_PWM_Update(MotorA.PWM);	
			}
			if(Remote_Rdy)
			{
						Remote_Rdy=0;
						ir_key=Remote_Process();
						switch(ir_key)
						{
							case IR_PREV:
								MotorA_Brk();
								State = RECIPROCATE_INPUT;//PREV键返回到原来状态
								break;
							case IR_PLAY://紧急关闭
								MotorA_Brk();
								State = INIT ;		
								break;
							}
			}
}
/*****
检测摆杆速度，当速度大于某个值是，进入LQR控制环节。
上一个状态，菜单选择，下一个闭环控制
*****/
void FREE_LQR_Task(void)
{

		if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//调节周期1ms
		{
					TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
					alpha = ENC1_Get_AnglularPosition()+180;
					if(alpha>180)
							{
								  alpha=alpha-360;
							}
							else if(alpha <180)
							{
								  alpha=alpha+360;
							}
				  v_alpha = 	0.6*ENC1_Calc_Rot_Speed();//度每秒
					if((abs(alpha) > 17)&(abs(v_alpha)>100))//速度界限。产生某个速度，说明手松开了
					{
						State = LQR_CONTROL;//进入倒立
					}
		}
			if(TB_DisplayDelay_IsElapsed()==TRUE)
		{
						TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
						clear_screen();	
						display_GB2312_string(1,1,"2固定位置启动");    
						display_GB2312_string(3,1,"请转动到165度"); 
						sprintf(str,"当前：%f",alpha);
						display_GB2312_string(5,1,str); 
						display_GB2312_string(7,1,"放手注意安全"); 
		}
		if(Remote_Rdy)
			{
						Remote_Rdy=0;
						ir_key=Remote_Process();
						switch(ir_key)
						{
							case IR_PREV:
								MotorA_Brk();
								State = MENU;//PREV键返回到菜单状态
								break;
							case IR_PLAY://紧急关闭
								MotorA_Brk();
								State = INIT ;		
								break;
							}
			}
}
/****
起振并进入LQR闭环控制
****/
void LQR_CONTROL_Task(void)
{
	/***1ms*/
		if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//调节周期1ms
		{
					TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
					alpha = ENC1_Get_AnglularPosition()+180;
				/****角度限定在-180，180范围之内*/
					while(abs(alpha)>180)
					{
						  if(alpha>180)
							{
								  alpha=alpha-360;
							}
							else
								  alpha=alpha+360;
					}
// 					v_alpha = 	0.6*ENC1_Calc_Rot_Speed();//度每秒
					
					//平均速度
					v_alpha=0.6*ENC1_Calc_Average_Speed();
					//角位移
					theta 	= 	ENC_Get_AnglularPosition();
					/****角度限定在-180，180范围之内*/
					while(abs(theta)>180)
					{
						  if(theta>180)
							{
								  theta=theta-360;
							}
							else
								  theta=theta+360;
					}
					
					v_theta =  0.6*ENC_Calc_Average_Speed();//度每秒
// 					v_theta =  0.6*ENC_Calc_Rot_Speed();//度每秒
			if((abs(alpha)<30)&(abs(v_alpha)<2000))//进入维持稳定状态的条件
			{
				//维持稳定
// 	     		MotorA.PWM = -(-3.8730 * theta +72.3887 * alpha    - 3.8824 * v_theta + 6.8216 *v_alpha)/5;//第一好参数
// 				MotorA.PWM = -(-3.8730 * theta +81.2340 * (alpha)  - 4.6512 * v_theta + 7.6743 *v_alpha)/6.5;
// 				MotorA.PWM = -(-3.8730 * theta +79.5930 * (alpha)  - 3.9095 * v_theta + 6.8569 *v_alpha)/6.5;		
				MotorA.PWM = -(K1 * theta +K2 * (alpha)  +K3 * v_theta + K4 *v_alpha)/G;							
				MotorA_PWM_Update(MotorA.PWM);
			}
			else//起振控制，回到原点
			{
				alpha_pi = alpha *PI /180;
				v_alpha_pi = v_alpha * PI/180;
// 				E = 3*v_alpha_pi*v_alpha_pi+490*(cos(alpha_pi)-1);//放大了490
				E = 3*v_alpha_pi*v_alpha_pi+490*(cos(alpha_pi)-1);//放大了490
				if(v_alpha_pi*cos(alpha_pi)>0)
				{
					MotorA.PWM = -E*6;
				}
				else if(v_alpha_pi*cos(alpha_pi)<0)
				{
						MotorA.PWM =E*6;
				}
				else if(v_alpha_pi*cos(alpha_pi)==0)
				{
					MotorA.PWM = 0;
				}
				MotorA_PWM_Update(MotorA.PWM);	
			}
		}
		/***遥控***/
		if(Remote_Rdy)
			{
				Remote_Rdy=0;
				ir_key=Remote_Process();
				switch(ir_key)
				{
					case IR_PLAY:
						MotorA_Brk();
						State = INIT ;		
						break;
					case IR_PREV://返回到菜单状态
						MotorA_Brk();
						State = MENU;
						break;
					case IR_NEXT:
						AimAngle =180;
						State = LQR_CIRCULAR;
						break;
				}
			}
			if(KeyFlag ==1)
			{
				KeyFlag =0;
				IsCycleOK =0;
				ThetaDelta =0;
				DeltaNum =0;
				State = LQR_CIRCULAR;
					clear_screen();	
						display_GB2312_string(1,1,"倒立圆周运动");    
						display_GB2312_string(3,1,""); 
			
				LED0_Toggle();
			}
}
void LQR_CIRCULAT_INPUT_Task(void)
{
		if(TB_DisplayDelay_IsElapsed()==TRUE)
		{
						TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
						clear_screen();	
						display_GB2312_string(1,1,"倒立圆周运动");    
						display_GB2312_string(3,1,"输入转的角度"); 
						sprintf(str,"当前：%f",AimTheta);
						display_GB2312_string(5,1,str); 
						display_GB2312_string(7,1,"PLAY键完成"); 
		}
			if(Remote_Rdy)
			{
						Remote_Rdy=0;
						ir_key=Remote_Process();
						switch(ir_key)
						{
							case IR_VOL_UP://加10度
								AimTheta +=10;	
								break;
							case IR_VOL_DOWN://减10度
								AimTheta -=10;
							break;
							case IR_CH_UP:
								AimTheta += 360;//加1圈
							break;
							case IR_CH_DOWN:
								AimTheta -= 360;
							break;
							case IR_PLAY:
			        	MotorA_Forward(200);//促使扰动，触发起振
								delay_ms(4);
								MotorA_Backward(1000);
								delay_ms(4);
								IsCycleOK = 0;
								State = LQR_CIRCULAR;//输入完成，进入倒立运动状态
								break;
							case IR_PREV://返回到菜单状态
								MotorA_Brk();
								State = MENU;
							break;
							}
			}
}
/***
保持倒立状态并做圆周运动
***/
void LQR_CIRCULAR_Task(void)
{
		if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//调节周期1ms
		{
					TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
					LED0_Toggle();
					alpha = ENC1_Get_AnglularPosition()+180;
					
			
				/****角度限定在-180，180范围之内*/
					while(abs(alpha)>180)
					{
						  if(alpha>180)
							{
								  alpha=alpha-360;
							}
							else
								  alpha=alpha+360;
						}
						v_alpha	= 0.6*ENC1_Calc_Average_Speed();
						
						theta 	= 	ENC_Get_AnglularPosition();	
// 						DeltaNum++;
// 					if(DeltaNum<3600)
// 					{
// 						theta+=10;
// 					}
							/****角度限定在-180，180范围之内*/
					while(abs(theta)>180)
					{
						
						  if(theta>180)
							{
// 								 flag2=1;
								  theta=theta-360;
							}
							else
								  theta=theta+360;
					}
						v_theta =  0.6*ENC_Calc_Average_Speed();//度每秒�
			if((abs(alpha)<30)&(abs(v_alpha)<2000))//进入维持稳定状态的条件
			{
// 				if(DeltaNum<360)
// 				{
				if(abs(theta)>178)
				{
					MotorA.PWM =-(K1 *theta  +K2 * (alpha)  +K3 * v_theta + K4 *v_alpha)/G;		
				}
				else
				{
					DeltaNum+=0.01;
					MotorA.PWM =-DeltaNum*18 -(K1 *theta  +K2 * (alpha)  +K3 * v_theta + K4 *v_alpha)/G;	
				}					
// 				}
// 				else
// 				{
// 						MotorA.PWM = -(K1 *theta  +K2 * (alpha)  +K3 * v_theta + K4 *v_alpha)/G;							
// 				}
// 					MotorA.PWM = -(K1 *theta  +K2 * (alpha)  +K3 * v_theta + K4 *v_alpha)/G;							
// 				 MotorA.PWM = ThetaDelta-(K1 * theta +K2 * (alpha)  +K3 * v_theta + K4 *v_alpha)/G;							
				MotorA_PWM_Update(MotorA.PWM);
			}
			else//起振控制，回到原点
			{
				alpha_pi = alpha *PI /180;
				v_alpha_pi = v_alpha * PI/180;
				E = 3*v_alpha_pi*v_alpha_pi+490*(cos(alpha_pi)-1);//放大了490
				if(v_alpha_pi*cos(alpha_pi)>0)
				{
					MotorA.PWM = -E*6;
				}
				else if(v_alpha_pi*cos(alpha_pi)<0)
				{
						MotorA.PWM =E*6;
				}
				else if(v_alpha_pi*cos(alpha_pi)==0)
				{
					MotorA.PWM = 0;
				}
				MotorA_PWM_Update(MotorA.PWM);	
			}
		}
		if(Remote_Rdy)
			{
						Remote_Rdy=0;
						ir_key=Remote_Process();
						switch(ir_key)
						{
							case IR_PLAY:
								MotorA_Brk();
								State = INIT ;		
								break;
							case IR_PREV://返回到菜单状态
								MotorA_Brk();
								State = MENU;
							break;
							case IR_NEXT:
// 								ThetaDelta=0;
								DeltaNum=0;
// 								Diec =-Diec;
								IsCycleOK=0;
								break;
							}
			}
			if(KeyFlag ==1)
			{
				KeyFlag =0;
				IsCycleOK =0;
				ThetaDelta =0;
				DeltaNum =0;
				LED0_Toggle();
// 				State = LQR_CIRCULAR;
			}
}

void LQR_TEST_Task(void)
{
		if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//速度调节
		{
					TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
// 					alpha 	= 	100*(s32)(ENC1_Get_AnglularPosition()+180);//摆臂
					alpha = ENC1_Get_AnglularPosition()+180;
					v_alpha = 	06*ENC1_Calc_Rot_Speed();//度每秒
// 					theta 	= 	100*(s32)ENC_Get_AnglularPosition()%36000;//旋臂
					theta 	= 	ENC_Get_AnglularPosition();
					v_theta =  0.6*ENC_Calc_Rot_Speed();//度每秒
		}
	 		if(TB_DisplayDelay_IsElapsed()==TRUE)
			{
						TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
						clear_screen();
						sprintf(str,"alpha:%f",alpha);
						display_GB2312_string(5,1,str);
						sprintf(str,"Va:%f",v_alpha);
						display_GB2312_string(7,1,str);
						sprintf(str,"theta:%f",theta);
						display_GB2312_string(1,1,str);
						sprintf(str,"Vtheat:%f",v_theta);
						display_GB2312_string(3,1,str);
						LED1_Toggle();
			}
	
}
void INPUT_Task()
{
}
void POSITION_INPUT_Task()
{			
}
void SPEED_CONTROL_Task()
{
		if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//速度调节
		{
					TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
// 					MotorA.SpeedExpected = 1200*sin(BufferNum * 14*PI/ BUFFER_SIZE );
					MotorA.SpeedTest = ENC_Calc_Rot_Speed() ;//转600
					if(BufferNum < BUFFER_SIZE)
					{
						SpeedBuffer[BufferNum] = MotorA.SpeedTest;
						BufferNum++;
					}
// 					MotorA.PWM = 5*(MotorA.SpeedExpected - MotorA.SpeedTest);
 					MotorA.PWM = PI_Controller(MotorA.SpeedExpected - MotorA.SpeedTest,&SpeedPID);
					MotorA_PWM_Update(MotorA.PWM);
					
			
// 					sprintf(str,"%d\t%d\n",MotorA.PositionTest,	MotorA.PWM);
// 					printf(str);
		    	LED2_Toggle();
					
		}
// 			if(TB_DisplayDelay_IsElapsed()==TRUE)
// 	{
// 								TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
// 								clear_screen();
// 								sprintf(str,"Pa:%d",MotorA.PositionTest);
// 								display_GB2312_string(1,1,str);
// 								sprintf(str,"Va:%f",MotorA.SpeedTest);
// 								display_GB2312_string(3,1,str);
// 								sprintf(str,"PWMa:%d",MotorA.PWM);
// 								display_GB2312_string(7,1,str);
// 								sprintf(str,"VaAim:%f",MotorA.SpeedExpected);
// 								display_GB2312_string(5,1,str);
// // 								LED1_Toggle();
// 			}
			if(Remote_Rdy)//红外处理
			{
				Remote_Rdy=0;
				ir_key=Remote_Process();
				switch(ir_key)
				{
					case IR_VOL_UP:
					MotorA.SpeedExpected += 10;
					break;
					case IR_VOL_DOWN:
					MotorA.SpeedExpected -= 10;
					break;
					case IR_PLAY:
						MotorA_Brk();
						MotorB_Brk();
						State=IDLE;
					break;
				}
			}
}
void POSITION_CONTROL_Task()
{
		if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//速度调节
		{
					TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
					MotorA.SpeedTest = ENC_Calc_Rot_Speed() ;//转600
 					MotorA.PWM = PI_Controller(MotorA.SpeedExpected - MotorA.SpeedTest,&SpeedPID);
					MotorA_PWM_Update(MotorA.PWM);
		    	LED2_Toggle();
		}
		if(TB_PositionPIDDelay_IsElapsed()==TRUE)
		{
				TB_SetPositionPIDDelay_Time(POSITION_PID_DELAY);
			MotorA.PositionExpected = 30*sin(BufferNum *30*PI/ BUFFER_SIZE );
// 				MotorA.PositionExpected = (BufferNum>100)?3600:0;
				if(BufferNum < BUFFER_SIZE)
						{
							SpeedBuffer[BufferNum] = MotorA.SpeedTest;
							PositionBuffer[BufferNum] = MotorA.PositionTest;
							BufferNum++;
						}	
				MotorA.PositionTest = ENC_Get_AnglularPosition();	
				MotorA.SpeedExpected = PD_Controller(MotorA.PositionExpected - MotorA.PositionTest,&PositionPID)
												+ FF_Controller(MotorA.PositionExpected,&PositionFF);
		}
// 		if(TB_DisplayDelay_IsElapsed()==TRUE)
// 			{
// 								TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
// 								clear_screen();
// 								sprintf(str,"Pa:%d",MotorA.PositionTest);
// 								display_GB2312_string(5,1,str);
// 								sprintf(str,"Va:%f",MotorA.SpeedTest);
// 								display_GB2312_string(7,1,str);
// 								sprintf(str,"Pb:%d",MotorB.PositionTest);
// 								display_GB2312_string(1,1,str);
// 								sprintf(str,"Vb:%f",MotorB.SpeedTest);
// 								display_GB2312_string(3,1,str);
// 			}
// 			if ( IsKeyPressed )		//如果有键按下
// 			{
// 					//清除中断标志
// 					IsKeyPressed = 0;
// 					switch(zlg_key)
// 					{
// 						case KEY1:
// 							MotorA.PositionExpected +=360;
// 		
// 						break;
// 						case KEY2:
// 							MotorA_Brk();
// 							State=IDLE;
// 						break;
// 						case KEY3:
// 							MotorB.PositionExpected +=360;
// 						break;
// 						case KEY4:
// 							MotorB_Brk();
// 						State=IDLE;
// 						break;
// 						case KEY13:
// 							MotorA_Brk();
// 						MotorB_Brk();
// 						State=IDLE;
// 						break;
// 					}
// 		}
	if(Remote_Rdy)//红外处理
			{
				Remote_Rdy=0;
				ir_key=Remote_Process();
				switch(ir_key)
				{
					case IR_PLAY:
						MotorA_Brk();
						MotorB_Brk();
						State=IDLE;
					break;
				}
			}
}

void TEST_KEY_AND_IR_Task()
{
	
	if(TB_DisplayDelay_IsElapsed()==TRUE)
		{
			TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
			clear_screen();
			display_GB2312_string(5,1,"按键与红外测试");
			display_GB2312_string(7,1,str);
			LED1_Toggle();
		}
	if( IsKeyPressed )		//如果有键按下
		{
				IsKeyPressed = 0;	//清除中断标志
				sprintf(str,"%d",zlg_key);
				delay_ms(10);
		}
	if(Remote_Rdy)
		{
			Remote_Rdy=0;
			ir_key=Remote_Process();
			sprintf(str,"%d",ir_key);
			printf(str);
		}
}

void TEST_MOTOR_Task()
{
		u32 pwm_duty=100;
	
		if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//速度调节
				{
						TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
					
						MotorA.SpeedTest=ENC_Calc_Rot_Speed() ;//转600	
// 						MotorB.SpeedTest=ENC1_Calc_Rot_Speed() ;//转600
						MotorB.SpeedTest=ENC1_Calc_Average_Speed();
				}
		if(TB_PositionPIDDelay_IsElapsed()==TRUE)
				{
					TB_SetPositionPIDDelay_Time(POSITION_PID_DELAY);
					MotorA.PositionTest = ENC_Get_AnglularPosition();
					MotorB.PositionTest = ENC1_Get_AnglularPosition();
				}
		if(TB_DisplayDelay_IsElapsed()==TRUE)
			{
						TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
						clear_screen();
						sprintf(str,"Pa:%f",MotorA.PositionTest);
						display_GB2312_string(5,1,str);
						sprintf(str,"Va:%f",MotorA.SpeedTest);
						display_GB2312_string(7,1,str);
						sprintf(str,"Pb:%f",MotorB.PositionTest);
						display_GB2312_string(1,1,str);
						sprintf(str,"Vb:%f",MotorB.SpeedTest);
						display_GB2312_string(3,1,str);
						LED1_Toggle();
			}
			if( IsKeyPressed )		//如果有键按下
			{
					IsKeyPressed = 0;	//清除中断标志
					switch(zlg_key)
					{
						case KEY1:
							MotorA_Forward(200);
						break;
						case KEY2:
							MotorB_Forward(200);
						break;
						case KEY4:
							MotorA_Backward(200);
						break;
						case KEY5:
							MotorB_Backward(200);
						break;
						case KEY13:
							MotorA_Brk();
							MotorB_Brk();
						break;
					}
			}
			if(Remote_Rdy)
			{
					Remote_Rdy=0;
					ir_key=Remote_Process();
					switch(ir_key)
					{
						case IR_KEY1:
							MotorA_Forward(pwm_duty);
							MotorB_Forward(pwm_duty);
						break;
						case IR_KEY2:
							MotorA_Backward(pwm_duty);
							MotorB_Backward(pwm_duty);
							break;
						case IR_VOL_UP:
							pwm_duty+=100;
							MotorA_Forward(pwm_duty);
						break;
						case IR_VOL_DOWN:
							pwm_duty-=100;
							MotorA_Forward(pwm_duty);
						break;
						case IR_PLAY:
							MotorA_Brk();
							MotorB_Brk();
						break;
					}
				}
}

void TEST_LED_Task()
{
		if(TB_Delay_IsElapsed()==TRUE)
		{
			TB_SetDelay_Time(LED_TEST_DELAY);
			LED0_Toggle();
			LED1_Toggle();
		}
}
void TEST_LCD_Task()
{
	if(TB_DisplayDelay_IsElapsed()==TRUE)
		{
				TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
				clear_screen();
		  	display_photo(Photo0);
			
		}
}
void TEST_ADC_Task()
{
		if( TB_ADC_Delay_IsElapsed()==true)
		{
				TB_SetADC_Time(ADC_DELAY);
				LED2_Toggle();
		}
}

void TEST_DS18B20_Task()
{
	short temp;
	if(TB_DisplayDelay_IsElapsed()==TRUE)
		{
				TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
				clear_screen();
				temp=DS18B20_Get_Temp();
			if(temp<0)
			{
				temp=-temp;
				sprintf(str,"T:-%3.2f .C",(float)temp/10);
//				sprintf(str,"T:-%d%d .C",temp/10,temp%10);
			}
			else
			{
			  sprintf(str,"T:%3.2f .C",(float)temp/10);
//				sprintf(str,"T:%d%d .C",temp/10,temp%10);
			}
			display_string_5x7(1,1,"TEST_DS18B20");
			display_string_5x7(2,2,str);
		}
}

void TEST_SPEED_Task(void)
{
	if(TB_SpeedPIDDelay_IsElapsed() == TRUE)//速度调节
	{
		TB_SetSpeedPIDDelay_Time(SPEED_PID_DELAY);
		MotorA.SpeedTest=ENC_Calc_Rot_Speed() ;//转600	
		if(BufferNum < BUFFER_SIZE)
		{
			SpeedBuffer[BufferNum] = MotorA.SpeedTest;
			BufferNum++;
		}
	}
	if(Remote_Rdy)//红外处理
	{
		Remote_Rdy=0;
		ir_key=Remote_Process();
		switch(ir_key)
		{
			case IR_EQ:
				for(BufferNum=0;BufferNum < BUFFER_SIZE;BufferNum++)
				{
					sprintf(str,"%d\n",SpeedBuffer[BufferNum]);
					printf(str);
				}
				break;
			}
		}
}

// /* 功率步进模式 */
// void POWER_INC_Task()
// {

// 	short temp;
// 	if(TB_DisplayDelay_IsElapsed()==TRUE)
// 		{
// 				TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
// 				clear_screen();
// 				temp=DS18B20_Get_Temp();
// 				if(temp<0)
// 				{
// 					temp=-temp;
// 					sprintf(str,"温度:-%3.2f .C",(float)temp/10);
// 				}
// 				else
// 				{
// 					sprintf(str,"温度:%3.2f 'C",(float)temp/10);
// 				}
// 				display_GB2312_string(1,1,"一，功率步进");  
// 			
// 				display_GB2312_string(3,1,str);    
// 				sprintf(str,"功率:%3.1f",pwm_duty*0.2);
// 				display_GB2312_string(5,1,str); 
// 				display_GB2312_string(7,1,"黄源 王涛 梁伟"); 
// 		}
// 	if(Remote_Rdy)
// 		{
// 				Remote_Rdy=0;
// 				ir_key=Remote_Process();
// 				switch(ir_key)
// 				{
// 					case IR_CH_UP:
// 							pwm_duty+=10;//功率增加
// 						TIM1_PWM_Update(pwm_duty);
// 					break;
// 					case IR_CH_DOWN:
// 						pwm_duty-=10;//功率增加
// 						TIM1_PWM_Update(pwm_duty);
// 						break;
// 					case IR_KEY1:
// 						pwm_duty+=0.1;
// 						TIM1_PWM_Update(pwm_duty);
// 						break;
// 					case IR_KEY2:
// 							pwm_duty-=0.1;
// 						TIM1_PWM_Update(pwm_duty);
// 						break;
// 					case IR_VOL_UP:
// 						pwm_duty+=0.5;//功率增加
// 						if(pwm_duty>100)
// 							pwm_duty=100;
// 						else if(pwm_duty<0)
// 							pwm_duty=0;
// 						TIM1_PWM_Update(pwm_duty);
// 					break;
// 					case IR_VOL_DOWN://功率减小
// 						pwm_duty-=0.5;
// 						if(pwm_duty>100)
// 							pwm_duty=100;
// 						else if(pwm_duty<0)
// 							pwm_duty=0;
// 						TIM1_PWM_Update(pwm_duty);
// 					break;
// 					case IR_PLAY://停止加热
// 							pwm_duty=0.5;
// 					break;
// 					case IR_PREV:
// 						pwm_duty=0;
// 						TIM1_PWM_Update(0);
// 						State=MENU;
// 						break;
// 				}
// 			
// 		}
// }
// /* 温度控制模式 */
// void TEMPERATURE_CONTROL_Task()
// {
// 	short TemperatureTest;
// 	if(TB_DisplayDelay_IsElapsed()==TRUE)
// 		{
// 				TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
// 				TemperatureTest=DS18B20_Get_Temp();
// 				pwm_duty=PID_Controller((TemperatureAim-TemperatureTest),&TemperaturePID);
// 				TIM1_PWM_Update(pwm_duty);//反向的
// 		}
// }
// void TEMPERATURE_CONTROL_INPUT_Task()
// {
// 	short temp;
// 	if(TB_DisplayDelay_IsElapsed()==TRUE)
// 		{
// 				TB_SetDisplayDelay_Time(LCD_DISPLAY_DELAY);
// 				clear_screen();
// 				temp=DS18B20_Get_Temp();
// 				TemperatureTest=(float)temp/10;
// 			if(IsInputOK)
// 			{
// 				
// // 				pwm_duty=(TemperatureAim-TemperatureTest)*TemperaturePID.Kp;
// 				pwm_duty=PID_Controller(TemperatureAim-TemperatureTest,&TemperaturePID);
// 				TIM1_PWM_Update(pwm_duty);
// 				sprintf(str,"%f\n",TemperatureTest);
// 				printf(str);
// 				LED0_Toggle();
// 			}
// 				if(temp<0)
// 				{
// 					temp=-temp;
// 					sprintf(str,"温度:-%3.2f .C",TemperatureTest);
// 				}
// 				else
// 				{
// 					sprintf(str,"温度:%3.2f 'C",TemperatureTest);
// 				}
// 				display_GB2312_string(1,1,"二，温度控制");  
// 				display_GB2312_string(3,1,str);    
// 				sprintf(str,"目标:%3.1fD:%2.1f",TemperatureAimTemp,pwm_duty);
// 				display_GB2312_string(5,1,str); 
// 				display_GB2312_string(7,1,"黄源 王涛 梁伟"); 
// 		}
// 		if(Remote_Rdy)
// 		{
// 				Remote_Rdy=0;
// 				ir_key=Remote_Process();
// 				switch(ir_key)
// 				{
// 					case IR_CH_UP:
// 							TemperatureAimTemp+=10;//温度增加
// // 							IsInputOK=0;
// 					break;
// 					case IR_CH_DOWN:
// 							TemperatureAimTemp-=10;//温度减小
// // 							IsInputOK=0;
// 						break;
// 					case IR_VOL_UP:
// 						TemperatureAimTemp+=1;//温度增加
// // 						IsInputOK=0;
// 					break;
// 					case IR_VOL_DOWN:
// 						TemperatureAimTemp-=1;//温度减小
// // 						IsInputOK=0;
// 					break;
// 					case IR_PLAY://开始加热
// 						if(IsInputOK)
// 						{
// 							IsInputOK=0;
// 						}
// 						else
// 						{
// 							IsInputOK=1;
// 							TemperatureAim=TemperatureAimTemp;
// 							if(TemperatureAim>80)
// 							{
// 								TemperaturePID.Kp=55;
// 								TemperaturePID.Ki=0.4;		
// 							}
// 							else if(TemperatureAim>70)
// 							{
// 								TemperaturePID.Kp=25;
// 								TemperaturePID.Ki=0.3;
// 							}
// 							else if(TemperatureAim>60)
// 							{
// 								TemperaturePID.Kp=18;
// 								TemperaturePID.Ki=0.3;
// 							}
// 							else
// 							{
// 								TemperaturePID.Kp=11;
// 								TemperaturePID.Ki=0.2;
// 							}
// 						}
// 					break;
// 					case IR_PREV:
// 						pwm_duty=0;
// 						TIM1_PWM_Update(0);
// 						State=MENU;
// 						break;
// 				}
// 			
// 		}
// }




















