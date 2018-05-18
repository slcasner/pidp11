#!/bin/sh
echo "*** Start portmapper for RPC service ... fails if already running"
sudo rpcbind &
sleep 2

echo "*** Kill possibly running instances of Blinkenlight server ... only one allowed !"
sudo kill `pidof server11`
sleep 1


# which bootscript to run?
if [ $# -eq 0 ]
then
	# depending on SR switch settings, if a PiDP front panel is attached
	sudo ./scanswitch
	thefile=$(ls -t ../bootscripts/$?*.script | sort -g | tail -n 1)
else
	# depending on the command line argument, without PiDP attached
	thefile=$(ls -t ../bootscripts/$1?*.script | sort -g | tail -n 1)
fi
echo $thefile


echo "*** Start client/server ***"
sudo ./server11 &
sleep 5
sudo ./client11 $thefile