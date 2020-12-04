/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Console Sink
 */

#include <shared/bslenum.h>
#include <appl/diag/bsldnx.h>

#include <sal/core/libc.h> 
#include <sal/appl/io.h> 
#include <appl/diag/bslsink.h>
#include <appl/diag/bslcons.h>
#ifdef BCM_DNX_SUPPORT
/* { */
#include <shared/shrextend/shrextend_error.h>
#include <appl/diag/bslenable.h>
/* } */
#endif
STATIC int
bsldnx_cons_vfprintf(void *file, const char *format, va_list args)
{
    return sal_vprintf(format, args);
}

STATIC int
bsldnx_cons_check(bsl_meta_t *meta)
{
    return bslcons_is_enabled();
}

STATIC int
bsldnx_cons_init(int* sink_id)
{
    static bslsink_sink_t console_sink;
    bslsink_sink_t *sink = bslsink_sink_find("dnx console");

    if (sink == NULL) { /*no dnx console - create a new one*/
        /* Create console sink */
        sink = &console_sink;
        bslsink_sink_t_init(sink);
        sal_strncpy(sink->name, "dnx console", sizeof(sink->name));
        sink->vfprintf = bsldnx_cons_vfprintf;
        sink->check = bsldnx_cons_check;
        sink->enable_range.min = bslSeverityOff+1;
        sink->enable_range.max = bslSeverityCount-1;
        sal_strncpy(sink->prefix_format, "%f[%l]%F unit %u:",
                    sizeof(sink->prefix_format));
        sink->prefix_range.min = bslSeverityOff+1;
        sink->prefix_range.max = bslSeverityWarn;
        sink->options = BSLSINK_OPT_NO_ECHO;

        /* Clear all unit, they will be set in bsldnx_mgmt_init */
        SHR_BITCLR_RANGE(sink->units, 0, BSLSINK_MAX_NUM_UNITS);
        *sink_id = bslsink_sink_add(sink);
    } else{
        *sink_id = sink->sink_id;
    }
    
    return 0;
}

STATIC int
bsldnx_unit_move(int unit, int old_sink_id, int new_sink_id)
{
    bslsink_sink_t *old_sink, *new_sink;

    old_sink = bslsink_sink_find_by_id(old_sink_id);
    if (old_sink == NULL){
        return -1;
    }

    new_sink = bslsink_sink_find_by_id(new_sink_id);
    if (new_sink == NULL){
        return -1;
    }
    SHR_BITCLR(old_sink->units, unit);
    SHR_BITCLR(old_sink->units, BSLSINK_UNIT_UNKNOWN);
    SHR_BITSET(new_sink->units, unit);
    SHR_BITSET(new_sink->units, BSLSINK_UNIT_UNKNOWN);
    return 0;
}
#ifdef BCM_DNX_SUPPORT
/* { */
/*
 * Set minimal/maximal severity level for displaying the 'prefix' as defined for DNX.
 * The 'prefix' is, by standard usage, 'line number', 'procedure name', 'file name', etc.
 */
static int
  bsldnx_set_prefix_range_min_max(bsl_severity_t min_severity, bsl_severity_t max_severity)
{
    bslsink_sink_t *sink ;
    int ret ;

    ret = 0 ;
    sink = bslsink_sink_find("dnx console") ;
    /*
     * If severity is out of range then do nothing.
     */
    if ((min_severity < (bslSeverityOff + 1)) || (min_severity >= bslSeverityCount)) {
        ret = 1 ;
        goto exit ;
    }
    if ((max_severity < (bslSeverityOff + 1)) || (max_severity >= bslSeverityCount)) {
        ret = 2 ;
        goto exit ;
    }
    /*
     * If there is no sink for 'dnx' (yet) then do nothing.
     */
    if (sink != NULL) {
        sink->prefix_range.min = min_severity ;
        sink->prefix_range.max = max_severity ;
    } else{
        ret = 3 ;
        goto exit ;
    }
exit:
    return (ret) ;
}
/*
 * Get minimal/maximal severity level for displaying the 'prefix' as defined for DNX.
 * The 'prefix' is, by standard usage, 'line number', 'procedure name', 'file name', etc.
 */
static int
  bsldnx_get_prefix_range_min_max(bsl_severity_t *min_severity, bsl_severity_t *max_severity)
{
    bslsink_sink_t *sink ;
    int ret ;

    ret = 0 ;
    sink = bslsink_sink_find("dnx console") ;
    /*
     * If there is no sink for 'dnx' (yet) then return error and '*xxx_severity=0'.
     */
    if (sink != NULL) {
        *min_severity = sink->prefix_range.min ;
        *max_severity = sink->prefix_range.max ;
    } else{
        *min_severity = *max_severity = 0 ;
        ret = 1 ;
        goto exit ;
    }
exit:
    return (ret) ;
}
/* } */
#endif
int
bsldnx_mgmt_init(int unit)
{
    int dnx_console_sink_id;
    bslsink_sink_t *console_sink_find = bslsink_sink_find("console");
    if (console_sink_find == NULL){
        return -1;
    }
    if (bsldnx_cons_init(&dnx_console_sink_id) != 0) {
        return -1;
    }
#ifdef BCM_DNX_SUPPORT
/* { */
    /*
     * Open access channel, to 'prefix control' system, for DNX error/debug
     * system. See LOG_DEBUG_EX.
     */
    set_proc_get_prefix_range_min_max(bsldnx_get_prefix_range_min_max) ;
    set_proc_set_prefix_range_min_max(bsldnx_set_prefix_range_min_max) ;
    /*
     * Open access channel, to 'severity control' system, for DNX error/debug
     * system. See SHR_SET_SEVERITY_FOR_MODULE/SHR_GET_SEVERITY_FOR_MODULE.
     */
    set_proc_bslenable_get(bslenable_get) ;
    set_proc_bslenable_set(bslenable_set) ;
/* } */
#endif
    return bsldnx_unit_move(unit,
                            console_sink_find->sink_id,
                            dnx_console_sink_id);
}

