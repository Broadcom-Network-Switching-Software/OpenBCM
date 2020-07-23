#!/tools/bin/python
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Convert Hex file to binary.
#
#

import sys
import argparse
import binascii
parser = argparse.ArgumentParser()
parser.add_argument('input',help="input hex file")
parser.add_argument('output',help="output binary file")
args=parser.parse_args()

input_fd = open(args.input, 'r')
output_fd =  open(args.output, 'wb')
lines = input_fd.readlines()

for line in lines:
    words = line.split()
    for a in words:
        b = binascii.a2b_hex(a)
        output_fd.write(b)

input_fd.close()
output_fd.close()
