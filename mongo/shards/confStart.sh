#!/bin/sh
export MONGODB_BIN
if [ "$MONGODB_BIN" = "" ];then
	MONGODB_BIN='/app/mongodb/mongodb/bin'
fi
#'/zhjs/mongodb/bin'
shards_root=${MONGODB_BIN}'/shards'
startlog=${shards_root}'/start.log'
db_root=${shards_root}"/dbs"
log_root=${shards_root}"/logs"
portbegin=30001
nodebase="confignode"

if [ ! -f $startlog ];then
	touch $startlog
fi

if [ ! -d $log_root ];then
        echo "logroot isn't exist.make it." >>$startlog
        mkdir -p  $log_root
fi

echo >>$startlog
echo >>$startlog
echo `date` >>$startlog
echo "starting">>$startlog
for i in {1..3}
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
numactl --interleave=all ${MONGODB_BIN}/mongod -f ${shards_root}/configsvr.conf --dbpath $dbpath --logpath $logpath --port $port --auth --keyFile ${shards_root}/keyFile >>$startlog
done
