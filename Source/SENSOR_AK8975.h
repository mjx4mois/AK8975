/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: E-COMPASS
     File Name		: SENSOR_AK8975.c
     Function 		: SENSOR_AK8975
     Create Date	: 2017/07/11
---------------------------------------------------------------------- */
#ifndef __AK8975_HEADER__
#define __AK8975_HEADER__


//Note : CSB(PIN2) must connect to high , enable  I2C .
//--------------------------------- Define SLAVE ADDRESS -------------------------------------
// AK8975 SLAVE ADDRESS

#define AK8975_CAD0_ADDR_PIN		0	 // PIN13  ,connect  "0" GND or "1" Vcc
#define	AK8975_CAD1_ADDR_PIN		0	 // PIN11  ,connect  "0" GND or "1" Vcc
//---------------------------------
//	CAD1	CAD0		SLV ADDR(8-BIT)
//		0		0			0x18	
//		0		1			0x1A
//		1		0			0x1C
//		1		1			0x1E
//---------------------------------
#if   (AK8975_CAD0_ADDR_PIN == 0)  && (AK8975_CAD1_ADDR_PIN == 0)
	#define AK8975_SLAVE_ADDRESS 		0x18		//SLV ADDR
#elif (AK8975_CAD0_ADDR_PIN == 1)  && (AK8975_CAD1_ADDR_PIN == 0)
	#define AK8975_SLAVE_ADDRESS 		0x1A		//SLV ADDR
#elif (AK8975_CAD0_ADDR_PIN == 0)  && (AK8975_CAD1_ADDR_PIN == 1)
	#define AK8975_SLAVE_ADDRESS 		0x1C		//SLV ADDR
#elif (AK8975_CAD0_ADDR_PIN == 1)  && (AK8975_CAD1_ADDR_PIN == 1)
	#define AK8975_SLAVE_ADDRESS 		0x1E		//SLV ADDR
#endif

//--------------------------------- Define SLAVE ADDRESS -------------------------------------



// ------------- DEFINE AK8975 REGISTER  ------------
#define AK8975_REG_WIA					0x00		//R			ID: 0x48
//------------------------------------------------
#define AK8975_REG_INFO					0x01		//R			
//Device information for AKM.
//------------------------------------------------
#define AK8975_REG_ST1					0x02		//R			Data status 1
//bit 0 [DRDY]    "0" -> normal ; "1" -> when data is "ready" in single measurement mode or self-test mode.
// It returns to  "0"  when any one of ST2 register or measurement data register (HXL to HZH) is read!
//------------------------------------------------
// 2¡¦s complement value .   13-bit resolution .
#define AK8975_REG_HXL					0x03		//R			X-axis data 
#define AK8975_REG_HXH					0x04		//R			X-axis data
#define AK8975_REG_HYL					0x05		//R			Y-axis data
#define AK8975_REG_HYH					0x06		//R			Y-axis data
#define AK8975_REG_HZL					0x07		//R			Z-axis data
#define AK8975_REG_HZH					0x08		//R			Z-axis data
//------------------------------------------------
#define AK8975_REG_ST2					0x09		//R			Data status 2
//bit 3 [HOFL]  Magnetic sensor overflow . "0" -> normal ; "1" -> Magnetic sensor overflow occurred  
//When next measurement start, it returns to ¡§0¡¨.

//bit 2 [DERR] Data Error.  "0" -> normal ; "1" -> Data read error occurred. 
//When ST2 register is read, it returns to "0" .
//------------------------------------------------
// Control mode set
#define AK8975_REG_CNTL					0x0A		//R/W
// bit 3 ~ bit 0       		MODE
//   "0000"			Power-down mode
//   "0001"			Single measurement mode
//   "1000"			Self-test mode
//   "1111"			Fuse ROM access mode
//else prohibited. 
//When CNTL register is accessed to be written, registers from 02H to 09H are initialized.
//------------------------------------------------
#define AK8975_REG_RSV					0x0B		//R/W			**DO NOT ACCESS -> DONT USE THIS REG!
//------------------------------------------------
#define AK8975_REG_ASTC					0x0C		//R/W
//Auto Self Test Control 
//bit 6 [SELF]  "0" -> normal ; "1" ->  Generate magnetic field for self-test
// ** bit 7 , bit 5~0 must be set "0" !!
// ** Do not write "1" to any bit other than SELF bit in ASTC register. If "1"  is written to any bit other than SELF bit, normal measurement can not be done.
//------------------------------------------------
#define AK8975_REG_TS1					0x0D		//R/W			**DO NOT ACCESS -> DONT USE THIS REG! TS1 and TS2 are test registers for shipment test.
#define AK8975_REG_TS2					0x0E		//R/W			**DO NOT ACCESS -> DONT USE THIS REG! TS1 and TS2 are test registers for shipment test.
//------------------------------------------------
#define AK8975_REG_I2CDIS				0x0F		//R/W
//bit 0 [I2CDIS] when read "1" -> I2C disable.
//how to disable I2C ?
//-> write 0x1B to this REG[0x0F] , and the I2CDIS = 1.
//     re-setting I2CDIST to ¡§0¡¨ is prohibited. 
//     To EN I2C , reset AK8975 by re -power on. 
//------------------------------------------------
#define AK8975_REG_ASAX					0x10		//R			Fuse ROM  ,sensitivity adjustment value  **can be read only in Fuse access mode.
#define AK8975_REG_ASAY					0x11		//R			Fuse ROM  ,sensitivity adjustment value  **can be read only in Fuse access mode.
#define AK8975_REG_ASAZ					0x12		//R			Fuse ROM  ,sensitivity adjustment value  **can be read only in Fuse access mode.
//Sensitivity adjustment data for each axis is stored to fuse ROM on shipment.
// ** See AK8975 Spec page.28
//  Hadj = H *(      (ASA - 128 ) /256 + 1     )
// Hadj -> adjusted measurement data.
// H -> the measurement data read out from the measurement data register.
// ASA -> REG_ASAX,Y,Z data. 
// ------------- DEFINE AK8975 REGISTER   ------------


// ------------- DEFINE AK8975 STRUCT   ------------
typedef struct AK8975_DATA
{	//AK8975 resolution -> 13 bit
//Note : Little endian
//      two's complemnt         Hex	  	Dec		Magnetic flux density [uT]
//	0000 1111 1111 1111    0FFF		4095    	 1229(max.)
//	1111 0000 0000 0000    F000		-4096   	-1229(min.)
	INT16S X_AXIS;
	INT16S Y_AXIS;
	INT16S Z_AXIS;	
}AK8975_AXIS;

typedef struct AK8975_ASA
{	//AK8975 resolution -> 13 bit
	CHAR8U ASA_X;
	CHAR8U ASA_Y;
	CHAR8U ASA_Z;
}AK8975_ASA_REG;

typedef enum 
{
	AK8975_MODE_PWR_DOWM =0x01,//Power-down mode
	AK8975_MODE_SING_MEA,//Single measurement mode
	AK8975_MODE_SELF_TEST,//Self-test mode
	AK8975_MODE_FUSE_ROM//Fuse ROM access mode
}AK8975_M;
// ------------- DEFINE AK8975 STRUCT   ------------



//********************************************* SYSTEM *************************************************
//--------------------------------------------------------------------------------------------------
//read AK8975 ID  	, REG 0X00    ; ID VALUE = 0X48
//read AK8975 INFO	, REG 0X01
CHAR8S AK8975_ID(CHAR8U *id, CHAR8U *info);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
 //read AK8975 X,Y,Z axis data , REG 0X03~0X08
 //When CNTL register(0X0A) is accessed to be written, registers from 0x02 to 0x09 are initialized.
CHAR8S AK8975_READ_AXIS(AK8975_AXIS *data);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// read & check data ready ?	, REG 0X02  bit0 DRDY
// read & check data error ?	, REG 0X09  bit2 DERR 	 bit3 HOFL
// if not ready or error -> return -1
// if data ready & no error -> return 0
CHAR8S AK8975_CHK_RDY_ERR(void);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// initial AK8975 sensor  for begin measurement.
// read the ASA register [0x10~0x12] for adjust.
// power on -> mode = '0000' power down mode.
CHAR8S AK8975_INIT(void);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// Control mode set  , REG 0x0A
//set mode 
/*
		AK8975_MODE_PWR_DOWM =0x01,//Power-down mode
		AK8975_MODE_SING_MEA,//Single measurement mode
		AK8975_MODE_SELF_TEST,//Self-test mode
		AK8975_MODE_FUSE_ROM//Fuse ROM access mode
*/
//When CNTL register is accessed to be written, registers from 02H to 09H are initialized.
CHAR8S AK8975_MODE_SET(CHAR8U mode);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// Control mode READ , REG 0x0A
//READ mode 
/*
		AK8975_MODE_PWR_DOWM =0x01,//Power-down mode
		AK8975_MODE_SING_MEA,//Single measurement mode
		AK8975_MODE_SELF_TEST,//Self-test mode
		AK8975_MODE_FUSE_ROM//Fuse ROM access mode
*/
CHAR8S AK8975_MODE_READ(CHAR8U *mode);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//Auto Self Test Control   , REG 0X0C
//
//bit 6 [SELF]  "0" -> normal ; "1" ->  Generate magnetic field for self-test
// ** bit 7 , bit 5~0 must be set "0" !!
// ** Do not write "1" to any bit other than SELF bit in ASTC register. If "1"  is written to any bit other than SELF bit, normal measurement can not be done.
CHAR8S AK8975_AUTO_TEST(AK8975_AXIS *data);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//read ASA register , REG 0X10 , 0X11 , 0X12
//to calculate & fix the axis.
//after read REG 0X10~0X12 , set mode -> power down mode '0000'
CHAR8S AK8975_READ_ASA_ADJ(AK8975_ASA_REG *asa);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//Sensitivity adjustment data for each axis is stored to fuse ROM on shipment.
// ** See AK8975 Spec page.28
//  Hadj = H *(      (ASA - 128 ) /256 + 1     )
//  Hadj = H *(ASA+128)/256
// Hadj -> adjusted measurement data.
// H 	 -> the measurement data read out from the measurement data register.
// ASA  -> REG_ASAX,Y,Z data. 
void AK8975_CAL_AXIS(AK8975_AXIS *data);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//for AK8975 delay
void AK8975_DELAY(INT32U count);
//--------------------------------------------------------------------------------------------------
//********************************************* SYSTEM *************************************************




#endif 		//	#ifndef __AK8975_HEADER__
