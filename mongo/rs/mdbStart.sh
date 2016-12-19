

#!/bin/sh
export MONGODB_BIN='/zhjs/mongodb/bin'
db_root=${MONGODB_BIN}"/zipdbs"
log_root=${MONGODB_BIN}"/logs"
portbegin=27111
nodebase="node"

if [ ! -d $log_root ];then
        echo "logroot isn't exist.make it." >>authstart.log
        mkdir -p  $log_root
fi

echo >>start.log
echo >>start.log
echo `date` >>start.log
echo "starting">>start.log
for i in 1
do
node=$nodebase$i
port=$portbegin
let portbegin++

dbpath=$db_root"/"$node
if [ ! -d $dbpath ];then
        echo $dbpath"isn't exist.make it." >>start.log
        mkdir -p $dbpath
fi

logpath=$log_root"/"$node".log"
startlog=${MONGODB_BIN}'/start.log'
echo "===========================================================" >>$startlog
echo "node:"$node >>$startlog
echo "dbpath:"$dbpath >>$startlog
echo "logpath:"$logpath >>$startlog
echo "port:"$port >>$startlog
echo "mongod starting..." >>$startlog
numactl --interleave=all ${MONGODB_BIN}/mongod -f ${MONGODB_BIN}/node.conf --dbpath $dbpath --logpath $logpath --port $port --syncdelay 1 >>$startlog
done
