#!/bin/sh
CONN='/app/mongodb/mongodb/bin/mongo --port 27111'
$CONN <<EOF
if(db.isMaster().primary!=null){
Mongo(db.isMaster().primary).getDB('yjjs').dropDatabase()
}else{
print('dropDatabase error!')
}
exit
