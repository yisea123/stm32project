/*
 * sdio.c
 *
 *  Created on: 2019��5��15��
 *      Author: pli
 */




#include "sdio.h"
#include "string.h"

SD_HandleTypeDef        SDCARD_Handler;     //SD�����

HAL_SD_CardInfoTypeDef  SDCardInfo;         //SD����Ϣ�ṹ��
DMA_HandleTypeDef SDTxDMAHandler,SDRxDMAHandler;    //SD��DMA���ͺͽ��վ��

//SD_ReadDisk/SD_WriteDisk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
//��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
uint8_t SDIO_DATA_BUFFER[512];

//SD����ʼ��
//����ֵ:0 ��ʼ����ȷ������ֵ����ʼ������
uint8_t SD_Init(void)
{
    uint8_t SD_Error;

    //��ʼ��ʱ��ʱ�Ӳ��ܴ���400KHZ
    SDCARD_Handler.Instance=SDIO;
    SDCARD_Handler.Init.ClockEdge=SDIO_CLOCK_EDGE_RISING;          //������
    SDCARD_Handler.Init.ClockBypass=SDIO_CLOCK_BYPASS_DISABLE;     //��ʹ��bypassģʽ��ֱ����HCLK���з�Ƶ�õ�SDIO_CK
    SDCARD_Handler.Init.ClockPowerSave=SDIO_CLOCK_POWER_SAVE_DISABLE;    //����ʱ���ر�ʱ�ӵ�Դ
    SDCARD_Handler.Init.BusWide=SDIO_BUS_WIDE_1B;                        //1λ������
    SDCARD_Handler.Init.HardwareFlowControl=SDIO_HARDWARE_FLOW_CONTROL_DISABLE;//�ر�Ӳ������
    SDCARD_Handler.Init.ClockDiv=SDIO_TRANSFER_CLK_DIV;            //SD����ʱ��Ƶ�����25MHZ

    SD_Error=HAL_SD_Init(&SDCARD_Handler);
    if(SD_Error!=HAL_OK) return 1;

    SD_Error=HAL_SD_WideBusOperation_Config(&SDCARD_Handler,SDIO_BUS_WIDE_4B);//ʹ�ܿ�����ģʽ
    if(SD_Error!=HAL_OK) return 2;
    return 0;
}

//SDMMC�ײ�������ʱ��ʹ�ܣ��������ã�DMA����
//�˺����ᱻHAL_SD_Init()����
//hsd:SD�����
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    DMA_HandleTypeDef TxDMAHandler,RxDMAHandler;
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_SDIO_CLK_ENABLE();    //ʹ��SDIOʱ��
    __HAL_RCC_DMA2_CLK_ENABLE();    //ʹ��DMA2ʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��

    //PC8,9,10,11,12
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    GPIO_Initure.Alternate=GPIO_AF12_SDIO;  //����ΪSDIO
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��

    //PD2
    GPIO_Initure.Pin=GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     //��ʼ��

#if (SD_DMA_MODE==1)                        //ʹ��DMAģʽ
    HAL_NVIC_SetPriority(SDMMC1_IRQn,2,0);  //����SDMMC1�жϣ���ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);        //ʹ��SDMMC1�ж�

    //���÷���DMA
    SDRxDMAHandler.Instance=DMA2_Stream3;
    SDRxDMAHandler.Init.Channel=DMA_CHANNEL_4;
    SDRxDMAHandler.Init.Direction=DMA_PERIPH_TO_MEMORY;
    SDRxDMAHandler.Init.PeriphInc=DMA_PINC_DISABLE;
    SDRxDMAHandler.Init.MemInc=DMA_MINC_ENABLE;
    SDRxDMAHandler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
    SDRxDMAHandler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
    SDRxDMAHandler.Init.Mode=DMA_PFCTRL;
    SDRxDMAHandler.Init.Priority=DMA_PRIORITY_VERY_HIGH;
    SDRxDMAHandler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;
    SDRxDMAHandler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    SDRxDMAHandler.Init.MemBurst=DMA_MBURST_INC4;
    SDRxDMAHandler.Init.PeriphBurst=DMA_PBURST_INC4;

    __HAL_LINKDMA(hsd, hdmarx, SDRxDMAHandler); //������DMA��SD���ķ���DMA��������
    HAL_DMA_DeInit(&SDRxDMAHandler);
    HAL_DMA_Init(&SDRxDMAHandler);              //��ʼ������DMA

    //���ý���DMA
    SDTxDMAHandler.Instance=DMA2_Stream6;
    SDTxDMAHandler.Init.Channel=DMA_CHANNEL_4;
    SDTxDMAHandler.Init.Direction=DMA_MEMORY_TO_PERIPH;
    SDTxDMAHandler.Init.PeriphInc=DMA_PINC_DISABLE;
    SDTxDMAHandler.Init.MemInc=DMA_MINC_ENABLE;
    SDTxDMAHandler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
    SDTxDMAHandler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
    SDTxDMAHandler.Init.Mode=DMA_PFCTRL;
    SDTxDMAHandler.Init.Priority=DMA_PRIORITY_VERY_HIGH;
    SDTxDMAHandler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;
    SDTxDMAHandler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    SDTxDMAHandler.Init.MemBurst=DMA_MBURST_INC4;
    SDTxDMAHandler.Init.PeriphBurst=DMA_PBURST_INC4;

    __HAL_LINKDMA(hsd, hdmatx, SDTxDMAHandler);//������DMA��SD���ķ���DMA��������
    HAL_DMA_DeInit(&SDTxDMAHandler);
    HAL_DMA_Init(&SDTxDMAHandler);              //��ʼ������DMA


    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 3, 0);  //����DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 3, 0);  //����DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
#endif
}

//�õ�����Ϣ
//cardinfo:����Ϣ�洢��
//����ֵ:����״̬
uint8_t SD_GetCardInfo(HAL_SD_CardInfoTypeDef *cardinfo)
{
    uint8_t sta;
    sta=HAL_SD_Get_CardInfo(&SDCARD_Handler,cardinfo);
    return sta;
}
 #if (SD_DMA_MODE==1)        //DMAģʽ

//ͨ��DMA��ȡSD��һ������
//buf:�����ݻ�����
//sector:������ַ
//blocksize:������С(һ�㶼��512�ֽ�)
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SD_ReadBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt)
{
    uint8_t err=0;
    err=HAL_SD_ReadBlocks_DMA(&SDCARD_Handler,buf,sector,blocksize,cnt);//ͨ��DMA��ȡSD��һ������
    if(err==0)//��ȡ�ɹ�
    {
        //�ȴ���ȡ���
        err=HAL_SD_CheckReadOperation(&SDCARD_Handler,(uint32_t)SD_TIMEOUT);
    }

    return err;
}

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//blocksize:������С(һ�㶼��512�ֽ�)
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SD_WriteBlocks_DMA(uint32_t *buf,uint64_t sector,uint32_t blocksize,uint32_t cnt)
{
    uint8_t err=0;
    err=HAL_SD_WriteBlocks_DMA(&SDCARD_Handler,buf,sector,blocksize,cnt);//ͨ��DMAдSD��һ������
    if(err==0)//д�ɹ�
    {
       err=HAL_SD_CheckWriteOperation(&SDCARD_Handler,(uint32_t)SD_TIMEOUT);//�ȴ���ȡ���/
    }
    return err;
}

//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SD_ReadDisk(uint8_t* buf,uint32_t sector,uint32_t cnt)
{
    uint8_t sta=HAL_OK;
    long long lsector=sector;
    uint8_t n;
    if(SDCardInfo.CardType!=STD_CAPACITY_SD_CARD_V1_1)lsector<<=9;
    if((uint32_t)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            sta=SD_ReadBlocks_DMA((uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);
            memcpy(buf,SDIO_DATA_BUFFER,512);
            buf+=512;
        }
    }else
    {
        sta=SD_ReadBlocks_DMA((uint32_t*)buf,lsector, 512,cnt);
    }
    return sta;
}

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SD_WriteDisk(uint8_t *buf,uint32_t sector,uint32_t cnt)
{
    uint8_t sta=HAL_OK;
    long long lsector=sector;
    uint8_t n;
    if(SDCardInfo.CardType!=STD_CAPACITY_SD_CARD_V1_1)lsector<<=9;
    if((uint32_t)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            memcpy(SDIO_DATA_BUFFER,buf,512);
            sta=SD_WriteBlocks_DMA((uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector��д����
            buf+=512;
        }
    }else
    {
        sta=SD_WriteBlocks_DMA((uint32_t*)buf,lsector,512,cnt);//���sector��д����
    }
    return sta;
}

//SDMMC1�жϷ�����
void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&SDCARD_Handler);
}

//DMA2������6�жϷ�����
void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmatx);
}

//DMA2������3�жϷ�����
void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmarx);
}
#else                                   //��ѵģʽ

//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SD_ReadDisk(uint8_t* buf,uint32_t sector,uint32_t cnt)
{
    uint8_t sta=HAL_OK;
    long long lsector=sector;
    uint8_t n;
    lsector<<=9;
    INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    if((uint32_t)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            sta=HAL_SD_ReadBlocks(&SDCARD_Handler,(uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector�Ķ�����
            memcpy(buf,SDIO_DATA_BUFFER,512);
            buf+=512;
        }
    }else
    {
        sta=HAL_SD_ReadBlocks(&SDCARD_Handler,(uint32_t*)buf,lsector,512,cnt);//����sector�Ķ�����
    }
    INTX_ENABLE();//�������ж�
    return sta;
}

//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SD_WriteDisk(uint8_t *buf,uint32_t sector,uint32_t cnt)
{
    uint8_t sta=HAL_OK;
    long long lsector=sector;
    uint8_t n;
    lsector<<=9;
    INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
    if((uint32_t)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            memcpy(SDIO_DATA_BUFFER,buf,512);
            sta=HAL_SD_WriteBlocks(&SDCARD_Handler,(uint32_t*)SDIO_DATA_BUFFER,lsector+512*n,512,1);//����sector��д����
            buf+=512;
        }
    }else
    {
        sta=HAL_SD_WriteBlocks(&SDCARD_Handler,(uint32_t*)buf,lsector,512,cnt);//���sector��д����
    }
	INTX_ENABLE();//�������ж�
    return sta;
}
#endif
