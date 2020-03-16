#if 0
/*
IS62WV51216 简介
IS62WV51216 是 ISSI（Integrated  Silicon  Solution,  Inc）公司生产的一颗 16 位宽 512K（512*16，即 1M 字节）容量的 CMOS 静态内存芯片。该芯片具有如下几个特点：
l 高速。具有 45ns/55ns 访问速度。
l 低功耗。
l TTL 电平兼容。
l 全静态操作。不需要刷新和时钟电路。
l 三态输出。
l 字节控制功能。支持高/低字节控制。
看看实现 IS62WV51216 的访问，需要对 FSMC进行哪些配置。 这里就做一个概括性的讲解。步骤如下： 
1）使能 FSMC 时钟，并配置 FSMC 相关的 IO 及其时钟使能。
要使用 FSMC，当然首先得开启其时钟。然后需要把 FSMC_D0~15，FSMCA0~18 等相关IO 口，全部配置为复用输出，并使能各 IO 组的时钟。 
使能 FSMC 时钟的方法：
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE); 
对于其他 IO 口设置的方法前面讲解很详细，这里不做过多的讲解。
2）设置 FSMC BANK1 区域 3。
此部分包括设置区域 3 的存储器的工作模式、位宽和读写时序等。我们使用模式 A、16 位宽，读写共用一个时序寄存器。使用的函数是：
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct)
3）使能 BANK1 区域 3。
使能 BANK 的方法跟前面 LCD 实验也是一样的，这里也不做详细讲解，函数是：
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState)；
通过以上几个步骤，我们就完成了 FSMC 的配置，可以访问 IS62WV51216 了，这里还需要注意，因为我们使用的是 BANK1 的区域 3，所以 HADDR[27:26]=10，故外部内存的首地址为 0X68000000。
*/
#endif


//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM3_ADDR ((u32)(0x68000000))        
                           
//初始化外部SRAM
void FSMC_SRAM_Init(void)
{    
    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef readWriteTiming;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOG,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
    GPIO_InitStructure.GPIO_Pin = 0xFF33;                  //PORTD复用推挽输出 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;          //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = 0xFF83;                  //PORTE复用推挽输出 
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = 0xF03F;                  //PORTD复用推挽输出 
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = 0x043F;                  //PORTD复用推挽输出 
    GPIO_Init(GPIOG, &GPIO_InitStructure);
       
    readWriteTiming.FSMC_AddressSetupTime = 0x00;     //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;     //地址保持时间（ADDHLD）模式A未用到    
    readWriteTiming.FSMC_DataSetupTime = 0x03;         //数据保持时间（DATAST）为3个HCLK 4/72M=55ns(对EM的SRAM芯片)     
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;     //模式A 

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;// 这里我们使用NE3 ，也就对应BTCR[4],[5]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM; //SRAM 
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit 
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; 
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;    //存储器写使能 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // 读写使用相同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //读写同样时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //初始化FSMC配置

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE); // 使能BANK3                                         
                                            
}
                                                              
//在指定地址开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
{
    for(;n!=0;n--) 
    {                                         
        *(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;     
        WriteAddr+=2;//这里需要加2，是因为STM32的FSMC地址右移一位对其.加2相当于加1.
        pBuffer++;
    } 
}

//在指定地址开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
{
    for(;n!=0;n--) 
    {                                             
        *pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr); 
        ReadAddr+=2;//这里需要加2，是因为STM32的FSMC地址右移一位对其.加2相当于加1.
    } 
} 
////////////////////////////////////////////////////////////////////////////////////////
//测试函数
//在指定地址写入1个字节
//addr:地址
//data:要写入的数据
void fsmc_sram_test_write(u8 data,u32 addr)
{             
    FSMC_SRAM_WriteBuffer(&data,addr,1);//写入1个字节
}
//读取1个字节
//addr:要读取的地址
//返回值:读取到的数据
u8 fsmc_sram_test_read(u32 addr)
{
    u8 data;
    FSMC_SRAM_ReadBuffer(&data,addr,1);
    return data;
}
