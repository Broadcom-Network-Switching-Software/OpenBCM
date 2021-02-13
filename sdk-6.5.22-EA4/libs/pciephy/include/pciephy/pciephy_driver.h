#ifndef _PCIEPHY_DRIVER_H_
#define _PCIEPHY_DRIVER_H_

#include <pciephy/pciephy.h>

typedef int(*pciephy_diag_reg_read_f)(pciephy_access_t *sa, uint16_t address,
                                     uint16_t *pdata);

typedef int(*pciephy_diag_reg_write_f)(pciephy_access_t *sa, uint16_t address,
                                      uint16_t data);

typedef int (*pciephy_diag_reg_mwr_f)(pciephy_access_t *sa, uint16_t address,
                                     uint16_t mask, uint8_t lsb, uint16_t val);

typedef int (*pciephy_diag_pram_read_f)(pciephy_access_t *sa, uint32_t address,
                                     uint32_t size);

typedef int (*pciephy_diag_dsc_f)(pciephy_access_t *sa, int array_size);

typedef int (*pciephy_diag_state_f)(pciephy_access_t *sa, int array_size);

typedef int (*pciephy_diag_eyescan_f)(pciephy_access_t *sa, int array_size);

typedef struct pciephy_driver_s {
    pciephy_diag_reg_read_f read;
    pciephy_diag_reg_write_f write;
    pciephy_diag_reg_mwr_f mwr;
    pciephy_diag_pram_read_f pram_read;
    pciephy_diag_dsc_f dsc;
    pciephy_diag_state_f state;
    pciephy_diag_eyescan_f eyescan;
} pciephy_driver_t;

#endif /* _PCIEPHY_DRIVER_H_ */
