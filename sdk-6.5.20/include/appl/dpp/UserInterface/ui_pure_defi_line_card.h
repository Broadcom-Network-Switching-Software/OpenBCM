/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_PURE_DEFI_LINE_CARD_INCLUDED
/* { */
#define UI_PURE_DEFI_LINE_CARD_INCLUDED
/*
 * Note:
 * the following definitions must range between PARAM_LINE_CARD_START_RANGE_ID (1400)
 * and PARAM_LINE_CARD_END_RANGE_ID (1799).
 * See ui_pure_defi.h
 */
#define PARAM_LINE_TRIMMERS_READ_ID                   1401
#define PARAM_LINE_TRIMMERS_WRITE_ID                  1402
#define PARAM_LINE_TRIMMERS_READ_BYTE_NUM_ID          1403
#define PARAM_LINE_TRIMMERS_WRITE_DATA_ID             1404
#define PARAM_LINE_TRIMMERS_INTERNAL_ADDR_ID          1405

#define PARAM_LINE_FPGA_WRITE_ID                      1410
#define PARAM_LINE_FPGA_READ_ID                       1411
#define PARAM_LINE_FPGA_WRITE_DATA_ID                 1412

#define PARAM_LINE_CARD_FAP10M_1_I2C_ID               1500
#define PARAM_LINE_CARD_FAP10M_1_E2PROM_ID            1501
#define PARAM_LINE_CARD_PP_1_E2PROM_ID                1502
#define PARAM_LINE_CARD_READ_ID                       1503
#define PARAM_LINE_CARD_WRITE_ID                      1504
#define PARAM_LINE_CARD_DATA_ID                       1505
#define PARAM_LINE_CARD_RESET_ID                      1506
#define PARAM_LINE_CARD_MILISEC_ID                    1507
#define PARAM_LINE_CARD_STAY_DOWN_ID                  1508
#define PARAM_LINE_CARD_FAP10M_1_ID                   1509
#define PARAM_LINE_CARD_PP_1_ID                       1510
#define PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_ID         1512
#define PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_OD_ID      1513
#define PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_RDW_ID     1514
#define PARAM_LINE_CARD_CLOCK_SYNTHESIZERS_VDW_ID     1515
#define PARAM_LINE_CARD_CLOCK_SYNTHESIZER_OSCILLATOR_ID 1516
#define PARAM_LINE_CARD_SILENT_ID                     1517
#define PARAM_LINE_CARD_VAL_READ_ID                   1518
#define PARAM_LINE_CARD_MASK_ID                       1519
#define PARAM_LINE_CARD_TIMEOUT_ID                    1520
#define PARAM_LINE_CARD_VAL_ID                        1521
#define PARAM_LINE_CARD_OPERATION_ID                  1522
#define PARAM_LINE_CARD_NOF_READ_ID                   1523
#define PARAM_LINE_CARD_ERASE_ID                      1524
#define PARAM_LINE_CARD_PP_1_I2C_ID                   1525
#define PARAM_LINE_CARD_PP_1_PCI_ID                   1526
#define PARAM_LINE_CARD_C2C_RESET_ID                  1531
#define PARAM_LINE_CARD_PP_LOAD_C2C_DFCDL_ID          1532
#define PARAM_LINE_CARD_UTILS_ID                      1533
#define PARAM_LINE_CARD_DISP_ERR_COUNTERS_ID          1534
#define PARAM_LINE_CARD_CLEAR_AFTER_TREAD_ID          1535
#define PARAM_LINE_CARD_FAP10M_CHECK_EDDR_ID          1536
#define PARAM_LINE_CARD_FAP10M_CHECK_IDDR_ID          1537
#define PARAM_LINE_CARD_PP_LOAD_IDDR_DFCDL_ID         1538
#define PARAM_LINE_CARD_PP_CHECK_IDDR_ID              1539
#define PARAM_LINE_CARD_PACKET_INJECT_TEST_ID         1540
#define PARAM_LINE_CARD_PACKET_INJECT_START_ID        1541
#define PARAM_LINE_CARD_DISPLAY_ALL_COUNTERS_ID       1542
#define PARAM_LINE_CARD_TRACE_PRINT_ID                1543
#define PARAM_LINE_CARD_PACKET_INJECT_NUM_PKTS_ID     1544
#define PARAM_LINE_CARD_PACKET_SIZE_ID                1545
#define PARAM_LINE_CARD_REPEATED_PATTERN_ID           1546
#define PARAM_LINE_CARD_RANDOM_PATTERN_ID             1547
#define PARAM_LINE_CARD_PATTERN_DATA_ID               1548

#define PARAM_LINE_CARD_FAP10M_LOAD_EDDR_DFCDL_ID     1549
#define PARAM_LINE_CARD_FAP10M_LOAD_IDDR_DFCDL_ID     1550
#define PARAM_LINE_CARD_FAP10M_LOAD_C2C_DFCDL_ID      1551
#define PARAM_LINE_CARD_DFCDL_RX_CLK_ID               1552
#define PARAM_LINE_CARD_DFCDL_TX_CLK_ID               1553

#define PARAM_LINE_CARD_FAP10M_LOAD_EDDR_FTDLL_ID     1554
#define PARAM_LINE_CARD_FAP10M_LOAD_IDDR_FTDLL_ID     1555
#define PARAM_LINE_CARD_FAP10M_LOAD_C2C_FTDLL_ID      1556
#define PARAM_LINE_CARD_FTDLL_START_BURST_ID          1557
#define PARAM_LINE_CARD_FTDLL_CLK_OUT_ID              1558

#define PARAM_LINE_CARD_C2C_DFCDL_NARROW_ID           1559
#define PARAM_LINE_CARD_C2C_FTDLL_NARROW_ID           1560
#define PARAM_LINE_CARD_C2C_DFCDL_WIDE_ID             1561
#define PARAM_LINE_CARD_C2C_FTDLL_WIDE_ID             1562
#define PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID            1564
#define PARAM_LINE_CARD_FTDLL_AUTO_POLL_ID            PARAM_LINE_CARD_DFCDL_AUTO_POLL_ID
#define PARAM_LINE_CARD_DFCDL_DQS_ID                  1566
#define PARAM_LINE_CARD_FTDLL_DQS_ID                  1567
#define PARAM_LINE_CARD_PP_LOAD_DDR_FTDLL_ID          1568
#define PARAM_LINE_CARD_PP_LOAD_C2C_FTDLL_ID          1569

#define PARAM_LINE_CARD_FPGA_DOWNLOAD_ID              1570

#define PARAM_LINE_CARD_CLOCK_CONFIGURE_WITH_FREQ_ID  1571
#define PARAM_LINE_CARD_CLOCK_ADD_HALF_ID             1572
#define PARAM_LINE_CARD_CLOCK_GET_ID                  1573

#define PARAM_RUN_BIST_TEST_ID                        1576
#define PARAM_RUN_BIST_TEST_1_ID                      1577

#define PARAM_IO_PAD_CALIBRATION_GET_ID               1580
#define PARAM_IDDR_PAD_CALIBRATION_ID                 1581
#define PARAM_EDDR_PAD_CALIBRATION_ID                 1582
#define PARAM_C2C_PAD_CALIBRATION_ID                  1583
#define PARAM_PAD_CALIBRATION_DATA_P_ID               1584
#define PARAM_PAD_CALIBRATION_DATA_N_ID               1585
#define PARAM_PAD_CALIBRATION_ADDR_P_ID               1586
#define PARAM_PAD_CALIBRATION_ADDR_N_ID               1587

#define PARAM_LINE_CARD_ENABLE_JUMBO_ID               1591


/* } */
#endif /*UI_PURE_DEFI_LINE_CARD_INCLUDED*/
