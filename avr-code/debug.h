#pragma once

#ifdef XDEBUG
void DEBUG_init(void);
void DEBUG_msg(const char *msg, ...);
#else
#define DEBUG_init()
#define DEBUG_msg(msg, ...)
#endif
