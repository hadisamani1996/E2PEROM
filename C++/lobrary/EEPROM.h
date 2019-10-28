#ifndef	__EEPROM_H
#define	__EEPROM_H

#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "I2C.h"

#define EEPROM_I2C								I2C1
#define EEPROM_START_ADDRESS			500		
#define HARDWARE_ADDRESS					0xA0
#define ADDRESS_BYTES							16
#define MAX_ADDRESS								0xffff
#define PAGE_SIZE									128
#define DATA_SIZE									63
//#define ID_ID										3
#define ID_BYTE										4
class eeprom
{
	public:
		eeprom();
		bool flag_time_overflow( uint32_t* i , uint32_t timeout );
		void delay_ms( uint32_t ms );
		bool check_address(uint16_t address);
		bool i2c_byte_write(uint16_t address,uint8_t data);
		bool i2c_byte_read(uint16_t address,uint8_t* data);
		void byte_write(uint16_t address,uint8_t data);
		void byte_read(uint16_t address,uint8_t* data);
		bool i2c_page_write(uint16_t address, uint8_t data_bytes , uint8_t* data);
		bool i2c_page_read(uint16_t address, uint8_t data_bytes , uint8_t* receive);
		void page_write(uint16_t address, uint8_t data_bytes , uint8_t* data);
		void page_read(uint16_t address, uint8_t data_bytes , uint8_t* receive);
		void burst_write(uint8_t* data);
		bool burst_read(uint16_t address,uint32_t nbyte,uint8_t* receive_data);
		uint16_t find_address(void);
		uint32_t id_read(uint16_t id_address);
		bool id_write(uint16_t id_address,uint32_t id);
		uint16_t calculate_next_address(uint16_t address);
	private:

};
#endif
	
	





