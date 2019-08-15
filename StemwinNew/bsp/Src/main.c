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
#include "rtc.h"
#include "bsp.h"
#include "w25qxx.h"
#include "main.h"
HeapRegion_t xHeapRegions[] =
{
 	{ ( uint8_t * ) 0x10000000UL, 0x10000 }, //<< Defines a block of 64K bytes starting at address of 0x10000000UL --CCR
//	{ ( uint8_t * ) 0x20000000UL, 0x018000 },     // 96KiB from RAM (192KiB)
	{ ( uint8_t * ) 0XC0600000UL, 2000 *1024 },
	{ NULL, 0 }                //<< Terminates the array.
 };
typedef struct {
	osMessageQId* ptrQid;
	uint16_t size;
}QueIDInit;

osMessageQId MB_MAINSTEP			= NULL;

static const QueIDInit QID[]=
{
	{&MB_MAINSTEP,				4},
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
void SaveDataTsk(void* p_arg);
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

TaskHandle_t flashTask_Handler;


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

static void CreateAllQid(void)
{
	uint16_t qNum = sizeof(QID)/sizeof(QueIDInit);
	for(uint16_t idx = 0; idx<qNum; idx++)
	{
		osMessageQDef(TSK_Queue, QID[idx].size, uint32_t);
		*(QID[idx].ptrQid) = osMessageCreate(osMessageQ(TSK_Queue), NULL);
	}
}

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
    CreateAllQid();
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

	BUTTON_SetDefaultSkinClassic();
	WIDGET_SetDefaultEffect(&WIDGET_Effect_3D);	//Enable 3D

	WM_MULTIBUF_Enable(1);  		//开启STemWin多缓冲,RGB屏可能会用到
//	my_mem_init(SRAMEX);		    //初始化外部内存池
//	my_mem_init(SRAMCCM);		    //初始化CCM内存池
#if 1
	xTaskCreate((TaskFunction_t )ctrlTask,
					(const char*    )"ctrl_task",
					(uint16_t       )400,
					(void*          )NULL,
					(UBaseType_t    )osPriorityAboveNormal,
					(TaskHandle_t*  )&ctrlTask_Handler);
#endif
    //创建触摸任务
#if 1
	xTaskCreate((TaskFunction_t )touch_task,
				(const char*    )"touch_task",
				(uint16_t       )TOUCH_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )osPriorityHigh,
				(TaskHandle_t*  )&TouchTask_Handler);

#if 1
	//创建LED0任务
	xTaskCreate((TaskFunction_t )led0_task,
				(const char*    )"led0_task",
				(uint16_t       )LED0_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )osPriorityNormal,
				(TaskHandle_t*  )&Led0Task_Handler);
#endif

#if 1
    xTaskCreate((TaskFunction_t )Tsk,//MainTask12,//tsk,
                (const char*    )"emwindemo_task",
                (uint16_t       )EMWINDEMO_STK_SIZE*10,
                (void*          )NULL,
                (UBaseType_t    )osPriorityNormal,
                (TaskHandle_t*  )&EmwindemoTask_Handler);
#if 1

    xTaskCreate((TaskFunction_t )SaveDataTsk,//MainTask12,//tsk,
                (const char*    )"flash_task",
                (uint16_t       )EMWINDEMO_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )osPriorityBelowNormal,
                (TaskHandle_t*  )&flashTask_Handler);
#endif

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
uint16_t uiStarted = 0;
//触摸任务的任务函数
void touch_task(void *pvParameters)
{
	while(1)
	{
		//if(uiStarted == 1)
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
		osDelay(500);		//延时500ms
		//AddData(0,0);
	}
}

enum
{
	e_read_th,
	e_read_relay,
	e_write_relay,
	e_read_relay1,
	e_write_relay1,
	e_read_relay2,
	e_write_relay2,
	e_cmd_max,
};

extern uint16_t relayInput[4];
extern uint16_t relayOutput[4];
extern int16_t tempTh[2];

static uint16_t ChkInput(void)
{
	uint16_t ret = 1;
	if((relayInput[0] == 1) && (relayInput[1] == 0))
	{
		ret = 0;
	}
	return ret;
}

void SetIOState(uint16_t state);

void ctrlTask(void* p_arg)
{
#define TICK_EACH_TIME		100
	(void)p_arg;
	uint16_t state = 1;
	MX_USART3_UART_Init();
	MX_USART2_UART_Init();
	uint32_t tickOut = TICK_EACH_TIME;
	uint32_t startTick = 0;
	uint32_t newTick;
	uint16_t IOState = 0;
	osEvent event;
	uint32_t controlLimit[2] = {0,0};
	uint32_t timeExecReq[2] = {0,0};
	for(uint16_t index = 0; index < 3;index++)
	{
		UpdateTH();
		osDelay(40);
	}
	DISPLAY_DATA_DHT11();
	while(1)
	{
		event = osMessageGet(MB_MAINSTEP, tickOut );
		if( event.status != osEventMessage )
		{
			tickOut = TICK_EACH_TIME;
			switch(state)
			{
			case e_read_th:
				UpdateTH();
				tickOut = 0;
				break;
			case e_read_relay:
			case e_read_relay1:
			case e_read_relay2:
				ReadRelay();
				break;
			case e_write_relay:
			case e_write_relay1:
			case e_write_relay2:
				WriteRelay();
				break;
			}
			state = (state+1)%e_cmd_max;
			if(state == 4)
			{
				DISPLAY_DATA_DHT11();
			}
		}
		else//message
		{
			if(event.value.v == IO_STATE_ON)
			{
				IOState = 1;
			}
			else if(event.value.v == IO_STATE_OFF)
			{
				IOState = 0;
			}
		}
		controlLimit[0] = timeExecReq[0] = prvReadBackupRegister(INTERVAL_SET);
		controlLimit[1] = timeExecReq[1] = prvReadBackupRegister(TIME_EXEC);
		newTick = HAL_GetTick();
#if 0
		if( prvReadBackupRegister(MANUAL_STATE) != 0)
		{
			//manual
			uint32_t tick = GetTickDeviation(startTick, newTick);
			if(tick >= timeExecReq[IOState]*1000)
			{
				//finish?
				startTick = newTick;
				if(IOState)	IOState = 0;
				else IOState=1;
			}
			else
			{
				uint32_t tickOut1 = timeExecReq[IOState]*1000 - tick;
				if(tickOut > tickOut1)
				{
					tickOut = tickOut1;
				}
			}
		}
		else
		{
			//auto
			if()
			if(ChkInput() == 1)
			{
				IOState = 1;
			}

		}
#endif
		//humidity <= lowlimit
		if( prvReadBackupRegister(MANUAL_STATE) == 0)
		{
			if(controlLimit[0]*10 >= tempTh[1])
			{
				IOState = 0;
			}
			else
			{
				IOState = 1;
				if(ChkInput() == 0)
				{
					IOState = 0;
				}
			}

		}
		if(controlLimit[1]*10 <= tempTh[1])
		{
			SetWarningState(1);
		}
		else
		{
			SetWarningState(0);
		}
		if(IOState != 0)
		{
			relayOutput[0] = relayOutput[1] = 0;
			relayOutput[2] = relayOutput[3] = 1;
			SetIOState(IOState);
		}
		else
		{
			relayOutput[0] = relayOutput[1] = 1;
			relayOutput[2] = relayOutput[3] = 0;
			SetIOState(IOState);
		}

	}
}



uint16_t currentUseFlashID = 0;
uint32_t currentFlashAdr = 0;
static const flashWriteAdr[STORE_FLASH_MAX] =
{
		0x0000,
		STORE_SIZE_MAX,
		STORE_SIZE_MAX*2,
		STORE_SIZE_MAX*3,
		STORE_SIZE_MAX*4,

};
static uint16_t SaveFlashRTC()
{
	uint16_t stIdx = 0;
	uint32_t val[STORE_FLASH_MAX];
	for(uint16_t i = 0; i < STORE_FLASH_MAX;i++)
	{
		val[i] = prvReadBackupRegister(ADR_FLASHSTART1+i);
		if(ValidChkT32(&val[i], 0) != 0)
		{
			val[i] = 0;
			prvWriteBackupRegister(ADR_FLASHSTART1+i, val[i]);
		}
	}

	for(uint16_t i = 0; i< STORE_FLASH_MAX;i++)
	{
		if(val[stIdx] > val[i])
			stIdx = i;
	}
	val[stIdx] = GetCurrentST();
	prvWriteBackupRegister(ADR_FLASHSTART1+stIdx, val[stIdx]);
	currentUseFlashID = stIdx;
	currentFlashAdr = flashWriteAdr[stIdx];

	for(uint32_t i = 0; i< STORE_SECTOR_SIZE;i++)
	{
		uint32_t secSize = 4096;
		W25QXX_Erase_Sector(currentFlashAdr/secSize+i);
	}
	return stIdx;
}
static uint16_t historyIdx[STORE_FLASH_MAX];
static uint32_t historyTime[STORE_FLASH_MAX] = {0,0,0,0,0};
static uint16_t historyCnt = 0;

static uint32_t loadStartAdr = 0x00;
static uint32_t loadAdr = 0x0;
static uint32_t loadCnt;

static uint16_t tmpHumdata[READBACK_DATA_SIZE*2];

uint16_t loadFromFlash(int idx, uint16_t dir, int* ptrSizeId)
{
	//
	uint32_t readSize = READBACK_SIZE;
	uint16_t ret = OK;
	static uint16_t oldIdx = 0xFFFF;
	static uint32_t loadTimeST = 0x0;
	static int sizeId = 0;
	if(idx < 0 || idx > 3)
	{
		UpdateDisplay(FATAL_ERROR);
		return OK;
	}
	if(idx != oldIdx)
	{
		LoadFlashSegsRTC();
		dir = DIR_NONE;
		oldIdx = idx;
		if(historyIdx[idx] < STORE_FLASH_MAX)
		{
			loadAdr = loadStartAdr = flashWriteAdr[historyIdx[idx]];
			loadTimeST = historyTime[idx];
			sizeId = 0;
		}
		else
		{
			sizeId = 10000;
			loadStartAdr = 0x0;
			ret = FATAL_ERROR;
		}
	}
	if(loadTimeST == 0)
	{
		ret = FATAL_ERROR;
	}
	//if have valid data
	if(ret == OK)
	{
		readSize = READBACK_SIZE;
		if(dir == DIR_INC)
		{
			sizeId = sizeId + 1;
			loadAdr = loadAdr + READBACK_SIZE*sizeId;
			if(loadAdr >= (loadStartAdr + STORE_SIZE_MAX))
			{
				readSize = 0;
				ret = FATAL_ERROR;
			}
			else
			{
				uint32_t size = loadAdr - loadStartAdr;
				if(size < READBACK_SIZE)
					readSize = size;
			}

		}
		else if(dir == DIR_DEC)
		{
			sizeId = sizeId - 1;
			loadAdr = loadStartAdr + READBACK_SIZE*sizeId;
			if(sizeId  < 0)
			{
				ret = FATAL_ERROR;
			}
			else
			{
				readSize = READBACK_SIZE;
			}

		}
	}

	if(ret != FATAL_ERROR)
	{
		W25QXX_Read((void*)&tmpHumdata[0], loadAdr, readSize);
		int val = UpdateHistNewData(&tmpHumdata[0], readSize/2, &loadTimeST);
		if(val != readSize/4)
		{
			ret = WARNING;
		}
	}
	else
	{
		UpdateDisplay(ret);
	}
	*ptrSizeId = sizeId;
	return ret;

}

uint16_t LoadFlashSegsRTC(void)
{
	uint16_t stIdx = 0;
	uint32_t val;
	static uint32_t historyTimeRTC[STORE_FLASH_MAX] = {0,0,0,0,0};
	historyCnt = 0;
	for(uint16_t i = 0; i < STORE_FLASH_MAX;i++)
	{
		historyTimeRTC[i] = historyTime[i] = 0;
		historyIdx[i] = i;
		if(i != currentUseFlashID)
		{
			val = prvReadBackupRegister(ADR_FLASHSTART1+i);
			if(ValidChkT32(&val, 0) == 0)
			{
				historyCnt++;
				historyTimeRTC[i] = historyTime[i] = val;
			}
		}
	}

	for(uint16_t i = 0; i< STORE_FLASH_MAX;i++)
	{
		for(uint16_t j = i+1; j< STORE_FLASH_MAX;j++)
		{
			if(historyTime[i] < historyTime[j])
			{
				uint32_t tmp = historyTime[i];
				historyTime[i] = historyTime[j];
				historyTime[j] = tmp;
				uint16_t kk = historyIdx[i];
				historyIdx[i] = historyIdx[j];
				historyIdx[j] = kk;
			}
		}
	}
	return historyCnt;
}






#define TMP_STORE_SIZE		16
static int16_t tempStoreData[TMP_STORE_SIZE+4];

static int32_t TickTimeDelay(const uint32_t oldTick, const uint32_t reqTick)
{
	uint32_t newTick = HAL_GetTick();
	newTick = GetTickDeviation(oldTick, newTick);
	if(newTick < reqTick)
		return (int32_t)(reqTick - newTick);
	else
		return 20;
}

extern int16_t tempTh[2];
void SaveDataTsk(void* p_arg)
{
	(void)p_arg;
	uint32_t lastTick = 0;
	uint32_t delayTick = 990;
	uint32_t adr = 0;
	uint16_t tmpId = 0;

	while(1)
	{
		osDelay(delayTick);
		if(lastTick == 0)
		{
			SaveFlashRTC();
			adr = currentFlashAdr;
			tmpId = 0;
		}
		lastTick = HAL_GetTick();

		tempStoreData[tmpId++] = tempTh[0];
		tempStoreData[tmpId++] = tempTh[1];
		if(tmpId >= TMP_STORE_SIZE)
		{
			W25QXX_Write((void*)&tempStoreData[0], adr, TMP_STORE_SIZE*2);
			adr += TMP_STORE_SIZE*2;
			tmpId = 0;
		}
		delayTick = TickTimeDelay(lastTick, 1000);
		if(adr-currentFlashAdr >= STORE_SIZE_MAX)
		{
			lastTick = 0;
			delayTick = 0;
		}




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

