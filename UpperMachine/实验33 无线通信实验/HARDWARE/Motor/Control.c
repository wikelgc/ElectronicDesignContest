#include "Control.h"

/*************************控制器参数初始化************************/
void Controller_Init(void)
{
	/* PID 反馈控制器参数初始化 */
	SpeedPID.Kp = MAXON_RE25_SPEED_KP;
	SpeedPID.Ki = MAXON_RE25_SPEED_KI;
	SpeedPID.Kd = 0;
	SpeedPID.ErrorBond = 0 ;
	SpeedPID.ErrorPre =0;
	SpeedPID.OutputMax = PWM_MAX;
	SpeedPID.OutputMin = PWM_MIN;
	SpeedPID.ErrorSum =  0;
	SpeedPID.ErrorSumMax = INT32_MAX;
	SpeedPID.ErrorSumMin = INT32_MIN;
	
	
	PositionPID.Kp = MAXON_RE25_POSITION_KP ;
	PositionPID.Ki = 0 ;
	PositionPID.Kd = MAXON_RE25_POSITION_KD;
	PositionPID.ErrorBond = 0;
	PositionPID.ErrorPre  = 0;
	PositionPID.OutputMax = SPEED_MAX;
	PositionPID.OutputMin = SPEED_MIN;
	PositionPID.ErrorSum  = 0;
	PositionPID.ErrorSumMax = INT32_MAX;
	PositionPID.ErrorSumMin = INT32_MIN;
	
	/* FeedForward 前馈控制器参数初始化 */
	PositionFF.Kv = 1.5;//1.6;
	PositionFF.Ka = 0;
	PositionFF.FFmax = SPEED_MAX;
	PositionFF.FFmin = SPEED_MIN;
}



//定义全局变量
float PID_P = 35.0;
float PID_I = 0.00;
float PID_D = 0.00;

void set_PID(u16 P,u16 I,u16 D)
{
	PID_P = P/100.0;
	PID_I = I/100.0;
	PID_D = D/100.0;
}

//PI控制器
float PI_P = 10;
float PI_I = 0;
void set_PI(u16 P,u16 I)
{
	PI_P = P/100.0;
	PI_I = I/100.0;
}

/* 清除积分误差 */
void ErrorClear(PID_Typedef *PID )
{
	PID->ErrorSum=0;
	
}

/*********************速度环*0*****************/
s32  SpeedPI_Regulator(s32 SpeedError,PID_Typedef *PID)//只需要PI
{
	s32 output;
 
	PID->ErrorSum += SpeedError; 
	/* 抗积分饱和 */
	if(PID->ErrorSum > PID->ErrorSumMax)
	{
		PID->ErrorSum = PID->ErrorSumMax;
	}
	else if(PID->ErrorSum < PID->ErrorSumMin)
	{
		PID->ErrorSum=PID->ErrorSumMin;
	}
 	
	output = SpeedError*PID->Kp + PID->ErrorSum *PID->Ki ;//计算输出
	if(output > PID->OutputMax)
				output = PID->OutputMax;
	else if(output < PID->OutputMin)
			output = PID->OutputMin;
	return output;
}



/***************************位置环0*****************************/
s32 PositionPID_Regulator(s32 PositionError,PID_Typedef *PID)
{
	s32 output;
 
	PID->ErrorSum += PositionError; 
	/* 抗积分饱和 */
	if(PID->ErrorSum > PID->ErrorSumMax)
	{
		PID->ErrorSum = PID->ErrorSumMax;
	}
	else if(PID->ErrorSum < PID->ErrorSumMin)
	{
		PID->ErrorSum=PID->ErrorSumMin;
	}
 	
	output = PositionError*PID->Kp + PID->ErrorSum *PID->Ki ;//计算输出
	if(output > PID->OutputMax)
				output = PID->OutputMax;
	else if(output < PID->OutputMin)
			output = PID->OutputMin;
	return output;
}


/*P控制器*/
double P_Controller(float Error,PID_Typedef *PID)
{
	double output;
	
	output = PID->Kp * Error;
	/*输出限制幅度*/
	if(output > PID->OutputMax)
	{
		output = PID->OutputMax;
	}
	else if(output < PID->OutputMin)
	{
		output = PID->OutputMin;
	}
	
	return output;
}


/*PD 控制器*/
double PD_Controller(float Error,PID_Typedef *PID)
{
	double output;
	double pTerm;
	double dTerm;
	
	pTerm = PID->Kp * Error;
	dTerm = PID->Kd * (Error - PID->ErrorPre);
	PID->ErrorPre = Error;
	output = pTerm + dTerm;
	/*输出限制幅度*/
	if(output > PID->OutputMax)
	{
		output = PID->OutputMax;
	}
	else if(output < PID->OutputMin)
	{
		output = PID->OutputMin;
	}
	
	return output;
}




/*PI 控制器*/
double PI_Controller(PID_Typedef *PID)
{
	double output;
	double pTerm;
	double iTerm;
	
	//
	if(abs_Double(PID->errNow) < PID->KiBond)//
	{
		PID->ErrorSum+=PID->errNow;	//
	}
	/*避免积分饱和溢出*/
	if(PID->ErrorSum > PID->ErrorSumMax)
	{
		PID->ErrorSum = PID->ErrorSumMax;
	}
	else if(PID->ErrorSum < PID->ErrorSumMin)
	{
		PID->ErrorSum = PID->ErrorSumMin;
	}
	
	pTerm = PID->Kp * PID->errNow;				//比例作用
	iTerm = PID->Ki * PID->ErrorSum;//积分作用
	
	output=iTerm+pTerm;
	/*输出限制幅度*/
	if(output > PID->OutputMax)
	{
		output = PID->OutputMax;
	}
	else if(output < PID->OutputMin)
	{
		output = PID->OutputMin;
	}
	return output;
}

//PI控制器的例程
PID_Typedef PI_control;
int User_PIControl(float spdNow)
{
	double output;
	PI_control.spdTag = 0;
	if(360 - spdNow < spdNow)
	{
		spdNow = spdNow - 360;
	}
	PI_control.errNow = PI_control.spdTag - spdNow;
	
	//设定pid参数
	PI_control.Kp = PI_P;
	PI_control.Ki = PI_I;
	
	PI_control.ErrorBond = 0;
	PI_control.OutputMax = 979;
	PI_control.OutputMin = -979;
	
	PI_control.ErrorSum = 0;
	PI_control.KiBond = 1000;
	PI_control.ErrorSumMax = INT32_MAX;
	PI_control.ErrorSumMin = INT32_MIN;

	output=PI_Controller(&PI_control);     //?'DD????�?PID??法
	
//	PI_control.control = (int)output;       	//???????佃??? ??�?�??店?pwm
//	
//	MotorB_PWM_Update(-PI_control.control); 	
//	
//	return 0;
	return output;
}




/**************************************位置式pid*************************************************/
/* 一般的位置式PID  */
double PID_Pos_Controller(PID_Pos_Typedef *PID)
{
	double output;
	double pTerm;//比例作用
	double iTerm;//积分作用
	double dTerm;//微分作用
	
	//积分分离
	if(abs_Double(PID->errNow) < PID->KiBond)//积分分离 ,小误差是开始积分作用
	{
		PID->ErrorSum+=PID->errNow;	//计算误差累计积分
	}
	
	/* 抗饱和积分*/
	if(PID->ErrorSum > PID->ErrorSumMax)
	{
		PID->ErrorSum = PID->ErrorSumMax;
	}
	else if(PID->ErrorSum < PID->ErrorSumMin)
	{
		PID->ErrorSum=PID->ErrorSumMin;
	}
	/*  */
	pTerm=PID->Kp*PID->errNow;//比例作用
	iTerm=PID->Ki*PID->ErrorSum;//积分作用
	dTerm=PID->Kd*(PID->errNow-PID->ErrorPre);//微分作用
	output=  pTerm+iTerm+dTerm;//pid的输出值
	
	PID->ErrorPre=PID->errNow;//记录前一次误差
	
	/* 限幅度输出 */
	if(output > PID->OutputMax)
	{
		output=PID->OutputMax;
	}
	else if(output < PID->OutputMin)
	{
		output=PID->OutputMin;
	}
	return output ;
}

/***************位置控制**************/
PID_Pos_Typedef PID_Pos;//定义PID算法的结构体

//一般的pid控制算法

//int User_PidPosControl(u16 spdNow,u16 spdTag)
//{
//	double output;
//  // spdNow = 180;
//	PID_Pos.spdTag = spdTag;

//	PID_Pos.errNow =  PID_Pos.spdTag - spdNow; //计算并写入速度误差
//   
//   //修改pid参数
//	PID_Pos.Kp = 35;			   //比例系数参数
//	PID_Pos.Ki = 0; 	           //积分系数参数
//	PID_Pos.Kd = 1;      	       //微分系数参数
//	
//	PID_Pos.ErrorBond = 0;
//	PID_Pos.ErrorPre  = 0;
//	PID_Pos.OutputMax = 979;
//	PID_Pos.OutputMin = -979;
//	PID_Pos.ErrorSum  = 0;
//	PID_Pos.KiBond = 1000;
//	PID_Pos.ErrorSumMax = INT32_MAX;
//	PID_Pos.ErrorSumMin = INT32_MIN;
// 
//	output=PID_Pos_Controller(&PID_Pos);     //执行绝对式PID算法
//	
////	PID_Pos.control = (int)output;       	//对控制值取整 获取的值是pwm
////	
////	MotorB_PWM_Update(PID_Pos.control); 	 //放入pwm用于收敛速度的控制
////	
////	return 0;
//	return output;
//}
/**************************************************************************************************/




/**************************************增量式pid*************************************************/
/* 一般的增量式PID  */
int PID_Inc_Controller(PID_Inc_Typedef *PID_INC)
{
	
	double pTerm,iTerm,dTerm;  //PID各项值
	float dErrP, dErrI, dErrD;

	//计算误差累计积分
	if(abs_Double(PID_INC->errNow) < PID_INC->KiBond)//积分分离 ,小误差是开始积分作用
	{
		PID_INC->ErrorSum+=PID_INC->errNow;	//计算误差累计积分
	}
	
	// 抗饱和积分
	if(PID_INC->ErrorSum > PID_INC->ErrorSumMax)
	{
		PID_INC->ErrorSum = PID_INC->ErrorSumMax;
	}
	else if(PID_INC->ErrorSum < PID_INC->ErrorSumMin)
	{
		PID_INC->ErrorSum=PID_INC->ErrorSumMin;
	}

	
	dErrP = PID_INC->errNow - PID_INC->errOld1;
	dErrI = PID_INC->errNow;
	dErrD = PID_INC->errNow - 2 * PID_INC->errOld1 + PID_INC->errOld2;

	pTerm = PID_INC ->Kp * dErrP;
	iTerm = PID_INC ->Ki * dErrI;
	dTerm = PID_INC ->Kd * dErrD;	

	/*增量式PID计算*/
	PID_INC->dCtrOut=  pTerm+iTerm+dTerm; 
	
	PID_INC->errOld2 = PID_INC->errOld1; //二阶误差微分
	PID_INC->errOld1 = PID_INC->errNow;  //一阶误差微分

	
	if(PID_INC->Kp == 0 && PID_INC->Ki == 0 && PID_INC->Kd == 0)
		PID_INC->dCtrOut = 0;
	else
		PID_INC->ctrOut += PID_INC->dCtrOut;
	
	/* 限幅度输出 */
	if(PID_INC->ctrOut>PID_INC->OutputMax)
	{
		PID_INC->ctrOut=PID_INC->OutputMax;
	}
	else if(PID_INC->ctrOut<PID_INC->OutputMin)
	{
		PID_INC->ctrOut=PID_INC->OutputMin;
	}
	
	return (int)PID_INC->ctrOut ;
}


/***************增量控制**************/
//PID_Inc_Typedef PID_Inc;//定义PID算法的结构体

////一般的pid控制算法
//int User_PidIncControl(u16 spdNow)
//{
//	double output;
//	// spdNow = V2;
//	PID_Inc.spdTag = 400;

//	PID_Inc.errNow =  PID_Pos.spdTag - spdNow; //计算并写入速度误差

//	//修改pid参数
//	PID_Inc.Kp = PID_P;			   //比例系数参数
//	PID_Inc.Ki = PID_I; 	           //积分系数参数
//	PID_Inc.Kd = PID_D;      	       //微分系数参数
//	
//	PID_Inc.ErrorBond = 1000;
//	//PID_Inc.ErrorPre  = 0;
//	PID_Inc.OutputMax = 979;
//	PID_Inc.OutputMin = -979;
//	PID_Inc.ErrorSum  = 0;
//	PID_Inc.ErrorSumMax = INT32_MAX;
//	PID_Inc.ErrorSumMin = INT32_MIN;

//	output=PID_Inc_Controller(&PID_Inc);     //执行绝对式PID算法

//	PID_Inc.control = (int)output;       //对控制值取整 获取的值是pwm

//	MotorB_PWM_Update(PID_Inc.control);  //放入pwm用于收敛速度的控制
//	
//	return 0;
//}



double FF_Controller(double rin,FForward_Typedef *FF )
{
	double output;
	
	FF->rinV = rin - FF->rinPre; //一阶微分
	FF->rinA = FF->rinV - FF->rinV_Pre;//二阶微分
	FF->rinPre = rin;
	FF->rinV_Pre = FF->rinV;
	output = FF->rinV * FF->Kv + FF->rinA * FF->Ka ;//
	if(output > FF->FFmax)
	{
		output = FF->FFmax;
	}
	else if(output < FF->FFmin)
	{
		output =  FF->FFmin;
	}
	
	return output;
}
