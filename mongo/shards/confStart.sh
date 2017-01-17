#!/bin/sh
export MONGODB_BIN='/zhjs/mongodb/bin'
startlog=${MONGODB_BIN}'/shards/start.log'
db_root=${MONGODB_BIN}"/shards/dbs"
log_root=${MONGODB_BIN}"/shards/logs"
portbegin=30000
nodebase="confignode"



if [ ! -d $log_root ];then
        echo "logroot isn't exist.make it." >>$startlog
        mkdir -p  $log_root
fi

echo >>$startlog
echo >>$startlog
echo `date` >>$startlog
echo "starting">>$startlog
for i in 1
do
node=$nodebase$i
port=$portbegin
let portbegin++

dbpath=$db_root"/"$node
if [ ! -d $dbpath ];then
        echo $dbpath"isn't exist.make it." >>$startlog
        mkdir -p $dbpath
fi

logpath=$log_root"/"$node".log"

echo "===========================================================" >>$startlog
echo "node:"$node >>$startlog
echo "dbpath:"$dbpath >>$startlog
echo "logpath:"$logpath >>$startlog
echo "port:"$port >>$startlog
echo "mongod starting..." >>$startlog
numactl --interleave=all ${MONGODB_BIN}/mongod -f ${MONGODB_BIN}/node.conf --dbpath $dbpath --logpath $logpath --port $port >>$startlog
done
