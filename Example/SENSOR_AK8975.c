/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: E-COMPASS
     File Name		: SENSOR_AK8975.c
     Function		: SENSOR_AK8975
     Create Date	: 2017/07/11
---------------------------------------------------------------------- */

#include <stdio.h>
#include <math.h>
#include <datatype_Layer.h>
#include <swi2c_Layer.h>
#include <SENSOR_AK8975.h>


AK8975_ASA_REG AK8975_ASA_XYZ_DATA;	/*asa x,y,z reg data    ;  initial read ASA register & write in . */

/********************************************** SYSTEM **************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*
	read AK8975 ID  	, REG 0X00    ; ID VALUE = 0X48
	read AK8975 INFO	, REG 0X01
*/
CHAR8S AK8975_ID(CHAR8U *id, CHAR8U *info)
{
	CHAR8U read_data[2] = {0};
	CHAR8S status = 0;
		
		i2c_stop_hang();

		status = i2c_read_n_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_WIA,2,&read_data[0]);
		     
		if(status !=1) 
		{
			return -1; 	/*read fail*/
		}
		
		*id = read_data[0];
		*info = read_data[1];
		
		return 0;
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	read AK8975 X,Y,Z axis data , REG 0X03~0X08
	When CNTL register(0X0A) is accessed to be written, registers from 0x02 to 0x09 are initialized.
 */
CHAR8S AK8975_READ_AXIS(AK8975_AXIS *data)
{
	CHAR8U raw_data[6] = {0};
	CHAR8S status = 0 , status1 = 0;
	CHAR8U count = 0 ;
		
		/*set Single measurement mode */
		status = AK8975_MODE_SET(AK8975_MODE_SING_MEA);	
		if(status !=0) 
		{
			return -1;	/*set mode fail */
		}

		for(count = 0 ; count < 4 ; count ++)	/*retry 4  */
		{
			AK8975_DELAY(0xFF0);
		
			status = AK8975_CHK_RDY_ERR();		/*check RDRY , HOFL , DERR bit */

			if(status == 0)
			{
				/*data ready & no error */
				break;
			}
			else if(status == -1 )
			{
				/*read status fail */
				status1 = -1;
			}
			else if(status == -2 )
			{
				/*data not ready or error flag! */
				status1 = -2;
			}
		}

		i2c_stop_hang();
		i2c_stop_hang();
		
		/*read x,y,z axis raw data */
		status = i2c_read_n_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_HXL,6,&raw_data[0]);
		if(status !=1)
		{
			return -1;	/* read fail */
		}
			
		data->X_AXIS = (raw_data[1]<<8) + raw_data[0];
		data->Y_AXIS = (raw_data[3]<<8) + raw_data[2];		
		data->Z_AXIS = (raw_data[5]<<8) + raw_data[4];	
			
		AK8975_CAL_AXIS(data);	
			
		return 0;	/* read success ok! */

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	read & check data ready ?	, REG 0X02  bit0 DRDY
	read & check data error ?	, REG 0X09  bit2 DERR 	 bit3 HOFL
	if not ready or error -> return -1
	if data ready & no error -> return 0
*/
CHAR8S AK8975_CHK_RDY_ERR(void)
{
	CHAR8U ready = 0,error = 0;
	CHAR8S status = 0;
		
		i2c_stop_hang();
		i2c_stop_hang();	
		status = i2c_read_1_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_ST1,&ready);	/*read STATUS 1*/
		if(status !=1) 
		{
			return -1;	/* read fail*/
		}

		i2c_stop_hang();
		i2c_stop_hang();
		status = i2c_read_1_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_ST2,&error);		/*read STATUS 2*/
		if(status !=1)
		{
			return -1;	/* read fail*/
		}
			
		if( ( ready & 0x01 == 0x01) && ( error & 0x0C == 0x00 )) /*data ready & no error*/
		{
			return 0;	/* data ready & no error !*/
		}
		else
		{
			return -2;	/*status error.*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	initial AK8975 sensor  for begin measurement.
	read the ASA register [0x10~0x12] for adjust.
	 power on -> mode = '0000' power down mode.
*/
CHAR8S AK8975_INIT(void)
{

		CHAR8S status = 0;	

		/*first read ASA register value*/
		status = AK8975_READ_ASA_ADJ(&AK8975_ASA_XYZ_DATA); 
                 
		if(status !=0)
		{
			return -1; /*read fail*/
		}
		return 0; /* initial ok.*/
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	Control mode set  , REG 0x0A
	set mode 

		AK8975_MODE_PWR_DOWM =0x01,	->Power-down mode
		AK8975_MODE_SING_MEA,			->Single measurement mode
		AK8975_MODE_SELF_TEST,			->Self-test mode
		AK8975_MODE_FUSE_ROM			->Fuse ROM access mode

	When CNTL register is accessed to be written, registers from 02H to 09H are initialized.
*/
CHAR8S AK8975_MODE_SET(CHAR8U mode)
{
	CHAR8U set_mode = 0x00; 	/*default power down mode */
	CHAR8S status = 0;
			
		if(mode != AK8975_MODE_PWR_DOWM && mode != AK8975_MODE_SING_MEA && mode != AK8975_MODE_SELF_TEST && mode != AK8975_MODE_FUSE_ROM  )
		{
			return -1; 	/* set mode error .*/
		}

		switch(mode)
		{
			case 0x01:/*AK8975_MODE_PWR_DOWM*/
					set_mode = 0x00; /* set Power down mode*/
				break;
				
			case 0x02:/*AK8975_MODE_SING_MEA*/
					set_mode = 0x01; /* set Single measurement mode*/
				break;
				
			case 0x03:/*AK8975_MODE_SELF_TEST*/
					set_mode = 0x08; /* set Self-test mode*/
				break;
				
			case 0x04:/*AK8975_MODE_FUSE_ROM*/
					set_mode = 0x0F; /* set Fuse ROM access mode*/
				break;							
		}
		
		i2c_stop_hang();
		i2c_stop_hang();
		status = i2c_write_1_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_CNTL,set_mode);
		if(status != 1 )
		{
			return -1;	/*write fail.*/
		}
		
		return 0;	/* write success!*/
			
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	Control mode READ , REG 0x0A
	READ mode 

		AK8975_MODE_PWR_DOWM =0x01,	->Power-down mode
		AK8975_MODE_SING_MEA,			->Single measurement mode
		AK8975_MODE_SELF_TEST,			->Self-test mode
		AK8975_MODE_FUSE_ROM			->Fuse ROM access mode
*/
CHAR8S AK8975_MODE_READ(CHAR8U *mode)
{
	CHAR8U read_data = 0;
	CHAR8S status = 0;

		i2c_stop_hang();	
		i2c_stop_hang();
		status = i2c_read_1_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_CNTL,&read_data);	/*read STATUS 1*/
		if(status !=1) 
		{
			return -1;	/*read fail*/
		}

		
		if(read_data == 0x00)/*power-down mode*/
		{
			*mode = AK8975_MODE_PWR_DOWM;
		}
		else if(read_data == 0x01)/*single measurement mode*/
		{
			*mode = AK8975_MODE_SING_MEA;
		}
		else if(read_data == 0x08)/*self-test mode*/
		{
			*mode = AK8975_MODE_SELF_TEST;
		 }
		else if(read_data == 0X0F)/*fuse ROM access mode*/
		{
			*mode = AK8975_MODE_FUSE_ROM;
		}
		else 
		{
			/*nothing.*/
		}
		return 0;
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	Auto Self Test Control   , REG 0X0C

	bit 6 [SELF]  "0" -> normal ; "1" ->  Generate magnetic field for self-test
	** bit 7 , bit 5~0 must be set "0" !!
	** Do not write "1" to any bit other than SELF bit in ASTC register. If "1"  is written to any bit other than SELF bit, normal measurement can not be done.
*/
CHAR8S AK8975_AUTO_TEST(AK8975_AXIS *data)
{
	CHAR8S status = 0,status1 = 0;
	CHAR8U count = 0;	
	CHAR8U raw_data[6] = {0};

		i2c_stop_hang();	
		i2c_stop_hang();	
		
		/*set the ASTC reg[0x0C] SELF bit[6] & any bit must set "0". -> write 0x40.*/
		status = i2c_write_1_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_ASTC,0X40);
		if(status != 1 )
		{
			return -1; /*write fail.*/
		}
		
		/*set Self Test mode*/
		status = AK8975_MODE_SET(AK8975_MODE_SELF_TEST);	
		if(status !=0) 
		{
			return -1;/*set mode fail*/
		}
		
       	/*  status= PINA.6;*/
		for(count = 0 ; count < 8 ; count ++)	/*retry 8 times */ 
		{
			AK8975_DELAY(0xFF0);
		
			status = AK8975_CHK_RDY_ERR();		/*check RDRY , HOFL , DERR bit*/

			if(status == 0)
			{
				/*data ready & no error*/
				break;
			}
			else if(status == -1 )
			{
				/*read status fail*/
				status1 = -1;
			}
			else if(status == -2 )
			{
				/*data not ready or error flag!*/
				status1 = -2;
			}
		}

		i2c_stop_hang();
		i2c_stop_hang();
		
		/* read x,y,z axis raw data*/
		status = i2c_read_n_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_HXL,6,&raw_data[0]);
		if(status !=1)
		{
			return -1; 	/* read fail*/
		}
		
		i2c_stop_hang();
		i2c_stop_hang();
		/* ** after read data , clear the ASTC reg[0x0C] SELF bit[6] & any bit must set "0". -> write 0x00.*/
		status = i2c_write_1_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_ASTC,0X00);
		if(status !=1)
		{
			return -1; 	/* write fail.*/		
		}
		
		data->X_AXIS =     (raw_data[1]<<8)   + raw_data[0];
		data->Y_AXIS =     (raw_data[3]<<8)   + raw_data[2];		
		data->Z_AXIS =     (raw_data[5]<<8)   + raw_data[4];	
               
		AK8975_CAL_AXIS(data);	

		return 0;	/* read success ok!*/

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	read ASA register , REG 0X10 , 0X11 , 0X12
	to calculate & fix the axis.
	after read REG 0X10~0X12 , set mode -> power down mode '0000'
*/
CHAR8S AK8975_READ_ASA_ADJ(AK8975_ASA_REG *asa)
{
	CHAR8S status = 0;
	CHAR8U asa_data[3] = {0};
		
		status = AK8975_MODE_SET(AK8975_MODE_FUSE_ROM);
		if(status !=0)
		{
			return -1;	/* set fail*/
		}
		
		i2c_stop_hang();
		i2c_stop_hang();
		
		status = i2c_read_n_byte_data(AK8975_SLAVE_ADDRESS,AK8975_REG_ASAX,3,&asa_data[0]);
		if(status !=1) 
		{
			return -1;	/*read fail*/
		}
		
		asa->ASA_X = asa_data[0];
		asa->ASA_Y = asa_data[1];
		asa->ASA_Z = asa_data[2];

		/*after read REG 0X10~0X12 , set power down mode*/
		status = AK8975_MODE_SET(AK8975_MODE_PWR_DOWM);
		if(status !=0) 
		{
			return -1; /* set fail*/
		}
		
		return 0; /* read ok!*/
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	Sensitivity adjustment data for each axis is stored to fuse ROM on shipment.
	** See AK8975 Spec page.28

	Hadj -> adjusted measurement data.
	H 	 -> the measurement data read out from the measurement data register.
	ASA  -> REG_ASAX,Y,Z data. 

	Hadj = H *(      (ASA - 128 ) /256 + 1     )
	Hadj = H *(    ASA   +   128   )  /   256

	H is in the range of -4096 to 4095.  The magnetometer has a range of
	+1229uT & -1229uT .  To go from the raw value to uT is:

	HuT = H * 1229/4096, or  H*3/10.

	Since 1uT = 100 gauss, our final scale factor becomes:

	Hadj = H * ((ASA + 128) / 256) * 3/10 * 100

	Hadj = H * ((ASA + 128) * 30 / 256
*/
void AK8975_CAL_AXIS(AK8975_AXIS *data)
{
	INT16S temp=0;
	FLOAT temp2=0,temp3=0;        

        data->X_AXIS = (data->X_AXIS *3 /10);
        data->Y_AXIS = (data->Y_AXIS *3 /10);
        data->Z_AXIS = (data->Z_AXIS *3 /10); 
         
	/*X axis*/
	temp2 = (FLOAT)data->X_AXIS * (((((FLOAT)AK8975_ASA_XYZ_DATA.ASA_X-128)*0.5)/128)+1);
	data->X_AXIS = (INT16S)temp2;

	/*Y axis*/
        temp2 = (FLOAT)data->Y_AXIS * (((((FLOAT)AK8975_ASA_XYZ_DATA.ASA_Y-128)*0.5)/128)+1);
   	data->Y_AXIS = (INT16S)temp2;
		
	/*Z axis*/
	temp2 = (FLOAT)data->Y_AXIS * (((((FLOAT)AK8975_ASA_XYZ_DATA.ASA_Y-128)*0.5)/128)+1);
	data->Z_AXIS = (INT16S)temp2;              
        
			
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*for AK8975 delay*/
void AK8975_DELAY(INT32U count)
{
	INT32U delay_cnt = 0;
	CHAR8U delay = 0 ;
	
		for(delay_cnt=0;delay_cnt<=count;delay_cnt++)
		{
			/* nonsense */
		 	delay ++;
		 	delay --;
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* simple calculate */
/*calculate the AK8975L azimuth*/
/* return degree */
/* the coordinates is 

			y
			^
			|     /
                   	|    /
			|   /  
                   	|  /		
			| /     degree
                   	|/  
			------------> x

     and point to South
*/
FLOAT AK8975_GET_AZIMUTH(AK8975_AXIS data)
{
	FLOAT x_data,y_data,result_data,cal;

		/* calculate the azimuth */
		result_data =atan2((FLOAT)data.Y_AXIS,(FLOAT)data.X_AXIS)*180.0/3.14159 +180.0;
		return result_data;
}
/*--------------------------------------------------------------------------------------------------*/
/********************************************** SYSTEM **************************************************/
