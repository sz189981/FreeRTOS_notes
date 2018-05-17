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
学习使用 FreeRTOS与任务状态或者信息查询有关的 API函数，
包括 uxTaskGetSystemState()、vTaskGetInfo()、 eTaskGetState()和 vTaskList()。

2、实验设计
本实验设计三个任务： start_task、 led0_task 和 query_task ，这三个任务的任务功能如下：
    start_task：用来创建其他 2 个任务。
    led0_task ：控制 LED0 灯闪烁，提示系统正在运行。
    query_task：任务状态和信息查询任务，在此任务中学习使用与任务的状态和信息查询有
                 关的 API 函数。
实验需要一个按键 KEY_UP，这个按键的功能如下：
    KEY_UP: 控制程序的运行步骤。
*/
#define START_TASK_PRIO 						1   //任务优先级
#define START_STK_SIZE 						  128	//任务堆栈大小
TaskHandle_t StartTask_Handler; 			        //任务句柄
void start_task(void *pvParameters); 	            //任务函数

#define LED0_TASK_PRIO                         2
#define LED0_STK_SIZE                        128
TaskHandle_t Led0Task_Handler;
void led0_task(void * pvParameters);


#define QUERY_TASK_PRIO                         3
#define QUERY_STK_SIZE                        256
TaskHandle_t QueryTask_Handler;
void query_task(void * pvParameters);


char InfoBuffer[1000];  //保存信息的数组

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
    LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 11-1");
    LCD_ShowString(30,50,200,16,16,"Task Info Query");
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
    xTaskCreate((TaskFunction_t )led0_task,
                (const char *   )"led0_task",
                (uint16_t       )LED0_STK_SIZE,
                (void *         )NULL,
                (UBaseType_t    )LED0_TASK_PRIO,
                (TaskHandle_t*  )&Led0Task_Handler);
    //创建List任务
    xTaskCreate((TaskFunction_t )query_task,
                (const char*    )"query_task",
                (uint16_t       )QUERY_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )QUERY_TASK_PRIO,
                (TaskHandle_t*  )&QueryTask_Handler);
                vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL(); //退出临界区
}

//led0任务函数
void led0_task(void * pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(500);    //延时500ms，也就是500个时钟节拍
    }
}

//query任务函数
void query_task(void * pvParameters)
{
    u32             TotalRunTime;
    UBaseType_t     ArraySize,x;
    TaskStatus_t*   StatusArray;
    
    //第一步：函数uxTaskGetSystemState()的使用:获取系统中任务状态
    printf("/**********第一步：函数uxTaskGetSystemState()的使用***********/\r\n");
    ArraySize = uxTaskGetNumberOfTasks();                            //获取系统任务数量
    StatusArray = pvPortMalloc(ArraySize*sizeof(TaskStatus_t));      //申请内存
    if(StatusArray != NULL)
    {
        ArraySize=uxTaskGetSystemState((TaskStatus_t* )StatusArray,   //获取系统中所有任务的信息，并将获取到的信息保存
                                    (UBaseType_t   )ArraySize,        //到StatusArray中
                                    (uint32_t*     )&TotalRunTime);
        printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
        for(x=0;x<ArraySize;x++)
        {
            //通过串口打印出获取到的系统任务的有关信息，比如任务名称、
            //任务优先级和任务编号。
            printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",
                    StatusArray[x].pcTaskName,
                    (int)StatusArray[x].uxCurrentPriority,
                    (int)StatusArray[x].xTaskNumber);
        }
    }
    vPortFree(StatusArray); //释放内存
    printf("/*********************结束********************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    while(KEY_Scan(0)!=WKUP_PRES)           //等待KEY_UP键按下
        delay_ms(10);
    
    //第二步：函数vTaskGetInfo()的使用：获取某个任务信息
    TaskHandle_t TaskHandle;
    TaskStatus_t TaskStatus;
    
    printf("/************第二步：函数vTaskGetInfo()的使用**************/\r\n");
    TaskHandle=xTaskGetHandle("led0_task");         //根据任务名获取任务句柄
    //获取LED0_Task的任务信息
    vTaskGetInfo((TaskHandle_t )TaskHandle,         //任务句柄
                 (TaskStatus_t*)&TaskStatus,        //任务信息结构体
                 (BaseType_t   )pdTRUE,             //允许通缉任务堆栈历史最小剩余大小
                 (eTaskState   )eInvalid);          //函数自己获取任务运行状态
    //通过串口打印出指定任务的有关信息
    printf("任务名：            %s\r\n",TaskStatus.pcTaskName);
    printf("任务编号：          %d\r\n",(int)TaskStatus.xTaskNumber);
    printf("任务状态：          %d\r\n",TaskStatus.eCurrentState);
    printf("任务当前优先级：    %d\r\n",(int)TaskStatus.uxCurrentPriority);
    printf("任务基优先级：      %d\r\n",(int)TaskStatus.uxBasePriority);
    printf("任务堆栈基地址：    %#x\r\n",(int)TaskStatus.pxStackBase);
    printf("任务堆栈历史剩余最小值：%d\r\n",TaskStatus.usStackHighWaterMark);
    printf("/***************结束***********************/\r\n");
    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    while(KEY_Scan(0)!=WKUP_PRES)
        delay_ms(10);
    
    //第三步：函数eTaskGetState()的使用 :用来查询某个任务的运行状态 ，
    //比如：运行态、阻塞态、挂起态、就绪态等，返回值是个枚举类型。
    eTaskState TaskState;
    char TaskInfo[10];
    
    printf("/******第三步：函数eTaskGetState()的使用****/\r\n");
    TaskHandle=xTaskGetHandle("query_task");        //根据任务名获取任务句柄
    TaskState=eTaskGetState(TaskHandle);
    memset(TaskInfo,0,10);
    switch((int)TaskState)
    {
        case 0:sprintf(TaskInfo,"Running");break;
        case 1:sprintf(TaskInfo,"Ready");break;
        case 2:sprintf(TaskInfo,"Suspend");break;
        case 3:sprintf(TaskInfo,"Delete");break;
        case 4:sprintf(TaskInfo,"Invalid");break;
    }
    printf("任务状态值：%d，对应的状态为:%s\r\n",TaskState,TaskInfo);
    printf("/*************结束**********/\r\n");
    printf("按下KEU_UP键继续！\r\n\r\n\r\n");
    while(KEY_Scan(0)!=WKUP_PRES)
        delay_ms(10);
    
    //第四步：函数vTaskList()的使用：获取所有任务的信息
    printf("/************第三步：函数vTaskList()的使用*********/\r\n");
    vTaskList(InfoBuffer);          //获取所有任务的信息
    printf("%s\r\n",InfoBuffer);    //通过串口打印所有任务的信息
    
    
    while(1)
    {
        LED1=~LED1;
        vTaskDelay(1000);           //延时1秒，也就是1000个时钟节拍
    }
}















