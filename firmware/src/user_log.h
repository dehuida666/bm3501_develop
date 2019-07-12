
#ifndef __USER_LOG_H__
#define __USER_LOG_H__

#include <stdio.h>

#define USER_DEBUG_FLAG

#ifdef USER_DEBUG_FLAG

#define User_Log   printf

#else

#define User_Log 

#endif
/*
*******************************************************************************
*/

#endif

