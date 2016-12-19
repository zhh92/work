#include "mongodbapi.h"

/////////////////////////////////////pubilc///////////////////////////////////////////////
void C_Mongo::initialize(){
	mongo::client::initialize();
}
C_Mongo::C_Mongo(const string hostname, const string mdb, const string usename, const string passwd, const string rsname, const double wr_timeout)
{


	if (mdb.empty() || usename.empty() || passwd.empty() || hostname.empty())
	{
		exit(0);
	}

	//初始化变量
	doc.clear();

	m_datebase = mdb;
	m_usename = usename;
	m_passwd = passwd;

	_servers.clear();
	if (!getipport(hostname.c_str(), _servers))
	{
		exit(0);
	}

	_p_conn.reset(new DBClientReplicaSet(rsname, _servers, wr_timeout));
}


C_Mongo::~C_Mongo()
{
}

int C_Mongo::setdoc(const string mdb,const string mcollection)
{
		if(mdb.empty() || mcollection.empty())
		{
			return RET_IN_PARAM_ERR;
		}
		doc=mdb+"."+mcollection;
		return RET_OK;
}

int C_Mongo::conn()
{	
		try 
		{
		    return in_conn();
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif   
							    
		    return RET_SOCKET_ERR;
		}	
}


int C_Mongo::dropdoc(const string mdb,const string mcollection)
{
		try 
		{
		    return in_dropdoc(mdb,mcollection);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif   
							    
		    return RET_SOCKET_ERR;
		}	
}	


int C_Mongo::setindex(const string key,bool Unique)
{
		try 
		{
		    return in_setindex(key,Unique);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
				 
		    return RET_SOCKET_ERR;
		}	
}


int C_Mongo::insert(vector<string> fields,vector<string> values)
{
		try 
		{
		    return in_insert(fields,values);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif 
							    
		    return RET_SOCKET_ERR;
		}	
}


int C_Mongo::insert(string field1,string value1,string field2,string value2)
{
		try 
		{
		    return in_insert(field1,value1,field2,value2);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
							    
		    return RET_SOCKET_ERR;
		}	
}


int C_Mongo::batchinsert( vector< map<string,string> >in)
{
		try 
		{
		    return in_batchinsert(in);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
							    
		    return RET_SOCKET_ERR;
		}	
}


int	C_Mongo::select(vector<string> in,string conditon_field,string conditon_value,vector< map<string,string> >& out)
{
		try 
		{
		    return in_select(in,conditon_field,conditon_value,out);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
							    
		    return RET_SOCKET_ERR;
		}	
}


int C_Mongo::update(string key ,string value,string conditon_field,string conditon_value)
{
		try 
		{
		    return in_update(key,value,conditon_field,conditon_value);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
							    
		    return RET_SOCKET_ERR;
		}	
}


int C_Mongo::update(map<string,string> key_value, map<string,string> conditon_value)	
{
		try 
		{
		    return in_update(key_value,key_value);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
						    
		    return RET_SOCKET_ERR;
		}	
}


int C_Mongo::remove(string filed,string value)
{
		try 
		{
		    return in_remove(filed,value);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
		    

							    
		    return RET_SOCKET_ERR;
		}	
}	


int C_Mongo::remove(map<string,string> in)	
{
		try 
		{
		    return in_remove(in);
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
							    
		    return RET_SOCKET_ERR;
		}	
}

int C_Mongo::dropalldoc()
{
		try 
		{
		    return in_dropalldoc();
		}
		catch ( std::exception& e ) 
		{
				#ifdef D_BUG
		    cout << "ERROR: " << e.what() << endl;
		    #endif
							    
		    return RET_SOCKET_ERR;
		}	
}

int C_Mongo::exist(string collection){
	try{
		return in_exist(collection);
	}catch(std::exception& e){
		#ifdef D_BUG
				cout<<"ERROR:"<<e.what()<<endl;
		#endif
		
		return RET_SOCKET_ERR;
	}
}
/////////////////////////////private///////////////////////////////////
int C_Mongo::in_conn()
{
		string errmsg;

		if(!_p_conn->connect())
		{
				return RET_CONNECT_ERR;				
		}else if(!_p_conn->auth(m_datebase, m_usename, m_passwd,errmsg)){
			return RET_AUTH_ERR;
		}
		return RET_OK;
}

int C_Mongo::in_dropdoc(const string mdb,const string mcollection)
{
		if(mdb.empty() || mcollection.empty())
		{
			return RET_IN_PARAM_ERR;
		}
		doc=mdb+"."+mcollection;	
		
		if(!_p_conn->dropCollection(doc) )	
		{
				return RET_DEAL_ERR;
		}
		return RET_OK;
}

int C_Mongo::in_dropalldoc()
{
	    int ireturn=0;
		string collectname;	
		//找出数据库所有集合
		list< string > list_all_doc;
		list_all_doc.clear();
		list_all_doc=_p_conn->getCollectionNames (m_datebase);
		list< string >::iterator it;
		for( it=list_all_doc.begin(); it!=list_all_doc.end(); it++)
		{
				collectname.clear();
				collectname=doc=*it;
				if(collectname.find("filter_",0)==string::npos)
					continue;
			  
				if(!_p_conn->dropCollection(collectname) )	
				{
						return RET_DEAL_ERR;
				}
		}
		return RET_OK;
}

int C_Mongo::in_setindex(const string key,bool Unique=true)
{

		if(key.empty())
		{
				return RET_IN_PARAM_ERR;
		}		
		//创建索引
		IndexSpec is;
		is.addKey(key);
		is.unique(Unique);
		is.background(true);
		_p_conn->createIndex(doc,is);
		
		//确认索引是否创建成功
		std::list< std::string > l_indexes=_p_conn->getIndexNames(doc);
		string keyname = key + "_1";
		if(l_indexes.end()==find(l_indexes.begin(),l_indexes.end(), keyname))
		{
			return RET_DEAL_ERR;
		}
		return RET_OK;
}


int C_Mongo::in_insert(string field1,string value1,string field2,string value2)
{
		BSONObjBuilder query,tepquery;
		BSONObj tmpobj,rejs,idxobj;
		string errmsg;
		
		if(field1.empty()||value1.empty()||field2.empty()||value2.empty())
		{
				return RET_IN_PARAM_ERR;
		}	
		tepquery.append( field1 ,value1);	
		idxobj=tepquery.obj();
		
		query.append( field1 ,value1);		
		query.append( field2 ,value2);	
		tmpobj=query.obj();
		
		try{
			_p_conn->insert(doc, tmpobj);
		}catch ( mongo::OperationException& e )
                {
			if(11000==e.obj().getIntField("code")){
				return RET_DUPLICATE_ERR;
			}else{
				return RET_DEAL_ERR;
			}
                }

		if(""!=_p_conn->getLastError(m_datebase)){
			return RET_DEAL_ERR;
		}
		return RET_NO_DUPLICATE;
		/*
		rejs = _p_conn->findOne(doc, idxobj);
		if (!rejs.isEmpty()) {
			if (value2 == rejs.getStringField(field2)) {
				return RET_NO_DUPLICATE;
			}
			return RET_DUPLICATE_ERR;
		}
		else {
			return RET_DEAL_ERR;
		}
		*/
			
}

int C_Mongo::in_insert(vector<string> fields,vector<string> values)
{
		BSONObjBuilder query;
		BSONObj tmpobj,rejs;
		string errmsg;
		
		if(fields.size()!=values.size())
		{
				return RET_IN_PARAM_ERR;
		}	

		for(int i=0;i<fields.size();i++)	
		{
				query.append( (fields[i]) ,(values[i]));					
		}
		tmpobj=query.obj();
		
		_p_conn->insert(doc, tmpobj);
		//!!!!!!!!!!!!重单是否是完全相同，若完全相同此处会有问题!!!!!!!!!!!!!!!!!!!
		rejs=_p_conn->findOne(doc,tmpobj);
		if (rejs.isEmpty())
			return RET_DEAL_ERR;
		for (int i = 0; i != fields.size(); ++i) {
			if (values[i] != rejs.getStringField(fields[i])) {
				return RET_DUPLICATE_ERR;
			}
		}
		return RET_NO_DUPLICATE;
}


//!!!!!!!!!!!!!!!此处延续旧逻辑，会有插入中报错断开，但是之前插入信息已入库问题，此问题会导致错误再调用时重单，不建议业务逻辑中使用!!!!!!!!!!!!!!!!!!!!!!
int C_Mongo::in_batchinsert( vector< map<string,string> >in)
{
		string errmsg;
		BSONObj rejs;
		
			
		if(in.size()<=0)
		{
				return RET_IN_PARAM_ERR;
		}

		vector<BSONObj> vec_insert;
		vec_insert.clear();
		
		for(vector< map<string,string> >::iterator iter=in.begin();iter!=in.end();iter++)	
		{
				
				BSONObj  tempobj;
				BSONObjBuilder query;	
				for(map<string,string>::iterator map_iter=iter->begin();map_iter!=iter->end();map_iter++)	
				{	
						query.append( map_iter->first ,map_iter->second );	
				}			
				tempobj=query.obj();
				vec_insert.push_back(tempobj);
		}
		_p_conn->insert(doc,vec_insert);
		errmsg.clear();
		errmsg=_p_conn->getLastError(m_datebase);
		if(!errmsg.empty())
		{				
				return RET_DEAL_ERR;
		}	
		else
		{					
				return RET_OK;	
		}			
}


int C_Mongo::in_select(vector<string> in,string conditon_field,string conditon_value,vector< map<string,string> >& out)		
{
		BSONObjBuilder query;
		BSONObj rejs;
		string mkey;
		
		//检查输入参数
		if(conditon_field.empty() || conditon_value.empty() || in.size()<=0)
		{
				return RET_IN_PARAM_ERR;
		}
		
		//组合类似oralce中,select 字段1,字段2,....字段n		
		for(vector<string>::iterator iter=in.begin();iter!=in.end();++iter)
		{
				query.append(*iter,1);
		}
		
		map<string,string> temp;
		rejs=query.obj();

		auto_ptr<DBClientCursor> cursor = _p_conn->query(doc,MONGO_QUERY(conditon_field<<conditon_value),0,0,&rejs);
		
		if(_p_conn->isFailed()){
			return RET_DEAL_ERR;
		}

		while(cursor->more())
		{
			temp.clear();
			BSONObj ob=cursor->next();
			for(vector<string>::iterator iter=in.begin();iter!=in.end();++iter)
			{
					mkey.clear();
					mkey=*iter;
					temp[*iter]=ob.getStringField(mkey.c_str());	
			}	
			out.push_back(temp);				
		}
		
		return RET_OK;
}


int C_Mongo::in_update(string key ,string value,string conditon_field,string conditon_value)
{

		int ret;
		string errmsg;
		
		if( key.empty() || value.empty() || conditon_field.empty() || conditon_value.empty())
		{
				return RET_IN_PARAM_ERR;
		}

		_p_conn->update(doc,MONGO_QUERY(conditon_field<<conditon_value),BSON("$set"<< BSON( key << value )),true,true);
		errmsg=_p_conn->getLastError(m_datebase);
		if(!errmsg.empty())
		{
				return RET_DEAL_ERR;
		}
		return RET_OK;
}

int C_Mongo::in_update(map<string,string> key_value, map<string,string> conditon_value)
{
		BSONObjBuilder query;
		BSONObjBuilder condtions;
		string errmsg;
		
		//??????????????意图不明????????????????????
		if(key_value.size()<=0 || conditon_value.size()<=0)
		{
				return RET_IN_PARAM_ERR;
		}
		
		map<string,string>::iterator iter;
		for(iter=key_value.begin();iter!=key_value.end();++iter)
		{
			
				query.append(iter->first,iter->second);
		}
		
		for(iter=conditon_value.begin();iter!=conditon_value.end();++iter)
		{
				
				condtions.append(iter->first,iter->second);
		}		

		_p_conn->update(doc,Query(condtions.obj()),BSON("$set"<<query.obj()),true,true);
		errmsg=_p_conn->getLastError(m_datebase);
		if(!errmsg.empty())
		{
				return RET_DEAL_ERR;
		}
		return RET_OK;
}


//!!!!!!!!!!!!!!!!!!!!竟态关系,不安全!!!!!!!!!!!!!!!!!!!!!
int C_Mongo::in_remove(string filed,string value)
{
		BSONObj rejs;
		if(filed.empty() || value.empty())
		{
			return RET_IN_PARAM_ERR;
		}

		_p_conn->remove(doc,BSON(filed << value))	;	
		rejs=_p_conn->findOne(doc,BSON(filed << value));
				
		if(!rejs.isEmpty())
		{
				return RET_DEAL_ERR;
		}	
		else
		{ 
				return RET_OK;
		}	
}

int C_Mongo::in_remove(map<string,string> in)
{
		BSONObjBuilder query;
		string errmsg;
		
		if(in.size()==0)
		{
			return RET_IN_PARAM_ERR;			
		}
		//拼删除条件
		map<string,string>::iterator iter;
		for(iter=in.begin();iter!=in.end();++iter)
		{
			
				query.append(iter->first,iter->second);
		}		

		_p_conn->remove(doc,Query(query.obj() ))	;
		
		errmsg=_p_conn->getLastError(m_datebase);
		if(!errmsg.empty())
		{
				return RET_DEAL_ERR;
		}
		return RET_OK;
}

int C_Mongo::in_exist(string collection){
	std::list< std::string > l_cnames=_p_conn->getCollectionNames(m_datebase);
	for(std::list<std::string>::iterator iter=l_cnames.begin();iter!=l_cnames.end();iter++){
		if(collection==*iter){
			return RET_OK;
		}
	}
	return RET_DEAL_ERR;
}
	
bool C_Mongo::getipport(const char* hostname,vector<HostAndPort> &servers)
{
	
		char chip[50],port[50],chTemp[1024],chMinTemp[128];

		char *pchTempstring=NULL;
		char *pchTemp=NULL;	
		char *pchRecy = NULL;
		
		memset(chTemp,0,sizeof(chTemp));
		strcpy(chTemp,hostname);
		
		if( chTemp[0]=='\0')
		{
				return false;
		}

		pchTempstring=chTemp;
		while( (pchTemp=strchr(chTemp,';'))!=NULL)
		{
				memset(chMinTemp,0,sizeof(chMinTemp));
				strncpy(chMinTemp,chTemp,pchTemp-pchTempstring);							
				pchRecy=strchr(chMinTemp, ':');
				if (pchRecy == NULL)
				{
						return false;
				}	
				memset(chip,0,sizeof(chip));
				memset(port,0,sizeof(port));
				strncpy(chip, chMinTemp, pchRecy-chMinTemp);
				strcpy(port, pchRecy+1);
			
				HostAndPort hp(string(chip),atoi(port));
				servers.push_back(hp);
				
				strcpy(chTemp,pchTemp+1);
				
		}
		if(servers.size()==0)
		{
				return false;
		}
		return true;
}



