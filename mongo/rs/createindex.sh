#!/bin/sh
if [ $# == 0 ]
then
hostport='127.0.0.1:27111'
else
hostport=$1
fi
CONN='mongo '$hostport
$CONN <<EOF > master.temp
print(db.isMaster().primary)
EOF

n=0
while read line
do
let 'n+=1'
if [ $n == 3 ]
then
break
fi
done < master.temp
rm master.temp

mongo $line createindex.js
