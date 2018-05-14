#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
/************************************************
ALIENTEK 探索者 STM32F407 开发板 FreeRTOS 实验 2-1
FreeRTOS 移植实验-库函数版本
技术支持： www.openedv.com
淘宝店铺： http://eboard.taobao.com
关注微信公众平台微信号： "正点原子"，免费获取 STM32 资料。
广州市星翼电子科技有限公司
作者：正点原子 @ALIENTEK
************************************************/
#define START_TASK_PRIO 						1 //任务优先级
#define START_STK_SIZE 						128	//任务堆栈大小
TaskHandle_t StartTask_Handler; 			//任务句柄
void start_task(void *pvParameters); 	//任务函数

#define LED0_TASK_PRIO 							2 //任务优先级
#define LED0_STK_SIZE 						 50 //任务堆栈大小
TaskHandle_t LED0Task_Handler; 				//任务句柄
void led0_task(void *p_arg); 					//任务函数

#define LED1_TASK_PRIO 						 3  //任务优先级
#define LED1_STK_SIZE 						50  //任务堆栈大小
TaskHandle_t LED1Task_Handler; 				//任务句柄
void led1_task(void *p_arg);					//任务函数

#define FLOAT_TASK_PRIO 						4 //任务优先级
#define FLOAT_STK_SIZE 						128 //任务堆栈大小
TaskHandle_t FLOATTask_Handler; 			//任务句柄
void float_task(void *p_arg); 				//任务函数

//●  main()函数
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//设置系统中断优先级分组 4
	delay_init(168); 																//初始化延时函数
	uart_init(115200); 															//初始化串口
	LED_Init(); 																		//初始化 LED 端口
	
	//创建开始任务
	xTaskCreate((TaskFunction_t )start_task, 		//任务函数
							(const char* )	"start_task", 		//任务名称
							(uint16_t )				START_STK_SIZE, 			//任务堆栈大小
							(void* )					NULL, 								//传递给任务函数的参数
							(UBaseType_t )		START_TASK_PRIO, 			//任务优先级
							(TaskHandle_t* )	&StartTask_Handler); 	//任务句柄
	vTaskStartScheduler(); 									//开启任务调度
}
//● 任务函数
//开始任务任务函数
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); //进入临界区
	
	//创建 LED0 任务
	xTaskCreate((TaskFunction_t )led0_task,
							(const char* )	"led0_task",
							(uint16_t )			 LED0_STK_SIZE,
							(void* )				 NULL,
							(UBaseType_t )	 LED0_TASK_PRIO,
							(TaskHandle_t* ) &LED0Task_Handler);
							
	//创建 LED1 任务
	xTaskCreate((TaskFunction_t )led1_task,
							(const char* )	"led1_task",
							(uint16_t )			 LED1_STK_SIZE,
							(void* )				 NULL,
							(UBaseType_t )	 LED1_TASK_PRIO,
							(TaskHandle_t* ) &LED1Task_Handler);
	
	//浮点测试任务
	xTaskCreate((TaskFunction_t )float_task,
							(const char* )	"float_task",
							(uint16_t )			 FLOAT_STK_SIZE,
							(void* )				 NULL,
							(UBaseType_t )	 FLOAT_TASK_PRIO,
							(TaskHandle_t* ) &FLOATTask_Handler);
	
	vTaskDelete(StartTask_Handler); //删除开始任务
	
	taskEXIT_CRITICAL(); //退出临界区
}
//LED0 任务函数
void led0_task(void *pvParameters)
{
	while(1)
	{
		LED0=~LED0;
		vTaskDelay(500);
	}
}
//LED1 任务函数
void led1_task(void *pvParameters)
{
	while(1)
	{
		LED1=0;
		vTaskDelay(200);
		LED1=1;
		vTaskDelay(800);
	}
}
//浮点测试任务
void float_task(void *p_arg)
{
	static float float_num=0.00;
	while(1)
	{
		float_num+=0.01f;
		printf("float_num 的值为: %.4f\r\n",float_num);
		vTaskDelay(1000);
	}
}
