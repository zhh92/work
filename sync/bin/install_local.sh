#!/bin/sh
#PACKET_PATH='/home/mongodb/tcl/bin/tools/'
#INSTALL_PATH='/home/mongodb/tcl/bin/tools/'

PACKET_PATH=$1
INSTALL_PATH=$2
PWD=`pwd`

MONGODBCLIENT_RUNTIME=${INSTALL_PATH}'/clientRuntime'
cd ${PACKET_PATH}
tar zxvf libs.tgz 1>/dev/null
mv libs/runtime $MONGODBCLIENT_RUNTIME


MONGODB_BIN=${INSTALL_PATH}'/mongodb/bin/'

cd ${PACKET_PATH}
tar zxvf packet.tgz 1>/dev/null
cd packet
tar zxvf mongodb-linux-x86_64*.tgz 1>/dev/null
rm mongodb-linux-x86_64*.tgz -fr
mv mongodb-linux-x86_64* ${INSTALL_PATH}/mongodb
mv mdbStart.sh ${MONGODB_BIN}
mv node.conf ${MONGODB_BIN}
chmod +x ${MONGODB_BIN}/mdbStart.sh


#echo ${MONGODB_BIN}'/mdbStart.sh'>> /etc/rc.local
#echo 'export MONGODB_BIN='${MONGODB_BIN} >> ~/.bashrc
#echo 'export MONGODBCLIENT_RUNTIME='${MONGODBCLIENT_RUNTIME} >> ~/.bashrc

${MONGODB_BIN}/mdbStart.sh

cd ..
rm packet -rf
cd $PWD
