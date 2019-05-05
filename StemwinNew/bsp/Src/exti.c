#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//外部中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/5
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define EVENTBIT_0 (1<<0)

//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();               //开启GPIOH时钟
    
    GPIO_Initure.Pin=GPIO_PIN_3;               	//PH3
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //中断线3-PH3
    HAL_NVIC_SetPriority(EXTI3_IRQn,6,0);       //抢占优先级为6，子优先级为0
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线3
}

//任务句柄
extern TaskHandle_t EventGroupTask_Handler;

//中断服务函数
void EXTI3_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	
	//delay_xms(50);						//消抖
	if(KEY0==0)
	{
	//	xTaskNotifyFromISR((TaskHandle_t	)EventGroupTask_Handler, 	//任务句柄
	//					   (uint32_t		)EVENTBIT_0, 				//要更新的bit
	//					   (eNotifyAction	)eSetBits, 					//更新指定的bit
	//					   (BaseType_t*		)xHigherPriorityTaskWoken);

	//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		
	}
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);	//清除中断标志位
}
