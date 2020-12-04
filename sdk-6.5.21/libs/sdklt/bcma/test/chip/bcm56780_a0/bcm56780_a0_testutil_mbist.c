/*! \file bcm56780_a0_testutil_mbist.c
 *
 * Chip-specific MBIST data.
 */

#include <sal/sal_types.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/testcase/bcma_testcase_mem_mbist.h>
#define C(STR) BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_ ## STR
#define L(STR) BCMA_TESTCASE_MEM_MBIST_OP_LOOP_ ## STR

int
bcm56780_a0_mem_mbist_info(int unit,
                           const bcma_testutil_mem_mbist_entry_t **array,
                           int *test_count)
{

    if (array == NULL || test_count == NULL) {
        return SHR_E_PARAM;
    }

    *test_count = 0;
    return SHR_E_NONE;
}
