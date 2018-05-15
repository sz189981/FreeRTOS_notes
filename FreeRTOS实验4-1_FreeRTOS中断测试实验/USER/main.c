#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
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
#define START_TASK_PRIO 						1 //�������ȼ�
#define START_STK_SIZE 						256	//�����ջ��С
TaskHandle_t StartTask_Handler; 			//������
void start_task(void *pvParameters); 	//������

#define INTERRUPT_TASK_PRIO					2
#define INTERRUPT_STK_SIZE				256
TaskHandle_t INTERRUPTTask_Handler;
void interrupt_task(void * p_arg);

//��  main()����
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//����ϵͳ�ж����ȼ����� 4
	delay_init(168); 																//��ʼ����ʱ����
	uart_init(115200); 															//��ʼ������
	LED_Init(); 																		//��ʼ�� LED �˿�
	TIM3_Int_Init(10000-1, 8400-1);			//��ʼ����ʱ��3������Ϊ1S
	TIM5_Int_Init(10000-1, 8400-1);			//��ʼ����ʱ��5������Ϊ1S
	
	
	//������ʼ����
	xTaskCreate((TaskFunction_t )start_task, 		//������
							(const char* )	"start_task", 		//��������
							(uint16_t )				START_STK_SIZE, 			//�����ջ��С
							(void* )					NULL, 								//���ݸ��������Ĳ���
							(UBaseType_t )		START_TASK_PRIO, 			//�������ȼ�
							(TaskHandle_t* )	&StartTask_Handler); 	//������
	vTaskStartScheduler(); 									//�����������
}
//�� ������
//��ʼ����������
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); //�����ٽ���
	
	//�����жϲ�������
	xTaskCreate((TaskFunction_t )interrupt_task,
							(const char* )	"interrupt_task",
							(uint16_t )			 INTERRUPT_STK_SIZE,
							(void* )				 NULL,
							(UBaseType_t )	 INTERRUPT_TASK_PRIO,
							(TaskHandle_t* ) &INTERRUPTTask_Handler);
	
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
	
	taskEXIT_CRITICAL(); //�˳��ٽ���
}


//�жϲ���������
void interrupt_task(void * pvParameters)
{
	static u32 total_num = 0;
	while(1)
	{
		total_num+=1;
		if(total_num == 5)
		{
			printf("�ر��ж�...........\r\n");
			portDISABLE_INTERRUPTS();
			delay_xms(5000);									//��ʱ5s
			printf("���ж�...........\r\n");
			portENABLE_INTERRUPTS();
		}
		LED0=~LED0;
		LED1=~LED1;
		vTaskDelay(1000);
	}
}




