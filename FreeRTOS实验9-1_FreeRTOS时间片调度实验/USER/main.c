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
ALIENTEK ̽���� STM32F407 ������ FreeRTOS ʵ�� 2-1
FreeRTOS ��ֲʵ��-�⺯���汾
����֧�֣� www.openedv.com
�Ա����̣� http://eboard.taobao.com
��ע΢�Ź���ƽ̨΢�źţ� "����ԭ��"����ѻ�ȡ STM32 ���ϡ�
������������ӿƼ����޹�˾
���ߣ�����ԭ�� @ALIENTEK
************************************************/
/*
1��ʵ��Ŀ��
ѧϰʹ�� FreeRTOS ��ʱ��Ƭ���ȡ�

2��ʵ�����
��ʵ������������� start_task�� task1_task �� task2_task �� ���� task1_task �� task2_task
���������ȼ���ͬ����Ϊ 2�� ��������������������£�
start_task�������������� 2 ������
task1_task ������ LED0 ����˸�� ����ͨ�����ڴ�ӡ task1_task �����д�����
task2_task �� ���� LED1 ����˸������ͨ�����ڴ�ӡ task2_task �����д�����
*/
#define START_TASK_PRIO 						1 //�������ȼ�
#define START_STK_SIZE 						256	//�����ջ��С
TaskHandle_t StartTask_Handler; 			//������
void start_task(void *pvParameters); 	//������

#define TASK1_TASK_PRIO                         2
#define TASK1_STK_SIZE                        128
TaskHandle_t Task1Task_Handler;
void task1_task(void * pvParameters);


#define TASK2_TASK_PRIO                         2
#define TASK2_STK_SIZE                        128
TaskHandle_t Task2Task_Handler;
void task2_task(void * pvParameters);

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={  WHITE, BLACK, BLUE,  BRED,
                        GRED,  GBLUE, RED,   MAGENTA,
                        GREEN, CYAN,  YELLOW,BROWN,
                        BRRED, GRAY };
    
//��  main()����
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//����ϵͳ�ж����ȼ����� 4
    delay_init(168);                    //��ʼ����ʱ����
    uart_init(115200);                  //��ʼ������
    LED_Init();                         //��ʼ�� LED �˿�
    KEY_Init();                         //��ʼ������
    LCD_Init();                         //��ʼ��LCD

    POINT_COLOR = RED;
    LCD_ShowString(30,10,200,16,16,"ATK STM32F103/F407");	
    LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 9-1");
    LCD_ShowString(30,50,200,16,16,"FreeRTOS Round Robin");
    LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,90,200,16,16,"2016/11/25");

    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char* )  "start_task",           //��������
                (uint16_t )     START_STK_SIZE,         //�����ջ��С
                (void* )        NULL,                   //���ݸ��������Ĳ���
                (UBaseType_t )  START_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t*) &StartTask_Handler);
    vTaskStartScheduler(); //�����������
}
//�� ������
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); //�����ٽ���

    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,
                (const char *   )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void *         )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //����List����
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
                vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL(); //�˳��ٽ���
}

//task1������
void task1_task(void * pvParameters)
{
    u8 task1_num=0;
    while(1)
    {
        task1_num++;                          //����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
        LED0=!LED0;
        taskENTER_CRITICAL();                   //�����ٽ���
        printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
        taskEXIT_CRITICAL();                    //�˳��ٽ���
        //��ʱ10ms��ģ����������10ms���κ������������������
        delay_xms(10);
    }
}

//task2������
void task2_task(void * pvParameters)
{
    u8 task2_num = 0;
    while(1)
    {
        task2_num++;
        LED1=!LED1;
        taskENTER_CRITICAL();
        printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
        taskEXIT_CRITICAL();
        //��ʱ10ms��ģ����������10ms���κ������������������
        delay_xms(10);
        
    }
}


