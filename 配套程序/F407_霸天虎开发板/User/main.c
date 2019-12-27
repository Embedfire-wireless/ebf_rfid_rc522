/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RFID-RC522模块实验
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./font/fonts.h"	
#include "./systick/bsp_SysTick.h"


/**
  * @brief  测试函数
  * @param  无
  * @retval 无
  */
void IC_test ( void )
{
	char cStr [ 30 ];
  uint8_t ucArray_ID [ 4 ];   //先后存放IC卡的类型和UID(IC卡序列号)
	uint8_t ucStatusReturn;     //返回状态
  static uint8_t ucLineCount = 0; 
	
  while ( 1 )
  { 
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK ) //寻卡
      //若失败再次寻卡
			ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );		                                                 

		if ( ucStatusReturn == MI_OK  )
		{ 
      //防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）
			if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
			{
				sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X", 
                  ucArray_ID [ 0 ], ucArray_ID [ 1 ], 
                  ucArray_ID [ 2 ], ucArray_ID [ 3 ] );
								
				printf ( "%s\r\n",cStr ); 
				
				if ( ucLineCount == 0 )
          
      	LCD_SetTextColor(BLUE);
				ILI9806G_DispStringLine_EN( LINE(ucLineCount), cStr);
        			
				ucLineCount ++;
				
				if ( ucLineCount == 14 ) 
          ucLineCount = 0;								
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
	SysTick_Init();       //滴答时钟初始化
	
	Debug_USART_Config(); //USART1 配置模式为 115200 8-N-1，中断接收 
	
	RC522_Init ();        //RC522模块所需外设的初始化配置
 	
	ILI9806G_Init ();     //LCD 初始化
	ILI9806G_GramScan ( 6 );

	printf ( "WF-RC522 Test\n" );
	
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);

  ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH); //清屏，显示全黑 
	ILI9806G_DispStringLine_EN(LINE(16) ,
             "Please put the IC card on WF-RC522 antenna area ..." );
	
	PcdReset ();
	M500PcdConfigISOType ( 'A' );//设置工作方式
		
  while ( 1 )
  {
    IC_test ();//IC卡检测	
  }	
}


/*********************************************END OF FILE**********************/

