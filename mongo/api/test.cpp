#include "mongodbapi.h"
int main( int argc, const char **argv ) 
{	
		mongo::client::initialize();


		string hostname="192.168.74.76:27111;";
		C_Mongo  gv_mongo(hostname,"yjjs","yjjs","yjjs123");
		if(gv_mongo.conn()!=RET_OK)
		{
				//提交批次失败，退出
				return -1;
		}
		string starttime="201408031001";
		string doc="filter_"+starttime.substr(0,6);
		gv_mongo.setdoc("yjjs",doc);
		int ireturn;
		
		/*
		map<string,string> data;
		char c_cn[64];
		char c_batch[32];
		for(int j=0;j!=100000;++j){
			memset(c_cn,0,64);
			memset(c_batch,0,32);
			sprintf(c_cn,"%039d",rand()%RAND_MAX);
			sprintf(c_batch,"%015d",rand()%RAND_MAX);
			data[string(c_cn)]=string(c_batch);
		}
		map<string,string>::iterator iter=data.begin();
		gv_mongo.setindex("callnumber");
		while(iter!=data.end()){
			ireturn=gv_mongo.insert("callnumber", iter->first,"batch",iter->second);
			iter++;
		}
		*/
		ireturn=gv_mongo.insert("callnumber", "185518882502014101115101020141011151059","batch","011020141212011");
		if( (ireturn!=RET_DUPLICATE_ERR)&&(ireturn!=RET_NO_DUPLICATE) )
		{
					if(ireturn==RET_DEAL_ERR)
					{
							//重做
							gv_mongo.insert("callnumber", "186518882502014101115101020141011151059","batch","011020141212011");
					}
					if(ireturn==RET_IN_PARAM_ERR)
					{
							//重新输入函数入参
							gv_mongo.insert("callnumber", "186518882502014101115101020141011151059","batch","011020141212011");
					}
					if(ireturn==RET_SOCKET_ERR)
					{
									
									if(gv_mongo.conn()!=RET_OK)
									{
											//提交批次失败，退出
											return -1;
									}
									else
									{		
											//重试一次
											gv_mongo.setdoc("yjjs",doc);
											
											ireturn=gv_mongo.insert("callnumber", "186518882502014101115101020141011151059","batch","011020141212011");
											if(ireturn!=RET_DUPLICATE_ERR||ireturn!=RET_NO_DUPLICATE)
											{
													//提交批次失败，退出
											}
									}
					}
					
		}
		//按照批次删除索引
		gv_mongo.setdoc("yjjs",doc);
		ireturn=gv_mongo.remove("batch","011020141212011"); 
		if(ireturn!=RET_OK)
		{
				if(ireturn==RET_IN_PARAM_ERR)
				{	
							//检查入参 重新输入函数入参
							ireturn=gv_mongo.remove("batch","011020141212011");
				}	
				if(ireturn==RET_DEAL_ERR)
				{	
						//重试
						ireturn=gv_mongo.remove("batch","011020141212011");
				}	
				if(ireturn==RET_SOCKET_ERR)
				{	
									
									if(gv_mongo.conn()!=RET_OK)
									{
											//提交批次失败，退出
											return -1;
									}
									else
									{		
											//重试一次
											gv_mongo.setdoc("yjjs",doc);
											
											ireturn=gv_mongo.remove("batch","011020141212011");
											if(ireturn!=RET_OK)
											{
													//提交批次失败，退出
											}
									}
				}			
		}
		
////		批量插入
//		vector< map<string,string> >batchin;
//		map<string,string>map_tmp;
//		char chtmp[4];
//		for (int i=0;i<100000;i++)
//		{
//				memset(chtmp,0,sizeof(chtmp));
//				sprintf(chtmp,"%d",i);
//				
//				map_tmp.clear();
//				map_tmp.insert( map<string,string>::value_type( string("callnumber"), string(chtmp) ) );	
//				map_tmp.insert( map<string,string>::value_type( string("starttime"), string("20141011151010") ) );				
//				map_tmp.insert( map<string,string>::value_type( string("endtime"), string("20141011151059") ) );	
//
//				batchin.push_back(map_tmp);
//		}
//		gv_mongo.batchinsert(batchin);

////		查询测试		
//		vector<string> selectin;
//		selectin.clear();
//		selectin.push_back("callnumber");
//		selectin.push_back("starttime");
//		selectin.push_back("endtime");
//		string conditon_field="starttime";
//		string conditon_value="20141011151010";
//		
//		vector< map<string,string> > selectout;
//		selectout.clear();
//		if(gv_mongo.select(selectin,conditon_field,conditon_value,selectout)!=RET_OK)
//		{
//				return 0;
//		}	
//		cout<<"count: "<<selectout.size()<<endl;
//		if(selectout.size()==0)
//		{				
//				return 0;
//		}
//		int i=1;
//		for(vector< map<string,string> >::iterator iter=selectout.begin();iter!=selectout.end();iter++)	
//		{
//				
//				cout<<"count: "<<i<<endl ;
//				for(map<string,string>::iterator map_iter=iter->begin();map_iter!=iter->end();map_iter++)	
//				{	
//						cout<<map_iter->first<<": "<<map_iter->second <<endl;	
//				}	
//				i++	;
//		}
	
//  单一条件更新单一字段
//		gv_mongo.update("callnumber" ,"18651888888","starttime","20141011151010");

		
////	多条件更新测试
//		map<string,string> key_value;
//		key_value.clear();
//		key_value.insert( map<string,string>::value_type( string("callnumber"), string("18651999999") ) );	
//		key_value.insert( map<string,string>::value_type( string("endtime"), string("20141011111111") ) );	
//			
//		map<string,string> conditonvalue;
//		conditonvalue.clear();
//		conditonvalue.insert( map<string,string>::value_type( string("callnumber"), string("18651888888") ) );	
//		conditonvalue.insert( map<string,string>::value_type( string("endtime"), string("20141011151059") ) );	
//		conditonvalue.insert( map<string,string>::value_type( string("starttime"), string("20141011151010") ) );		
//		
//		gv_mongo.update(key_value,conditonvalue);		
//
//
////	 单一条件删除测试
//		gv_mongo.remove("callnumber","18651888888");
//
////	 多条件删除	
//		map<string,string> in	;
//		in.clear();
//		in.insert( map<string,string>::value_type( "callnumber", "18551677620" ) );
//		in.insert( map<string,string>::value_type( "starttime", "20141016151010" ) );				
//		in.insert( map<string,string>::value_type( "endtime", "20141016151059") );			
//		gv_mongo.remove(in);
		return 0;
}
