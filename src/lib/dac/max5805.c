#include "max5805.h"

//device i2c address
static uint8_t address = 0x36;

//buffer register
static uint8_t b[2] = {0x00,0x00};


//real reference voltage of the max5805.
static float ref = 0.0;


int max5805_init(uint8_t address_t)
{
  int returncode = 0;

   i2c_init();

		address = address_t;
		//reset device
		b[0] = 0x00;
		b[1] = 0x00;
		returncode = i2c_writeReg(address, 0b00101101, b, 2);
		if (returncode != 0)
				return 1;
		//set default value to middle scale
		b[0] = 0x00;
		b[1] = 0b01000000;
		returncode = i2c_writeReg(address, 0b01100000, b, 2);
		if (returncode != 0)
				return 1;
		//clear output, codeload to middlescale
		b[0] = 0b10000000;
		b[1] = 0b00000000;
		returncode = i2c_writeReg(address, 0b10100000, b, 2);
		if (returncode != 0)
				return 1;
		return 0;
}

//enable and disable output
int max5805_outenable(bool enable){
  int returncode = 0;

		if(enable)
		{
				//output normal
				b[0] = 0x00;
				b[1] = 0x00;
				returncode  = i2c_writeReg(address, 0b01000000, b, 2);
		}
		else
		{
				//output high impedance
				b[0] = 0x00;
				b[1] = 0b11000000;
				returncode = i2c_writeReg(address, 0b01000000, b, 2);
		}
		if (returncode != 0)
				return 1;
		return 0;
}

int  max5805_setref(float ref_t)
{
  int returncode = 0;

		float acceptedDiff = 0.1;
		uint8_t reg_t = 0x00;

		if(fabsf(ref_t-2.5) < acceptedDiff)
				reg_t = 0x01;
		else if(fabsf(ref_t-2.048) < acceptedDiff)
				reg_t = 0x02;
		else if(fabsf(ref_t-4.096) < acceptedDiff)
				reg_t = 0x03;
		else
				return 1;

		//useless
		b[0] = 0x00;
		b[1] = 0x00;

		//REF command, always on, drive pin
		reg_t |= 0b00101100;

		returncode = i2c_writeReg(address, reg_t, b, 2);
		if (returncode != 0)
				return 1;

		//if i2c_writeReg, set global reference voltage
		ref = ref_t;

		return 0;
}

/*
   {
   float getref()
   //set reference to 2.5v
   //modify if needed, the two lsb change the reference
   returncode = i2c_readReg(address, 0b11110000, b, 2);
   b[0] &= 0b00000011;
   switch b[0]
   {
    case 0b00000000:
    //fuck, ref is ext
    break;

    case 0b00000001:
    return 2.5;
    break;

    case 0b00000010:
    return 2.048;
    break;

    case 0b00000011:
    return 4.096;
    break;
   }
   }
 */



 int max5805_codeload(float input)
 {
   int returncode = 0;

 	unsigned int d; ///output code

     //d=(Vout*2^n)/vref
     d = (input * (4096.0/ref));

     //max code to 2^12-1 to avoid overflow (and a 0v output!)
     d = d < 4096 ? d : 4095;

     d = d << 4;

 	//split into two bytes
     b[0] = d >> 8;
 	b[1] = d & 0xFF;

 	returncode = i2c_writeReg(address, 0b10100000, b, 2);
     	if (returncode != 0)
 				return 1;
     return 0;
 }

 int max5805_codeloadRaw(uint16_t input)
 {
   int returncode = 0;

 		//split into two bytes
         b[0] = input >> 8;
 		b[1] = input & 0xFF;

 		returncode = i2c_writeReg(address, 0b10100000, b, 2);
     	if (returncode != 0)
 				return 1;
    return returncode;
 }