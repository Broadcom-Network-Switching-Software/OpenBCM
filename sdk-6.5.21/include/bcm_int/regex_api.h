/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _RE_H_
#define _RE_H_

#if defined(INCLUDE_REGEX)

#define DFA_TRAVERSE_START      0x1
#define DFA_TRAVERSE_END        0x2
#define DFA_STATE_FINAL         0x4

typedef enum {
        REGEX_ERROR_NONE            = 0,
        REGEX_ERROR_INVALID_CLASS   = -1,
        REGEX_ERROR_EXPANSION_FAIL  = -2,
        REGEX_ERROR_NO_POST         = -3,
        REGEX_ERROR_NO_DFA          = -4,
        REGEX_ERROR_NO_NFA          = -5,
        REGEX_ERROR_NO_MEMORY       = -6,
        REGEX_ERROR                 = -7
} regex_error;

typedef enum {
    REGEX_CB_OK    = 0,
    REGEX_CB_ABORT,
} regex_cb_error_t;

#define BCM_TR3_REGEX_CFLAG_EXPAND_LCUC     0x01
#define BCM_TR3_REGEX_CFLAG_EXPAND_UC       0x02
#define BCM_TR3_REGEX_CFLAG_EXPAND_LC       0x04
#define BCM_TR3_REGEX_CFLAG_ANCHORED        0x08

#define REGEX_MAX_CHARACTER_CLASSES      256
#define REGEX_MAX_CHARACTER_CLASS_BYTES  (BITS2BYTES(REGEX_MAX_CHARACTER_CLASSES))
#define REGEX_MAX_CHARACTER_CLASS_WORDS  (BITS2WORDS(REGEX_MAX_CHARACTER_CLASSES))

/*
 * Call user provided callback to map a DFA State to HW.
 */
typedef regex_cb_error_t (*regex_dfa_state_cb)(unsigned int flags, 
                            int match_idx, int in_state, int from_c, int to_c, 
                            int to_state, int num_dfa_state, void *user_data);


int bcm_regex_compile(char **re, unsigned int *res_flags, 
                      int num_pattern, unsigned int cflags, void** dfa);

int bcm_regex_dfa_traverse(void *dfa, regex_dfa_state_cb compile_dfa_cb,
                            void *user_data);

int bcm_regex_dfa_free(void *dfa);

extern void _bcm_regex_sw_dump(int unit);

#endif /* INCLUDE_REGEX */

#endif /* _RE_H_ */
