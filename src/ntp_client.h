#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <cstdint>

/// @returns current time since epoc in seconds, on error returns 0
uint32_t ntp_update();

#endif //NTP_CLIENT_H
