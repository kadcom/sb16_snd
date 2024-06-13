#!/usr/bin/fish

set -x WATCOM /opt/watcom
set -x PATH $WATCOM/armo64 $WATCOM/armo $PATH
set -x EDPATH $WATCOM/eddat

# these variables change based on compilation target
# defaults are set for native compilation
set -x INCLUDE $WATCOM/h
# set -x LIB 
