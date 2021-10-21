/*
 * $Id: flowtracker.h
 */


#ifndef _SOC_FLOWTRACKER_H
#define _SOC_FLOWTRACKER_H

#include <soc/mem.h>

extern int _soc_flowtracker_mem_skip(int unit, soc_mem_t mem);

extern int _soc_flowtracker_reg_skip(int unit, soc_mem_t reg);

extern int _soc_flowtracker_counters_enable(int unit);

#endif /* _SOC_FLOWTRACKER_H */
