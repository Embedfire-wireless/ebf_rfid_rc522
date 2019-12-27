/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   RFID-RC522模块实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>
#include "bsp_lcd.h"

/**
  * @brief  IC测试函数
  * @param  无
  * @retval 无
  */
void IC_test ( void )
{
	char cStr [ 30 ];
  u8 ucArray_ID [ 4 ];  /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                           
	u8 ucStatusReturn;    /*返回状态*/                                                                                         
  static u8 ucLineCount = 0; 

  while ( 1 )
  { 
    /*寻卡*/
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK )  
      /*若失败再次寻卡*/
			ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );		                                                 

		if ( ucStatusReturn == MI_OK  )
		{
      /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
			if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
			{
				sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",
                  ucArray_ID [ 0 ], 
                  ucArray_ID [ 1 ], 
                  ucArray_ID [ 2 ], 
                  ucArray_ID [ 3 ] );
				
				printf ( "%s\r\n",cStr ); 
				
				if ( ucLineCount == 0 )
				  ILI9341_Clear ( 0, 0, 240, 271, macBACKGROUND);	
				
				ILI9341_DispString_EN ( 0, ucLineCount * 16, cStr,
                                macBACKGROUND, macBLUE );
				
				ucLineCount ++;
				
				if ( ucLineCount == 17 ) ucLineCount = 0;				
			}		
		}		
  }	
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{
  /*滴答时钟初始化*/
	SysTick_Init ();   
	
  /*USART1 配置模式为 115200 8-N-1，中断接收*/
	USART1_Config ();   
	
  /*RC522模块所需外设的初始化配置*/
	RC522_Init ();     
	
   /*LCD 初始化*/
	LCD_Init ();      
		
	printf ( "WF-RC522 Test\n" );
		
	ILI9341_Clear ( 0,   0, 240, 320, macBACKGROUND);	
	ILI9341_Clear ( 0, 272, 240,   2, macGREEN);
	ILI9341_DispString_EN ( 0, 280, 
      "Please put the IC card on WF-RC522 antenna area ...",
                  macBACKGROUND,
                   macRED );
	PcdReset ();
  
  /*设置工作方式*/
	M500PcdConfigISOType ( 'A' );
	
  while ( 1 )
  {
    /*IC卡检测*/
    IC_test ();                	
  }	    
}
/****************************END OF FILE**********************/

