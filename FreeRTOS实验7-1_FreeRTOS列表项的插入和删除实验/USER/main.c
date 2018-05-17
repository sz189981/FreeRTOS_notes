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
学习使用 FreeRTOS 列表和列表项相应的操作函数的使用，观察这些操作函数的运行结果
和我们理论分析的是否一致。
2、实验设计
本实验设计 3 个任务： start_task、 task1_task 和 list_task，这三个任务的任务功能如下：
start_task：用来创建其他 2 个任务。
task1_task：应用任务 1，控制 LED0 闪烁，用来提示系统正在运行。
task2_task: 列表和列表项操作任务，调用列表和列表项相关的 API 函数，并且通过串口
输出相应的信息来观察这些 API 函数的运行过程。
实验需要用到 KEY_UP 按键，用于控制任务的运行。
*/
#define START_TASK_PRIO 						1 //任务优先级
#define START_STK_SIZE 						256	//任务堆栈大小
TaskHandle_t StartTask_Handler; 			//任务句柄
void start_task(void *pvParameters); 	//任务函数

#define TASK1_TASK_PRIO                         2
#define TASK1_STK_SIZE                        128
TaskHandle_t Task1Task_Handler;
void task1_task(void * pvParameters);


#define LIST_TASK_PRIO                         3
#define LIST_STK_SIZE                        128
TaskHandle_t ListTask_Handler;
void list_task(void * pvParameters);

//●  列表和列表项的定义
//定义一个测试用的列表和3个列表项
List_t TestList;									//测试用列表
ListItem_t 	ListItem1;						//测试用列表项1
ListItem_t	ListItem2;						//测试用列表项2
ListItem_t	ListItem3;						//测试用列表项3



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
    LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 7-1");
    LCD_ShowString(30,50,200,16,16,"list and listItem");
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
    xTaskCreate((TaskFunction_t )list_task,
                (const char*    )"list_task",
                (uint16_t       )LIST_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LIST_TASK_PRIO,
                (TaskHandle_t*  )&ListTask_Handler);
                vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL(); //退出临界区
}

//task1任务函数
void task1_task(void * pvParameters)
{
    u8 task1_num=0;

    POINT_COLOR = BLACK;
    
    LCD_DrawRectangle(5,110,115,314);    //画一个矩形
    LCD_DrawLine(5, 130, 115, 130);         //画线
    POINT_COLOR =  BLUE;
    LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
    while(1)
    {
        task1_num++;                          //任务执1行次数加1 注意task1_num1加到255的时候会清零！！
        LED0=!LED0;
        //printf("任务1已经执行：%d次\r\n",task1_num);
        LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]);     //填充区域
        LCD_ShowxNum(86,111,task1_num,3,16,0x80);               //显示任务执行次数
        vTaskDelay(1000);                                       //延时1s，也就是1000个时钟节拍
    }
}

//list任务函数
void list_task(void * pvParameters)
{
    //第一步：初始化列表和列表项
    vListInitialise(&TestList);
    vListInitialiseItem(&ListItem1);
    vListInitialiseItem(&ListItem2);
    vListInitialiseItem(&ListItem3);

    ListItem1.xItemValue = 40;	//列表项值为40
    ListItem2.xItemValue = 60;
    ListItem3.xItemValue = 50;

    //第二步：打印列表和其他列表项的地址
    printf("/***********列表和列表项的地址***********/\r\n");
    printf("项目                             地址    \r\n");
    printf("TestList                         %#x     \r\n",(int)&TestList);
    printf("TestList->pxIndex                %#x     \r\n",(int)TestList.pxIndex);
    printf("TestList->xListEnd               %#x     \r\n",(int)(&TestList.xListEnd));
    printf("ListItem1                        %#x     \r\n",(int)&ListItem1);
    printf("ListItem2                        %#x     \r\n",(int)&ListItem2);
    printf("ListItem3                        %#x     \r\n",(int)&ListItem3);
    printf("/*****************结束******************/\r\n");

    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)	//等待KEY_UP键按下
        delay_ms(10);

    //第三步：向列表TestList添加列表项ListItem1,并通过串口打印所有
    //列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
    //项在列表中的情况。
    vListInsert(&TestList, &ListItem1);	//插入列表项1
    printf("/***********添加列表项ListItem1**********/\r\n");
    printf("项目                              地址    \r\n");
    printf("TestList->xListEnd->pxNext        %#x     \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x     \r\n",(int)(ListItem1.pxNext));
    printf("/***********前后向连接分割线**************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x     \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x     \r\n",(int)(ListItem1.pxPrevious));
    printf("/*****************结束*******************/\r\n");

    printf("按下KEY_UP键继续!\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);

    //第四步:向列表TestList添加列表项ListItem2，并通过串口打印所有
    //列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
    //项在列表中的连接情况
    vListInsert(&TestList, &ListItem2);				//插入列表项2
    printf("/***********添加列表项ListItem2***********/\r\n");
    printf("项目                              地址     \r\n");
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem2->pxNext                 %#x      \r\n",(int)(ListItem2.pxNext));
    printf("/***********前后向连接分割线***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious             %#x      \r\n",(int)(ListItem2.pxPrevious));
    printf("/***********结束***************************\r\n");

    printf("按下KEY_UP键继续！\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);

    //第五步：向列表TestLsit添加列表项ListItem3，并通过串口打印所有
    //列表项中成员变量pxNext和pxPrevious的值，通过这两个值观察列表
    //项在列表中的连接情况。
    vListInsert(&TestList, &ListItem3);
    printf("/***********添加列表项ListItem3***********/\r\n");
    printf("项目                              地址     \r\n");
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem2->pxNext                 %#x      \r\n",(int)(ListItem2.pxNext));
    printf("ListItem3->pxNext                 %#x      \r\n",(int)(ListItem3.pxNext));
    printf("/***********前后向连接分割线***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious             %#x      \r\n",(int)(ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious             %#x      \r\n",(int)(ListItem3.pxPrevious));
    printf("/***********结束***************************\r\n");

    printf("按下KEY_UP键继续！\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);
        
    //第六步：删除ListItem2,并通过串口打印所有列表项中成员变量pxNext和
    //pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
    uxListRemove(&ListItem2);			//删除ListItem2
    printf("/***********删除列表项ListItem2***********/\r\n");
    printf("项目                              地址     \r\n");
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem3->pxNext                 %#x      \r\n",(int)(ListItem3.pxNext));
    printf("/***********前后向连接分割线***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious             %#x      \r\n",(int)(ListItem3.pxPrevious));
    printf("/***********结束***************************\r\n");

    printf("按下KEY_UP键继续！\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);

    //第七步：增加ListItem2，并通过串口打印所有列表项中成员变量pxNext和
    //pxPrevious的值，通过这两个值观察列表项在列表中的连接情况。
    TestList.pxIndex=TestList.pxIndex->pxNext;		//pxIndex向后移一项
                                                                                                //这样pxIndex就会指向ListItem1
    vListInsertEnd(&TestList, &ListItem2);				//列表末尾添加列表项ListItem2
    printf("/***********在末尾添加列表项ListItem2******/\r\n");
    printf("项目                              地址     \r\n");
    printf("TestList->pxIndex                 %#x      \r\n",(int)(TestList.pxIndex));
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem2->pxNext                 %#x      \r\n",(int)(ListItem2.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem3->pxNext                 %#x      \r\n",(int)(ListItem3.pxNext));
    printf("/***********前后向连接分割线***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem2->pxPrevious             %#x      \r\n",(int)(ListItem2.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious             %#x      \r\n",(int)(ListItem3.pxPrevious));
    printf("/***********结束***************************\r\n");

    while(1)
    {
        LED1=!LED1;
        vTaskDelay(1000);
    }

}







