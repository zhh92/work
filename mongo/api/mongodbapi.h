
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!changed log!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//date:2016/10/20
//1:���ӿ��ڱ������в��õ����̵�����ģʽ��ԭ�����ݿ����޷���Ӧ�����ã���ɾ���������������ӵ���ռ���߳������ã���ɾ������ԭ�ź����޶�Ϊ1������ʵ����;����ɾ������
//2:mongo3.2.x�����ݾ�2.x���������ʶ���������������api�����䶯�����Ĳ��ֺ����������������������
//3���������Դ��������ͻ���ȡ���ɵ���ͨ�ͻ��ˣ�ʵ�������Զ����������������ڵ��Զ��л�
//4��C_Mongo���캯������²������ݸ��������ƣ����ݾɽӿڣ�Ĭ��Ϊyjjs
//5:setindex�ӿ��ṩ�Ƿ���Ψһ���������ݾɽӿڣ�Ĭ��Ϊtrue




#ifndef _MONGODB_C_API_H_
#define _MONGODB_C_API_H_

#include<stdlib.h>  
#include<stdio.h>    
#include <iostream>

#include "mongo/client/dbclient.h"

#include "boost/shared_ptr.hpp"
 
//����ֵ����
//////////ҵ������ֵ////////////////
#define RET_OK 1						 //��������
#define RET_DUPLICATE_ERR	2	 //�ظ�
#define RET_NO_DUPLICATE 3	 //���ظ�

///////////������󷵻�ֵ///////////////
#define RET_IN_PARAM_ERR -1	 //��δ���
#define RET_DEAL_ERR -2			 //�������
#define RET_CONNECT_ERR -3	 //���Ӵ���
#define RET_AUTH_ERR -4			 //��Ȩ����

///////////�쳣���󷵻�ֵ///////////////
#define	RET_SOCKET_ERR -5		 //�쳣����

//��д��ʱ
#define WR_WAITTIME 30


using namespace std;
using namespace mongo;
using namespace boost;

 
class C_Mongo
{
	public:
			/*
			* ���룺
			* hostname :mogodb ȫ�����������ip,mongos�˿�
									����: 192.168.74.120:30000;192.168.74.76:30000;;192.168.74.122:30007;
			* mdb	 :���ݿ���
			* usename :���ݿ��û���	
			*	passwd	:���ݿ�����							
			* size :tcp������,Ĭ��������Ϊ1
							����ж���̻��߶��߳�,���������߳���(�ӽ�����)����size
			* wr_timeout:  tcp��д��ʱʱ������,Ĭ��30s 										
			*/
			C_Mongo(const string hostname,const string mdb,const string usename,const string passwd, const string rsname="yjjs",const double wr_timeout=WR_WAITTIME);
			~C_Mongo();
	public:
			static void initialize ();		
	public:
			
			/*��������,��Ȩ,��������
			*/
			int conn();
			
			/*�л����ݿ⼯��
			* ����: 
			*			mdb:	���ݿ���
			*			mcollection:	������					
			*/			
			int setdoc(const string mdb,const string mcollection);
			
			/*ɾ�����ݿ⼯��
			* ����: 
			*			mdb:	���ݿ���
			*			mcollection:	������					
			*/			
			int dropdoc(const string mdb,const string mcollection);			

			/*��������
			* ����: 
			*			key:	��Ҫ���������ֶ���				
			*/	
			int setindex(const string key, bool Unique = true);
			
			
			/*���в���
			* ����: 
			*			vector<string> fields:	�ֶ���			
			*			vector<string> values:	�ֶ�ֵ			
			*/							
			int insert(vector<string> fields,vector<string> values);

			/*���в���
			* ����: 
			*			field1:	�ֶ���			
			*			value1:	�ֶ�ֵ	
			*			field2:	�ֶ���			
			*			value2:	�ֶ�ֵ						
			*/							
			int insert(string field1,string value1,string field2,string value2);
			
			/*��������
			* ����: 
			*		vector< map<string,string> >in ��������					
			*		����map<string,string>:	key -value					
			*/				
			int batchinsert( vector< map<string,string> >in);
			
			/*����һ������ѯһ����¼
			* ����: 
			*			vector<string> in:	��Ҫ�������ֶ���			
			*			string conditon_field:	�����ֶ���	
			*			string conditon_value:	�����ֶ�ֵ	
			* ���: 			
			*			map<string,string> & out:key -value	��Ӧֵ								
			*/	
			int select(vector<string> in,string conditon_field,string conditon_value,vector< map<string,string> >& out);
			
	
			/*��һ����update����
			* ����: 
			*			string key:			��Ҫ���µ�����	
			*			string value:		��Ҫ���µ���ֵ
			*			string conditon_field:	�����ֶ���	
			*			string conditon_value:	�����ֶ�ֵ					
			*/	
			int update(string key ,string value,string conditon_field,string conditon_value);
			
			/*������update����
			* ����: 
			*			map<string,string> key_value:	��Ҫ���µ�key-value			
			*			map<string,string> conditon_value:	���µ����� key-value
			*/	
			int update(map<string,string> key_value, map<string,string> conditon_value);			

			/*��һ����ɾ����¼
			* ����: 
			*			filed :	��������			
			*			filed:	������ֵ			
			*/
			int remove(string filed,string value);	

			/*������ɾ����¼
			* ����: 
			*			map<string,string>in ���������
			*			string :	��������			
			*			string:	������ֵ			
			*/
			int remove(map<string,string> in);	
			/*ɾ�����µ����м���
			* ����: 
			*			const string mdb ���ݿ���
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
		
			//���ݿ�.������
			string doc;	
			//���ӹ�������
			shared_ptr<DBClientReplicaSet> _p_conn;
			//host+port
			vector<HostAndPort> _servers;	
			//���ݿ���,�û���,����
 			string m_datebase,m_usename,m_passwd;

};
#endif
