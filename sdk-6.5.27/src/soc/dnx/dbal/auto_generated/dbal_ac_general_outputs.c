
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */
#include <soc/drv.h>

uint8
dbal_is_jr2_a0(
int unit)
{
    return (SOC_IS_JERICHO2_A0(unit));
}
uint8
dbal_is_jr2_b0(
int unit)
{
    return (SOC_IS_JERICHO2_B(unit));
}
uint8
dbal_is_j2c_a0(
int unit)
{
    return (SOC_IS_J2C(unit));
}
uint8
dbal_is_q2a_a0(
int unit)
{
    return (SOC_IS_Q2A_A0(unit));
}
uint8
dbal_is_q2a_b0(
int unit)
{
    return (SOC_IS_Q2A_B(unit));
}
uint8
dbal_is_j2p_a0(
int unit)
{
    return (SOC_IS_J2P(unit));
}
uint8
dbal_is_j2x_a0(
int unit)
{
    return (SOC_IS_J2X(unit));
}
