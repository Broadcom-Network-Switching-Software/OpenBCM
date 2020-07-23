/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
cint_reset();

int unit = 0;
bcm_port_t port = 1;
bcm_cos_queue_t cosq = 2;
bcm_cosq_control_t type;
int arg, out_arg = -1;

/* Configure the ADT group of a queue. */
type = bcmCosqControlAdtPriGroup;
arg = BCM_COSQ_ADT_PRI_GROUP_HIGH;

print bcm_cosq_control_set(unit, port, cosq, type, arg);
print bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
print out_arg;

arg = BCM_COSQ_ADT_PRI_GROUP_LOW;

print bcm_cosq_control_set(unit, port, cosq, type, arg);
print bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
print out_arg;

arg = BCM_COSQ_ADT_PRI_GROUP_MIDDLE;

print bcm_cosq_control_set(unit, port, cosq, type, arg);
print bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
print out_arg;

/* Configure ADT alpha index value used to compute margin value for shared pool */
type = bcmCosqControlEgressPoolAdtHighPriAlpha;
arg = bcmCosqControlDropLimitAlpha_1_16;

print bcm_cosq_control_set(unit, port, cosq, type, arg);
print bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
print out_arg;

arg = bcmCosqControlDropLimitAlpha_8;

print bcm_cosq_control_set(unit, port, cosq, type, arg);
print bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
print out_arg;

/* Configure margin levels to be used to indicate which limit low priority alpha to use */
type = bcmCosqControlEgressPoolAdtMargin0;
arg = 100000;  /*400 cell/100 KB*/

print bcm_cosq_control_set(unit, port, cosq, type, arg);
print bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
print out_arg;

/* Configure the egress pool whose low priority packets admitted using limit alpha value are monitored */
port = -1;
cosq = -1;
type = bcmCosqControlAdtLowPriMonitorPool;
arg = 1;

print bcm_cosq_control_set(unit, port, cosq, type, arg);
print bcm_cosq_control_get(unit, port, cosq, type, &out_arg);
print out_arg;


/* Retrieve counter of low priority packets admitted using limit alpha value */
port = -1;
cosq = -1;
bcm_cosq_stat_t stat;
uint64 value;
void *p=&value;
int *pval=p;

stat = bcmCosqStatAdtAlpha0AdmittedLowPriPackets;
pval[0] = 0;
pval[1] = 100;

print value;
print bcm_cosq_stat_set(unit, port, cosq, stat, value);

pval[0] = -1;
pval[1] = -1;
print value;

print bcm_cosq_stat_get(unit, port, cosq, stat, &value);
print value;


/* Retrieve counter of high priority accepted bytes for a given service pool */
stat = bcmCosqStatEgressPoolAdtAdmittedHighPriBytes;
port = 1;
cosq = 2;

pval[0] = 0;
pval[1] = 100*254;

print value;
print bcm_cosq_stat_set(unit, port, cosq, stat, value);

pval[0] = -1;
pval[1] = -1;
print value;

print bcm_cosq_stat_get(unit, port, cosq, stat, &value);
print value;

