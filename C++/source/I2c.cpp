#include "I2C.h"

i2c* i2c::instance()
{
	static i2c instance;
	return &instance;
}
i2c::i2c()
{
	
}
void i2c::init_clock()
{	
	//RCC_ClocksTypeDef RCC_Clocks;
	//RCC_PCLK1Config( RCC_HCLK_Div4 );
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1 , ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE );
	//RCC_GetClocksFreq( &RCC_Clocks );
}
void i2c::init_gpio() 
{
	GPIO_InitTypeDef	GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB , &GPIO_InitStruct );
}
void i2c::init_hardware() 
{
	I2C_InitTypeDef	I2C_InitStruct;
	/**
	*ClockSpeed -> speed of I2C communication
	*ClockSpeed and DutyCycle use for determine Thigh and Tlow
	*see datasheet.
	**/
	I2C_StructInit( &I2C_InitStruct );
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_16_9; // changed with me
	I2C_InitStruct.I2C_OwnAddress1 = 1;
	I2C_InitStruct.I2C_Mode =	I2C_Mode_I2C;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init( I2C1 , &I2C_InitStruct );
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Cmd( I2C1 , ENABLE );
}
void i2c::deinit_i2c()
{
	GPIO_InitTypeDef  GPIO_InitStructure;  
    
  /* UnConfigure I2C */
  I2C_DeInit(I2C1);
  I2C_Cmd(I2C1, DISABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
           
  /* UnConfigure I2C_MEMS pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB,&GPIO_InitStructure);  
}
void i2c::init_i2c()
{
	init_clock();
	init_gpio();
	init_hardware();
}
