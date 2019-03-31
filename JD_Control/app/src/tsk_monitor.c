/*
 * tsk_monitor.c
 *
 *  Created on: 2019年3月27日
 *      Author: pli
 */

#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_ad7190.h"
#include "dev_ad5689.h"
// 放大倍数=R2/R1=2000/6800倍
#define OPA_RES_R1              6800  // 6.8k 运放输入端电阻
#define OPA_RES_R2              2000  // 2k 运放反馈电阻
#define REFERENCE_VOLTAGE       3297  // 参考电压（放大1000倍）
#define BIAS_VOLTAGE_IN1        0xFAB3E  // 输入1偏置电压，即把IN1和GND短接时AD7190转换结果
#define BIAS_VOLTAGE_IN2        0xF9DCA  // 输入2偏置电压，即把IN2和GND短接时AD7190转换结果
#define BIAS_VOLTAGE_IN3        0xFA8A4  // 输入3偏置电压，即把IN3和GND短接时AD7190转换结果
#define BIAS_VOLTAGE_IN4        0xFA9EB  // 输入4偏置电压，即把IN4和GND短接时AD7190转换结果

/* 私有变量 ------------------------------------------------------------------*/
__IO int32_t ad7190_data[4]; // AD7190原始转换结果
__IO int32_t bias_data[4];   // 零点电压的AD转换结果
__IO double voltage_data[4]; // 电压值（单位：mV）
uint16_t cntFlag[4] = {0,0,0,0,};

void GetAdData(void)
{
  uint8_t sample[4];
  int8_t temp,number;

  if(AD7190_RDY_STATE==0)
  {
    HAL_SPI_Receive(&hspi_AD7190,sample,4,0xFF);
    if((sample[3]&0x80)==0)
    {
      temp=(sample[3]&0x07)-4;
      if(temp>=0)
      {
#if ZERO_MODE==1
        bias_data[temp]=((sample[0]<<16)|(sample[1]<<8)|sample[2]);
#else
        ad7190_data[temp]=((sample[0]<<16) | (sample[1]<<8) | sample[2])-bias_data[temp];
#endif
        number=temp;
        voltage_data[number]=ad7190_data[number]>>4;
        voltage_data[number]=voltage_data[number]*REFERENCE_VOLTAGE/OPA_RES_R2*OPA_RES_R1/0xFFFFF;
        cntFlag[number] = cntFlag[number]+1;
      }
      else
      {
        printf("error:0x%X\n",sample[3]);
      }
    }
  }
}


void StartADCMonitor(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = 1;
	osEvent event;
	uint32_t cnt = 0;
	TSK_MSG localMsg;
	const uint8_t taskID = TSK_ID_ADC_MONITOR;

	InitTaskMsg(&localMsg);
	TracePrint(taskID,"started  \n");
	bias_data[0]=BIAS_VOLTAGE_IN1;
	bias_data[1]=BIAS_VOLTAGE_IN2;
	bias_data[2]=BIAS_VOLTAGE_IN3;
	bias_data[3]=BIAS_VOLTAGE_IN4;
	Adc_Setup();
	ad7190_unipolar_multichannel_conf();
	while (TASK_LOOP_ST)
	{
		event = osMessageGet(ADC_MONITOR, tickOut);
		if (event.status == osEventMessage)
		{
			tickOut = event.value.v;
			if(tickOut < 2)
				tickOut = 2;
		}

		GetAdData();

		//uint32_t weight_Zero_Data = weight_ad7190_ReadAvg(6);
		digitInput = GetInputPins();
		cnt++;
		if(cnt % 1000 == 0)
		{
			TraceUser("Pos, %d, Out,0x%x,input,0x%x,IN_0. 0x%05X->%0.3fV,%d,IN_1. 0x%05X->%0.3fV,%d,IN_2. 0x%05X->%0.3fV,%d,IN_3. 0x%05X->%0.3fV,%d,\n",
					motorPos_Read,digitOutput,digitInput,
					ad7190_data[0],voltage_data[0]/1000,cntFlag[0],\
					ad7190_data[1],voltage_data[1]/1000,cntFlag[1],\
					ad7190_data[2],voltage_data[2]/1000,cntFlag[2],\
					ad7190_data[3],voltage_data[3]/1000,cntFlag[3]);
		}

	}

}
