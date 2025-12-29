#include <time.h>
#include <stdint.h>

void time_delay_ms(uint32_t ms)
{
	struct timespec ts = {
		.tv_sec = ms / 1000,
		.tv_nsec = (ms % 1000) * 1000000
	};
	nanosleep(&ts, NULL);
}
