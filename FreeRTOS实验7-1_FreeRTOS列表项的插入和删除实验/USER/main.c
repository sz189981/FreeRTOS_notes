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
ѧϰʹ�� FreeRTOS �б���б�����Ӧ�Ĳ���������ʹ�ã��۲���Щ�������������н��
���������۷������Ƿ�һ�¡�
2��ʵ�����
��ʵ����� 3 ������ start_task�� task1_task �� list_task����������������������£�
start_task�������������� 2 ������
task1_task��Ӧ������ 1������ LED0 ��˸��������ʾϵͳ�������С�
task2_task: �б���б���������񣬵����б���б�����ص� API ����������ͨ������
�����Ӧ����Ϣ���۲���Щ API ���������й��̡�
ʵ����Ҫ�õ� KEY_UP ���������ڿ�����������С�
*/
#define START_TASK_PRIO 						1 //�������ȼ�
#define START_STK_SIZE 						256	//�����ջ��С
TaskHandle_t StartTask_Handler; 			//������
void start_task(void *pvParameters); 	//������

#define TASK1_TASK_PRIO                         2
#define TASK1_STK_SIZE                        128
TaskHandle_t Task1Task_Handler;
void task1_task(void * pvParameters);


#define LIST_TASK_PRIO                         3
#define LIST_STK_SIZE                        128
TaskHandle_t ListTask_Handler;
void list_task(void * pvParameters);

//��  �б���б���Ķ���
//����һ�������õ��б��3���б���
List_t TestList;									//�������б�
ListItem_t 	ListItem1;						//�������б���1
ListItem_t	ListItem2;						//�������б���2
ListItem_t	ListItem3;						//�������б���3



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
    LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 7-1");
    LCD_ShowString(30,50,200,16,16,"list and listItem");
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
    xTaskCreate((TaskFunction_t )list_task,
                (const char*    )"list_task",
                (uint16_t       )LIST_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LIST_TASK_PRIO,
                (TaskHandle_t*  )&ListTask_Handler);
                vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL(); //�˳��ٽ���
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
        //printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
        LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]);     //�������
        LCD_ShowxNum(86,111,task1_num,3,16,0x80);               //��ʾ����ִ�д���
        vTaskDelay(1000);                                       //��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}

//list������
void list_task(void * pvParameters)
{
    //��һ������ʼ���б���б���
    vListInitialise(&TestList);
    vListInitialiseItem(&ListItem1);
    vListInitialiseItem(&ListItem2);
    vListInitialiseItem(&ListItem3);

    ListItem1.xItemValue = 40;	//�б���ֵΪ40
    ListItem2.xItemValue = 60;
    ListItem3.xItemValue = 50;

    //�ڶ�������ӡ�б�������б���ĵ�ַ
    printf("/***********�б���б���ĵ�ַ***********/\r\n");
    printf("��Ŀ                             ��ַ    \r\n");
    printf("TestList                         %#x     \r\n",(int)&TestList);
    printf("TestList->pxIndex                %#x     \r\n",(int)TestList.pxIndex);
    printf("TestList->xListEnd               %#x     \r\n",(int)(&TestList.xListEnd));
    printf("ListItem1                        %#x     \r\n",(int)&ListItem1);
    printf("ListItem2                        %#x     \r\n",(int)&ListItem2);
    printf("ListItem3                        %#x     \r\n",(int)&ListItem3);
    printf("/*****************����******************/\r\n");

    printf("����KEY_UP������!\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)	//�ȴ�KEY_UP������
        delay_ms(10);

    //�����������б�TestList����б���ListItem1,��ͨ�����ڴ�ӡ����
    //�б����г�Ա����pxNext��pxPrevious��ֵ��ͨ��������ֵ�۲��б�
    //�����б��е������
    vListInsert(&TestList, &ListItem1);	//�����б���1
    printf("/***********����б���ListItem1**********/\r\n");
    printf("��Ŀ                              ��ַ    \r\n");
    printf("TestList->xListEnd->pxNext        %#x     \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x     \r\n",(int)(ListItem1.pxNext));
    printf("/***********ǰ�������ӷָ���**************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x     \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x     \r\n",(int)(ListItem1.pxPrevious));
    printf("/*****************����*******************/\r\n");

    printf("����KEY_UP������!\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);

    //���Ĳ�:���б�TestList����б���ListItem2����ͨ�����ڴ�ӡ����
    //�б����г�Ա����pxNext��pxPrevious��ֵ��ͨ��������ֵ�۲��б�
    //�����б��е��������
    vListInsert(&TestList, &ListItem2);				//�����б���2
    printf("/***********����б���ListItem2***********/\r\n");
    printf("��Ŀ                              ��ַ     \r\n");
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem2->pxNext                 %#x      \r\n",(int)(ListItem2.pxNext));
    printf("/***********ǰ�������ӷָ���***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious             %#x      \r\n",(int)(ListItem2.pxPrevious));
    printf("/***********����***************************\r\n");

    printf("����KEY_UP��������\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);

    //���岽�����б�TestLsit����б���ListItem3����ͨ�����ڴ�ӡ����
    //�б����г�Ա����pxNext��pxPrevious��ֵ��ͨ��������ֵ�۲��б�
    //�����б��е����������
    vListInsert(&TestList, &ListItem3);
    printf("/***********����б���ListItem3***********/\r\n");
    printf("��Ŀ                              ��ַ     \r\n");
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem2->pxNext                 %#x      \r\n",(int)(ListItem2.pxNext));
    printf("ListItem3->pxNext                 %#x      \r\n",(int)(ListItem3.pxNext));
    printf("/***********ǰ�������ӷָ���***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious             %#x      \r\n",(int)(ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious             %#x      \r\n",(int)(ListItem3.pxPrevious));
    printf("/***********����***************************\r\n");

    printf("����KEY_UP��������\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);
        
    //��������ɾ��ListItem2,��ͨ�����ڴ�ӡ�����б����г�Ա����pxNext��
    //pxPrevious��ֵ��ͨ��������ֵ�۲��б������б��е����������
    uxListRemove(&ListItem2);			//ɾ��ListItem2
    printf("/***********ɾ���б���ListItem2***********/\r\n");
    printf("��Ŀ                              ��ַ     \r\n");
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem3->pxNext                 %#x      \r\n",(int)(ListItem3.pxNext));
    printf("/***********ǰ�������ӷָ���***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious             %#x      \r\n",(int)(ListItem3.pxPrevious));
    printf("/***********����***************************\r\n");

    printf("����KEY_UP��������\r\n\r\n\r\n");
    while(KEY_Scan(0) != WKUP_PRES)
        delay_ms(10);

    //���߲�������ListItem2����ͨ�����ڴ�ӡ�����б����г�Ա����pxNext��
    //pxPrevious��ֵ��ͨ��������ֵ�۲��б������б��е����������
    TestList.pxIndex=TestList.pxIndex->pxNext;		//pxIndex�����һ��
                                                                                                //����pxIndex�ͻ�ָ��ListItem1
    vListInsertEnd(&TestList, &ListItem2);				//�б�ĩβ����б���ListItem2
    printf("/***********��ĩβ����б���ListItem2******/\r\n");
    printf("��Ŀ                              ��ַ     \r\n");
    printf("TestList->pxIndex                 %#x      \r\n",(int)(TestList.pxIndex));
    printf("TestList->xListEnd->pxNext        %#x      \r\n",(int)(TestList.xListEnd.pxNext));
    printf("ListItem2->pxNext                 %#x      \r\n",(int)(ListItem2.pxNext));
    printf("ListItem1->pxNext                 %#x      \r\n",(int)(ListItem1.pxNext));
    printf("ListItem3->pxNext                 %#x      \r\n",(int)(ListItem3.pxNext));
    printf("/***********ǰ�������ӷָ���***************/\r\n");
    printf("TestList->xListEnd->pxPrevious    %#x      \r\n",(int)(TestList.xListEnd.pxPrevious));
    printf("ListItem2->pxPrevious             %#x      \r\n",(int)(ListItem2.pxPrevious));
    printf("ListItem1->pxPrevious             %#x      \r\n",(int)(ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious             %#x      \r\n",(int)(ListItem3.pxPrevious));
    printf("/***********����***************************\r\n");

    while(1)
    {
        LED1=!LED1;
        vTaskDelay(1000);
    }

}







