#!/bin/sh

if [ $# -ne 3 ]
then
echo 'Usage:'
echo '  '$0' host1:port1 host2:port2 host3:port3'
exit -1
fi

config="config={_id:'shard1Rs',members:[{_id:0,host:'"$1"'},{_id:1,host:'"$2"'},{_id:2,host:'"$3"'}]}"
CONN='mongo '$1
$CONN <<EOF
use yjjs
print("@Connect success!")
print("@RS initiate!")
if(rs.initiate(${config}).ok==1){
	print("@RS initializing")
	var i=0
	for(;i!=6;i++)
	{
		sleep(5000)
		if(rs.status().ok!=1){
			print("@RS initialization isn't completed!Plz waiting with patience!")
		}else{
			break
		}
	}
	if(i==6){
		print("@RS initialization failed")
	}
	print("@RS initialization success!")	
}else{
	print("@RS initialization has error!")
}
exit
