	IMPORT  gwind_start_flag 
	IMPORT  gwind_cur_stack
	IMPORT  gwind_high_stack
	;IMPORT  wind_sw_hook
	   
	EXPORT  wind_start_switch               
	EXPORT  wind_thread_switch
	EXPORT  wind_interrupt_switch
	EXPORT  wind_save_sr 
	EXPORT  wind_restore_sr
	EXPORT  PendSV_Handler
        	
     
NVIC_INT_CTRL   	EQU     0xE000ED04  ; 中断控制寄存器
NVIC_SYSPRI2    	EQU     0xE000ED22  ; 系统优先级寄存器(2)
NVIC_PENDSV_PRI 	EQU         0xFFFF  ; PendSV中断和系统节拍中断
                                        ; (都为最低，0xff).
NVIC_PENDSVSET  	EQU     0x10000000  ; 触发软件中断的值.


	PRESERVE8 
	AREA    |.text|, CODE, READONLY
	THUMB 
    
           
wind_save_sr
    MRS     R0, PRIMASK  	;读取PRIMASK到R0,R0为返回值 
    CPSID   I				;PRIMASK=1,关中断(NMI和硬件FAULT可以响应)
    BX      LR			    ;返回

wind_restore_sr
    MSR     PRIMASK, R0	   	;读取R0到PRIMASK中,R0为参数
    BX      LR				;返回


;/**************************************************************************************
;* 函数名称: wind_start_switch
;*
;* 功能描述: 启动第一个线程
;* 
;* 参    数: None
;*
;* 返 回 值: None
;**************************************************************************************/  

wind_start_switch
	LDR     R4, =NVIC_SYSPRI2      ; set the PendSV exception priority
	LDR     R5, =NVIC_PENDSV_PRI
	STR     R5, [R4]

	MOV     R4, #0                 ; set the PSP to 0 for initial context switch call
	MSR     PSP, R4

	LDR     R4, =gwind_start_flag         ; gwind_start_flag = TRUE
	MOV     R5, #1
	STRB    R5, [R4]

								   ;切换到最高优先级的线程
	LDR     R4, =NVIC_INT_CTRL     ;trigger the PendSV exception (causes context switch)
	LDR     R5, =NVIC_PENDSVSET
	STR     R5, [R4]

	CPSIE   I                      ;enable interrupts at processor level
WIND_OS_HANG
	B       WIND_OS_HANG            ;should never get here

;/**************************************************************************************
;* 函数名称: wind_thread_switch
;*
;* 功能描述: 线程级上下文切换         
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/
  
wind_thread_switch
	PUSH    {R4, R5}
	LDR     R4, =NVIC_INT_CTRL  	;触发PendSV异常 (causes context switch)
	LDR     R5, =NVIC_PENDSVSET
	STR     R5, [R4]
	POP     {R4, R5}
	BX      LR

;/**************************************************************************************
;* 函数名称: wind_interrupt_switch
;*
;* 功能描述: 中断级线程切换
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/

wind_interrupt_switch
	PUSH    {R4, R5}
	LDR     R4, =NVIC_INT_CTRL      ;触发PendSV异常 (causes context switch)
	LDR     R5, =NVIC_PENDSVSET
	STR     R5, [R4]
	POP     {R4, R5}
	BX      LR
	NOP

;/**************************************************************************************
;* 函数名称: PendSV_Handler
;*
;* 功能描述: PendSV is used to cause a context switch.
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/

PendSV_Handler
    CPSID   I                                                   ; Prevent interruption during context switch
    MRS     R0, PSP                                             ; PSP is process stack pointer 如果在用PSP堆栈,则可以忽略保存寄存器,参考CM3权威中的双堆栈-白菜注
    CBZ     R0, PendSV_Handler_Nosave		                    ; Skip register save the first time
	
	;Is the task using the FPU context? If so, push high vfp registers.
	;TST 	R14, #0x10
	;IT 		EQ
	;VSTMDBEQ R0!, {S16-S31} 
	
    SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =gwind_cur_stack                                       ; gwind_cur_stack = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
PendSV_Handler_Nosave
    LDR     R0, =gwind_cur_stack                                       ; gwind_cur_stack  = gwind_high_stack;
    LDR     R1, =gwind_high_stack
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = gwind_high_stack
    LDM     R0, {R4-R11}                                        ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20

	;Is the task using the FPU context? If so, push high vfp registers.
	;TST 	R14, #0x10
	;IT 		EQ
	;VLDMIAEQ R0!, {S16-S31} 
		
	MSR     PSP, R0                                             ; Load PSP with new process SP
    ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context
	NOP
    end  
	