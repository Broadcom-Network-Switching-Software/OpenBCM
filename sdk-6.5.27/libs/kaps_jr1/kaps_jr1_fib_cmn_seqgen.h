

#ifndef INCLUDED_KAPS_JR1_SEQGEN_H
#define INCLUDED_KAPS_JR1_SEQGEN_H

#include <stdint.h>
#include <kaps_jr1_externcstart.h>
#include "kaps_jr1_fib_cmn_pfxbundle.h"

uint32_t kaps_jr1_seq_num_gen_generate_next_pfx_seq_nr(
    void);

uint32_t kaps_jr1_seq_num_gen_get_current_pfx_seq_nr(
    void);

void kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(
    kaps_jr1_pfx_bundle * newPfx_p);

#include <kaps_jr1_externcend.h>
#endif 
