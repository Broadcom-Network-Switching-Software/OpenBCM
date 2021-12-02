
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_REG

 /*************
 * INCLUDES  *
 *************/
#include <shared/shrextend/shrextend_debug.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/port/port_ilkn_reg_access.h>
#include <bcm_int/dnx/port/port_flexe_reg_access.h>
#include <bcm_int/dnx/port/port_custom_reg_access.h>
/*************
*  MACROS*
*************/

/*************
* TYPE DEFS *
*************/

/*************
* GLOBALS*
*************/

/*************
* FUNCTIONS *
*************/

/** see header file */
shr_error_e
dnx_custom_access_init(
    int unit)
{
    soc_custom_reg_access_t *reg_access;

    SHR_FUNC_INIT_VARS(unit);

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    reg_access->flexe_access_lock = sal_mutex_create("flexe_access_lock");
    if (reg_access->flexe_access_lock == NULL)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INIT);
    }

    reg_access->custom_reg32_get = dnx_custom_reg32_get;
    reg_access->custom_reg32_set = dnx_custom_reg32_set;
    reg_access->custom_reg64_get = dnx_custom_reg64_get;
    reg_access->custom_reg64_set = dnx_custom_reg64_set;
    reg_access->custom_reg_above64_get = dnx_custom_reg_above_64_get;
    reg_access->custom_reg_above64_set = dnx_custom_reg_above_64_set;
    reg_access->custom_mem_array_read = dnx_custom_mem_array_read;
    reg_access->custom_mem_array_write = dnx_custom_mem_array_write;
    reg_access->custom_mem_array_fill_range = dnx_custom_mem_array_fill_range;

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_access_deinit(
    int unit)
{
    soc_custom_reg_access_t *reg_access;
    SHR_FUNC_INIT_VARS(unit);

    reg_access = dnx_drv_soc_info_custom_reg_access_get(unit);
    if (reg_access->flexe_access_lock != NULL)
    {
        sal_mutex_destroy(reg_access->flexe_access_lock);
        reg_access->flexe_access_lock = NULL;
    }

    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_is_reg_flexe_core(unit, reg))
    {
        uint32 data_32[SOC_REG_ABOVE_64_MAX_SIZE_U32];
        sal_memset(data_32, 0, sizeof(data_32));
        SHR_IF_ERR_EXIT(dnx_flexe_reg_get(unit, reg, index, data_32));
        *data = data_32[0];
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg32_get(unit, reg, port, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_is_reg_flexe_core(unit, reg))
    {
        uint32 data_32[SOC_REG_ABOVE_64_MAX_SIZE_U32];
        sal_memset(data_32, 0, sizeof(data_32));
        data_32[0] = data;
        SHR_IF_ERR_EXIT(dnx_flexe_reg_set(unit, reg, index, data_32));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg32_set(unit, reg, port, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (soc_is_reg_flexe_core(unit, reg))
    {
        uint32 data_32[SOC_REG_ABOVE_64_MAX_SIZE_U32];
        sal_memset(data_32, 0, sizeof(data_32));
        SHR_IF_ERR_EXIT(dnx_flexe_reg_get(unit, reg, index, data_32));
        COMPILER_64_SET(*data, data_32[1], data_32[0]);
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg64_get(unit, reg, port, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (soc_is_reg_flexe_core(unit, reg))
    {
        uint32 data_32[SOC_REG_ABOVE_64_MAX_SIZE_U32];
        sal_memset(data_32, 0, sizeof(data_32));
        data_32[0] = COMPILER_64_LO(data);
        data_32[1] = COMPILER_64_HI(data);
        SHR_IF_ERR_EXIT(dnx_flexe_reg_set(unit, reg, index, data_32));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg64_set(unit, reg, port, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_reg_above_64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (soc_is_reg_flexe_core(unit, reg))
    {
        sal_memset(data, 0, sizeof(soc_reg_above_64_val_t));
        SHR_IF_ERR_EXIT(dnx_flexe_reg_get(unit, reg, index, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg_above_64_get(unit, reg, port, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_reg_above_64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (soc_is_reg_flexe_core(unit, reg))
    {
        SHR_IF_ERR_EXIT(dnx_flexe_reg_set(unit, reg, index, data));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_jr2_ilkn_reg_above_64_set(unit, reg, port, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_mem_array_read(
    int unit,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    void *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Currently, only FlexeCore have custom memory */
    SHR_IF_ERR_EXIT(dnx_flexe_mem_array_read(unit, 0, mem, copyno, array_index, index, entry_data));
exit:
    SHR_FUNC_EXIT;
}
/** see header file */
shr_error_e
dnx_custom_mem_array_write(
    int unit,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    void *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Currently, only FlexeCore have custom memory */
    SHR_IF_ERR_EXIT(dnx_flexe_mem_array_write(unit, 0, mem, copyno, array_index, index, entry_data));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_custom_mem_array_fill_range(
    int unit,
    soc_mem_t mem,
    uint32 min_ar_index,
    uint32 max_ar_index,
    int copyno,
    int index_min,
    int index_max,
    const void *entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Currently, only FlexeCore have custom memory */
    SHR_IF_ERR_EXIT(dnx_flexe_mem_array_fill_range
                    (unit, 0, mem, min_ar_index, max_ar_index, copyno, index_min, index_max, entry_data));

exit:
    SHR_FUNC_EXIT;
}
