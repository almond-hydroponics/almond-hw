#ifndef PUSH_DATA_H
#define PUSH_DATA_H

#include "timer_override.h"
#include "device.h"

class Push_data
{
public:
	Push_data();

	/// @param values must be array of all fields in proper order. returns true on success (pushes every now and then, not every call)
	/// @param values_n - how many values to be pushed
	/// @param force_update - do we force update
	bool thingspeak_push(const int *values,
						 int index_start,
						 int values_n,
						 bool force_update);

	static const int push_interval_s = 15 * 60;
	static const int buffer_size = 4096;

protected:
	TimerOverride timer;
	static bool thingspeak_push_raw(const int *values,
							 int index_start,
							 int values_n,
							 char *buffer);
};


#endif //PUSH_DATA_H
