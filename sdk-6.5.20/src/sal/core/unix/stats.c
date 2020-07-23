/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    stats.c
 * Purpose: cpu occupation stats.
 */


#include <sys/types.h>
#include <sal/core/stats.h>
#include <sal/core/libc.h>

#if defined(linux)
#include <stdio.h>
#endif


#if defined(linux)
STATIC uint64 sal_atouint64(CONST char *str){
    uint64 val = COMPILER_64_INIT(0,0);

    while(*str >= '0' && *str <= '9') {
        COMPILER_64_UMUL_32(val, 10);
        COMPILER_64_ADD_32(val, *str - '0');
        ++str;
    }
    return val;
}

STATIC CONST char *advance_to_next_number(CONST char *str) {
    while(*str && (*str < '0' || *str > '9')) {
        ++str;
    }
    return (*str) ? str : NULL;
}

#define ADVANCE_TO_NEXT_NUMBER_RETURN_FALSE_ON_ERROR(_cur_location, _fp)\
    _cur_location = advance_to_next_number(_cur_location);              \
    if(!_cur_location) {                                                \
        fclose(_fp);                                                    \
        return FALSE;                                                   \
    }

#define TAKE_NUMBER_AND_ADVANCE(_str, _number)       \
    _number = sal_atouint64(_str);                   \
    while(*_str && (*_str >= '0' || *_str <= '9')) { \
        ++_str;                                      \
    }
    

int 
sal_cpu_stats_get(sal_cpu_stats_t* cpu_stats)
{
    FILE* f;
    uint64 nice = COMPILER_64_INIT(0,0);
    uint64 iowait = COMPILER_64_INIT(0,0);
    char line[200];
    CONST char *seperator;
    
    COMPILER_64_ZERO(cpu_stats->user);
    COMPILER_64_ZERO(cpu_stats->kernel);
    COMPILER_64_ZERO(cpu_stats->idle);
    COMPILER_64_ZERO(cpu_stats->total);
    
    if ((f=fopen("/proc/stat", "r" )) == NULL) {
        return FALSE;
    }

    fgets(line, sizeof(line), f);
    seperator = sal_strstr(line, "cpu");
    if(!seperator) {
        fclose(f);
        return FALSE;
    }

    ADVANCE_TO_NEXT_NUMBER_RETURN_FALSE_ON_ERROR(seperator, f);
    TAKE_NUMBER_AND_ADVANCE(seperator, cpu_stats->user);
    ADVANCE_TO_NEXT_NUMBER_RETURN_FALSE_ON_ERROR(seperator, f);
    TAKE_NUMBER_AND_ADVANCE(seperator, nice);
    ADVANCE_TO_NEXT_NUMBER_RETURN_FALSE_ON_ERROR(seperator, f);
    TAKE_NUMBER_AND_ADVANCE(seperator, cpu_stats->kernel);
    ADVANCE_TO_NEXT_NUMBER_RETURN_FALSE_ON_ERROR(seperator, f);
    TAKE_NUMBER_AND_ADVANCE(seperator, cpu_stats->idle);
    ADVANCE_TO_NEXT_NUMBER_RETURN_FALSE_ON_ERROR(seperator, f);
    TAKE_NUMBER_AND_ADVANCE(seperator, iowait);

    fclose(f);
    
    COMPILER_64_ADD_64(cpu_stats->kernel, nice);
    COMPILER_64_ADD_64(cpu_stats->kernel, iowait);

    COMPILER_64_ADD_64(cpu_stats->total, cpu_stats->user);
    COMPILER_64_ADD_64(cpu_stats->total, cpu_stats->idle);
    COMPILER_64_ADD_64(cpu_stats->total, cpu_stats->kernel);

    return TRUE;
} /* sal_cpu_stats_get */

#else

int 
sal_cpu_stats_get(sal_cpu_stats_t* cpu_stats)
{
    COMPILER_64_ZERO(cpu_stats->user);
    COMPILER_64_ZERO(cpu_stats->kernel);
    COMPILER_64_ZERO(cpu_stats->idle);
    COMPILER_64_ZERO(cpu_stats->total);
       
    return FALSE;
}
#endif

