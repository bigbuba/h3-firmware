#include <or1k-support.h>
#include <or1k-sprs.h>
#include "io.h"
#include "gpio.h"
#include "debug.h"
#include "clk.h"
#include "timer.h"

void enable_caches(void)
{
	unsigned addr;

	for (addr = 0; addr < 16 * 1024 + 32 * 1024; addr += 16)
		or1k_icache_flush(addr);

	or1k_icache_enable();
}

void reset(void)
{
	asm("l.j _start");
	asm("l.nop");
}

void handle_exception(uint32_t type, uint32_t pc, uint32_t sp)
{
	if (type == 8) {
		printf("interrupt\n");
	} else if (type == 5) {
		printf("timer\n");
	} else {
		printf("exception %u at pc=%x sp=%x\nrestarting...\n\n", type, pc, sp);
		reset();
	}
}

int main(void)
{
	enable_caches();
	gpio_init();
	uart0_init();
	clk_set_rate(CLK_CPUS, 300000000);

	// turn on leds
	gpio_set_pincfg(GPIO_BANK_A, 15, GPIO_FUNC_OUTPUT);
	gpio_set_pincfg(GPIO_BANK_L, 10, GPIO_FUNC_OUTPUT);
	set_bit(15, gpio_get_data_addr(GPIO_BANK_A));
	set_bit(10, gpio_get_data_addr(GPIO_BANK_L));
        uint8_t led_state = 1;

	puts("\nOpenRISC FW 1.0\n");

	while (1) 
	{
		delay_us(1000000); // 1 sec

		// blink leds so that world knows we're alive
		if ( led_state ) 
		{
			clr_bit(15, gpio_get_data_addr(GPIO_BANK_A));
	                clr_bit(10, gpio_get_data_addr(GPIO_BANK_L));
	                led_state = 0;
			printf("led_state = 0\n");
	        }
		else
		{
			set_bit(15, gpio_get_data_addr(GPIO_BANK_A));
	                set_bit(10, gpio_get_data_addr(GPIO_BANK_L));
	                led_state = 1;
			printf("led_state = 1\n");
	        }
	}

	return 0;
}
