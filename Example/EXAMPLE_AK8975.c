/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: E-COMPASS
     File Name		: EXAMPLE_AK8975.c
     Function		: EXAMPLE_AK8975
     Create Date	: 2017/07/12
---------------------------------------------------------------------- */

#ifndef __AK8975_EXAMPLE__
#define __AK8975_EXAMPLE__

#include <mega32a.h>
#include <stdio.h>
#include <delay.h>
#include <math.h>
#include <i2c.h>
#include <alcd.h>
#include "SENSOR_AK8975.h"


//------- Define AK8975 variable ------
//if = 1 , read AK8975 inifite .
//if = 0 , read once . 
#define EXAMPLE_AK8975_INIFITE_READ 	1


//------- Define AK8975 variable ------

AK8975_M AK8975_MODE;							//mode

extern AK8975_ASA_REG  AK8975_ASA_XYZ_DATA;	// asa x,y,z reg data    ;  initial read ASA register & write in .

AK8975_AXIS AK8975_XYZ_DATA;	// read x,y,z raw data

//------- Define AK8975 variable ------

void  EXAMPLE_AK8975(void);

void  EXAMPLE_AK8975(void)
{

	CHAR8U id_data,info_data,check_mode;
	CHAR8S status = 0 ; 
    	CHAR8U lcd_char_data[3][4]={0}; 
 

               	printf("-------------------- E-COMPASS AK8975 --------------------\r\n");
               	//E-COMPASS AK8975 ;

			//first initial AK8975 ; read ASA reg
			status = AK8975_INIT();
			if(status != 0)  printf("initial AK8975 fail!\r\n");
			else printf("initial AK8975 ID OK!\r\n");	

			delay_ms(1);

			//read AK8975 ID & INFO
			status = AK8975_ID(&id_data, &info_data);
			if(status != 0)  printf("read AK8975 ID fail!\r\n");
			else printf("read AK8975 ID = 0x%x , INFO = 0x%x \r\n",id_data,info_data);


			//read AK8975 mode -> check AK8975 in power down mode			
			status = AK8975_MODE_READ(&check_mode);
			if(status != 0) 
				{
					printf("read AK8975 mode fail!\r\n");
				}
			else  
				{
					printf("read AK8975 mode ok\r\n");
					if(check_mode == AK8975_MODE_PWR_DOWM) printf("power down mode\r\n");
					else 
						{printf("else mode\r\n");
					while(1); // stop next sequence
				 }
                }	


			//CHECK AUTO TEST , CHECK IC WORKING NORMALLY.
			// -100 <= x <= +100
			// -100 <= y <= +100
			// -1000 <= z <= -300
			status = AK8975_AUTO_TEST(&AK8975_XYZ_DATA);
			if(status !=0)
				{
					printf("AK8975 auto test fail\r\n");					
				}
			else 
				{
					printf("X axis = %d\r\n",AK8975_XYZ_DATA.X_AXIS);
					printf("Y axis = %d\r\n",AK8975_XYZ_DATA.Y_AXIS);		
					printf("Z axis = %d\r\n",AK8975_XYZ_DATA.Z_AXIS);
					printf("AK8975 auto test OK \r\n");       
              
				}

			delay_ms(3);


#if EXAMPLE_AK8975_INIFITE_READ == 1
		while(1)
			{
#endif
			//read AK8975 XYZ data.
			status = AK8975_READ_AXIS(&AK8975_XYZ_DATA);
			if(status != 0)
			{
					printf("read AK8975 data fail!\r\n");
			}
			else
			{

				printf("read AK8975 data OK! \r\n");

				printf("X axis = %d \r\n",AK8975_XYZ_DATA.X_AXIS);
				printf("Y axis = %d \r\n",AK8975_XYZ_DATA.Y_AXIS);		
				printf("Z axis = %d \r\n",AK8975_XYZ_DATA.Z_AXIS);	 
                            
                     
	                     // X AXIS   
	                     lcd_char_data[0][0]=(INT32U)(AK8975_XYZ_DATA.X_AXIS/1000)%10;
	                     lcd_char_data[0][1]=(INT32U)(AK8975_XYZ_DATA.X_AXIS/100)%10;   
	                     lcd_char_data[0][2]=(INT32U)(AK8975_XYZ_DATA.X_AXIS/10)%10;  
	                     lcd_char_data[0][3]=(INT32U)(AK8975_XYZ_DATA.X_AXIS)%10;   
	                     
	                     // Y AXIS   
	                     lcd_char_data[1][0]=(INT32U)(AK8975_XYZ_DATA.Y_AXIS/1000)%10;
	                     lcd_char_data[1][1]=(INT32U)(AK8975_XYZ_DATA.Y_AXIS/100)%10;   
	                     lcd_char_data[1][2]=(INT32U)(AK8975_XYZ_DATA.Y_AXIS/10)%10;  
	                     lcd_char_data[1][3]=(INT32U)(AK8975_XYZ_DATA.Y_AXIS)%10;         
	                     
	                     
	                     // Z AXIS   
	                     lcd_char_data[2][0]=(INT32U)(AK8975_XYZ_DATA.Z_AXIS/1000)%10;
	                     lcd_char_data[2][1]=(INT32U)(AK8975_XYZ_DATA.Z_AXIS/100)%10;   
	                     lcd_char_data[2][2]=(INT32U)(AK8975_XYZ_DATA.Z_AXIS/10)%10;  
	                     lcd_char_data[2][3]=(INT32U)(AK8975_XYZ_DATA.Z_AXIS)%10;         


	                     /* SHOW X axis DATA */                    
	            		lcd_gotoxy(0,0);
	            		lcd_putsf("X axis:");     
	            		if(AK8975_XYZ_DATA.X_AXIS<0)
	            		{
	            			lcd_putchar(45);		/* LCD show "-"  mean negative */
				}                             
	           		else
				{
					lcd_putchar(32);		/* LCD show " "  mean positive */
				}   
					
				  /* show X axis data on LCD */
		                lcd_putchar(48+lcd_char_data[0][0]);
		                lcd_putchar(48+lcd_char_data[0][1]);
		                lcd_putchar(48+lcd_char_data[0][2]);
		                lcd_putchar(48+lcd_char_data[0][3]);


				/* SHOW Y axis DATA */    
	          		 lcd_gotoxy(0,1);
	           		 lcd_putsf("Y axis:"); 
	             		if(AK8975_XYZ_DATA.Y_AXIS<0)
	           		{
	           			lcd_putchar(45);		/* LCD show "-"  mean negative */
				}                             
	            		else
				{
					lcd_putchar(32);		/* LCD show " "  mean positive */
				}   


				/* show Y axis data on LCD */
	                 	lcd_putchar(48+lcd_char_data[1][0]);
	                	lcd_putchar(48+lcd_char_data[1][1]);
	                 	lcd_putchar(48+lcd_char_data[1][2]);
	                 	lcd_putchar(48+lcd_char_data[1][3]);
	                    
	  
				/* SHOW Z axis DATA */    
	            		lcd_gotoxy(0,2);
	            		lcd_putsf("Z axis:"); 
	           		if(AK8975_XYZ_DATA.Z_AXIS<0)
	            		{
	            			lcd_putchar(45);
				}                             
	            		else
				{
					lcd_putchar(32);
				}

									   
				/* show Y axis data on LCD */
	                 	lcd_putchar(48+lcd_char_data[2][0]);
	                 	lcd_putchar(48+lcd_char_data[2][1]);
	                 	lcd_putchar(48+lcd_char_data[2][2]);
	                 	lcd_putchar(48+lcd_char_data[2][3]);
                    
                                 
			}
			delay_ms(100);			//delay 100 ms   
            

  
			
#if EXAMPLE_AK8975_INIFITE_READ == 1
			}
#endif
		
                printf("-------------------- E-COMPASS AK8975 --------------------\r\n");
}



#endif		//#ifndef __AK8975_EXAMPLE__
