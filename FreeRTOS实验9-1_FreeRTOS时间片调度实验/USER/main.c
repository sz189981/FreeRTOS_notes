#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
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
/*
1、实验目的
学习使用 FreeRTOS 的时间片调度。

2、实验设计
本实验设计三个任务： start_task、 task1_task 和 task2_task ， 其中 task1_task 和 task2_task
的任务优先级相同，都为 2， 这三个任务的任务功能如下：
start_task：用来创建其他 2 个任务。
task1_task ：控制 LED0 灯闪烁， 并且通过串口打印 task1_task 的运行次数。
task2_task ： 控制 LED1 灯闪烁，并且通过串口打印 task2_task 的运行次数。
*/
#define START_TASK_PRIO 						1 //任务优先级
#define START_STK_SIZE 						256	//任务堆栈大小
TaskHandle_t StartTask_Handler; 			//任务句柄
void start_task(void *pvParameters); 	//任务函数

#define TASK1_TASK_PRIO                         2
#define TASK1_STK_SIZE                        128
TaskHandle_t Task1Task_Handler;
void task1_task(void * pvParameters);


#define TASK2_TASK_PRIO                         2
#define TASK2_STK_SIZE                        128
TaskHandle_t Task2Task_Handler;
void task2_task(void * pvParameters);

//LCD刷屏时使用的颜色
int lcd_discolor[14]={  WHITE, BLACK, BLUE,  BRED,
                        GRED,  GBLUE, RED,   MAGENTA,
                        GREEN, CYAN,  YELLOW,BROWN,
                        BRRED, GRAY };
    
//●  main()函数
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//设置系统中断优先级分组 4
    delay_init(168);                    //初始化延时函数
    uart_init(115200);                  //初始化串口
    LED_Init();                         //初始化 LED 端口
    KEY_Init();                         //初始化按键
    LCD_Init();                         //初始化LCD

    POINT_COLOR = RED;
    LCD_ShowString(30,10,200,16,16,"ATK STM32F103/F407");	
    LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 9-1");
    LCD_ShowString(30,50,200,16,16,"FreeRTOS Round Robin");
    LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,90,200,16,16,"2016/11/25");

    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char* )  "start_task",           //任务名称
                (uint16_t )     START_STK_SIZE,         //任务堆栈大小
                (void* )        NULL,                   //传递给任务函数的参数
                (UBaseType_t )  START_TASK_PRIO,        //任务优先级
                (TaskHandle_t*) &StartTask_Handler);
    vTaskStartScheduler(); //开启任务调度
}
//● 任务函数
//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //进入临界区

    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,
                (const char *   )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void *         )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //创建List任务
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
                vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL(); //退出临界区
}

//task1任务函数
void task1_task(void * pvParameters)
{
    u8 task1_num=0;
    while(1)
    {
        task1_num++;                          //任务执1行次数加1 注意task1_num1加到255的时候会清零！！
        LED0=!LED0;
        taskENTER_CRITICAL();                   //进入临界区
        printf("任务1已经执行：%d次\r\n",task1_num);
        taskEXIT_CRITICAL();                    //退出临界区
        //延时10ms，模拟任务运行10ms，次函数不会引起任务调度
        delay_xms(10);
    }
}

//task2任务函数
void task2_task(void * pvParameters)
{
    u8 task2_num = 0;
    while(1)
    {
        task2_num++;
        LED1=!LED1;
        taskENTER_CRITICAL();
        printf("任务2已经执行：%d次\r\n",task2_num);
        taskEXIT_CRITICAL();
        //延时10ms，模拟任务运行10ms，次函数不会引起任务调度
        delay_xms(10);
        
    }
}


