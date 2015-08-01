#!/bin/bash

unset CCP4
unset CCP4_BROWSER
unset CDOC
unset CETC
unset CLIBS
unset CPROG
unset DBCCP4I_TOP
unset BINSORT_SCR
unset IMOSFLM_VERSION
unset CCP4_HELPDIR
unset CCP4I_HELP
unset MOLREPLIB
unset MOSFLM_WISH
unset PISA_CONF_FILE
unset PUBLIC_FONT84
unset GFORTRAN_UNBUFFERED_ALL
unset TCL_LIBRARY
unset TK_LIBRARY

bindir=$(cd $(dirname $0); pwd)

source $bindir/ccp4.setup-sh

ccp4-python "$@"

exit
