#include <stdio.h>
#include <time.h>

time_t get_timestamp()
{
	time_t current_timestamp;
	current_timestamp = time(NULL);
	return current_timestamp;
}
