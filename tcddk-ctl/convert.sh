#!/bin/sh -x

HEX_SRC=${1/.cld/.hex}
S_SRC=${1/.cld/.s}

../bin/srec.exe -r -s $1

dos2unix < ${S_SRC} | sed -e 's/^S1......//' -e 's/..$//' -e 's/^S[09].*$//' -e 's/../0x&, /g' > ${HEX_SRC}

