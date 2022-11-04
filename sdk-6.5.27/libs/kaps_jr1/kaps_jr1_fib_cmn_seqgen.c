

#include "kaps_jr1_fib_cmn_seqgen.h"
#include "kaps_jr1_fib_cmn_pfxbundle.h"


static uint32_t g_pfxSeqNr = 0;

uint32_t
kaps_jr1_seq_num_gen_generate_next_pfx_seq_nr(
    void)
{
    ++g_pfxSeqNr;

    return g_pfxSeqNr;
}

uint32_t
kaps_jr1_seq_num_gen_get_current_pfx_seq_nr(
    void)
{
    return g_pfxSeqNr;
}

void
kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(
    struct kaps_jr1_pfx_bundle *newPfx_p)
{

#ifdef KAPS_JR1_LPM_DEBUG
    newPfx_p->m_nSeqNum = kaps_jr1_seq_num_gen_get_current_pfx_seq_nr();
#endif
    return;
}
