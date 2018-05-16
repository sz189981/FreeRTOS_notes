#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "exti.h"
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
ѧϰʹ�� FreeRTOS ���������ͻָ���� API ���������� vTaskSuspend()�� vTaskResume()
�� xTaskResumeFromISR()��
2��ʵ�����
��ʵ����� 4 ������ start_task�� key_task�� task1_task �� task2_task��
���ĸ���������������£�
	start_task�������������� 3 ������
	key_task	�� �����������񣬼�ⰴ���İ��½�������ݲ�ͬ�İ������ִ�в�ͬ�Ĳ�����
	task1_task��Ӧ������ 1��
	task2_task: Ӧ������ 2��

ʵ����Ҫ�ĸ������� KEY0�� KEY1�� KEY2 �� KEY_UP�����ĸ������Ĺ������£�
	KEY0: �˰���Ϊ�ж�ģʽ�����жϷ������лָ����� 2 �����С�
	KEY1: �˰���Ϊ����ģʽ�����ڻָ����� 1 �����С�
	KEY2: �˰���Ϊ����ģʽ�����ڹ������� 2 �����С�
	KEY_UP: �˰���Ϊ����ģʽ�����ڹ������� 1 �����С�
*/
#define START_TASK_PRIO 						1 //�������ȼ�
#define START_STK_SIZE 						256	//�����ջ��С
TaskHandle_t StartTask_Handler; 			//������
void start_task(void *pvParameters); 	//������

#define KEY_TASK_PRIO														2
#define KEY_STK_SIZE													128
TaskHandle_t KeyTask_Handler;			
void key_task(void * pvParameters);


#define TASK1_TASK_PRIO                         2
#define TASK1_STK_SIZE                        128
TaskHandle_t Task1Task_Handler;
void task1_task(void * pvParameters);


#define TASK2_TASK_PRIO                         3
#define TASK2_STK_SIZE                        128
TaskHandle_t Task2Task_Handler;
void task2_task(void * pvParameters);


//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };
    
//��  main()����
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//����ϵͳ�ж����ȼ����� 4
	delay_init(168); 																//��ʼ����ʱ����
	uart_init(115200); 															//��ʼ������
	LED_Init(); 																		//��ʼ�� LED �˿�
	KEY_Init();							//��ʼ������
	EXTIX_Init();						//��ʼ���ⲿ�ж�
	LCD_Init();							//��ʼ��LCD
	 
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"ATK STM32F103/F407");	
	LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 6-1");
	LCD_ShowString(30,50,200,16,16,"Task Creat and Del");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2016/11/25");
	
	//������ʼ����
	xTaskCreate((TaskFunction_t )start_task, 		//������
							(const char* )	"start_task", 		//��������
							(uint16_t )				START_STK_SIZE, 			//�����ջ��С
							(void* )					NULL, 								//���ݸ��������Ĳ���
							(UBaseType_t )		START_TASK_PRIO, 			//�������ȼ�
							(TaskHandle_t*)		&StartTask_Handler);
	vTaskStartScheduler(); //�����������
}
//�� ������
//��ʼ����������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); //�����ٽ���
  //����KEY����
	xTaskCreate((TaskFunction_t )key_task,
							(const char*		)"key_task",
							(uint16_t				)KEY_STK_SIZE,
							(void *					)NULL,
							(UBaseType_t		)KEY_TASK_PRIO,
							(TaskHandle_t*		)&KeyTask_Handler);
	//����TASK1����
  xTaskCreate((TaskFunction_t )task1_task,
							(const char *   )"task1_task",
							(uint16_t       )TASK1_STK_SIZE,
							(void *         )NULL,
							(UBaseType_t    )TASK1_TASK_PRIO,
							(TaskHandle_t*  )&Task1Task_Handler);
	//����TASK2����
	xTaskCreate((TaskFunction_t )task2_task,
							(const char*    )"task2_task",
							(uint16_t       )TASK2_STK_SIZE,
							(void*          )NULL,
							(UBaseType_t    )TASK2_TASK_PRIO,
							(TaskHandle_t*  )&Task2Task_Handler);
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
	taskEXIT_CRITICAL(); //�˳��ٽ���
}

//key������
void key_task(void * pvParameters)
{
	u8 key;
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case WKUP_PRES:
				vTaskSuspend(Task1Task_Handler);
				printf("��������1������!\r\n");
				break;
			case KEY1_PRES:
				vTaskResume(Task1Task_Handler);
				printf("�ָ�����1������!\r\n");
				break;
			case KEY2_PRES:
				vTaskSuspend(Task2Task_Handler);
				printf("��������2������!\r\n");
				break;
		}
		vTaskDelay(10);			//��ʱ10ms
	}
}

//task1������
void task1_task(void * pvParameters)
{
    u8 task1_num=0;

    POINT_COLOR = BLACK;
    
    LCD_DrawRectangle(5,110,115,314);    //��һ������
    LCD_DrawLine(5, 130, 115, 130);         //����
    POINT_COLOR =  BLUE;
    LCD_ShowString(6,111,110,16,16,"Task1 Run:000");
    while(1)
    {
        task1_num++;                          //����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
        LED0=!LED0;
        printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
        LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]);     //�������
        LCD_ShowxNum(86,111,task1_num,3,16,0x80);               //��ʾ����ִ�д���
        vTaskDelay(1000);                                       //��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}

//task2������
void task2_task(void * pvParameters)
{
    u8 task2_num=0;
    POINT_COLOR = BLACK;
    
    LCD_DrawRectangle(125,110,234,314);     //��һ������
    LCD_DrawLine(125,130,234,130);          //����
    POINT_COLOR = BLUE;
    LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
    while(1)
    {
        task2_num++;                        //����2ִ�д�����1 ע��task1_num2�ӵ�255ʱ�����㣡��
        LED1=!LED1;
        printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
        LCD_ShowxNum(206,111,task2_num,3,16,0x80);      //��ʾ����ִ�д���
        LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]);    //�������
        vTaskDelay(1000);                               //��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}
