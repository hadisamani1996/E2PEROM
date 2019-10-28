#include "stm32f10x.h"
#include "I2C.h"
#include "EEPROM.h"

static i2c* I2C;
static eeprom* EEPROM;

uint8_t send[100]={0};
uint8_t	read[ 500 ] = { 0 };

int main()
{
	int j,i;
	static uint16_t addre=0;
	I2C->init_i2c();
	for(i=0,j=0;j<63;j++,i++) send[i]=j;
	EEPROM->burst_write(send);
	for(i=0,j=63;j<126;j++,i++) send[i]=j;
	EEPROM->burst_write(send);
	for(i=0,j=126;j<189;j++,i++) send[i]=j;
	EEPROM->burst_write(send);
	for(i=0,j=189;j<252;j++,i++) send[i]=j;
	EEPROM->burst_write(send);
	for(i=0,j=252;j<315;j++,i++) send[i]=j;
	EEPROM->burst_write(send);
	EEPROM->page_read(addre,255,&read[0]);
	addre=0;
	addre=EEPROM->find_address();
	while(1)
	{
		
	}
}





