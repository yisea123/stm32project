#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "tftlcd.h"
#include "sdram.h"
#include "key.h"
#include "pcf8574.h"
#include "touch.h"
#include "string.h"
#include "malloc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "limits.h"
#include "GUI.h"
#include "WM.h"
#include "GUIDEMO.h"
#include "assert.h"

HeapRegion_t xHeapRegions[] =
{
 	{ ( uint8_t * ) 0x10000000UL, 0x10000 }, //<< Defines a block of 64K bytes starting at address of 0x10000000UL --CCR
//	{ ( uint8_t * ) 0x20000000UL, 0x018000 },     // 96KiB from RAM (192KiB)
	{ ( uint8_t * ) 0XC0600000UL, 2000 *1024 },
	{ NULL, 0 }                //<< Terminates the array.
 };

/************************************************
 ALIENTEK 阿波罗STM32F429开发板 FreeRTOS实验21-1
 FreeRTOS+EMWIN移植-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao0.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO			1
//任务堆栈大小	
#define START_STK_SIZE 			256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO			2
//任务堆栈大小
#define TOUCH_STK_SIZE			128
//任务句柄
TaskHandle_t TouchTask_Handler;
//touch任务
void touch_task(void *pvParameters);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO 			3
//任务堆栈大小
#define LED0_STK_SIZE			128
//任务句柄
TaskHandle_t Led0Task_Handler;
TaskHandle_t ctrlTask_Handler;

//led0任务
void led0_task(void *pvParameters);
void MainTask1(void *pvParameters);
//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO		4
//任务堆栈大小
#define EMWINDEMO_STK_SIZE		512
//任务句柄
TaskHandle_t EmwindemoTask_Handler;

void memoryTest()
{
	uint32_t val = 0xC0000000;
	uint32_t* ptrVal = &val;
	for(uint32_t adr = 0xC0000000; adr<0xC2000000; adr+=4)
	{
		ptrVal = (uint32_t*) adr;
		*ptrVal = val;
		if(*ptrVal != val)
		{
			assert(0);
		}
		val+=4;
	}
}

//emwindemo_task任务

void MainTask12(void *pvParameters);
void emwindemo_task(void *pvParameters);
void ctrlTask(void *pvParameters);
void Tsk(void*p);
int main(void)
{
	HAL_Init();                     //初始化HAL库

#if 1
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //SDRAM初始化
    __HAL_RCC_CRC_CLK_ENABLE();		//使能CRC时钟
				        //触摸屏初始化
#if 1
    Init_RTC();
    TFTLCD_Init();  		        //LCD初始化
    TP_Init();
    W25QXX_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_DMA_Init();
    PCF8574_Init();
    W25QXX_Init();
    PCF8574_WriteBit(6,0);
//    vPortDefineHeapRegions( xHeapRegions ); // << Pass the array into vPortDefineHeapRegions().
 //   memoryTest();
    my_mem_init(SRAMEX);		    //初始化内部内存池
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  					//STemWin初始化
	GUI_UC_SetEncodeUTF8();

	WM_MULTIBUF_Enable(1);  		//开启STemWin多缓冲,RGB屏可能会用到
//	my_mem_init(SRAMEX);		    //初始化外部内存池
//	my_mem_init(SRAMCCM);		    //初始化CCM内存池

	xTaskCreate((TaskFunction_t )ctrlTask,
					(const char*    )"ctrl_task",
					(uint16_t       )400,
					(void*          )NULL,
					(UBaseType_t    )1,
					(TaskHandle_t*  )&ctrlTask_Handler);
    //创建触摸任务
#if 1
	xTaskCreate((TaskFunction_t )touch_task,
				(const char*    )"touch_task",
				(uint16_t       )TOUCH_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )TOUCH_TASK_PRIO,
				(TaskHandle_t*  )&TouchTask_Handler);


	//创建LED0任务
	xTaskCreate((TaskFunction_t )led0_task,
				(const char*    )"led0_task",
				(uint16_t       )LED0_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )LED0_TASK_PRIO,
				(TaskHandle_t*  )&Led0Task_Handler);

#if 1
    xTaskCreate((TaskFunction_t )Tsk,//MainTask12,//tsk,
                (const char*    )"emwindemo_task",
                (uint16_t       )EMWINDEMO_STK_SIZE*5,
                (void*          )NULL,
                (UBaseType_t    )EMWINDEMO_TASK_PRIO,
                (TaskHandle_t*  )&EmwindemoTask_Handler);
#endif
#endif
//	MainTask1(NULL);
    osKernelStart();
#endif
#endif
}
#if 0
//开始任务任务函数
void start_task(void *pvParameters)
{
	__HAL_RCC_CRC_CLK_ENABLE();		//使能CRC时钟
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  					//STemWin初始化
	WM_MULTIBUF_Enable(1);  		//开启STemWin多缓冲,RGB屏可能会用到
    taskENTER_CRITICAL();           //进入临界区

    //创建EMWIN Demo任务

    xTaskCreate((TaskFunction_t )emwindemo_task,             
                (const char*    )"emwindemo_task",           
                (uint16_t       )EMWINDEMO_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EMWINDEMO_TASK_PRIO,        
                (TaskHandle_t*  )&EmwindemoTask_Handler);

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
#endif
//EMWINDEMO任务
void emwindemo_task(void *pvParameters)
{
//	GUI_CURSOR_Show();
//
	//Tsk();

	  while (1) {
		  GUI_Exec();
		  osDelay(20);

	  }
}

//触摸任务的任务函数
void touch_task(void *pvParameters)
{
	while(1)
	{
		GUI_TOUCH_Exec();
		vTaskDelay(5);		//延时5ms
	}
}

//LED0任务
void led0_task(void *p_arg)
{
	while(1)
	{
		LED0 = !LED0;
		vTaskDelay(500);		//延时500ms
		//AddData(0,0);
	}
}

void ctrlTask(void* p_arg)
{
	(void)p_arg;
	uint16_t state = 1;
	MX_USART3_UART_Init();
	MX_USART2_UART_Init();
	while(1)
	{
		if(state != 0)
			UpdateTH();
		ReadRelay();
		osDelay(100);
		WriteRelay();
		//osDelay(100);

	}
}

uint16_t				systemReset = 0;

void ResetDevice(uint16_t type)
{
	extern uint16_t			systemReset;
	if(type == 0)
	{
		if(systemReset != 0)
		{
			NVIC_SystemReset();
		}
	}
	else
	{
		NVIC_SystemReset();
	}
}


uint16_t	autoReset = 1;
void AssertReset(void)
{
	if(autoReset)
	{
		ResetDevice(1);
	}
}


void vApplicationMallocFailedHook( void )
{

}

