#!/bin/sh

source ~/.bashrc

if [ ! -r 'hostlist' ]
then
	echo "can't read file:hostlist!"
	exit
fi

if [ $# -ne 2 ]
then
	echo "wrong param!"
	echo "Usage:"$0" filename remotepath"
	exit
fi

if [ ! -f $1 ]
then
	echo $1" isn't a right file!"
	exit
fi

localhost=$(/sbin/ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:")

while read line
do
	if [[ $line != $localhost && -n $line ]]
	then
		Scp $1 $line':'$2
	fi 
done < hostlist
