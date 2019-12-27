/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RFID-R522模块
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F429 开发板
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
#include "./lcd/bsp_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "./font/fonts.h"
#include "./sdram/bsp_sdram.h"

/**
  * @brief  测试函数
  * @param  无-
  * @retval 无
  */
void IC_test ( void )
{
	char cStr [ 30 ];
  u8 ucArray_ID [ 4 ]; /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                            
	u8 ucStatusReturn;   /*返回状态 */                                                                                           
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
         
				  LCD_SetTextColor(LCD_COLOR_RED);
				  LCD_DisplayStringLine(LINE(ucLineCount), (uint8_t* )cStr);
        
				  ucLineCount ++;
				
				if ( ucLineCount == 12 ) ucLineCount = 0;							
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
  SysTick_Init();   

  /*USART1 配置模式为 115200 8-N-1，中断接收*/
  Debug_USART_Config();   

  /*RC522模块所需外设的初始化配置*/
  RC522_Init ();     

  /*初始化液晶屏*/
  LCD_Init();
  LCD_LayerInit();

  printf ( "WF-RC522 Test\n" );

  LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
  
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
  LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
  
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/		
  LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
  
  LCD_SetTextColor(LCD_COLOR_BLUE);

  LCD_DisplayStringLine(LINE(14),
               (uint8_t*)"Please put the IC card on WF-RC522 antenna area ..." );

  PcdReset ();
  M500PcdConfigISOType ( 'A' );/*设置工作方式*/

  while ( 1 )
  {
    IC_test ();/*IC卡检测*/	
  }	
}



/*********************************************END OF FILE**********************/

