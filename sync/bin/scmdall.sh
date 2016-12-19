#!/bin/sh

source ~/.bashrc

cmd=''
for argv in $@
do
	cmd=$cmd' '$argv	
done

localhost=$(/sbin/ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:")

while read line
do
	if [[ $line != $localhost && -n $line ]]
	then
		Scmd $line $cmd
	fi 
done < hostlist
