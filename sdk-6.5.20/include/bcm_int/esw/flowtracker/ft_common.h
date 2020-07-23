/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ft_common.h
 * Purpose: Function declarations for common flowtracker routines.
 */

#ifndef _BCM_INT_FT_COMMON_H_
#define _BCM_INT_FT_COMMON_H_

#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <bcm/flowtracker.h>

/*
 * Donot change enum sequence. Append in last always
 * FlowTracker types in pipeline.
 */
typedef enum bcmi_ft_type_e {
    bcmiFtTypeNone = 0,
    bcmiFtTypeIFT = 1,
    bcmiFtTypeMFT = 2,
    bcmiFtTypeEFT = 3,
    bcmiFtTypeAIFT = 4,
    bcmiFtTypeAMFT = 5,
    bcmiFtTypeAEFT = 6,
    bcmiFtTypeCount = 7
} bcmi_ft_type_t;

#define BCMI_FT_TYPE_STRINGS \
{ \
    "None", \
    "Ingress", \
    "Mmu", \
    "Egress", \
    "AggregateIngress", \
    "AggregateMmu", \
    "AggregateEgress", \
    "Count"  \
}

/* Flowtracker Type Flags */
#define BCMI_FT_TYPE_F_IFT          (1 << bcmiFtTypeIFT)
#define BCMI_FT_TYPE_F_MFT          (1 << bcmiFtTypeMFT)
#define BCMI_FT_TYPE_F_EFT          (1 << bcmiFtTypeEFT)
#define BCMI_FT_TYPE_F_AIFT         (1 << bcmiFtTypeAIFT)
#define BCMI_FT_TYPE_F_AMFT         (1 << bcmiFtTypeAMFT)
#define BCMI_FT_TYPE_F_AEFT         (1 << bcmiFtTypeAEFT)

#define BCMI_FT_TYPE_F_MICRO        (BCMI_FT_TYPE_F_IFT  |          \
                                     BCMI_FT_TYPE_F_MFT  |          \
                                     BCMI_FT_TYPE_F_EFT)

#define BCMI_FT_TYPE_F_AGG          (BCMI_FT_TYPE_F_AIFT |          \
                                     BCMI_FT_TYPE_F_AMFT |          \
                                     BCMI_FT_TYPE_F_AEFT)

#define BCMI_FT_TYPE_F_ANY          (BCMI_FT_TYPE_F_MICRO |         \
                                     BCMI_FT_TYPE_F_AGG)

/* Check if only one of the ft type is set */
#define BCMI_FT_TYPE_F_IS_UNIQUE(_ft_type_flags_)                   \
    (!((_ft_type_flags_) & ((_ft_type_flags_) - 1)))

/* Get ft type when only 1b is set */
#define BCMI_FT_TYPE_F_UNIQUE_GET(_ft_type_flags_, _ft_type_)       \
    do {                                                            \
        int idx = 0;                                                \
        for (idx = 0; idx < bcmiFtTypeCount; idx++) {               \
            if ((_ft_type_flags_) & (1 << idx)) {                   \
                (_ft_type_) = idx;                                  \
            }                                                       \
        }                                                           \
    } while (0)

#define BCMI_FT_GROUP_FT_TYPE_BMP_GET(_gt_, _ft_type_bmp_)              \
    do {                                                                \
        if ((_gt_) == bcmFlowtrackerGroupTypeNone) {                    \
            (_ft_type_bmp_) = BCMI_FT_TYPE_F_MICRO;                     \
        } else if ((_gt_) == bcmFlowtrackerGroupTypeAggregateIngress) { \
            (_ft_type_bmp_) = BCMI_FT_TYPE_F_AIFT;                      \
        } else if ((_gt_) == bcmFlowtrackerGroupTypeAggregateMmu) {     \
            (_ft_type_bmp_) = BCMI_FT_TYPE_F_AMFT;                      \
        } else { /* bcmFlowtrackerGroupTypeAggregateEgress */           \
            (_ft_type_bmp_) = BCMI_FT_TYPE_F_AEFT;                      \
        }                                                               \
    } while(0)

#define BCMI_FT_GROUP_CHECK_FT_TYPE_GET(_gftbmp_, _cftbmp_, _fftbmp_,_fft_)  \
    do {                                                            \
        _fftbmp_ = (_gftbmp_ & _cftbmp_);                           \
        if (BCMI_FT_TYPE_F_IS_UNIQUE(_fftbmp_)) {                   \
            BCMI_FT_TYPE_F_UNIQUE_GET(_fftbmp_, _fft_);             \
        } else {                                                    \
            /* Could not determine ft_type for the check, Lets take \
               first ft_type */                                     \
            if (_fftbmp_ & BCMI_FT_TYPE_F_IFT) {                    \
                _fft_ = bcmiFtTypeIFT;                              \
            } else if (_fftbmp_ & BCMI_FT_TYPE_F_MFT) {             \
                _fft_ = bcmiFtTypeMFT;                              \
            } else {                                                \
                _fft_ = bcmiFtTypeEFT;                              \
            }                                                       \
        }                                                           \
    } while(0)


/* Generic memory allocation routine. */
#define BCMI_FT_ALLOC(_ptr_, _size_, _descr_)                       \
    do {                                                            \
        int _lsize_;                                                \
        _lsize_ = (_size_ );                                        \
        if (NULL == (_ptr_)) {                                      \
            (_ptr_) = sal_alloc((_lsize_), (_descr_));              \
        }                                                           \
        if((_ptr_) != NULL) {                                       \
            sal_memset((_ptr_), 0, (_lsize_));                      \
        }  else {                                                   \
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,                       \
                    (BSL_META("FT Error: Allocation"                \
                              " failure %s\n"), (_descr_)));        \
        }                                                           \
    } while (0)

/* Generic memory allocation. If error, cleanup */
#define BCMI_FT_ALLOC_IF_ERR_CLEANUP(_ptr_, _size_, _descr_)        \
    do {                                                            \
        BCMI_FT_ALLOC(_ptr_, _size_, _descr_);                      \
        if ((_ptr_) == NULL) {                                      \
            rv = BCM_E_MEMORY;                                      \
            goto cleanup;                                           \
        }                                                           \
    } while (0)

/* Generic memory free routine. */
#define BCMI_FT_FREE(_ptr_)                                         \
    do {                                                            \
        if (NULL != (_ptr_)) {                                      \
            sal_free((_ptr_));                                      \
        }                                                           \
        (_ptr_) = NULL;                                             \
    } while (0)

int
bcmi_ft_group_type_mem_get(
    int unit,
    int key_or_data,
    bcm_flowtracker_group_type_t group_type,
    soc_mem_t *mem);

int
bcmi_ft_type_support_check(
    int unit,
    bcmi_ft_type_t ft_type,
    int *support);

#endif /* BCM_FLOWTRACKER_SUPPORT */
#endif /* _BCM_INT_FT_COMMON_H_ */
