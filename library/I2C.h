#ifndef	__I2C_H
#define	__I2C_H
#include "stm32f10x.h"
class i2c {
	public:
		static i2c* instance();
		void init_clock();
		void init_gpio();
		void init_hardware();
		void init_i2c();
		void deinit_i2c();
		bool flag_time_overflow( int* i , int timeout );
		void delay_ms( uint32_t ms );
		void handler();
	private:
		i2c();
};
#endif





