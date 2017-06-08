function datenum(n){
	switch(n)
	{
		case 1:;
		case 3:;
		case 5:;
		case 7:;
		case 8:;
		case 10:;
		case 12:return 31;
		default:return 30;
	}
}

	function createIndex(x,n){
		var prefix=x;
		prefix+='_2016';
		
		if(n<10){
			prefix+='0';
		}
		prefix+=String(n);
		
		var num=datenum(n);
		for(var i=1;i<num+1;i++){
			var tn=prefix;
			if(i<10){
				tn+='0';
			}
			tn+=String(i);
			db[tn].ensureIndex({index:1},{unique:1});
			db[tn].ensureIndex({batch:1});
		}	
	}

161-171
181-191
for(var i=161;i!=172;i++){
	var x=String(i)+'0';
	for(var j=8;j!=11;j++){
		createIndex(String(i)+'0',j);
	}
}
for(var i=181;i!=192;i++){
	var x=String(i)+'0';
	for(var j=8;j!=11;j++){
		createIndex(String(i)+'0',j);
	}
}
for(var i=811;i!=815;i++){
	var x=String(i)+'0';
	for(var j=8;j!=11;j++){
		createIndex(String(i)+'0',j);
	}
}

for(var i=112;i!=113;i++){
	var x=String(i)+'0';
	for(var j=9;j!=12;j++){
		createIndex(String(i)+'0',j);
	}
}

for(var i=161;i!=172;i++){
	var x=String(i)+'0';
	for(var j=9;j!=12;j++){
		createIndex(String(i)+'0',j);
	}
}

for(var i=181;i!=192;i++){
	var x=String(i)+'0';
	for(var j=9;j!=12;j++){
		createIndex(String(i)+'0',j);
	}
}

for(var i=701;i!=709;i++){
	var x=String(i)+'0';
	for(var j=9;j!=12;j++){
		createIndex(String(i)+'0',j);
	}
}

for(var i=811;i!=815;i++){
	var x=String(i)+'0';
	for(var j=9;j!=12;j++){
		createIndex(String(i)+'0',j);
	}
}

索引数查询
db.getCollectionNames().forEach(
	function(x){
		if(3!=db[x].getIndexes().length){
			print(x);
		}
	}
)

重命名
db.getCollectionNames().forEach(
	function(x){
		db[x].renameCollection('0000_'+x);
	}
)

匹配
function matchcount(x,y){
	var count=0
	db.getCollectionNames().forEach(
		function(x){
			if(x.match('0000_')){
				return;
			}
			for(var i=x;i!=y+1;i++){
				if(x.match(String(i)+'0_')){
					count+=1;
				}
			}
		}
	)
	print(count)
}


db.getCollectionNames().forEach(
	function(x){
		if(x.match('0000_')){
			return;
		}
		db[x].drop()
	}
)


function newDateCount(){
	var count=0;
	db.getCollectionNames().forEach(
		function(x){
			if(x.match('0000_')){
				return;
			}
			count+=db[x].count();
		}
	)
	print(count)
}


db.getCollectionNames().forEach(
	function(x){
		if(x.match('0000_')){
			return;
		}
		db[x].drop()
	}
)

db.getCollectionNames().forEach(
	function(x){
		if(x.match('0000_'){
			db[x].drop()
		}
		if(x.match('_2016'){
			db[x].drop()
		}
	}
)

db.getCollectionNames().forEach(
	function(x){
		if(x.match('_2017')){
			if(db[x].count()>0){
				print(x,db[x].count(),db.getIndexes().length)
			}
		}
	}
)

db.currentOp({secs_running:{$gt:50}})

db.currentOp({secs_running:{$gt:3},ns:{$regex:'yjjs'}})