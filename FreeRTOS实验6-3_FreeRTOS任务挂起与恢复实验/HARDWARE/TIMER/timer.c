#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "timer.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

//● 中断初始化及处理过程
//通用定时器 3 中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器 3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 					//使能 TIM3 时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 									//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; 									//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure); 						//初始化 TIM3
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 											//允许定时器 3 更新中断
	TIM_Cmd(TIM3,ENABLE); 																				//使能定时器 3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 								//定时器 3 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x04; 		//抢占优先级 4 (1)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 					//子优先级 0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//通用定时器 5 中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器 5!
void TIM5_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 						//使能 TIM5 时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 										//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; 										//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure); 							//初始化 TIM5
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); 												//允许定时器 5 更新中断
	TIM_Cmd(TIM5,ENABLE); 																					//使能定时器 5
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; 									//定时器 5 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x05; 			//抢占优先级 5 (2)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 						//子优先级 0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//定时器 3 中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		printf("<<TIM3 输出.......>>\r\n");
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
}
//定时器 5 中断服务函数
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{
		printf("[[TIM5 输出.......]]\r\n");
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update); //清除中断标志位
}


