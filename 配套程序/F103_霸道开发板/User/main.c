/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   RFID-RC522模块
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./SysTick/bsp_SysTick.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include <stdbool.h>

void IC_CMT ( uint8_t * UID, uint8_t * KEY, uint8_t RW, uint8_t * Dat );
/**
  * @brief  IC测试函数
  * @param  无
  * @retval 无
  */
uint8_t buffer[100];
uint8_t buff1[] = {0x50 ,0xBA, 0xA9, 0x89};
uint8_t buff2[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF};   // 卡A密钥
uint8_t buff3[]={0x55 ,0xBA, 0xA9, 0x89, 0x50 ,0xBA, 0xA9, 0x89, 0x50 ,0xBA, 0xA9, 0x89, 0x50 ,0xBA, 0xA9, 0x89};    // 写入的数据
void IC_test ( void )
{
	char cStr [ 30 ];
  uint8_t ucArray_ID [ 4 ];    /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                         
	uint8_t ucStatusReturn;      /*返回状态*/                                                                                          
  static uint8_t ucLineCount = 0; 
  int i;
  
  while ( 1 )
  { 
    /*******************卡片扇区读写测试****************************/
    buff3[0]++;
    
    IC_CMT(buff1, buff2, 0, buff3);    // 写卡

    IC_CMT(buff1, buff2, 1, buffer);   // 读卡
    
    printf ("读到的数据为：\n");
    
    for( i=0; i<16; i++)
    {
      printf ("0x%X\n", buffer[i]);    // 打印读到的数据
    }
    /*******************卡片扇区读写测试 END****************************/  
    
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
          LCD_SetTextColor(RED); 	
				
				ILI9341_DispStringLine_EN(LINE(ucLineCount) , (char* )cStr );
       
				IC_CMT(buff1, buff2, 1, buffer);   // 需要在读一次下次再能写进去（不知道为什么这样，应该是这样能改变某种模式吧）
        
				ucLineCount ++;
				
				if ( ucLineCount == 17 ) ucLineCount = 0;	    
			}		
		}		
  }	
}
char PcdComMF522 ( uint8_t ucCommand, 
                    uint8_t * pInData,
                    uint8_t ucInLenByte, 
                    uint8_t * pOutData, 
                    uint32_t * pOutLenBit )	;
uint8_t buff[] = {0x06, 0x61, 0x1, 0x2, 0x3, 0x4 ,0x6, 0x1, 0x2, 0x3, 0x4};

uint32_t bufferlen;
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
  /*滴答时钟初始化*/
  SysTick_Init ();
  
	/*LCD 初始化*/
	ILI9341_Init ();  
  
	/* USART config */
	USART_Config(); 
  
  /*RC522模块所需外设的初始化配置*/
  RC522_Init ();     
	
  printf ( "WF-RC522 Test\n" );
	
  /*其中0、3、5、6 模式适合从左至右显示文字，*/
	ILI9341_GramScan ( 6 ); 
  LCD_SetFont(&Font8x16);
	LCD_SetColors(BLACK,BLACK);
  
  /* 清屏，显示全黑 */
  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
  
	/********显示字符串示例*******/
  LCD_SetTextColor(RED);  

  ILI9341_DispStringLine_EN(LINE(18),
       (char* )"Please put the IC card on WF-RC522 antenna area ..."); 
  
  LCD_SetTextColor(YELLOW);
  
  PcdReset ();
   
  /*设置工作方式*/   
	M500PcdConfigISOType ( 'A' );
  
  while(1)
  { 
    /*IC卡检测	*/    
    IC_test ();     

  }

}


/* ------------------------------------------end of file---------------------------------------- */

