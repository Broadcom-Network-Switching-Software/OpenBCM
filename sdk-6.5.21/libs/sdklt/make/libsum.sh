#!/bin/sh
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK library source checksum generation.
#
# Generate checksum file as <output-dir>/<dest-file> for SDK
# components <comp1>, <comp2>, etc.
#

if [ "$3" = "" ]; then
    echo "Usage: "`basename $0`" <output-dir> <dest-file> <comp1> [<comp2>...]"
    exit 1
fi

# Get output directory for checksum files
SUMDIR=$1
shift

# Get file name for SDK checksum
SUMFILE=$1
shift

# Treat remaining arguments as SDK components
COMPS=$*

# Default checksum generator
if [ "$GSUM" = "" ]; then
    GSUM=sha1sum
fi

# Prepare directory
mkdir -p $SUMDIR
rm -f $SUMDIR/*.sum

TMPSUM=$SUMDIR/tmp.sum
ALLSUM=$SUMDIR/files.sum

# Generate checksums per component
for c in $COMPS; do
    for d in `find $SDK/${c} -name generated -prune -o -type d -print`; do
        $GSUM ${d}/*.[ch] 1>>$TMPSUM 2>/dev/null
    done
done

# Create sorted list of source file checksums (useful for debug)
cat $TMPSUM | sort -k 2 > $ALLSUM
rm -f $TMPSUM

# Create final checksum across sorted list of source file checksums
$GSUM $ALLSUM | cut -d ' ' -f 1 > $SUMDIR/$SUMFILE
