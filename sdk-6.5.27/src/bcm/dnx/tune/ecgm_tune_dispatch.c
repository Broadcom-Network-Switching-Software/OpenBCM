/*
 * ecgm_tune_dispatch.c
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
* Include files.
* {
*/
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/tune/ecgm_tune_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
#include <bcm_int/dnx/tune/ecgm_tune.h>
#include "ecgm_tune.h"

/** see header file */
shr_error_e
dnx_ecgm_interface_tune(
    int unit,
    bcm_port_t port,
    int port_rate)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_port_init(unit, port, port_rate));
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_ecgm_port_tune(
    int unit,
    bcm_port_t port,
    int port_rate)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_tune_single_port(unit, port, port_rate));
    }
exit:
    SHR_FUNC_EXIT;
}
