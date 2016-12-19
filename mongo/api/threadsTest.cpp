#include "mongodbapi.h"
#include <iostream>
#include <vector>
#include<time.h>

#include<boost/thread.hpp>
using namespace boost;

#define THREAD_NUM_DEFAULT 10
#define DATA_NUM 100000


void insert(map<string,string>& Map,string& doc,string& db){
	
	
	string hostname="136.142.13.27:27111;136.142.13.28:27111;136.142.13.29:27111;";
	C_Mongo gv_mongo(hostname,"test","yjjs","yjjs123");
	int iret=gv_mongo.conn();
	if(iret!=RET_OK){
		cout<<"connect error!"<<endl;
		cout<<iret<<endl;
		exit(0);
	}else{
		gv_mongo.setdoc(db,doc);
		if(RET_OK!=gv_mongo.exist(doc)){
			gv_mongo.setindex("callnumber");
		}
		for(map<string,string>::iterator iter=Map.begin();iter!=Map.end();iter++){
			int ret=gv_mongo.insert("callnumber",iter->first,"batch",iter->second);
			if(ret==RET_DEAL_ERR){
                                cout<<"deal error!"<<endl;
                        }else if(ret==RET_SOCKET_ERR){
				cout<<"socket err!"<<endl;
                        }else if(ret==RET_DUPLICATE_ERR){
				//cout<<"dup err!"<<endl;
			}
		}
	}
}

int main( int argc, const char **argv )
{
	
		 C_Mongo::initialize();
	
                map<string,string> data;
                int threadnum;
                if(argc>2){
                        threadnum=atoi(argv[2]);
                }else{
                        threadnum=THREAD_NUM_DEFAULT;
                }
                thread_group tg;
                if(argc>1){
                        srand((int)time(0));
                }
                char c_cn[64];
                char c_batch[32];
                for(int j=0;j!=DATA_NUM;++j){
                        memset(c_cn,0,64);
                        memset(c_batch,0,32);
                        sprintf(c_cn,"%039d",rand()%RAND_MAX);
                        sprintf(c_batch,"%015d",rand()%RAND_MAX);
                        data.insert(pair<string,string>(string(c_cn),string(c_batch)));
		}                

                for(int i=0;i!=threadnum;i++){
                        char nodenum[2];
                        sprintf(nodenum,"%d",i);
                        string doc("node"+string(nodenum));
			string db1="test";
			string db2="test2";
			tg.create_thread(bind(&insert,data,doc,db1));
			tg.create_thread(bind(&insert,data,doc,db2));
                }
                tg.join_all();
}
