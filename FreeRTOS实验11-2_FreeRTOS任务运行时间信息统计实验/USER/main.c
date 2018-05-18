#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"  
#include "string.h"
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
    学习使用 FreeRTOS 运行时间壮态统计函数 vTaskGetRunTimeStats()的使用。
2、实验设计
    本实验设计四个任务： start_task、 task1_task 、 task2_task 和 RunTimeStats_task，这四个任
    务的任务功能如下：
    start_task：用来创建其他 3 个任务。
    task1_task ： 应用任务 1， 控制 LED0 灯闪烁，并且刷新 LCD 屏幕上指定区域的颜色。
    task2_task ： 应用任务 2，控制 LED1 灯闪烁，并且刷新 LCD 屏幕上指定区域的颜色。
    RunTimeStats_task：获取按键值，当 KEY_UP 键按下以后就调用函数 vTaskGetRunTimeStats()
    获取任务的运行时间信息，并且将其通过串口输出到串口调试助手上。
    实验需要一个按键 KEY_UP，用来获取系统中任务运行时间信息。
*/
#define START_TASK_PRIO 						1   //任务优先级
#define START_STK_SIZE 						  128	//任务堆栈大小
TaskHandle_t StartTask_Handler; 			        //任务句柄
void start_task(void *pvParameters); 	            //任务函数

#define TASK1_TASK_PRIO                         2
#define TASK1_STK_SIZE                        128
TaskHandle_t Task1Task_Handler;
void task1_task(void * pvParameters);


#define TASK2_TASK_PRIO                         3
#define TASK2_STK_SIZE                        128
TaskHandle_t Task2Task_Handler;
void task2_task(void * pvParameters);

#define RUNTIMESTATS_TASK_PRIO                  4
#define RUNTIMESTATS_STK_SIZE                 128
TaskHandle_t RunTimeStats_Handler;
void RunTimeStats_task(void * pvParameters);


char RunTimeInfo[400];  //保存任务运行时间信息

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
    LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 11-2");
    LCD_ShowString(30,50,200,16,16,"Get Run Time Stats");
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
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    //创建RunTimeStats任务
    xTaskCreate((TaskFunction_t )RunTimeStats_task,
                (const char*    )"RunTimeStats_task",
                (uint16_t       )RUNTIMESTATS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )RUNTIMESTATS_TASK_PRIO,
                (TaskHandle_t*  )&RunTimeStats_Handler);
                
                
                
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL(); //退出临界区
}


//task1任务函数
void task1_task(void * pvParameters)
{
    u8 task1_num = 0;
    
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(5,110,115,314);
    LCD_DrawLine(5,130,115,130);
    POINT_COLOR=BLUE;
    LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
    
    while(1)
    {
        task1_num++;    //任务1执行次数加1 注意task1_num加到255的时候会清零！！
        LED0=!LED0;
        LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]); //填充区域
        LCD_ShowxNum(86,111,task1_num,3,16,0x80);           //显示任务执行次数
        vTaskDelay(1000);                                   //延时1s，也就是1000个时钟节拍
    }    
}

//task2任务函数
void task2_task(void * pvParameters)
{
    u8 task2_num = 0;
    
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(125,110,234,314);
    LCD_DrawLine(125,130,234,130);
    POINT_COLOR=BLUE;
    LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
    
    while(1)
    {
        task2_num++;    //任务2执行次数加1 注意task2_num加到255的时候会清零！！
        LED1=!LED1;
        LCD_ShowxNum(206,111,task2_num,3,16,0x80);          //显示任务执行次数
        LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
        vTaskDelay(1000);                                   //延时1s，也就是1000个时钟节拍
    }    
}

//RunTimeStats任务
void RunTimeStats_task(void * pvParameters)
{
    u8 key=0;
    while(1)
    {
        key=KEY_Scan(0);
        if(key==WKUP_PRES)
        {
            memset(RunTimeInfo,0,400);          //信息缓冲区清零
            vTaskGetRunTimeStats(RunTimeInfo);  //获取任务运行时间信息
            printf("任务名\t\t\t运行时间\t运行所占百分比\r\n");
            printf("%s\r\n",RunTimeInfo);
        }
        vTaskDelay(10);                         //延时10ms，也就是1000个时钟节拍
    }
}






