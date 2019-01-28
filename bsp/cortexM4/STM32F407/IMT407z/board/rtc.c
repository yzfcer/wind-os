#include "rtc.h"
#include "led.h"

/*********************************************************************************
*************************MCU���� STM32F407���Ŀ�����******************************
**********************************************************************************
* �ļ�����: rtc.c                                                                *
* �ļ�������RTC��ʼ����ʹ��                                                      *
* �������ڣ�2015.03.11                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    ����RTC��ʼ����ʹ��                                                      * 
**********************************************************************************
*********************************************************************************/

//RTCʱ�����ݽṹ�壬ʱ������ڷֿ�
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

NVIC_InitTypeDef   NVIC_InitStructure;

									 
u8_t const month_amendBuf[12]={0,3,3,6,1,4,6,2,5,0,3,5}; 
/****************************************************************************
* ��    ��: u8_t RTC_GetWeek(u16_t wyear,u8_t wmonth,u8_t wday)
* ��    �ܣ����ĳ�������ڼ�
* ��ڲ�����wyear����(���99)  wmonth����  wday����
* ���ز��������ڼ�
* ˵    ������2000~2099��Ч
            ����ֵ1-7���ζ�Ӧ����һ��������  
            �ú�����������ʱ��ʱ�������������ڼ���ȡʱ�����ڼ�ʱ����ʹ�øú�����
            ��ֱ�Ӷ�ȡ���ڼĴ���������ֵ����
            ע���βΡ��ꡱ��8λ����Ҫ����2λ����Ĳ���  15���Ϸ�    2015�����Ϸ�
****************************************************************************/																						 
u8_t RTC_GetWeek(u8_t wyear,u8_t wmonth,u8_t wday)
{	
	u16_t middata;
	u8_t yearL;
	
	yearL=wyear+100; 	 //��2000�꿪ʼ����100

	middata=yearL+yearL/4;
	middata=middata%7; 
	middata=middata+wday+month_amendBuf[wmonth-1];
	if (yearL%4==0&&wmonth<3)middata--;
	
	if((middata%7==0)) return 7;       //��STM32F103��rtc���̲�ͬ���ú��������췵��ֵΪ7
	return(middata%7);
}	
/****************************************************************************
* ��    ��: ErrorStatus RTC_SetTimes(u8_t year,u8_t month,u8_t date,u8_t hour,u8_t min,u8_t sec)
* ��    �ܣ�����RTCʱ��
* ��ڲ�����������ʱ����
* ���ز������ɹ����  1���ɹ�   0��ʧ��
* ˵    ����ע���βΡ��ꡱ��16λ����Ҫ����4λ����Ĳ���  15���Ϸ�    2015�����Ϸ�    
****************************************************************************/	
ErrorStatus RTC_SetTimes(u8_t year,u8_t month,u8_t date,u8_t hour,u8_t min,u8_t sec)
{
  RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=RTC_GetWeek(year,month,date);
	//RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;

	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	
	if(hour>12)
	     RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_PM;
	else RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
	
	return ( RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure) && RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure) );
}
/****************************************************************************
* ��    ��: void RTC_GetTimes(uint32_t RTC_Format)
* ��    �ܣ���ȡRTCʱ��
* ��ڲ�����RTC_Format����ȡʱ�����ݵ���������  RTC_Format_BIN���ֽ���  RTC_Format_BCD��BCD����������
* ���ز�������
* ˵    ����     
****************************************************************************/
void RTC_GetTimes(uint32_t RTC_Format)
{
		RTC_GetDate(RTC_Format,&RTC_DateStruct);
	  RTC_GetTime(RTC_Format,&RTC_TimeStruct);
}

//��һ����Ҫ����RTCʱ���øú���
u8_t RTC_Configuration(void)
{
  u16_t wait=0;  
	RTC_InitTypeDef RTC_InitStructure;
	
	  RCC_LSEConfig(RCC_LSE_ON);   //����LSE   
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			wait++;
				delay_ms(5);
				if(wait>=500)return 1;      //�糬��2.5s����û����,����������	 
			}
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ�� 

    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//����ʱ���ʽΪ��24Сʱ��
    RTC_Init(&RTC_InitStructure);
			
		RTC_SetTimes(15,3,11,13,02,30);	
		return 0;
}
/****************************************************************************
* ��    ��: u8_t RTC_InitConfig(void)
* ��    �ܣ�RTC��ʼ������
* ��ڲ�������
* ���ز������ɹ���� 0���ɹ�  1��ʧ��
* ˵    ���� 
****************************************************************************/
u8_t RTC_InitConfig(void)
{
  u8_t i=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE);	 //ʹ�ܺ󱸼Ĵ��� 
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5555)		//�Ƿ��һ������
	{
		i=RTC_Configuration();
		if(i==1) return 1;  //RTC����ʧ��
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5555);	  //��ǳ�ʼ������
	} 
  RTC_SetWakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	//����WAKE UP�ж�,1�����ж�һ��
	return 0;
}
/****************************************************************************
* ��    ��: void RTC_SetWakeUp(u32_t wksel,u16_t cnt)
* ��    �ܣ����û��Ѷ�ʱ����ʱ��
* ��ڲ�����wktime������ʱ��   autodata���Զ���װ��ֵ
* ���ز�������
* ˵    �������øú����Ļ���ʱ��Ϊ1S�ж�һ�Σ��൱��STM32F103�����е����ж�     
****************************************************************************/
void RTC_SetWakeUp(u32_t wktime,u16_t autodata)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);        //�ر�WAKE UP
	
	RTC_WakeUpClockConfig(wktime); //����ʱ��ѡ��
	
	RTC_SetWakeUpCounter(autodata);//����WAKE UP�Զ���װ�ؼĴ���
		
	RTC_ClearITPendingBit(RTC_IT_WUT);  //���RTC WAKE UP�ı�־
  EXTI_ClearITPendingBit(EXTI_Line22);//���LINE22�ϵ��жϱ�־λ 
	 
	RTC_ITConfig(RTC_IT_WUT,ENABLE); //����WAKE UP��ʱ���ж�
	RTC_WakeUpCmd( ENABLE);          //����WAKE UP ��ʱ����
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;            //LINE22
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //ʹ��LINE22
  EXTI_Init(&EXTI_InitStructure);                        //����
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        //�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);                              //����
}
/****************************************************************************
* ��    ��: void RTC_WKUP_IRQHandler(void)
* ��    �ܣ�RTC WAKE UP�жϷ�����
* ��ڲ�������
* ���ز�������
* ˵    �����൱�����ж�
****************************************************************************/
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET) //WK_UP�ж�
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	          //����жϱ�־
		LED0=!LED0; 
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);      //����ж���22���жϱ�־ 								
}

/****************************************************************************
* ��    ��: void RTC_SetAlarmA(u8_t week,u8_t hour,u8_t min,u8_t sec)
* ��    �ܣ���������ʱ��(����������,24Сʱ��)
* ��ڲ�����week:���ڼ�(1~7) ʱ����
* ���ز�������
* ˵    ���� 
****************************************************************************/
void RTC_SetAlarmA(u8_t week,u8_t hour,u8_t min,u8_t sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);  //�ȹر�����A 
	
  RTC_TimeTypeInitStructure.RTC_Hours=hour; 
	RTC_TimeTypeInitStructure.RTC_Minutes=min; 
	RTC_TimeTypeInitStructure.RTC_Seconds=sec; 
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;                               //����
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay; //��������
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;                        //ƥ�����ڣ�ʱ����
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
  RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
 
	RTC_ClearITPendingBit(RTC_IT_ALRA);  //���RTC����A�ı�־
  EXTI_ClearITPendingBit(EXTI_Line17); //���LINE17�ϵ��жϱ�־λ 
	
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);    //��������A�ж�
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);    //��������A 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;            //LINE17
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    //�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //ʹ��LINE17
  EXTI_Init(&EXTI_InitStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;        //�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);                              //����
}

//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET) //ALARM A�ж�  
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);           //����жϱ�־
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	     //����ж���17���жϱ�־ 											 
}



