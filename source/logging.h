#include "stdio.h"
#ifdef dedebug
#define ledstatus(x)	PRINTF(x)
#else
#define ledstatus(x)
#endif
#ifdef dedebug
#define logtask(x)	PRINTF(x)
#else
#define logtask(x)
#endif
