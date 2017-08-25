#include "filter.h"
FilterTask::~FilterTask(){
	if(m_ofWrongData.is_open()){
		m_ofWrongData.close();
	}
	if(m_ofErrData.is_open()){
		m_ofErrData.close();
	}
	if(m_ofData.is_open()){
		m_ofData.close();
	}
}
/*=============================================基础处理函数=============================================*/
/*GetRedisConfigInfo为参考旧代码抄袭而来，为了减少库链接自行实现IsDirectory，CheckPath函数，basefun库中另有完善实现*/
/*判断指定路径是否为目录*/
bool FilterTask::IsDirectory(char* pchPath){
	if(!strlen(pchPath)){
		return false;
	}
	struct stat s;  
    lstat( pchPath , &s );  
    if(S_ISDIR( s.st_mode )){
		return true;
	}else{  
        cout<<pchPath<<" is not a valid directory !"<<endl;  
        return false;  
    } 
	
}
/*判断指定路径是否为目录并在末尾添加/以便拼接文件全路径*/
bool FilterTask::CheckPath(char *pchPath){
	if(!IsDirectory(pchPath))
	{
		//Log(0, "[NewFilter::CheckPath] Got failed IsDirectory, the directory %s is not exist!.", pchPath);
		return false;
	}
	//FullPath(pchPath);
	strcat(pchPath,"/");
	return true;
}
/*从配置读取redis服务端信息*/
bool FilterTask::GetRedisConfigInfo(ConnParam *pstParam){
	char chCommonConfigFile[STRING_LEN];
	char chSectionName[STRING_LEN];
	char chTemp[STRING_LEN];
	char *pchAppPath = 0;
	

	if((pchAppPath = getenv("SETTLE_HOME")) == 0)
	{
		//Log(0, "[FilterTask::GetRedisConfigInfo] Got failed getenv, not get the environment.");
		return false;
	}
	if(!CheckPath(pchAppPath))
	{
		return false;
	}

	strcpy(chCommonConfigFile, pchAppPath);
	strcat(chCommonConfigFile, "/config/");
	strcat(chCommonConfigFile, "base.ini");

	C_Config Config;
	
	//need to change
	if (!Config.GetParam(chCommonConfigFile, "RedisFilter", "num", chTemp))
	{
		return false;
	}
	pstParam->iNum = atoi(chTemp);
	if (pstParam->iNum <= 0)
	{
		return false;
	}
	
	for(int i=1; i<=pstParam->iNum; i++)
	{
		sprintf(chSectionName,"ip%d\0", i);
		if (!Config.GetParam(chCommonConfigFile, "RedisFilter", chSectionName, chTemp))
		{
			return false;
		}
		strcpy(pstParam->sConInfo[i-1].chHostIp, chTemp);

		sprintf(chSectionName,"port%d\0", i);
		if (!Config.GetParam(chCommonConfigFile, "RedisFilter", chSectionName, chTemp))
		{
			return false;
		}
		pstParam->sConInfo[i-1].iport = atoi(chTemp);
	}

	return true;
}


/* Show designation files under dir_name ! */  
void FilterTask::showDesignationFiles( const char * dir_name ,vector<string> &svecFiles){  
     
    if( NULL == dir_name )  
    {  
        cout<<" dir_name is null ! "<<endl;  
        return;  
    }  
  
    
    struct stat s;  
    lstat( dir_name , &s );  
    if( ! S_ISDIR( s.st_mode ) )  
    {  
        cout<<"dir_name is not a valid directory !"<<endl;  
        return;  
    }  
      
    struct dirent * filename;    
    DIR * dir;
    dir = opendir( dir_name );  
    if( NULL == dir )  
    {  
        cout<<"Can not open dir "<<dir_name<<endl;  
        return;  
    }  
    cout<<"Successfully opened the dir !"<<endl;  
	
	char pwd[256]={0};
	getcwd(pwd,sizeof(pwd));
	
	chdir(dir_name);
    /* read all the files in the dir ~ */  
    while( ( filename = readdir(dir) ) != NULL )  
    {  
        //当前目录，上层目录，隐藏文件，以t开头的错单文件名不输出。
		if(strcmp( filename->d_name , "." ) == 0)
			continue;
		if(strcmp( filename->d_name , "..") == 0)
			continue;
		if((filename->d_name)[0]=='t')
			continue;
		lstat( filename->d_name , &s ); 
		if(S_ISDIR( s.st_mode )){
			continue;
		}
		svecFiles.push_back(filename->d_name);
        //cout<<filename ->d_name <<endl;  
    }
	chdir(pwd);
	closedir(dir);
}

/*将字符串数据构造成映射*/
bool FilterTask::construction(string& strData,vector<string>& svecFiledName,map<string,string> &mapData){
	vector<string> tsvec;
	boost::split(tsvec,strData,boost::is_any_of(","));
	
	vector<string>::iterator iter1=svecFiledName.begin();
	vector<string>::iterator iter2=tsvec.begin();
	for(;iter1!=svecFiledName.end();iter1++,iter2++){
		
		if(iter2==tsvec.end()){
			//字段缺失
			return false;
		}
		if(""==*iter2){
			//空值跳过
			continue;
		}
		mapData[*iter1]=*iter2;
	}
	return true;
}

//still need to change
void FilterTask::RedisErrorHandler(int iret,string smsg){
	int iTryNum=10;
	switch(iret){
		//宏定义值详见：C_RedisOption.h
		//重连
		case ERR_REPLAY:
		case ERR_READONLY:
				
				while(iTryNum--){
					try{
						iret=m_RedisFilter.Connect(m_stParam);
						if(!iret){
							break;
						}
					}catch(...){
						//redis exceptions
					}
				}
				if(iret){
					//重连失败
				}
				break;
		case ERR_CONNECTION_REFUSED:;break;
		case ERR_KEY_NOEXISTS:;break;
		case ERR_NODE_NOEXISTS:;break;
		case ERR_WRONGTYPE:;break;
		defalut:;;
	}
}



/*=============================================佣金框架套用=============================================*/
bool FilterTask::prepare(){
	BPO::options_description desc("Allowed options");

	desc.add_options()
					("help,h","show help")
					("usage","Usage:\n	filter -f 1 -d 工作目录\n	filter -f 0 -d 工作目录 -F 工作文件(相对工作目录）")
					(",f",BPO::value<int>(&m_iFlowFlag)->default_value(1),"工作流向,0:rollBack 1:filter。")
					("File,F",BPO::value<string>(&m_sFile),"工作文件")
					("Dir,d",BPO::value<string>(&m_sDir),"工作目录");
	BPO::variables_map vm;
	try{
			BPO::store(BPO::parse_command_line(g_argc,g_argv,desc), vm);
	}catch(exception& e){
			cout<<e.what()<<endl;
			cout<<desc<<endl;
			return 0;
	}
	
	BPO::notify(vm);

	if((m_iFlowFlag&&vm.count("Dir"))||(!m_iFlowFlag&&vm.count("File"))){
		//do it
	}else{
		cout<<desc<<endl;
		exit(0);
	}
	
	//连接redis
	try{
		if(!GetRedisConfigInfo(&m_stParam)){
			exit(-1);
		}
		iret=m_RedisFilter.Connect(m_stParam);
		if(!iret){
			//redis错误
			exit(-1);
		}
	}catch(...){
		//redis exceptions
		exit(0);
	}
	
	
	m_ofWrongData.open("wrongdata",ios::out|ios::app);
	m_ofErrData.open("errordata",ios::out|ios::app);
}
int FilterTask::run(){
	if(m_iFlowFlag){
		processDir(m_sDir);
	}else{
		processFile(m_sDir,m_sFile,0);
	}
}
void FilterTask::printManual(){
	BPO::options_description desc("Allowed options");
	desc.add_options()
					("help,h","show help")
					("usage","Usage:\n	filter -f 1 -d 工作目录\n	filter -f 0 -F 工作文件")
					(",f",BPO::value<int>(),"工作流向,0:rollBack 1:filter。")
					(",F",BPO::value<string>(),"工作文件")
					(",d",BPO::value<string>(),"工作目录");
	cout<<desc<<endl;
}


/*=============================================业务流程处理=============================================*/

bool FilterTask::processDir(string sDir){
	vector<string> svecFiles;
	//指定目录待定
	showDesignationFiles(sDir.c_str(),svecFiles);
	for(vector<string>::iterator iter=svecFiles.begin();iter!=svecFiles.end();iter++){
		//此处可以根据需求进行多线程处理
		processFile(sDir,*iter);
	}
}

//iFlowFlag 0:rollBack 1:update
bool FilterTask::processFile(string sdir,string sfilename,int iFlowFlag){
	string sin=sdir+sfilename;
	ifstream in(sin.c_str(),ios::in);
	if (! in.is_open()){
		return false;
	}
	if(m_ofData.is_open()){
		m_ofData.close();
	}
	//!1!!
	string soutfile=sdir+"/outdata/~"+sfilename;
	m_ofData.open(soutfile.c_str(),ios::out|ios::app);
	if(!m_ofData.is_open()){
		return false;
	}
	
	//处理数据
	string strData;
	//第一行为字段名
	if(getline(in,strData)){
		boost::split(m_svecFiledName,strData,boost::is_any_of(","));
	}else{
		//error
		return false;
	}
	map<string,string> mapData;
	while(getline(in,strData)){
		if(construction(strData,m_svecFiledName,mapData)){
			//文件名即为批次号
			int ret=processOneData(mapData,sfilename,iFlowFlag);
			if(ret){
				m_ofErrData<<sfilename<<":"<<ret<<":"<<strData<<endl;
				m_ofErrData.flush();
				if(WRONGVALUE!=ret){
					processOneData(mapData,sfilename,iFlowFlag);
				}
			}
		}else{
			m_ofWrongData<<sfilename<<":"<<strData<<endl;
			m_ofWrongData.flush();
		}
	}
	if(!in.eof()){
		//error
	}
	in.close();
}

//0:成功;-1:字段取值错误；-2：lock错误；-3：redis错误
int FilterTask::processOneData(map<string,string> &mapData,string sBatchNum,int iFlowFlag){

	
	//不在策略有效期内
	int iStartDate=atoi(mapData["START_DATE"].c_str());
	int iBillingCycleID=atoi(mapData["BILLING_CYCLE_ID"].c_str());
	int iEffDate=atoi(mapData["EFF_DATE"].c_str());
	int iExpDate=atoi(mapData["EXP_DATE"].c_str());
	if(iStartDate<iEffDate||iStartDate>iExpDate){
		return SUCCESS;
	}
	
	

	
	
	//根据策略范围提取排重周期年月日
	int BillingCycle=0;
	//新增字段，处理字段需要待定
	//int icycleType=atoi(mapData["CYCLE_TYPE"].c_str());
	int icycleType=atoi(mapData["FILTER_CYCLE_TYPE"].c_str());
	if(1==icycleType){
		BillingCycle=iBillingCycleID/100;
	}else if(2==icycleType){
		BillingCycle=iBillingCycleID;
	}else if(3==icycleType){
		BillingCycle=iStartDate;
	}else{
		//error
		return WRONGVALUE;
	}
	
	//将账期范围，FILTER_FIELD，FILTER_FIELD值拼装成排重索引
	char chkey[1024]={0};
	long lFilterField=atol(mapData["FILTER_FIELD"].c_str());
	if(1==lFilterField){
		sprintf(chkey,"%d|%ld:%s", BillingCycle, lFilterField, mapData["SERV_ID"].c_str());
	}else if(2==lFilterField){
		sprintf(chkey,"%d|%ld:%s", BillingCycle, lFilterField, mapData["OBJ_ID"].c_str());
	}else if(3==lFilterField){
		//新增字段，字段名待定
		sprintf(chkey,"%d|%ld:%s", BillingCycle, lFilterField, mapData["INDEX_KEY"].c_str());
	}else{
		//error
		return WRONGVALUE;
	}
	
	
	
	
	
	int tryNum=30;
	//抢占状态0：失败，1：成功
	int iLockFlag=0;
	bool ret;
	while(tryNum--){	

		//锁
		string sLock="lock";
		sLock+=chkey;
		char BatchFlag[256]={0};
		
		//多机器锁操作
		//锁过期时间：30s
		try{
			m_RedisFilter.Set(sLock.c_str(),sBatchNum.c_str(),"nx",30,iLockFlag);
		}catch(...){
			cout<<"redis exception!"<<endl;
		}
		//iLockFlag=1:抢占成功
		if(1==iLockFlag){
			//更新数据
				ret=iFlowFlag?update(chkey,mapData,sBatchNum):rollBack(chkey,mapData,sBatchNum);
			//主动释放锁
			try{
				iret=m_RedisFilter.delkey(sLock.c_str());
			}catch(...){
			cout<<"redis exception!"<<endl;
			}
			if(iret){
				//redis error
			}
			//跳出循环
			if(ret){
				return SUCCESS;
			}else{
				return PROCESSERROR;
			}
		}else{
			usleep(200);
		}
	}
	//尝试次数耗尽
	return LOCKERROR;
	
}


bool FilterTask::update(const char* chkey,map<string,string> &mapData,string sBatchNum,long lUpdateValue){
	//记录当前有效数据
	string sKeyMap="Hash";
	sKeyMap+=chkey;
	//Insert调用里不会检查value值是否为空，可能导致拼接出结果不合法，暂时在构建map处做判断排除value:""情况
	try{
		iret=m_RedisFilter.Insert(sKeyMap.c_str(),mapData);
	}catch(...){
			cout<<"redis exception!"<<endl;
		}
	if(iret){
		//redis错误
	}
	//记录当前有效值
	string sKeyValue="Value";
	sKeyValue+=chkey;
	char cUpdateValue[32]={0};
	sprintf(cUpdateValue,"%ld",lUpdateValue);
	try{
		iret=m_RedisFilter.Insert(sKeyValue.c_str(),cUpdateValue);
	}catch(...){
			cout<<"redis exception!"<<endl;
	}
	if(iret){
		//redis错误
	}
	//记录当前有效值所属批次
	string sKeyBatch="Batch";
	sKeyBatch+=chkey;
	iret=m_RedisFilter.Insert(sKeyBatch.c_str(),sBatchNum.c_str());
	if (iret){
		//redis错误
	}
	
	//输出加正数据
	//!!!!!!!!!!!!!!!!!!
	string stempdata="";
	for(vector<string>::iterator iter=m_svecFiledName.begin();iter!=m_svecFiledName.end();iter++){
			stempdata+=mapData[*iter];
			stempdata+=",";
	}
	string soutdata(stempdata,0,stempdata.size()-1);
	m_ofData<<soutdata<<endl;
}

bool FilterTask::update(const char* chkey,map<string,string> &mapData,string sBatchNum){
	//查询同索引目前有效值
	char cValidValue[256]={0};
	string sKeyValue="Value";
	sKeyValue+=chkey;
	try{
		iret=m_RedisFilter.GetString(sKeyValue.c_str(),cValidValue);
	}catch(...){
			cout<<"redis exception!"<<endl;
	}
	if (iret){
		//redis error
	}
	
	long lFilterMeasure=atol(mapData["FILTER_MEASURE"].c_str());
	long lUpdateValue;
	if(1==lFilterMeasure||2==lFilterMeasure){
		lUpdateValue=atol(mapData["CHARGE"].c_str());
	}else if(3==lFilterMeasure){
		//新增字段，字段名待定
		lUpdateValue=atoi(mapData["STRATEGY_LEVEL"].c_str());
	}else{
		//error
		return false;
	}
	
	
	if(!cValidValue[0]){
		//没有有效值直接更新数据
		update(chkey,mapData,sBatchNum,lUpdateValue);
		
	}else{
		long lValidValue=atoi(cValidValue);
		//根据FILTER_MEASURE与有效值比较判断是否要更新有效值
		if(1==lFilterMeasure&&lUpdateValue>lValidValue||
			2==lFilterMeasure&&lUpdateValue<lValidValue||
			3==lFilterMeasure&&lUpdateValue>lValidValue){
			//输出redis中有效值的减正数据
			//!!!!!!!!!!!!!!!!
			string sKeyMap="Hash";
			sKeyMap+=chkey;
			map<string,string> mapRet;
			try{
				iret=m_RedisFilter.HmgetAllField(sKeyMap.c_str(),mapRet);
			}catch(...){
				cout<<"redis exception!"<<endl;
			}
			if(iret){
			}
			mapRet["CHARGE"]="-"+mapRet["CHARGE"];
			string stempdata="";
			for(vector<string>::iterator iter=m_svecFiledName.begin();iter!=m_svecFiledName.end();iter++){
					stempdata+=mapRet[*iter];
					stempdata+=",";
			}
			string soutdata(stempdata,0,stempdata.size()-1);
			m_ofData<<soutdata<<endl;
			
			try{
				iret=m_RedisFilter.delkey(sKeyMap.c_str());
			}catch(...){
				cout<<"redis exception!"<<endl;
			}

			
			
			//更新新有效数据
			update(chkey,mapData,sBatchNum,lUpdateValue);	
		}else{
			//无需更新
		}
	}
	
}


bool FilterTask::rollBack(const char* chkey,map<string,string> &mapData,string sBatchNum){
	string sKeyBatch="Batch";
	sKeyBatch+=chkey;
	char cReturn[128]={0};
	try{
		iret=m_RedisFilter.GetString(sKeyBatch.c_str(),cReturn);
	}catch(...){
				cout<<"redis exception!"<<endl;
	}
	if(!strcmp(cReturn,sBatchNum.c_str())){
		string sKeyValue="Value";
		sKeyValue+=chkey;
		try{
			iret=m_RedisFilter.delkey(sKeyValue.c_str());
		}catch(...){
				cout<<"redis exception!"<<endl;
		}
		if(iret){
		}
		iret=m_RedisFilter.delkey(sKeyBatch.c_str());
		if(iret){
		}
		string sKeyMap="Hash";
		sKeyMap+=chkey;
		//输出redis中有效值的减正数据
		map<string,string> mapRet;
		try{
			iret=m_RedisFilter.HmgetAllField(sKeyMap.c_str(),mapRet);
		}catch(...){
				cout<<"redis exception!"<<endl;
		}
		if(iret){
		}
		//!!!!!!!!!!!!!!!!!
		mapRet["CHARGE"]="-"+mapRet["CHARGE"];
		string stempdata="";
		for(vector<string>::iterator iter=m_svecFiledName.begin();iter!=m_svecFiledName.end();iter++){
			stempdata+=mapRet[*iter];
			stempdata+=",";
		}
		string soutdata(stempdata,0,stempdata.size()-1);
		m_ofData<<soutdata<<endl;
		
		try{
			iret=m_RedisFilter.delkey(sKeyMap.c_str());
		}catch(...){
				cout<<"redis exception!"<<endl;
		}
		if(iret){
		}
	}
}

int FilterTask::test(){
	m_ofData.open("1.1.1.1",ios::out|ios::app);	
	
	map<string,string> mapData;

	mapData["BILLING_CYCLE_ID"]="20170822";
	mapData["EFF_DATE"]="20170801";
	mapData["EXP_DATE"]="20170831";
	mapData["CYCLE_TYPE"]="1";
	mapData["STRATEGY_ID"]=//1;
	mapData["SERVICE_TYPE_ID"]="111";
	mapData["OBJ_ID"]="222";
	mapData["INDEX_KEY"]="333";
	mapData["CHARGE"]="20";
	mapData["STRATEGY_LEVEL"]="3";
	processOneData(mapData,"1.1.1.1",1);
	mapData["CHARGE"]="10";
	processOneData(mapData,"1.1.1.1",1);
	mapData["CHARGE"]="30";
	processOneData(mapData,"1.1.1.1",1);
}

int main(int argc,char** argv){
	FilterTask task(argc,argv);
	task.prepare();
	task.run();
}
