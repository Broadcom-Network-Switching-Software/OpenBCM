/*! \file bcma_testcase_mem_mbist.h
 *
 * Test case for Memory built-in self testing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_MEM_MBIST_H
#define BCMA_TESTCASE_MEM_MBIST_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcma/test/bcma_testcase.h>

/**@{*/
/*!
 *  OP-code format for classic MBIST
 *     bit 0~3 : command
 *
 *         Type 0 : Basic (00XX)
 *           0000 : Sleep
 *           0001 : read control
 *           0010 : read data
 *           0011 : write data
 *
 *         Type 1 : Write Control with pattern (01XX, XX = pattern id)
 *           0100 : write control (pattern 0)
 *           0101 : write control (pattern 1)
 *           0110 : write control (pattern 2)
 *           0111 : write control (other value)
 *
 *         Type 2 : Write Data / Control with pattern (10XX, XX = pattern id)
 *           1000 : write data (pattern 0)   + control (pattern 0)
 *           1001 : write data (pattern 1)   + control (pattern 0)
 *           1010 : write data (pattern 2)   + control (pattern 0)
 *           1011 : write data (other value) + control (pattern 0)
 *
 *         Type 3: Verify Status with pattern (11XX, XX = pattern id)
 *           1100 : verify status (pattern 0)
 *           1101 : verify status (pattern 1)
 *           1110 : verify status (pattern 2)
 *           1111 : verify status (other value)
 *
 *     bit 4~7 : loop count
 *          N = 0~15, the command would be executed (N+1) times
 */
#define BCMA_TESTCASE_MEM_MBIST_B_0000    (0)
#define BCMA_TESTCASE_MEM_MBIST_B_0001    (1)
#define BCMA_TESTCASE_MEM_MBIST_B_0010    (2)
#define BCMA_TESTCASE_MEM_MBIST_B_0011    (3)
#define BCMA_TESTCASE_MEM_MBIST_B_0100    (4)
#define BCMA_TESTCASE_MEM_MBIST_B_0101    (5)
#define BCMA_TESTCASE_MEM_MBIST_B_0110    (6)
#define BCMA_TESTCASE_MEM_MBIST_B_0111    (7)
#define BCMA_TESTCASE_MEM_MBIST_B_1000    (8)
#define BCMA_TESTCASE_MEM_MBIST_B_1001    (9)
#define BCMA_TESTCASE_MEM_MBIST_B_1010    (10)
#define BCMA_TESTCASE_MEM_MBIST_B_1011    (11)
#define BCMA_TESTCASE_MEM_MBIST_B_1100    (12)
#define BCMA_TESTCASE_MEM_MBIST_B_1101    (13)
#define BCMA_TESTCASE_MEM_MBIST_B_1110    (14)
#define BCMA_TESTCASE_MEM_MBIST_B_1111    (15)

#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_SHIFT              (0)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_MASK \
            (BCMA_TESTCASE_MEM_MBIST_B_1111 <<  \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_SHIFT)

#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_SHIFT         (2)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_MASK \
            (BCMA_TESTCASE_MEM_MBIST_B_0011 <<       \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_0 \
            (BCMA_TESTCASE_MEM_MBIST_B_0000 <<    \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_1 \
            (BCMA_TESTCASE_MEM_MBIST_B_0001 <<    \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_2 \
            (BCMA_TESTCASE_MEM_MBIST_B_0010 <<    \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_3 \
            (BCMA_TESTCASE_MEM_MBIST_B_0011 <<    \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_SHIFT)

#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_SHIFT      (0)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_MASK \
            (BCMA_TESTCASE_MEM_MBIST_B_0011 <<          \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_0 \
            (BCMA_TESTCASE_MEM_MBIST_B_0000 <<       \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_1 \
            (BCMA_TESTCASE_MEM_MBIST_B_0001 <<       \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_2 \
            (BCMA_TESTCASE_MEM_MBIST_B_0010 <<       \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_SHIFT)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_OTHER \
            (BCMA_TESTCASE_MEM_MBIST_B_0011 <<           \
             BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_PATTERN_SHIFT)

#define BCMA_TESTCASE_MEM_MBIST_OP_LOOP_SHIFT  (4)
#define BCMA_TESTCASE_MEM_MBIST_OP_LOOP_MASK   \
            (BCMA_TESTCASE_MEM_MBIST_B_1111 << \
             BCMA_TESTCASE_MEM_MBIST_OP_LOOP_SHIFT)
/**@}*/


/**@{*/
/*! command - type 0 */
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_SLEEP         \
            (BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_0 | \
             BCMA_TESTCASE_MEM_MBIST_B_0000)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_READ_DATA     \
            (BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_0 | \
             BCMA_TESTCASE_MEM_MBIST_B_0001)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_READ_CONTROL  \
            (BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_0 | \
             BCMA_TESTCASE_MEM_MBIST_B_0010)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_WRITE_DATA    \
            (BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_0 | \
             BCMA_TESTCASE_MEM_MBIST_B_0011)
/**@}*/

/**@{*/
/*! command - type 1, 2, 3 */
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_WRITE_CONTROL \
            (BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_1)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_WRITE_DATA_CONTROL \
            (BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_2)
#define BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_STATUS_VERIFY \
            (BCMA_TESTCASE_MEM_MBIST_OP_COMMAND_TYPE_3)
/**@}*/

/**@{*/
/*!
 *  OP-code format for MBIST DMA
 *     bit 0~3 : command
 *          0000 : write pattern 0
 *          (.......)
 *          1101 : write pattern 13
 *          1110 : write data
 *          1111 : write long pattern
 *
 *     bit 4~7 : loop count
 *          N = 0~15, the command would be executed (N+1) times
 */

#define BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_SHIFT     (0)
#define BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_MASK \
            (BCMA_TESTCASE_MEM_MBIST_B_1111 <<      \
             BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_SHIFT)

#define BCMA_TESTCASE_MEM_MBIST_DMA_OP_LOOP_SHIFT        (4)
#define BCMA_TESTCASE_MEM_MBIST_DMA_OP_LOOP_MASK   \
            (BCMA_TESTCASE_MEM_MBIST_B_1111 <<     \
             BCMA_TESTCASE_MEM_MBIST_DMA_OP_LOOP_SHIFT)
/**@}*/

/**@{*/
/*! DMA command */
#define BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_WRITE_PATTERN_SHIFT (0)

#define BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_WRITE_DATA \
            (BCMA_TESTCASE_MEM_MBIST_B_1110)

#define BCMA_TESTCASE_MEM_MBIST_DMA_OP_COMMAND_WRITE_LONG_PATTERN \
            (BCMA_TESTCASE_MEM_MBIST_B_1111)

/**@}*/

/*! Brief description for Memory built-in self test case */
#define BCMA_TESTCASE_MEM_MBIST_DESC \
        "Run BIST (Built-In Self Testing) on SOC memories.\n"

/*! Flags for Memory built-in self test case */
#define BCMA_TESTCASE_MEM_MBIST_FLAGS \
        (BCMA_TEST_F_DESTRUCTIVE)

/*!
 * \brief Get Memory built-in self test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_mem_mbist_op_get(void);

#endif /* BCMA_TESTCASE_MEM_MBIST_H */

