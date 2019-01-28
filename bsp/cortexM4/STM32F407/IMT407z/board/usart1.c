#include "usart1.h"
#include "string.h"
#include "stdlib.h"  
#include "led.h" 
#if WIND_DEBUG_SUPPORT

void uart1_init(w_uint32_t baud)
{   //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ�� 
    //����1��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  //GPIOA9����ΪUSART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
    //USART1�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //����
    GPIO_Init(GPIOA,&GPIO_InitStructure);          //��ʼ��PA9��PA10
    //USART1 ��ʼ������
    USART_InitStructure.USART_BaudRate = baud;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1	
    USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 

    USART_ClearFlag(USART1, USART_FLAG_TC);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);         //��������ж�
    //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;      //����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;           //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);      //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

//����1����һ���ַ�
void uart1_send_char(w_uint8_t ch)
{
	while((USART1->SR&0x40)==0);
    USART1->DR = (w_uint8_t) ch;
}

void uart1_send_chars(w_uint8_t *str, w_uint16_t strlen)
{ 
    w_uint16_t k= 0 ; 
    do 
    { uart1_send_char(*(str + k)); k++; }   
    while (k < strlen); 
}

extern void wind_stdin_irq(char data);
void USART1_IRQHandler(void)
{
	w_uint8_t rec_data;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        rec_data =(w_uint8_t)USART_ReceiveData(USART1);
        wind_stdin_irq((char)rec_data);
    } 
} 
#endif
