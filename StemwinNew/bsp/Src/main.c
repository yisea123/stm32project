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

/************************************************
 ALIENTEK ������STM32F429������ FreeRTOSʵ��21-1
 FreeRTOS+EMWIN��ֲ-HAL��汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao0.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//�������ȼ�
#define START_TASK_PRIO			1
//�����ջ��С	
#define START_STK_SIZE 			256  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO			2
//�����ջ��С
#define TOUCH_STK_SIZE			128
//������
TaskHandle_t TouchTask_Handler;
//touch����
void touch_task(void *pvParameters);
void SaveDataTsk(void* p_arg);
//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO 			3
//�����ջ��С
#define LED0_STK_SIZE			128
//������
TaskHandle_t Led0Task_Handler;
TaskHandle_t ctrlTask_Handler;

//led0����
void led0_task(void *pvParameters);
void MainTask1(void *pvParameters);
//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO		4
//�����ջ��С
#define EMWINDEMO_STK_SIZE		512
//������
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

//emwindemo_task����

void MainTask12(void *pvParameters);
void emwindemo_task(void *pvParameters);
void ctrlTask(void *pvParameters);
void Tsk(void*p);
int main(void)
{
	HAL_Init();                     //��ʼ��HAL��

#if 1
    delay_init(180);                //��ʼ����ʱ����
    uart_init(115200);              //��ʼ��USART
    LED_Init();                     //��ʼ��LED 
    KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //SDRAM��ʼ��
    __HAL_RCC_CRC_CLK_ENABLE();		//ʹ��CRCʱ��
				        //��������ʼ��
#if 1
    Init_RTC();
    TFTLCD_Init();  		        //LCD��ʼ��
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
    my_mem_init(SRAMEX);		    //��ʼ���ڲ��ڴ��
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  					//STemWin��ʼ��
	GUI_UC_SetEncodeUTF8();

	WM_MULTIBUF_Enable(1);  		//����STemWin�໺��,RGB�����ܻ��õ�
//	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
//	my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��

	xTaskCreate((TaskFunction_t )ctrlTask,
					(const char*    )"ctrl_task",
					(uint16_t       )400,
					(void*          )NULL,
					(UBaseType_t    )1,
					(TaskHandle_t*  )&ctrlTask_Handler);
    //������������
#if 1
	xTaskCreate((TaskFunction_t )touch_task,
				(const char*    )"touch_task",
				(uint16_t       )TOUCH_STK_SIZE,
				(void*          )NULL,
				(UBaseType_t    )TOUCH_TASK_PRIO,
				(TaskHandle_t*  )&TouchTask_Handler);


	//����LED0����
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


    xTaskCreate((TaskFunction_t )SaveDataTsk,//MainTask12,//tsk,
                (const char*    )"flash_task",
                (uint16_t       )EMWINDEMO_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )6,
                (TaskHandle_t*  )&flashTask_Handler);

#endif
#endif
//	MainTask1(NULL);
    osKernelStart();
#endif
#endif
}
#if 0
//��ʼ����������
void start_task(void *pvParameters)
{
	__HAL_RCC_CRC_CLK_ENABLE();		//ʹ��CRCʱ��
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  					//STemWin��ʼ��
	WM_MULTIBUF_Enable(1);  		//����STemWin�໺��,RGB�����ܻ��õ�
    taskENTER_CRITICAL();           //�����ٽ���

    //����EMWIN Demo����

    xTaskCreate((TaskFunction_t )emwindemo_task,             
                (const char*    )"emwindemo_task",           
                (uint16_t       )EMWINDEMO_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EMWINDEMO_TASK_PRIO,        
                (TaskHandle_t*  )&EmwindemoTask_Handler);

    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}
#endif
//EMWINDEMO����
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

//���������������
void touch_task(void *pvParameters)
{
	while(1)
	{
		GUI_TOUCH_Exec();
		vTaskDelay(5);		//��ʱ5ms
	}
}

//LED0����
void led0_task(void *p_arg)
{
	while(1)
	{
		LED0 = !LED0;
		vTaskDelay(500);		//��ʱ500ms
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
		extern void DISPLAY_DATA_DHT11();
		DISPLAY_DATA_DHT11();
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

	for(uint16_t i = 1; i< STORE_FLASH_MAX;i++)
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
static uint16_t historyTime[STORE_FLASH_MAX] = {0,0,0,0,0};
static uint16_t historyCnt = 0;

static uint32_t loadStartAdr = 0x00;
static uint32_t loadAdr = 0x0;
static uint32_t loadCnt;

static uint16_t tmpHumdata[READBACK_DATA_SIZE*2];

uint16_t loadFromFlash(uint16_t idx, uint16_t dir)
{
	//
	uint16_t ret = OK;
	static uint16_t oldIdx = 0xFFFF;
	static uint32_t loadTimeST = 0x0;

	if(idx != oldIdx)
	{
		dir = DIR_NONE;
		oldIdx = idx;
		if(historyIdx[idx] < STORE_FLASH_MAX)
		{
			loadAdr = loadStartAdr = flashWriteAdr[historyIdx[idx]];
			loadTimeST = historyTime[historyIdx[idx]];
		}
		else
		{
			loadStartAdr = 0x0;
			ret = FATAL_ERROR;
		}
	}
	//if have valid data
	if(ret == OK)
	{
		if(dir == DIR_INC)
		{
			loadAdr = loadAdr + READBACK_SIZE;
			if( (loadAdr - loadStartAdr) >= STORE_SIZE_MAX)
			{
				ret = FATAL_ERROR;
			}
		}
		else if(dir == DIR_INC)
		{
			if(loadAdr > READBACK_SIZE)
				loadAdr = loadAdr - READBACK_SIZE;
			else
				ret = WARNING;
			if(loadAdr  < loadStartAdr)
			{
				ret = WARNING;
				loadAdr = loadStartAdr;
			}
		}
	}

	if(ret != FATAL_ERROR)
	{
		W25QXX_Read((void*)&tmpHumdata[0], loadAdr, READBACK_SIZE);
		int val = UpdateHistNewData(&tmpHumdata[0], READBACK_DATA_SIZE, loadTimeST);
		if(val > 0)
		{
		//	loadTimeST
		}
	}

	return ret;

}

uint16_t LoadFlashSegsRTC()
{
	uint16_t stIdx = 0;
	uint32_t val;

	for(uint16_t i = 0; i < STORE_FLASH_MAX;i++)
	{
		historyTime[i] = 0;
		historyIdx[i] = -1;
		if(i != currentUseFlashID)
		{
			val = prvReadBackupRegister(ADR_FLASHSTART1+i);
			if(ValidChkT32(&val, 0) == 0)
			{
				historyCnt++;
				historyTime[i] = val;
				historyIdx[i] = i;
			}
		}
	}

	for(uint16_t i = 0; i< STORE_FLASH_MAX;i++)
	{
		for(uint16_t j = 1; j< STORE_FLASH_MAX;j++)
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

