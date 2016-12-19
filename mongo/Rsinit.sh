#!/bin/sh

if [ $# -ne 4 ]
then
echo 'Usage:'
echo '  '$0' host1:port1 host2:port2 host3:port3'
exit -1
fi

config="config={_id:'yjjs',members:[{_id:0,host:'"$1"'},{_id:1,host:'"$2"'},{_id:2,host:'"$3"'}]}"
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
	}else{
		print("@Create user:yjjs")
		i=0
		for(;i!=5;i++){
			sleep(2000)
			if(db.isMaster().primary!=null){
				master=Mongo(db.isMaster().primary)
				if(master.getDB('yjjs').getUser('yjjs')!=null){
					print("@create user(yjjs) already existes!")
					print("@All done")
					quit()
				}
				master.getDB('yjjs').createUser({user:'yjjs',pwd:'yjjs123',roles:[{role:'readWrite',db:'yjjs'}]})
				if(master.getDB('yjjs').getUser('yjjs')!=null){
					print("@create user(yjjs) success!")
					print("@All done")
				}else{
					print("@create user(yjjs) error!")
				}
				quit()
			}
		}
	}	
}else{
	print("@RS initialization has error!")
}
exit
