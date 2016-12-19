#!/bin/sh
ROOT_PATH='/home/mongodb/'
PACKET_PATH='/home/mongodb/tcltest/packet/'
INSTALL_PATH='/home/mongodb/tcltest/install/'

PASSWORD='mongodb123'

export PATH
export PATH=`pwd`:$PATH

tar zxvf libs.tgz 1>/dev/null
INSTALLLIB=`pwd`'/libs/install'
export LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$INSTALLLIB:$LD_LIBRARY_PATH

cmd=''
for host in $*
do
sleep 10
remotepath=$host':'$PACKET_PATH
expect ssh.exp $host $PASSWORD 'mkdir -p '$PACKET_PATH  1>/dev/null
if [ $? -eq 0 ]
then
echo $host':mkdir '$PACKET_PATH' successs!'
else
echo $host':mkdir '$PACKET_PATH' error!'
exit
fi

expect ssh.exp $host $PASSWORD 'mkdir -p '$INSTALL_PATH 1>/dev/null
if [ $? -eq 0 ]
then
echo $host':mkdir '$INSTALL_PATH' successs!'
else
echo $host':mkdir '$INSTALL_PATH' error!'
exit
fi

expect scp.exp packet.tgz $PASSWORD $remotepath 1>/dev/null
if [ $? -eq 0 ]
then
echo $host':scp packet.tgz successs!'
else
echo $host':scp packet.tgz  error!'
exit
fi

expect scp.exp install_local.sh $PASSWORD $remotepath 1>/dev/null
if [ $? -eq 0 ]
then
echo $host':scp install_local.sh successs!'
else
echo $host':scp install_local.sh error!'
exit
fi

expect scp.exp libs.tgz $PASSWORD $remotepath 1>/dev/null
if [ $? -eq 0 ]
then
echo $host':scp libs.tgz successs!'
else
echo $host':scp libs.tgz error!'
exit
fi

expect ssh.exp $host $PASSWORD sh ${PACKET_PATH}/install_local.sh $PACKET_PATH $INSTALL_PATH 1>/dev/null
if [ $? -eq 0 ]
then
echo $host':exec install_local.sh successs!'
else
echo $host':exec install_local.sh error!'
exit
fi

done
