#include "EEPROM.h"

static uint32_t last_address=EEPROM_START_ADDRESS;
static uint32_t id=0;
uint32_t temporary_data[PAGE_SIZE]={0};
void burst_write(uint8_t* send_data)
{
	uint32_t virtual_address;
	uint32_t remain_page;
	//uint32_t remain_data;
	unsigned int remain_data;
	uint16_t point_data;
	virtual_address=calculate_next_address(last_address);
	if(MAX_ADDRESS<virtual_address)
	{
		last_address=EEPROM_START_ADDRESS;
	}
	remain_data=ID_BYTE;
	while(remain_data>0)
	{
		if(remain_page>remain_data)
		{
			//ادرس را در حافظه بنويس
			id_write(last_address,id);
			last_address+=remain_data;
			remain_data=0;
		}
		else
		{
			byte_write(last_address,id>>8*remain_page);
			last_address++;
			remain_page=PAGE_SIZE-(last_address%PAGE_SIZE);
		}
	}	
	point_data=0;
	remain_data=DATA_SIZE;
	remain_page=PAGE_SIZE-(last_address%PAGE_SIZE);
	//remain_page=PAGE_SIZE-(page_number*PAGE_SIZE);
	
	while(remain_data>0)
	{
		if(remain_page>remain_data)
		{
			page_write(last_address,remain_data,(send_data+point_data));
			last_address+=remain_data;
			remain_data=0;
		}
		else
		{
			page_write(last_address,remain_page,(send_data+point_data));
			remain_data=remain_data-remain_page;
			point_data+=remain_page;
			
			last_address+=remain_page;
			remain_page=PAGE_SIZE-(last_address%PAGE_SIZE);
		}
	}
	id++;
}

uint8_t burst_read(uint16_t read_address,uint32_t nbyte,uint8_t* receive_data)
{
	
	if(!check_address(read_address)) return 0;
	//maybe you can wite better function
	read_address+=ID_BYTE;
	page_read(read_address,nbyte,receive_data);
	return 1;
}
uint8_t check_address(uint16_t address)
{
	uint32_t virtual_address;
	virtual_address=calculate_next_address(last_address);
	if(virtual_address>MAX_ADDRESS) return ERROR;
	return SUCCESS;
}
uint8_t i2c_page_write(uint16_t address, uint8_t data_bytes , uint8_t* data)
{
	uint32_t counter,MaxTime=2000;
	switch(check_address(address))
	{
		case 0:
			return 0;
		case 1:
			I2C_GenerateSTART(EEPROM_I2C,ENABLE);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Transmitter);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) 
				{
					return 0;
				}
			}
			switch(ADDRESS_BYTES)
			{
				case 8:
					I2C_SendData(EEPROM_I2C,address);
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
				break;
				case 16:
					I2C_SendData(EEPROM_I2C,(uint8_t)((address&0xff00)>>8));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
					I2C_SendData(EEPROM_I2C,(uint8_t)(address&0x00ff));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
				break;
			}
			while(data_bytes--)
			{
				I2C_SendData(EEPROM_I2C,*data++);
				counter=0;
				while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
				{
					if(flag_time_overflow( &counter , MaxTime )) return 0;
				}
			}
			I2C_GenerateSTOP(EEPROM_I2C,ENABLE);
			counter=0;
			while(I2C_GetFlagStatus(EEPROM_I2C,I2C_FLAG_BUSY))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
	}
	delay_ms( 30 );
	return 1;		
}
uint8_t i2c_page_read(uint16_t address, uint8_t data_bytes , uint8_t* receive)
{
	uint32_t counter,MaxTime=2000;
	switch(check_address(address))
	{
		case 0:
			return 0;
		case 1:
			I2C_GenerateSTART(EEPROM_I2C,ENABLE);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Transmitter);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			counter=0;
			switch(ADDRESS_BYTES)
			{
				case 8:
					I2C_SendData(EEPROM_I2C,address);
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
				break;
				case 16:
					I2C_SendData(EEPROM_I2C,(uint8_t)((address&0xff00)>>8));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
					I2C_SendData(EEPROM_I2C,(uint8_t)(address&0x00ff));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
				break;
			}
			I2C_GenerateSTART(EEPROM_I2C,ENABLE); 
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Receiver);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_AcknowledgeConfig(EEPROM_I2C , ENABLE);
			while(data_bytes--)
			{
				counter=0;
				while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_RECEIVED))
				{
					if(flag_time_overflow( &counter , MaxTime )) return 0;
				}
				*receive++=I2C_ReceiveData(EEPROM_I2C);
			}
			I2C_AcknowledgeConfig(EEPROM_I2C,DISABLE);
			I2C_GenerateSTOP(EEPROM_I2C,ENABLE);
			counter=0;
			while(I2C_GetFlagStatus(EEPROM_I2C,I2C_FLAG_BUSY))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
	}
	delay_ms( 30 );
	return 1;
}
uint8_t i2c_byte_write(uint16_t address,uint8_t data)
{
	uint32_t counter,MaxTime=2000;
	switch(check_address(address))
	{
		case 0:
			return ERROR;
		case 1:
			I2C_GenerateSTART(EEPROM_I2C,ENABLE);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Transmitter);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			switch(ADDRESS_BYTES)
			{
				case 8:
					I2C_SendData(EEPROM_I2C,address);
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
				break;
				case 16:
					I2C_SendData(EEPROM_I2C,(uint8_t)((address&0xff00 )>>8));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}	
					I2C_SendData(EEPROM_I2C,(uint8_t)((address&0x00ff)));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
				break;
			}
			I2C_SendData(EEPROM_I2C,data);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_GenerateSTOP(EEPROM_I2C,ENABLE);
			counter=0;
			while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
		break;
	}
	delay_ms( 30 );
	return 1;
}
uint8_t i2c_byte_read(uint16_t address,uint8_t* data)
{
	uint32_t counter,MaxTime=2000;
	switch(check_address(address))
	{
		case 0:
			return 0;
		case 1:
			I2C_GenerateSTART(EEPROM_I2C,ENABLE);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Transmitter);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			switch(ADDRESS_BYTES)
			{
				case 8:
					I2C_SendData(EEPROM_I2C,address);
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
					break;
				case 16:
					I2C_SendData(EEPROM_I2C,(uint8_t)((address&0xff00 )>>8));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}	
					I2C_SendData(EEPROM_I2C,(uint8_t)((address&0x00ff)));
					counter=0;
					while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
					{
						if(flag_time_overflow( &counter , MaxTime )) return 0;
					}
				break;
			}
			I2C_GenerateSTART(EEPROM_I2C,ENABLE);	
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Receiver);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_RECEIVED))//I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
			{
				if(flag_time_overflow ( &counter , MaxTime )) return 0;
			}
			*data=I2C_ReceiveData(EEPROM_I2C);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_RECEIVED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_AcknowledgeConfig(EEPROM_I2C, DISABLE);
			I2C_GenerateSTOP(EEPROM_I2C, ENABLE);
			counter=0;
			while(I2C_GetFlagStatus(EEPROM_I2C, I2C_FLAG_BUSY))
			{
				if(flag_time_overflow ( &counter , MaxTime )) return 0;
			}
	}
	delay_ms( 30 );
	return 1;
}
void byte_write(uint16_t address,uint8_t data)
{
	uint8_t done=0;
	do
	{
		done=i2c_byte_write(address,data);
	}while(!done);
}
void byte_read(uint16_t address,uint8_t* data)
{
	uint8_t done=0;
	do
	{
		done=i2c_byte_read(address,data);
	}while(!done);
}
void page_write(uint16_t address, uint8_t data_bytes , uint8_t* data)
{
	uint8_t done=0;
	do
	{
	done=i2c_page_write(address,data_bytes,data);
	}while(!done);
}
void page_read(uint16_t address, uint8_t data_bytes , uint8_t* receive)
{
	uint8_t done=0;
	do
	{
		done=i2c_page_read(address,data_bytes,receive);
	}while(!done);
}
uint8_t id_write(uint16_t id_address,uint32_t id)
{
	int byte_id=ID_BYTE;
	uint32_t counter,MaxTime=2000;
	I2C_GenerateSTART(EEPROM_I2C,ENABLE);
	counter=0;
	while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Transmitter);
	counter=0;
	while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	switch(ADDRESS_BYTES)
	{
		case 8:
			I2C_SendData(EEPROM_I2C,id_address);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
		break;
		case 16:
			I2C_SendData(EEPROM_I2C,(uint8_t)((id_address&0xff00)>>8));
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_SendData(EEPROM_I2C,(uint8_t)(id_address&0x00ff));
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
		break;
	}
	while(byte_id--)
	{
		I2C_SendData(EEPROM_I2C,(id>>byte_id*8));
		counter=0;
		while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if(flag_time_overflow( &counter , MaxTime )) return 0;
		}
	}
	I2C_GenerateSTOP(EEPROM_I2C,ENABLE);
	counter=0;
	while(I2C_GetFlagStatus(EEPROM_I2C,I2C_FLAG_BUSY))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	delay_ms( 30 );
	return 1;
}
uint32_t id_read(uint16_t id_address)
{
	int byte_id=ID_BYTE;
	uint8_t receive_data;
	uint32_t counter,MaxTime=2000;
	I2C_GenerateSTART(EEPROM_I2C,ENABLE);
	counter=0;
	while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Transmitter);
	counter=0;
	while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	counter=0;
	switch(ADDRESS_BYTES)
	{
		case 8:
			I2C_SendData(EEPROM_I2C,id_address);
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
		break;
		case 16:
			I2C_SendData(EEPROM_I2C,(uint8_t)((id_address&0xff00)>>8));
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
			I2C_SendData(EEPROM_I2C,(uint8_t)(id_address&0x00ff));
			counter=0;
			while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				if(flag_time_overflow( &counter , MaxTime )) return 0;
			}
		break;
	}
	I2C_GenerateSTART(EEPROM_I2C,ENABLE); 
	counter=0;
	while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	I2C_Send7bitAddress(EEPROM_I2C,HARDWARE_ADDRESS,I2C_Direction_Receiver);
	counter=0;
	while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	I2C_AcknowledgeConfig(EEPROM_I2C , ENABLE);
	while(byte_id--)
	{
		counter=0;
		while(!I2C_CheckEvent(EEPROM_I2C,I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			if(flag_time_overflow( &counter , MaxTime )) return 0;
		}
		receive_data=(I2C_ReceiveData(EEPROM_I2C)<<byte_id*8);
		
	}
	I2C_AcknowledgeConfig(EEPROM_I2C,DISABLE);
	I2C_GenerateSTOP(EEPROM_I2C,ENABLE);
	counter=0;
	while(I2C_GetFlagStatus(EEPROM_I2C,I2C_FLAG_BUSY))
	{
		if(flag_time_overflow( &counter , MaxTime )) return 0;
	}
	delay_ms( 30 );
	return receive_data;
}
uint16_t find_address(void)
{
	uint16_t virtual_address=0;
	uint32_t max_id;
	uint16_t max_id_address;
	uint32_t current_id;
	max_id=id_read(virtual_address);
	max_id_address=virtual_address;
	while((virtual_address+DATA_SIZE+ID_BYTE)<MAX_ADDRESS)
	{
		virtual_address=calculate_next_address(virtual_address);
		current_id=id_read(virtual_address);
		if(current_id==0xffffffff) 
		{
			max_id_address=virtual_address;
			break;
		}
		if(current_id>max_id)
		{
			max_id=current_id;
			max_id_address=virtual_address;
		}
		else
		{
			max_id_address=virtual_address;
			break;
		}
	}
	return max_id_address;
}
uint16_t calculate_next_address(uint16_t address)
{
	return (address+DATA_SIZE+ID_BYTE);
}

uint8_t flag_time_overflow( uint32_t* i , uint32_t timeout )
{
	uint8_t overflow = 0;
	*i+=1;
	if( *i >= timeout )
	{
		*i=0;
		overflow = 1;
		deinit_i2c();
		for(int j=0; j<3000; j++){__asm("nop");}
		init_i2c();
		for(int j=0; j<3000; j++){__asm("nop");}
	}
	return overflow;
}
void delay_ms( uint32_t ms )
{
	volatile uint32_t nCount;
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);
	nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
	for (; nCount!=0; nCount--);
}


