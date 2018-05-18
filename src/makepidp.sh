
#!/bin/bash


# stop on error
set -e

# Debugging:
# set -x


pwd
#export MAKEOPTIONS=--silent
#export MAKETARGETS="clean all"

# optimize all compiles, see makefiles
#export MAKE_CONFIGURATION=DEBUG
export MAKE_CONFIGURATION=RELEASE


(
# the Blinkenlight API server for PiDP11
cd 11_pidp_server/pidp11
echo ; echo "*** blinkenlight_server for PiDP11"
MAKE_TARGET_ARCH=RPI make $MAKEOPTIONS $MAKETARGETS
)


(
# SimH client
cd 02.3_simh/4.x+realcons/src
echo ; echo "*** SimH 4.x for RaspberryPi (takes a while!)"
MAKE_TARGET_ARCH=RPI make $MAKEOPTIONS $MAKETARGETS pdp11_realcons
)

echo
echo "All OK!"
