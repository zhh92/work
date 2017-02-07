#!/bin/sh
#ps -ef|grep mongod|awk '$16!="" && $16<30004 && $16>30000 {system("kill -2 "$2)}'
#localhost:30001 localhost:30002 localhost:30003
#localhost:32001 localhost:32002 localhost:32003 localhost:31001 localhost:31002 localhost:31003
for i in $* ;do
	mongo $i/admin --eval "db.shutdownServer()"
done
