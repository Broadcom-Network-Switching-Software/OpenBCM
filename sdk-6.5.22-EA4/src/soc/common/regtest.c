/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * Register Test
 *
 *
 * This module is also used by the main SOC diagnostics register tests,
 * fronted by user interface code.
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/cm.h>
#include <soc/register.h>
#include <soc/debug.h>
#include <soc/drv.h>
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_ESW_SUPPORT)

#if defined(SER_TR_TEST_SUPPORT)

#define _SER_TEST_REG_SOC_FIELD_NAME(unit, field) \
    ((field >= 0)? SOC_FIELD_NAME((unit), (field)) : "INVALIDf")


/*
 * Function:
 *      ser_test_reg_write
 * Purpose:
 *      write to register without update cache
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_reg   - (IN) contains the current state of the test.
 * Returns:
 *      An error if one is generated by the ser_test_reg_write, otherwise SOC_E_NONE
 */
soc_error_t
ser_test_reg_write(int unit, ser_reg_test_data_t *test_reg)
{
    soc_error_t rv = SOC_E_NONE;

    if (SOC_REG_IS_64(unit, test_reg->reg)) {
        soc_reg64_field32_set(unit, test_reg->reg, test_reg->reg_buf,
                              test_reg->test_field, *test_reg->field_buf);
    } else {
        soc_reg_field_set(unit, test_reg->reg, ((uint32 *) test_reg->reg_buf),
                          test_reg->test_field, *test_reg->field_buf);
    }

    rv = soc_reg_set_nocache(unit, test_reg->reg, test_reg->port,
                             test_reg->index, *test_reg->reg_buf);

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                  "unit %d %s port %d reg write error\n"),
                   unit, test_reg->reg_name, test_reg->port));
    }
    return rv;
}


/*
 * Function:
 *      ser_test_reg_read
 * Purpose:
 *      Read register and update the test data.
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_reg   - (IN & OUT) contains the current state of the test data.
 *                    The members reg_buf and field_buf will be modified by this function
 * Returns:
 *      An error if one is generated by the soc_mem_read, otherwise SOC_E_NONE
 */

soc_error_t
ser_test_reg_read(int unit, ser_reg_test_data_t *test_reg)
{
    soc_error_t rv = SOC_E_NONE;
    soc_reg_t reg = test_reg->reg;
    uint32 data32 = 0;

    rv = soc_reg_get(unit, reg, test_reg->port, test_reg->index, test_reg->reg_buf);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                  "unit %d NACK received for %s port %d:\n\t"),
                   unit, SOC_REG_NAME(unit, reg), test_reg->port));
    }

    if (SOC_REG_IS_64(unit, reg)) {
        *test_reg->field_buf = soc_reg64_field32_get(unit, reg, *test_reg->reg_buf,
                                                     test_reg->test_field);
    } else {
        data32 = COMPILER_64_LO(*test_reg->reg_buf);
        *test_reg->field_buf = soc_reg_field_get(unit, reg, data32, test_reg->test_field);
    }

    return rv;
}



/*
 * Function:
 *      soc_ser_create_reg_test_data
 * Purpose:
 *      Populates and performs data verification on a ser_test_reg_t structure.  This was
 *      broken out into a separate function to avoid performing this work an excessive number
 *      of times.
 * Parameters:
 *      unit                 - (IN) Device Number
 *      reg_data             - (IN) A pointer to a uint32 array
 *      fieldData            - (IN) A pointer to a uint 32 array
 *      parity_enable_reg    - (IN) Register which turns on and off parity protection
 *      parity_enable_field   - (IN) The field in the parity enable register
 *      port                 - (IN) The port for this reg.
 *      index             - (IN) The index for this reg.
 *      reg_test_data            - (OUT)A pointer to a data structure cotaining SER test date.
 * Returns:
 *      SOC_E_NONE if the test passes, and error if it does
 */
void
soc_ser_create_reg_test_data(int unit, uint32 *reg_data, uint32 *field_data,
                             soc_reg_t parity_enable_reg,
                             soc_field_t parity_enable_field,
                             soc_reg_t reg, soc_field_t test_field,
                             soc_port_t port, int index,
                             ser_reg_test_data_t *reg_test_data)
{
    static soc_field_t field_names[] = {
                                        ECCf,
                                        EVEN_PARITYf,
                                        PARITYf,
                                        INVALIDf};
    int i, found = 0, field_length = 1, field_num = 0;
#ifdef SOC_REG_NAME
    size_t len;
#endif

    sal_memset(reg_test_data, 0 , sizeof(ser_reg_test_data_t));

    reg_test_data->reg = reg;
    reg_test_data->parity_enable_reg = parity_enable_reg;
    reg_test_data->parity_enable_field = parity_enable_field;
    reg_test_data->port = port;
    reg_test_data->index = index;
    reg_test_data->reg_info = &SOC_REG_INFO(unit, reg_test_data->reg);
    reg_test_data->test_field = test_field;

    /* An INVALIDf, 0 passed in for test_field implies we should select
     * any valid field. */
    if ((test_field == INVALIDf) || (test_field == 0) ||
        !SOC_REG_FIELD_VALID(unit, reg_test_data->reg, test_field)
       ) {
        /* select from preferred list */
        for (i = 0; (field_names[i] != INVALIDf); i++) {
            if (SOC_REG_FIELD_VALID(unit, reg_test_data->reg, field_names[i])) {
                reg_test_data->test_field = field_names[i];
                found = 1;
                break;
            }
        }

        /* select first multi-bit field or last 1-bit field */
        if (!found && (reg_test_data->reg_info != NULL)) {
           field_num = reg_test_data->reg_info->nFields;
           for (i = 0; i < field_num; i++) {
               reg_test_data->test_field = reg_test_data->reg_info->fields[i].field;
               field_length = soc_reg_field_length(unit, reg_test_data->reg,
                                                   reg_test_data->test_field);
               if (field_length > 1) {
                   found = 1;
                   break;
               }
           }
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                "unit %d, soc_ser_create_test_reg: error_inject_view: reg %s, test_field %s\n"),
                 unit, SOC_REG_NAME(unit, reg_test_data->reg),
                 _SER_TEST_REG_SOC_FIELD_NAME(unit, reg_test_data->test_field)));

    reg_test_data->reg_buf = (uint64 *) reg_data;
    reg_test_data->field_buf = field_data;
#ifdef SOC_REG_NAME
    /* coverity[secure_coding] */
    len = sal_strlen(SOC_REG_NAME(unit, reg_test_data->reg));
    if(sizeof(reg_test_data->reg_name)- 1 < len)
            len = sizeof(reg_test_data->reg_name)- 1;
    sal_strncpy(reg_test_data->reg_name, SOC_REG_NAME(unit, reg_test_data->reg), len);
    reg_test_data->reg_name[len] = 0;
    /* coverity[secure_coding] */
    len = sal_strlen(_SER_TEST_REG_SOC_FIELD_NAME(unit, reg_test_data->test_field));
    if(sizeof(reg_test_data->field_name)- 1 < len)
      len = sizeof(reg_test_data->field_name)- 1;
    sal_strncpy(reg_test_data->field_name,
               _SER_TEST_REG_SOC_FIELD_NAME(unit, reg_test_data->test_field), len);
    reg_test_data->field_name[len] = 0;
#else
    sprintf(reg_test_data->mem_name, "Reg ID: %d", reg_test_data->reg);
    sprintf(reg_test_data->field_name, "Field ID: %d", reg_test_data->test_field);
#endif
}

extern int soc_ser_test_long_sleep;
extern int soc_ser_test_long_sleep_time_us;

soc_error_t
_soc_ser_reg_inject_error(int unit, ser_reg_test_data_t *test_reg)
{
    if((test_reg->field_buf[0] & 1) == 1) {
        test_reg->field_buf[0] &= 0xFFFFFFFE;
    } else {
        test_reg->field_buf[0] |= 0x00000001;
    }

    test_reg->badData = test_reg->field_buf[0];

    /*Disable writing to cache for fudged data.*/
    SOC_IF_ERROR_RETURN(ser_test_reg_write(unit, test_reg));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _ser_test_parity_control
 * Purpose:
 *      Disables/enables parity for the provided register.
 * Parameters:
 *      unit          - (IN) Device Number
 *      test_reg     - (IN) Contains parity registers to set.
 *      enable        - (IN) (Bool) enable parity if true, disable if false
 *
 * Returns:
 *      SOC_E_NONE if no errors are encountered while setting parity.
 */
soc_error_t
_ser_test_reg_parity_control(int unit, ser_reg_test_data_t *test_reg, int enable)
{
    soc_error_t rv = SOC_E_NONE;
    rv = soc_reg_field32_modify(unit, test_reg->parity_enable_reg,
                                test_reg->port,
                                test_reg->parity_enable_field, enable);
    return rv;
}

/*
 * Function:
 *      ser_test_reg
 * Purpose:
 *      Serform a Software Error Recovery test on one register.
 * Parameters:
 *      unit                  - (IN) Device Number
 *      test_reg            - (IN) Structure which holds the test data and some
                                    intermediate results.
 *      error_count        - (OUT)Increments if  failed.
 * Returns:
 *      SOC_E_NONE if the test passes, an error if it does not
 */
soc_error_t
ser_test_reg(int unit, ser_reg_test_data_t *test_reg,
               int *error_count)
{
    int startErrorCount;
    soc_error_t rv = SOC_E_NONE;

    startErrorCount = *error_count;

    if (!SOC_REG_IS_VALID(unit, test_reg->reg)) {
        LOG_CLI((BSL_META_U(unit,
                 "%s is not a valid register for this platform. Skipping.\n"),
                 SOC_REG_NAME(unit, test_reg->reg)));
        return SOC_E_UNAVAIL;
    }

    rv = (ser_test_reg_read(unit, test_reg));
    if (SOC_FAILURE(rv)) {
        (*error_count)++;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "SER failed. 1st Read got NACK. mem: %s field: %s\n"),
                    test_reg->reg_name,
                    test_reg->field_name));
        return rv;
    }

    /*Disable Parity*/
    rv = (_ser_test_reg_parity_control(unit, test_reg, 0));
    if (SOC_FAILURE(rv)) {
        (*error_count)++;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "SER failed. Disable Parity did not work. reg: %s field: %s\n"),
                    test_reg->reg_name,
                    test_reg->field_name));
        return rv;
    }

    /*Inject error:*/
    rv = (_soc_ser_reg_inject_error(unit, test_reg));

    if (SOC_FAILURE(rv)) {
        (*error_count)++;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "SER failed. soc_ser_test_inject_error failed. reg: %s field: %s\n"),
                    test_reg->reg_name,
                    test_reg->field_name));
        return rv;
    }

    /*Read back write to ensure value is still fudged.*/
    rv = (ser_test_reg_read(unit, test_reg));
    if (SOC_FAILURE(rv)) {
        (*error_count)++;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                    "SER failed for could not read the data. reg: %s field: %s\n"),
                    test_reg->reg_name,
                    test_reg->field_name));
        return rv;
    }
    if (test_reg->badData != test_reg->field_buf[0]) {
        LOG_CLI((BSL_META_U(unit,
                 "SER failed for Injection Error for reg: %s field: %s\n"),
                 test_reg->reg_name,
                 test_reg->field_name));
        (*error_count)++;
        return SOC_E_FAIL;
    }

    /*Enable Parity (Even if it was not enabled before*/
    rv = (_ser_test_reg_parity_control(unit, test_reg, 1));
    if (SOC_FAILURE(rv)) {
        (*error_count)++;
        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "SER failed. Re-enable Parity did not work. reg: %s field: %s\n"),
                        test_reg->reg_name,
                        test_reg->field_name));
        return rv;
    }

    if (SAL_BOOT_QUICKTURN) {
        sal_usleep(10000); /* Add delay for QT to trigger parity error */
    }
    /*Read the register in order to trigger the error reporting */
    rv = ser_test_reg_read(unit, test_reg);

    sal_usleep(1500000); /*wait for memory and register ops to complete.*/
    if (SAL_BOOT_QUICKTURN) {
        sal_usleep(10000000);    /* add more delay for Quickturn.*/
    } else if (soc_ser_test_long_sleep) {
        sal_usleep(soc_ser_test_long_sleep_time_us);
    }
    /*Read the register once more to compare it to the old value*/
    rv = (ser_test_reg_read(unit, test_reg));
    if (SOC_FAILURE(rv)) {
        (*error_count)++;
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                      "SER failed. Received NACK on 2nd Read "
                      "for reg %s \n"),
                      test_reg->reg_name));
        return rv;
    }

    /* reg restore */
    if (soc_ser_reg_cache_get(unit, test_reg->reg, REG_PORT_ANY, 0, test_reg->reg_buf)
         == SOC_E_NONE) {
          if (test_reg->badData == test_reg->field_buf[0]) {
            (*error_count)++;
            LOG_ERROR(BSL_LS_SOC_SER,
                      (BSL_META_U(unit,
                       "SER failed to correct reg %s index %d field %s\n"),
                       test_reg->reg_name, test_reg->index,
                       test_reg->field_name));
            return SOC_E_FAIL;
          }
    } else if (0 != test_reg->field_buf[0]) {
        (*error_count)++;
        LOG_ERROR(BSL_LS_SOC_SER,
                  (BSL_META_U(unit,
                   "SER failed to clear reg %s index %d field %s \n"),
                   test_reg->reg_name, test_reg->index,
                   test_reg->field_name));
        return SOC_E_FAIL;
    } else {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                     "SER corrected reg %s \n"),
                     test_reg->reg_name));
    }

    if (startErrorCount != *error_count) {
        LOG_CLI((BSL_META_U(unit,
                 "SER failed to correct reg %s field: %s %d:%d\n"),
                 test_reg->reg_name,
                 test_reg->field_name,
                 startErrorCount, *error_count));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

#endif /* defined (SER_TR_TEST_SUPPORT)*/
#endif /* BCM_ESW_SUPPORT */
