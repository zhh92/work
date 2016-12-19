
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!changed log!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//date:2016/10/20
//1:本接口在被调用中采用单进程单对象模式，原有数据库锁无法起到应有作用，作删除处理，不存在链接的抢占，线程锁无用，作删除处理，原信号量限定为1，并无实质用途，作删除处理
//2:mongo3.2.x不兼容旧2.x版驱动，故对驱动进行升级，api有所变动，更改部分函数，宏调用以适用新驱动
//3：用驱动自带副本集客户端取代旧的普通客户端，实现链接自动管理，发生故障主节点自动切换
//4：C_Mongo构造函数添加新参数传递副本集名称，兼容旧接口，默认为yjjs
//5:setindex接口提供是否建立唯一索引，兼容旧接口，默认为true




#ifndef _MONGODB_C_API_H_
#define _MONGODB_C_API_H_

#include<stdlib.h>  
#include<stdio.h>    
#include <iostream>

#include "mongo/client/dbclient.h"

#include "boost/shared_ptr.hpp"
 
//返回值定义
//////////业务处理返回值////////////////
#define RET_OK 1						 //正常返回
#define RET_DUPLICATE_ERR	2	 //重复
#define RET_NO_DUPLICATE 3	 //不重复

///////////常规错误返回值///////////////
#define RET_IN_PARAM_ERR -1	 //入参错误
#define RET_DEAL_ERR -2			 //处理错误
#define RET_CONNECT_ERR -3	 //链接错误
#define RET_AUTH_ERR -4			 //鉴权错误

///////////异常错误返回值///////////////
#define	RET_SOCKET_ERR -5		 //异常错误

//读写超时
#define WR_WAITTIME 30


using namespace std;
using namespace mongo;
using namespace boost;

 
class C_Mongo
{
	public:
			/*
			* 输入：
			* hostname :mogodb 全部主机部署的ip,mongos端口
									例如: 192.168.74.120:30000;192.168.74.76:30000;;192.168.74.122:30007;
			* mdb	 :数据库名
			* usename :数据库用户名	
			*	passwd	:数据库密码							
			* size :tcp连接数,默认连接数为1
							如果有多进程或者多线程,可以依据线程数(子进程数)设置size
			* wr_timeout:  tcp读写超时时间设置,默认30s 										
			*/
			C_Mongo(const string hostname,const string mdb,const string usename,const string passwd, const string rsname="yjjs",const double wr_timeout=WR_WAITTIME);
			~C_Mongo();
	public:
			static void initialize ();		
	public:
			
			/*链接数据,鉴权,创建连接
			*/
			int conn();
			
			/*切换数据库集合
			* 输入: 
			*			mdb:	数据库名
			*			mcollection:	集合名					
			*/			
			int setdoc(const string mdb,const string mcollection);
			
			/*删除数据库集合
			* 输入: 
			*			mdb:	数据库名
			*			mcollection:	集合名					
			*/			
			int dropdoc(const string mdb,const string mcollection);			

			/*设置索引
			* 输入: 
			*			key:	需要设置索引字段名				
			*/	
			int setindex(const string key, bool Unique = true);
			
			
			/*多列插入
			* 输入: 
			*			vector<string> fields:	字段名			
			*			vector<string> values:	字段值			
			*/							
			int insert(vector<string> fields,vector<string> values);

			/*单列插入
			* 输入: 
			*			field1:	字段名			
			*			value1:	字段值	
			*			field2:	字段名			
			*			value2:	字段值						
			*/							
			int insert(string field1,string value1,string field2,string value2);
			
			/*按批插入
			* 输入: 
			*		vector< map<string,string> >in 批量数据					
			*		其中map<string,string>:	key -value					
			*/				
			int batchinsert( vector< map<string,string> >in);
			
			/*按单一条件查询一条记录
			* 输入: 
			*			vector<string> in:	需要检索的字段名			
			*			string conditon_field:	条件字段名	
			*			string conditon_value:	条件字段值	
			* 输出: 			
			*			map<string,string> & out:key -value	对应值								
			*/	
			int select(vector<string> in,string conditon_field,string conditon_value,vector< map<string,string> >& out);
			
	
			/*单一条件update单列
			* 输入: 
			*			string key:			需要更新的列名	
			*			string value:		需要更新的列值
			*			string conditon_field:	条件字段名	
			*			string conditon_value:	条件字段值					
			*/	
			int update(string key ,string value,string conditon_field,string conditon_value);
			
			/*多条件update多列
			* 输入: 
			*			map<string,string> key_value:	需要更新的key-value			
			*			map<string,string> conditon_value:	更新的条件 key-value
			*/	
			int update(map<string,string> key_value, map<string,string> conditon_value);			

			/*单一条件删除记录
			* 输入: 
			*			filed :	条件列名			
			*			filed:	条件列值			
			*/
			int remove(string filed,string value);	

			/*多条件删除记录
			* 输入: 
			*			map<string,string>in 多对条件组
			*			string :	条件列名			
			*			string:	条件列值			
			*/
			int remove(map<string,string> in);	
			/*删除库下的所有集合
			* 输入: 
			*			const string mdb 数据库名
			*/			
			int dropalldoc();
			
			int exist(string collection);
	private:
		
			bool 	getipport(const char* hostname,vector<HostAndPort> &servers);
			int 	in_dropdoc(const string mdb,const string mcollection);	
			int 	in_setindex(const string key,bool Unique);
			int 	in_insert(vector<string> fields,vector<string> values);
			int 	in_insert(string field1,string value1,string field2,string value2);
			int 	in_batchinsert( vector< map<string,string> >in);
			int		in_select(vector<string> in,string conditon_field,string conditon_value,vector< map<string,string> >& out);
			int 	in_update(string key ,string value,string conditon_field,string conditon_value);
			int 	in_update(map<string,string> key_value, map<string,string> conditon_value);	
			int 	in_remove(string filed,string value);	
			int 	in_remove(map<string,string> in);	
			int 	in_conn();
			int	  in_dropalldoc();
			int in_exist(string collection);
	private:
		
			//数据库.集合名
			string doc;	
			//链接管理容器
			shared_ptr<DBClientReplicaSet> _p_conn;
			//host+port
			vector<HostAndPort> _servers;	
			//数据库名,用户名,密码
 			string m_datebase,m_usename,m_passwd;

};
#endif
