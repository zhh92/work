#include "filter.h"
FilterTask::~FilterTask(){
	if(m_ofData.is_open()){
		m_ofData.close();
	}
	if(m_ofExclude.is_open()){
		m_ofExclude.close();
	}
	LOG4CXX_INFO(m_logger,"Process Stop!");
}
/*=============================================base=============================================*/
bool FilterTask::IsDirectory(char* pchPath){
	if(!strlen(pchPath)){
		return false;
	}
	struct stat s;  
    lstat( pchPath , &s );  
    if(S_ISDIR( s.st_mode )){
		return true;
	}else{

		sprintf(m_clog,"%s is not a valid directory !",pchPath);
		LOG4CXX_WARN(m_logger,m_clog);
        return false;  
    } 
	
}

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

bool FilterTask::GetRedisConfigInfo(const char* cFieldName,ConnParam *pstParam){
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
	if(!strcmp("RedisProxy",cFieldName)){
		char chConnectNum[60];
		memset(chConnectNum, 0, sizeof(chConnectNum));
		if(!Config.GetParam(chCommonConfigFile,(char *)"RedisProxy", (char *)"ProxyNum", chConnectNum))
		{
			sprintf(m_clog, "read %s %s %s error!", chCommonConfigFile, (char *)"RedisProxy", (char *)"ProxyNum");
			LOG4CXX_ERROR(m_logger,m_clog);
			return false;
		}
		int iProxyNum = atoi(chConnectNum);
		if (iProxyNum <= 0 || iProxyNum > 100)
		{
			sprintf(m_clog, "The value of %s %s %s invalid!", chCommonConfigFile, (char *)"RedisProxy", (char *)"ProxyNum");
			LOG4CXX_ERROR(m_logger,m_clog);
			return false;
		}
		pstParam->iProxyNum = iProxyNum;
		pstParam->iFlag=0;
	}
	
	//need to change
	if (!Config.GetParam(chCommonConfigFile, cFieldName, "num", chTemp))
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
		sprintf(chSectionName,"ip%d", i);
		if (!Config.GetParam(chCommonConfigFile, cFieldName, chSectionName, chTemp))
		{
			return false;
		}
		strcpy(pstParam->sConInfo[i-1].chHostIp, chTemp);

		sprintf(chSectionName,"port%d", i);
		if (!Config.GetParam(chCommonConfigFile, cFieldName, chSectionName, chTemp))
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
		LOG4CXX_ERROR(m_logger," dir_name is null ! ");
        return;  
    }
  
    
    struct stat s;  
    lstat( dir_name , &s );  
    if( ! S_ISDIR( s.st_mode ) )  
    {
		sprintf(m_clog,"%s is not a valid directory !",dir_name);
		LOG4CXX_ERROR(m_logger,m_clog);  
        return;  
    }  
      
    struct dirent * filename;    
    DIR * dir;
    dir = opendir( dir_name );  
    if( NULL == dir )  
    {  
		sprintf(m_clog,"Can not open dir %s",dir_name);
        LOG4CXX_ERROR(m_logger,m_clog);
        return;  
    }  
    sprintf(m_clog,"Successfully opened dir %s!",dir_name);
	LOG4CXX_DEBUG(m_logger,m_clog);
	
	char pwd[256]={0};
	getcwd(pwd,sizeof(pwd));
	
	chdir(dir_name);
    /* read all the files in the dir ~ */  
    while( ( filename = readdir(dir) ) != NULL )  
    {  

		if(strcmp( filename->d_name , "." ) == 0)
			continue;
		if(strcmp( filename->d_name , "..") == 0)
			continue;
		if((filename->d_name)[0]=='~')
			continue;
		if(string(filename->d_name).find("T100")==std::string::npos)
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
void FilterTask::showDesignationDirs( const char * dir_name ,vector<string> &svecDirs){  
    if( NULL == dir_name )  
    {  
        LOG4CXX_ERROR(m_logger," dir_name is null ! ");
        return;  
    }  
  
    
    struct stat s;  
    lstat( dir_name , &s );  
    if( ! S_ISDIR( s.st_mode ) )  
    {  
        LOG4CXX_ERROR(m_logger,"dir_name is not a valid directory !");
        return;  
    }  
      
    struct dirent * filename;    
    DIR * dir;
    dir = opendir( dir_name );  
    if( NULL == dir )  
    {  
		sprintf(m_clog,"Can not open dir %s!",dir_name);
        LOG4CXX_ERROR(m_logger,m_clog); 
        return;  
    }  
	sprintf(m_clog,"Successfully opened the dir %s!",dir_name);
    LOG4CXX_DEBUG(m_logger,m_clog);
	
	char pwd[256]={0};
	getcwd(pwd,sizeof(pwd));
	
	chdir(dir_name);
    /* read all the files in the dir ~ */  
    while( ( filename = readdir(dir) ) != NULL )  
    {  
		if(strcmp( filename->d_name , "." ) == 0)
			continue;
		if(strcmp( filename->d_name , "..") == 0)
			continue;
		if((filename->d_name)[0]=='T'&&strcmp(string(filename->d_name,0,4).c_str(),"T100"))
			continue;
		if((filename->d_name)[0]=='~')
			continue;
		lstat( filename->d_name , &s ); 
		if(S_ISDIR( s.st_mode )){
			svecDirs.push_back(filename->d_name);
        }
    }
	chdir(pwd);
	closedir(dir);
}


bool FilterTask::construction(string& strData,vector<string>& svecFiledName,map<string,string> &mapData){
	vector<string> tsvec;
	boost::split(tsvec,strData,boost::is_any_of("^"));
	
	vector<string>::iterator iter1=svecFiledName.begin();
	vector<string>::iterator iter2=tsvec.begin();
	for(;iter1!=svecFiledName.end();iter1++,iter2++){
		
		if(iter2==tsvec.end()){
			return false;
		}
		if(""==*iter2){
			continue;
		}
		mapData[*iter1]=*iter2;
	}
	return true;
}

string FilterTask::constructionStr(map<string,string>& mapData,string sSeparator){
	string stempdata;
	for(vector<string>::iterator iter=m_svecFiledName.begin();iter!=m_svecFiledName.end();iter++){
			stempdata+=mapData[*iter];
			stempdata+=sSeparator;
	}
	string soutdata(stempdata,0,stempdata.size()-1);
	return soutdata;
}
//still need to change
bool FilterTask::RedisErrorHandler(const int iret,string smsg){
	int iTryNum=11;
	
	switch(iret){
		//defines in C_RedisOption.h
		//reconnect
		case ERR_REPLAY:
		case ERR_READONLY:	
				while(--iTryNum){
					try{
						if(m_RedisFilter.Connect(m_stParam)){
							break;
						}
					}catch(...){
						//redis exceptions
					}
					sleep(2);
				}
				if(!m_RedisFilter.GetRedisContext()){
					LOG4CXX_ERROR(m_logger,"Redis reconnect error!");
					return false;
				}
				break;
				
		case ERR_CONNECTION_REFUSED:break;
		case ERR_KEY_NOEXISTS:break;
		case ERR_NODE_NOEXISTS:break;
		case ERR_WRONGTYPE:break;
		defalut:
			LOG4CXX_ERROR(m_logger,"Unkow redis return number!");
	}
	sprintf(m_clog,"RedisErrorHandler:%d:%s success!",iret,smsg.c_str());
	LOG4CXX_INFO(m_logger,m_clog);
	return true;
}

template<typename T>
bool FilterTask::RedisOp(T t,string sMsg){
	int itryN=4;
	while(!m_RedisFilter.GetRedisContext()&&--itryN){
		m_RedisFilter.Connect(m_stParam);
		sleep(1);
	}
	if(!m_RedisFilter.GetRedisContext()){
		sprintf(m_clog,"RedisOp:GetRedisContext error!msg:%s",sMsg.c_str());
		LOG4CXX_ERROR(m_logger,m_clog);
		return false;
	}
	int itrynum=4;
	while(--itrynum){
		try{	
			iret=t();
		}catch(...){
				sprintf(m_clog,"redis exception!");
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(ERR_KEY_NOEXISTS==iret){
				return true;
			}
			if(RedisErrorHandler(iret,sMsg)){
				itrynum--;
			}else{
				//redis error
				return false;
				
			}
		}else{
			break;
		}
	}
	if(iret){
		sprintf(m_clog,"%s error!",sMsg.c_str());
		LOG4CXX_ERROR(m_logger,m_clog);
		return false;
	}
	return true;
}

bool FilterTask::outputFile(map<string,string>& mapData,ofstream& outfile){

	outfile<<constructionStr(mapData,"^")<<endl;
	return true;
}

bool FilterTask::outputFinalFile(string sPath,string sFileName,int iCount){
	ifstream iftemp((sPath+"/~"+sFileName).c_str(),ios::in);
	#ifdef YJJS
	ofstream oftemp((sPath+"/"+sFileName).c_str(),ios::out);
	#else
	ofstream oftemp((sPath+"/~~"+sFileName).c_str(),ios::out);
	#endif
	char sHeadInfo[200];
	sprintf(sHeadInfo,"Total:%-12d Insert:%-12d Update:%-12d Delete:%-12d ", iCount, iCount, 0, 0);
	oftemp<<sHeadInfo<<endl;
	string scmd;
	string strData;
	while(getline(iftemp,strData)){
		oftemp<<strData<<endl;
	}
	iftemp.close();
	oftemp.close();
	scmd="rm -f "+sPath+"/~"+sFileName;
	system(scmd.c_str());
	return true;
}

bool FilterTask::redisConnectDb(const int dbID){
	return m_RedisFilter.Connect(m_stParam)&&!m_RedisFilter.SelectDb(dbID);
	
}

/*=============================================frame=============================================*/
bool FilterTask::prepare(){
	PropertyConfigurator::configureAndWatch("log4cxx.properties");
	m_logger=log4cxx::Logger::getLogger("MultiFilter");
	ostringstream oss; 	
	oss<<"PID:"<<getpid();  
	string pid= oss.str();
	MDC::put("PID",pid.c_str());
	LOG4CXX_INFO(m_logger,"Process Start!");
	BPO::options_description desc("Allowed options");

	#ifdef CLIENTINTERF	
	/*	
	desc.add_options()
					("help,h","show help")
					("usage","Usage:\n	filter -k shmKey -r randomNumber!")
					("Key,k",BPO::value<int>(&m_iKey),"shmKey")
					("Rnd,r",BPO::value<int>(&m_iRnd),"scheduler randomNumber!");
	*/
	if(g_argc<3){
		LOG4CXX_ERROR(m_logger,"Wrong Param!Usage:process_name  key rand");
		exit(-1);
	}else{
		m_iKey=atoi(g_argv[1]);
		m_iRnd=atoi(g_argv[2]);
	}
	#else
	desc.add_options()
					("help,h","show help")
					("usage","Usage:\n	filter -f 1 -d workpath\n	filter -f 0 -d workpath -F workfile")
					(",f",BPO::value<int>(&m_iFlowFlag)->default_value(1),"workFlow,0:rollBack 1:filter。")
					("File,F",BPO::value<string>(&m_sFile),"")
					("Dir,d",BPO::value<string>(&m_sDir)->default_value("/data/pas/outfile/"),"workpath");	
	
	BPO::variables_map vm;
	try{
			BPO::store(BPO::parse_command_line(g_argc,g_argv,desc), vm);
	}catch(exception& e){
			LOG4CXX_INFO(m_logger,e.what());
			cout<<desc<<endl;
			return 0;
	}
	
	BPO::notify(vm);
	if(vm.count("help")||vm.count("usage"))
	{
		cout<<desc<<endl;
		exit(0);
	}
	#endif
	
	#ifdef CLIENTINTERF
	/*
	if(!(vm.count("Key")&&vm.count("Rnd"))){
		cout<<desc<<endl;
		exit(0);	
	}
	*/
	#else
	if((m_iFlowFlag&&vm.count("Dir"))||(!m_iFlowFlag&&vm.count("File"))){
		//do it
	}else{
		cout<<desc<<endl;
		exit(0);
	}
	#endif
	
	try{
		if(!GetRedisConfigInfo("RedisMultiFilter",&m_stParam)){
			exit(-1);
		}
		if(!m_RedisFilter.Connect(m_stParam)){
			exit(-1);
		}
	}catch(...){
		//redis exceptions
		exit(0);
	}
	
	#ifdef CLIENTINTERF
	ConnParam tConnParam;
	C_GetParamFromRedis GetParamFromRedis;
	try{
		
		if(!GetRedisConfigInfo("RedisProxy",&tConnParam)){
			LOG4CXX_ERROR(m_logger,"GetRedisConfigInfo RedisProxy error!");
			exit(-1);
		}
		if (GetParamFromRedis.Init(tConnParam) == -1){
			LOG4CXX_ERROR(m_logger,"GetParamFromRedis Init error!");
			exit(-1);
		}
	}catch(...){
	}
		
	char tcPath[256];
	memset(tcPath,0,sizeof(tcPath));
  	if (!GetParamFromRedis.GetEtcConfig("MultiFilter.ini", "Common", "path", tcPath)){
		LOG4CXX_ERROR(m_logger,"GetParamFromRedis GetEtcConfig error!");
		m_sPath="/data/pas/outfile/";
		LOG4CXX_INFO(m_logger,"Set DataPath:/data/pas/outfile/ defalut!");
  	}	
	if(tcPath[0]){
		m_sPath=tcPath;
		sprintf(m_clog,"Get DataPath:%s",tcPath);
		LOG4CXX_INFO(m_logger,m_clog);
	}else{
		m_sPath="/data/pas/outfile/";
		LOG4CXX_INFO(m_logger,"Set DataPath:/data/pas/outfile/ defalut!");
	}
	char tcFields[2048]={0};
	if(!GetParamFromRedis.GetEtcConfig("MultiFilter.ini", "Common", "Field", tcFields)){
		LOG4CXX_ERROR(m_logger,"GetParamFromRedis GetEtcConfig error!");
  	}
	if(tcFields[0]){
		boost::split(m_svecRedisFileds,tcFields,boost::is_any_of(","));
	}
	#endif
	
	#ifdef YJJS
	char sBATCH_MEM_NUM[20]={0};
	int m_iMemNum=ParamDefineMgr::getParam("BATCH_MEM", "BATCH_MEM_NUM",sBATCH_MEM_NUM, sizeof(sBATCH_MEM_NUM))?atoi(sBATCH_MEM_NUM):3;
	char szBuf[128] = {0};
	G_CREATE_BATCHMEM(m_iMemNum,szBuf);
	#endif
	
	return true;
}
#ifdef CLIENTINTERF
int FilterTask::run(){
	
	if (!ClientInf.ShmatIPC(m_iKey))
	{
		LOG4CXX_ERROR(m_logger,"ShmatIPC failed!");
		return -1;	
	}
	LOG4CXX_INFO(m_logger,"ShmatIPC success!");
	ClientInf.ReportProcStatus("REG", " ", 0);
	vector<string> tsvec;
	boost::split(tsvec,g_argv[0],boost::is_any_of("/"));

	if (!ClientInf.RegNewProc((tsvec.rbegin())->c_str(), getpid(), m_iRnd))
	{
		LOG4CXX_ERROR(m_logger,"RegNewProc failed!");
		return -1;
	}
	LOG4CXX_INFO(m_logger,"RegNewProc success!");
	ClientInf.ReportProcStatus("LOAD","加载配置", 0);

	while(true){

		ClientInf.ReportProcStatus("IDLE", "申请指令", 0);
		LOG4CXX_DEBUG(m_logger,"apply instructions!");
		int iRetValue, iRet, iDirection;
		char chBatchCode[39+1]={0}, chInput[74+1]={0};
		int iBeginFlag = 0;		
		int iEndFlag = 0;
		iRet = ClientInf.GetIPCCommand(iRetValue, iDirection, chBatchCode, chInput, iBeginFlag, iEndFlag);
		
		if (iRet == GETCOMMAND_NOCOMMAND){
			LOG4CXX_INFO(m_logger,"GETCOMMAND_NOCOMMAND!");
			continue;
		}else if (iRet == PROC_CHECK_FAILURE){
			ClientInf.ReportProcStatus("ERROR", "校验失败", 0);
			LOG4CXX_ERROR(m_logger,"PROC_CHECK_FAILURE!");
			return -1;
		}
		
		if (iRetValue == IPC_INFO_PROCQUIT){
			LOG4CXX_INFO(m_logger,"IPC_INFO_PROCQUIT!");
			return 0;
		}
		
		if(chBatchCode[0] == '\0'){
			LOG4CXX_ERROR(m_logger,"BATCH_STAT_FAILURE:EMPTY!");
			iRet = ClientInf.NoCutJobCommit(BATCH_STAT_FAILURE);
			ClientInf.ReportProcStatus("COMMIT", "批次号为空", 0);
			if(iRet != COMMIT_SUCESS)
			{
					LOG4CXX_ERROR(m_logger,"ReportProcStatus Error!");
					return -1;
			}
			continue;
		}		
			
		ClientInf.ReportProcStatus("RUN", chBatchCode, 0);
		sprintf(m_clog,"RUN:%s,DIRECTION:%d",chBatchCode,iDirection);
		LOG4CXX_INFO(m_logger,m_clog);
		if(BATCH_DIRECTION_FORWARD==iDirection){
			if(processFile(m_sPath,chBatchCode)){
				NoCutJobCommit(BATCH_STAT_SUCCESS);		
			}else{
				NoCutJobCommit(BATCH_STAT_FAILURE);
			}
		}else if(BATCH_DIRECTION_BACKWARD==iDirection){	
			if(processFile(m_sPath,chBatchCode,0)){
				NoCutJobCommit(BATCH_STAT_SUCCESS);
			}else{
				NoCutJobCommit(BATCH_STAT_FAILURE);
			}
			#ifdef YJJS
			char szBuf[128] = {0};
			G_UNLOAD_ALL_USEINFO(chBatchCode,szBuf);
			char ccmd[256]={0};
			sprintf(ccmd,"rm -rf %s/%s",m_sPath,chBatchCode);
			system(ccmd);
			#endif
		}
		
		
		
		
	}
	
}
int FilterTask::NoCutJobCommit(int in_batchStatAfProcess){
	int iRet = ClientInf.NoCutJobCommit(in_batchStatAfProcess);
	if (iRet== PROC_CHECK_FAILURE){
		ClientInf.ReportProcStatus("ERROR", "进程校验失败", 0);
		LOG4CXX_ERROR(m_logger,"PROC_CHECK_FAILURE!");
		exit(-1);
	}else if (iRet== BATCH_FAILURE){
		ClientInf.ReportProcStatus("ERROR", "批次校验失败", 0);
		LOG4CXX_ERROR(m_logger,"BATCH_FAILURE!");
	}else if (iRet== COMMIT_SUCESS){
		ClientInf.ReportProcStatus("COMMIT", "批次提交成功", 0);
		sprintf(m_clog,"NoCutJobCommit:%d success!",in_batchStatAfProcess); 
		LOG4CXX_INFO(m_logger,m_clog);
	}
	return 1;
}
#else
int FilterTask::run(){
	if(m_iFlowFlag){
		processDir(m_sDir);
	}else{
		processFile(m_sDir,m_sFile,0);
	}
	return 1;
}
#endif
void FilterTask::printManual(){
}


/*=============================================business=============================================*/

bool FilterTask::processDir(string sDir){
	sprintf(m_clog,"processDir:%s",sDir.c_str());
	LOG4CXX_INFO(m_logger,m_clog);
	vector<string> svecDirs;
	showDesignationDirs(sDir.c_str(),svecDirs);
	for(vector<string>::iterator iter=svecDirs.begin();iter!=svecDirs.end();iter++){
		processFile(sDir,*iter);
	}
	return true;
}

//iFlowFlag 0:rollBack 1:update
bool FilterTask::processFile(string sdir,string sfilename,int iFlowFlag){
	sprintf(m_clog,"processFile:%s",sfilename.c_str());
	LOG4CXX_INFO(m_logger,m_clog);
	
	string sin=sdir+"/"+sfilename;
	vector<string> svecFiles;
	showDesignationFiles(sin.c_str(),svecFiles);

	m_mapOutFlag.clear();
	
	string strData;
	string soldData="";

	string scmd;
	
	//m_svecRbKey.clear();
	
	for(vector<string>::iterator iter=svecFiles.begin();iter!=svecFiles.end();iter++){
		ifstream in((sin+"/"+*iter).c_str(),ios::in);
		if (!in.is_open()){
			LOG4CXX_ERROR(m_logger,"open inputfile error!");
			return false;
		}
		if(m_ofData.is_open()){
			m_ofData.close();
		}
		if(m_ofExclude.is_open()){
			m_ofExclude.close();
		}
		string soutfile=sin+"/~"+*iter;
		string sExFileName=*iter;
		size_t pos=sExFileName.find("T100");
		if(pos==std::string::npos){
			sprintf(m_clog,"Wrong File Name:%s!",iter->c_str());
			LOG4CXX_WARN(m_logger,m_clog);
			continue;
		}
		sExFileName.replace(pos,4,"T125");
		string sexcludefile=sin+"/~"+sExFileName;
		/*
		if(fileExists(soutfile)){
			processOldDataIn(soutfile,sfilename);
		}
		*/
		if(iFlowFlag){
			m_ofData.open(soutfile.c_str(),ios::out);
			m_ofExclude.open(sexcludefile.c_str(),ios::out);
			m_ioutrows=0;
			m_iexrows=0;
			if(!m_ofData.is_open()){
				return false;
			}
			if(!m_ofExclude.is_open()){
				return false;
			}
		}

		//first line 
		getline(in,strData);
		//second line:fieldname
		if(getline(in,strData)){
			m_svecFiledName.clear();
			boost::split(m_svecFiledName,strData,boost::is_any_of(","));
		}else{
			//error
			in.close();
			return false;
		}
		if(iFlowFlag){
			m_ofData<<strData<<endl;
			m_ofExclude<<strData<<endl;
			
			//processOldDataOut(sfilename);
			processOtherBatchData(sfilename);
		}
		map<string,string> mapData;
		while(getline(in,strData)){
			if(construction(strData,m_svecFiledName,mapData)){

				int ret=processOneData(mapData,sfilename,iFlowFlag);
				if(ret){
					if(WRONGVALUE==ret){
						sprintf(m_clog,"ErrorData:%d:%s:%s",ret,sfilename.c_str(),iter->c_str());
						LOG4CXX_WARN(m_logger,m_clog);
						LOG4CXX_WARN(m_logger,strData.c_str());
						continue;
					}
					if(processOneData(mapData,sfilename,iFlowFlag)){
						sprintf(m_clog,"processOneData Error:%d",ret);
						LOG4CXX_ERROR(m_logger,m_clog);
						LOG4CXX_ERROR(m_logger,strData.c_str());
						return false;
					}else{
						LOG4CXX_INFO(m_logger,"Process again success!");
					}

				}
			}else{
				sprintf(m_clog,"WrongData:%s:%s",sfilename.c_str(),iter->c_str());
				LOG4CXX_WARN(m_logger,m_clog);
				LOG4CXX_WARN(m_logger,strData.c_str());
				return false;
			}
		}
		if(!in.eof()){
			sprintf(m_clog,"%s file eof error!",sfilename.c_str());
			LOG4CXX_ERROR(m_logger,m_clog);
			return false;
		}
		in.close();
		if(iFlowFlag){
			m_ofData.close();
			m_ofExclude.close();
			outputFinalFile(sin,*iter,m_ioutrows);
			outputFinalFile(sin,sExFileName,m_iexrows);
		}
	}
	return true;
}

bool FilterTask::processOtherBatchData(const string sBatchCode){
	string sKeyOtherBatchData="OtherBatchData";
	sKeyOtherBatchData+=sBatchCode;
	int iLen=0;
	boost::reference_wrapper<int> ref_iLen(iLen);
	sprintf(m_clog,"ListLen:%s",sKeyOtherBatchData.c_str());
	//if(!RedisListLen(sKeyOtherBatchData,iLen)){
	if(!RedisOp(boost::bind(&C_RedisOption::ListLen,&m_RedisFilter,sKeyOtherBatchData.c_str(),ref_iLen),m_clog)){
		return false;
	}
	const int SPAN=50;
	int iBegin=0;
	int iEnd=SPAN-1;
	vector<string> svReturn;
	boost::reference_wrapper<vector<string> > ref_svReturn(svReturn);
	while(iBegin<iLen){
		svReturn.clear();
		sprintf(m_clog,"ListRange:%s;iBegin:%d;iEnd:%d",sKeyOtherBatchData.c_str(),iBegin,iEnd);
		//if(!RedisListRange(sKeyOtherBatchData,iBegin,iEnd,svReturn)){
		if(!RedisOp(boost::bind(static_cast< int (C_RedisOption::*)(const char*,const int,const int,vector<string>&) >(&C_RedisOption::ListRange),&m_RedisFilter,sKeyOtherBatchData.c_str(),iBegin,iEnd,ref_svReturn),m_clog)){
			return false;
		}
		for(vector<string>::iterator iter=svReturn.begin();iter!=svReturn.end();iter++){
			m_ofData<<*iter<<endl;
			++m_ioutrows;
		}
		iBegin+=SPAN;
		iEnd+=SPAN;	
	}
	return true;
}

//0:success;-1:wrong value锛?2锛歭ock error锛?3锛歳edis error
int FilterTask::processOneData(map<string,string> &mapData,string sBatchNum,int iFlowFlag){
	LOG4CXX_DEBUG(m_logger,"processOneData begin!");
	
	if("0"==mapData["FILTER_ID"]){
		outputFile(mapData,m_ofData);
		++m_ioutrows;
		sprintf(m_clog,"FILTER_ID empty!");
		LOG4CXX_DEBUG(m_logger,m_clog);
		return SUCCESS;
	}
	
	//out of date
	long lStartDate=atol(mapData["ORDER_DATE"].c_str());
	long lBillingCycleID=atol(mapData["BILLING_CYCLE_ID"].c_str());
	long lEffDate=atol(mapData["FILTER_EFF_DATE"].c_str());
	long lExpDate=atol(mapData["FILTER_EXP_DATE"].c_str());
	//[)
	if(lStartDate<lEffDate||lStartDate>=lExpDate){
		
		outputFile(mapData,m_ofData);
		++m_ioutrows;
		LOG4CXX_DEBUG(m_logger,"Data out of date!");
		sprintf(m_clog,"ORDER_DATE:%s;FILTER_EFF_DATE:%s;FILTER_EXP_DATE:%s",mapData["ORDER_DATE"].c_str(),mapData["FILTER_EFF_DATE"].c_str(),mapData["FILTER_EXP_DATE"].c_str());
		LOG4CXX_DEBUG(m_logger,m_clog);
		sprintf(m_clog,"SERV_ID:%s;OBJ_ID:%s;CHECK_FIELD:%s",mapData["SERV_ID"].c_str(),mapData["OBJ_ID"].c_str(),mapData["CHECK_FIELD"].c_str());
		LOG4CXX_DEBUG(m_logger,m_clog);
		return SUCCESS;
	}

	int BillingCycle=0;
	int icycleType=atoi(mapData["FILTER_CYCLE_TYPE"].c_str());
	if(1==icycleType){
		BillingCycle=lBillingCycleID;
	}else if(2==icycleType){
		BillingCycle=lBillingCycleID/100;
	}else if(3==icycleType){
		lStartDate=lStartDate/1000000;
		BillingCycle=lStartDate;
	}else{
		sprintf(m_clog,"WRONGVALUE cycleType error!:%d",icycleType);
		LOG4CXX_ERROR(m_logger,m_clog);
		return WRONGVALUE;
	}
	
	char chkey[1024]={0};
	long lFilterField=atol(mapData["FILTER_FIELD"].c_str());
	if(1==lFilterField){
		sprintf(chkey,"%s_%d_%ld_%s", mapData["FILTER_ID"].c_str(),BillingCycle, lFilterField, mapData["SERV_ID"].c_str());
	}else if(2==lFilterField){
		sprintf(chkey,"%s_%d_%ld_%s", mapData["FILTER_ID"].c_str(),BillingCycle, lFilterField, mapData["OBJ_ID"].c_str());
	}else if(3==lFilterField){

		sprintf(chkey,"%s_%d_%ld_%s", mapData["FILTER_ID"].c_str(),BillingCycle, lFilterField, mapData["CHECK_FIELD"].c_str());
	}else{
		sprintf(m_clog,"WRONGVALUE FilterField error!:%ld",lFilterField);
		LOG4CXX_ERROR(m_logger,m_clog);
		return WRONGVALUE;
	}
		
	int tryNum=120;
	//status 0锛歠ail锛?锛歴uccess
	int iLockFlag=0;
	bool ret;
	while(tryNum--){	

		//lock
		string sLock="lock";
		sLock+=chkey;
		
		//expire锛?0s
		try{
			iret=m_RedisFilter.Set(sLock.c_str(),sBatchNum.c_str(),"nx",30,iLockFlag);
		}catch(...){
			LOG4CXX_ERROR(m_logger,"redis exception!");
		}
		if(iret){
			if(!RedisErrorHandler(iret,"lock")){
				return PROCESSERROR;
			}
		}
		//iLockFlag=1:success
		if(1==iLockFlag){
			//update
				ret=iFlowFlag?update(chkey,mapData,sBatchNum):rollBack(chkey,mapData,sBatchNum);
			//release lock
			sprintf(m_clog,"delKey:%s",sLock.c_str());
			//if(!RedisDelKey(sLock)){
			if(!RedisOp(boost::bind(&C_RedisOption::delkey,&m_RedisFilter,sLock.c_str()),m_clog)){
				return PROCESSERROR;
			}
			//jump loop
			if(ret){
				return SUCCESS;
			}else{
				return PROCESSERROR;
			}
		}else{
			sleep(1);
		}
	}
	return LOCKERROR;
	
}


bool FilterTask::update(const char* chkey,map<string,string> &mapData,string sBatchNum,long lUpdateValue){
	//VaildData
	string sKeyMap="Hash";
	sKeyMap+=chkey;

	#ifdef CLIENTINTERF
	map<string,string> tmapData;
	if(m_svecRedisFileds.size()){
		for(vector<string>::iterator iter=m_svecRedisFileds.begin();iter!=m_svecRedisFileds.end();++iter){
			tmapData[*iter]=mapData[*iter];
		}
		tmapData["BATCH_CODE"]=mapData["BATCH_CODE"];
		tmapData["CHARGE"]=mapData["CHARGE"];
	}else{
		tmapData=mapData;
	}
	boost::reference_wrapper<map<string,string> > ref_mapData(tmapData);
	#else
	boost::reference_wrapper<map<string,string> > ref_mapData(mapData);
	#endif
	sprintf(m_clog,"Hmset Key,Value:%s,%s",sKeyMap.c_str(),constructionStr(mapData,"^").c_str());
	//if(!RedisHmset(sKeyMap,mapData)){
	if(!RedisOp(boost::bind(&C_RedisOption::Hmset,&m_RedisFilter,sKeyMap.c_str(),ref_mapData),m_clog)){
		return false;
	}
	
	//VaildValue
	string sKeyValue="Value";
	sKeyValue+=chkey;
	char cUpdateValue[32]={0};
	sprintf(cUpdateValue,"%ld",lUpdateValue);

	sprintf(m_clog,"Insert Key,Value:%s,%s",sKeyValue.c_str(),cUpdateValue);
	//if(!RedisInsert(sKeyValue,cUpdateValue)){
	if(!RedisOp(boost::bind(static_cast< int (C_RedisOption::*)(const char*,const char*) >(&C_RedisOption::Insert),&m_RedisFilter,sKeyValue.c_str(),(const char*)cUpdateValue),m_clog)){
		return false;
	}

	/*
	string sKeyBatch="Batch";
	sKeyBatch+=mapData["BATCH_CODE"];
	if(!RedisHsetOneField(sKeyBatch,chkey,cUpdateValue)){
		return false;
	}
	*/
	
	string sKeyBatchKey="BatchKey";
	sKeyBatchKey+=mapData["BATCH_CODE"]+chkey;
	
	ostringstream oss;
	oss<<lUpdateValue;
	
	sprintf(m_clog,"InsertSet Key,Value:%s,%s",sKeyBatchKey.c_str(),oss.str().c_str());
	//if(!RedisInsertSet(sKeyBatchKey.c_str(),oss.str())){
	if(!RedisOp(boost::bind(static_cast< int (C_RedisOption::*)(const char*,const char*) >(&C_RedisOption::InsertMembers),&m_RedisFilter,sKeyBatchKey.c_str(),oss.str().c_str()),m_clog)){
		return false;
	}
	string sKV=chkey;
	sKV+=oss.str();
	m_mapOutFlag[sKV]=true;
	
	/*
	//BatchValue
	string sBatchValue="BatchValue"+mapData["BATCH_CODE"]+"_"+chkey;
	if(!RedisInsert(sBatchValue,cUpdateValue)){
		return false;
	}
	//Batch
	string sBatchSet="BatchSet|";
	sBatchSet+=mapData["BATCH_CODE"];
	if(!RedisInsertSet(sBatchSet,mapData["BATCH_CODE"])){
		return false;
	}
	*/
	/*
	string sKeyBatch="Batch";
	sKeyBatch+=chkey;
	iret=m_RedisFilter.Insert(sKeyBatch.c_str(),sBatchNum.c_str());
	if (iret){
		RedisErrorHandler(iret,"insert keybatch");
	}
	*/
	

	outputFile(mapData,m_ofData);
	++m_ioutrows;
	return true;
}

bool FilterTask::update(const char* chkey,map<string,string> &mapData,string sBatchNum){
	char cValidValue[256]={0};
	string sKeyValue="Value";
	sKeyValue+=chkey;
	
	sprintf(m_clog,"GetString:%s",sKeyValue.c_str());
	if(!RedisOp(boost::bind(&C_RedisOption::GetString,&m_RedisFilter,sKeyValue.c_str(),cValidValue),m_clog)){
		return false;
	}
	
	long lFilterMeasure=atol(mapData["FILTER_MEASURE"].c_str());
	long lUpdateValue;
	if(1==lFilterMeasure||2==lFilterMeasure){
		lUpdateValue=atol(mapData["CHARGE"].c_str());
	}else if(3==lFilterMeasure){
		lUpdateValue=atoi(mapData["CALC_PRIORITY"].c_str());
	}else{
		sprintf(m_clog,"FilterMeasure error!:%ld",lFilterMeasure);
		LOG4CXX_ERROR(m_logger,m_clog);
		outputFile(mapData,m_ofExclude);
		++m_iexrows;
		return true;
	}
	
	
	if(!cValidValue[0]){
		return update(chkey,mapData,sBatchNum,lUpdateValue);
		
	}else{
		long lValidValue=atoi(cValidValue);

		if((1==lFilterMeasure&&lUpdateValue>lValidValue)||
			(2==lFilterMeasure&&lUpdateValue<lValidValue)||
			(3==lFilterMeasure&&lUpdateValue>lValidValue)){
			string sKeyMap="Hash";
			sKeyMap+=chkey;
			map<string,string> mapRet;
			#ifdef CLIENTINTERF
			map<string,string> tmapRet;
			boost::reference_wrapper<map<string,string> > ref_mapRet(tmapRet);
			#else
			boost::reference_wrapper<map<string,string> > ref_mapRet(mapRet);
			#endif
			sprintf(m_clog,"HmgetAllField:%s",sKeyMap.c_str());
			if(!RedisOp(boost::bind(&C_RedisOption::HmgetAllField,&m_RedisFilter,sKeyMap.c_str(),ref_mapRet),m_clog)){
				return false;
			}
			#ifdef CLIENTINTERF
			mapRet=mapData;
			for(map<string,string>::iterator iter=tmapRet.begin();iter!=tmapRet.end();iter++){
				mapRet[iter->first]=mapRet[iter->second];
			}
			#endif
			
			mapRet["CHARGE"]=(0-atol(mapRet["CHARGE"].c_str()));
			if(mapRet["BATCH_CODE"]!=mapData["BATCH_CODE"]){
				string sKeyOtherBatchData="OtherBatchData";
				sKeyOtherBatchData+=mapData["BATCH_CODE"];

				sprintf(m_clog,"InsertList Key,Value:%s,%s",sKeyOtherBatchData.c_str(),constructionStr(mapRet,"^").c_str());
				//RedisInsertList(sKeyOtherBatchData,constructionStr(mapRet,"^"));
				if(!RedisOp(boost::bind(&C_RedisOption::InsertList,&m_RedisFilter,sKeyOtherBatchData.c_str(),constructionStr(mapRet,"^").c_str()),m_clog)){
					return false;
				}
			}else{
				string sKeyBatchKey="BatchKey";
				sKeyBatchKey+=mapData["BATCH_CODE"]+chkey;
				ostringstream oss;
				oss<<lValidValue;
				sprintf(m_clog,"SetDel Key,Value:%s,%s",sKeyBatchKey.c_str(),oss.str().c_str());
				//if(!RedisSetDel(sKeyBatchKey,oss.str())){
				if(!RedisOp(boost::bind(&C_RedisOption::MembersDelValue,&m_RedisFilter,sKeyBatchKey.c_str(),oss.str().c_str()),m_clog)){
					return false;
				}
			}
			outputFile(mapRet,m_ofData);
			++m_ioutrows;
			
			sprintf(m_clog,"delKey:%s",sKeyMap.c_str());
			
			//if(!RedisDelKey(sKeyMap)){
			if(!RedisOp(boost::bind(&C_RedisOption::delkey,&m_RedisFilter,sKeyMap.c_str()),m_clog)){
				return false;
			}

			sprintf(m_clog,"Update!key:%s;OldBatch:%s;lFilterMeasure:%ld;lUpdateValue:%ld;lValidValue:%ld",chkey,mapRet["BATCH_CODE"].c_str(),lFilterMeasure,lUpdateValue,lValidValue);
			LOG4CXX_DEBUG(m_logger,m_clog);
			
			return update(chkey,mapData,sBatchNum,lUpdateValue);
		}else{
			/*
			string sKeyBatch="Batch";
			sKeyBatch+=mapData["BATCH_CODE"];
			char cRet[64]={0};
			if(!RedisHgetOneField(sKeyBatch,chkey,cRet)){
				return false;
			}
			if(cRet[0]&&lUpdateValue==atol(cRet)&&find(m_svecRbKey.begin(),m_svecRbKey.end(),chkey)==m_svecRbKey.end()){
				outputFile(mapData,m_ofData);
				m_svecRbKey.push_back(chkey);
				++m_ioutrows;
			}else{
				outputFile(mapData,m_ofExclude);
				++m_iexrows;
				sprintf(m_clog,"Jump!key:%s;lFilterMeasure:%ld;lUpdateValue:%ld;lValidValue:%ld",chkey,lFilterMeasure,lUpdateValue,lValidValue);
				LOG4CXX_DEBUG(m_logger,m_clog);
			}
			*/
			string sKeyBatchKey="BatchKey";
			sKeyBatchKey+=mapData["BATCH_CODE"]+chkey;
			bool bRet=false;
			boost::reference_wrapper<bool> ref_bRet(bRet);
			
			ostringstream oss;
			oss<<lUpdateValue;
			sprintf(m_clog,"SIsMember Key,Value:%s,%s",sKeyBatchKey.c_str(),oss.str().c_str());
			//if(!RedisSetIsMember(sKeyBatchKey,oss.str(),bRet)){
			if(!RedisOp(boost::bind(&C_RedisOption::SIsMemeber,&m_RedisFilter,sKeyBatchKey.c_str(),oss.str().c_str(),ref_bRet),m_clog)){
				return false;
			}
			string sKV=chkey;
			sKV+=oss.str();
			if(bRet&&!m_mapOutFlag[sKV]){
				m_mapOutFlag[sKV]=true;
				outputFile(mapData,m_ofData);
				++m_ioutrows;
			}else{
				outputFile(mapData,m_ofExclude);
				++m_iexrows;
				sprintf(m_clog,"Jump!key:%s;lFilterMeasure:%ld;lUpdateValue:%ld;lValidValue:%ld",chkey,lFilterMeasure,lUpdateValue,lValidValue);
				LOG4CXX_DEBUG(m_logger,m_clog);
			}
			return true;
		}
	}
}


bool FilterTask::rollBack(const char* chkey,map<string,string> &mapData,string sBatchNum){
	/*
	string sKeyMap="Hash";
	sKeyMap+=chkey;
	char cReturn[128]={0};
	try{
		iret=m_RedisFilter.HgetOneField(sKeyMap.c_str(),"BATCH_CODE",cReturn);
	}catch(...){
				cout<<"redis exception!"<<endl;
	}
	if(iret){
		RedisErrorHandler(iret,"get keybatch");
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
			RedisErrorHandler(iret,"del keyvalue");
		}
		string sKeyMap="Hash";
		sKeyMap+=chkey;

		
		map<string,string> mapRet;
		try{
			iret=m_RedisFilter.HmgetAllField(sKeyMap.c_str(),mapRet);
		}catch(...){
				cout<<"redis exception!"<<endl;
		}
		if(iret){
			RedisErrorHandler(iret,"hmget keymap");
		}
		//!!!!!!!!!!!!!!!!!
		mapRet["CHARGE"]="-"+mapRet["CHARGE"];
		string stempdata="";
		for(vector<string>::iterator iter=m_svecFiledName.begin();iter!=m_svecFiledName.end();iter++){
			stempdata+=mapRet[*iter];
			stempdata+="^";
		}
		string soutdata(stempdata,0,stempdata.size()-1);
		m_ofData<<soutdata<<endl;
		++m_ioutrows;
		
		
		try{
			iret=m_RedisFilter.delkey(sKeyMap.c_str());
		}catch(...){
				cout<<"redis exception!"<<endl;
		}
		if(iret){
			RedisErrorHandler(iret,"del keymap");
		}
	}
	*/
	return true;
}


int main(int argc,char** argv){
	FilterTask task(argc,argv);
	task.prepare();
	task.run();
}



/*=============================================abandon=============================================*/
bool FilterTask::fileExists(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

bool FilterTask::processOldDataIn(string fileFullPath,const string sbatchCode){
	LOG4CXX_DEBUG(m_logger,"processOldDataIn begin!");
	ifstream t_if(fileFullPath.c_str(),ios::in);
	vector<string> t_svecFields;
	vector<string> t_svecDatas;
	string strData;
	string sOldDataBatch="OldData"+sbatchCode;
	//first line
	if(getline(t_if,strData)){
		boost::split(t_svecFields,strData,boost::is_any_of(","));
		while(getline(t_if,strData)){			
			/*
			boost::split(t_svecDatas,strData,boost::is_any_of("^"));
			vector<string>::iterator iter1=t_svecFields.begin();
			vector<string>::iterator iter2=t_svecDatas.begin();
			for(;iter1!=t_svecFields.end();iter1++,iter2++){
				if(t_svecDatas.end()==iter2)
					break;
				if("BATCH_CODE"==*iter1&&sbatchCode!=*iter2){
					//鍏edis
					int itrynum=4;
					while(itrynum--){
						try{
							iret=m_RedisFilter.InsertMembers(sOldDataBatch.c_str(),strData.c_str());
						}catch(...){
								cout<<"redis exception!"<<endl;
						}
						if(iret){
							if(RedisErrorHandler(iret,"insert olddata set")){
								itrynum--;
							}else{
								//redis error
							}
						}else{
							break;
						}
					}		
				}
			}
			*/

			
		}
	}
	if(!RedisInsertList(sOldDataBatch,strData)){
		return false;
	}
	if(t_if.eof()){
		t_if.close();
		string scmd="rm -f "+fileFullPath;
		system(scmd.c_str());
		return true;
	}else{
		t_if.close();
		return false;
	}
}

bool FilterTask::processOldDataOut(const string sbatchCode){
	LOG4CXX_DEBUG(m_logger,"processOldDataOut begin!");
	string sOldDataBatch="OldData"+sbatchCode;
	vector<string> tsvecOldData;
	int itrynum=4;
	while(itrynum--){
		try{
			iret=m_RedisFilter.GetAllList(sOldDataBatch.c_str(),tsvecOldData);
		}catch(...){
				LOG4CXX_ERROR(m_logger,"redis exception!");
		}
		if(iret){
			if(RedisErrorHandler(iret,"draw olddata set")){
				itrynum--;
			}else{
				//redis error
				return false;
			}
		}else{
			break;
		}
	}
	if(!iret){
		for(vector<string>::iterator iter=tsvecOldData.begin();iter!=tsvecOldData.end();iter++){
			m_ofData<<*iter<<endl;
			++m_ioutrows;
		}
		
		sprintf(m_clog,"delKey:%s",sOldDataBatch.c_str());
		//if(!RedisDelKey(sOldDataBatch)){
		if(!RedisOp(boost::bind(&C_RedisOption::delkey,&m_RedisFilter,sOldDataBatch.c_str()),m_clog)){
			return false;
		}
		return true;
	}else{
		return false;
	}
}

bool FilterTask::RedisInsert(string sKey,string sValue){
	int itrynum=4;
	while(itrynum--){
		try{
			iret=m_RedisFilter.Insert(sKey.c_str(),sValue.c_str());
		}catch(...){
				sprintf(m_clog,"redis insert exception!key:%s	value:%s",sKey.c_str(),sValue.c_str());
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"Insert")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"Insert error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisHmset(string sKey,map<string,string> &mapData){
	int itrynum=4;
	while(itrynum--){
		try{
			//iret=m_RedisFilter.Insert(sKeyMap.c_str(),mapData);
			iret=m_RedisFilter.Hmset(sKey.c_str(),mapData);
		}catch(...){
				LOG4CXX_ERROR(m_logger,"redis exception!");
		}
		if(iret){
			if(RedisErrorHandler(iret,"Hmset")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"Hmset error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisInsertSet(string sKey,string sValue){
	int itrynum=4;
	while(itrynum--){
		try{
			iret=m_RedisFilter.InsertMembers(sKey.c_str(),sValue.c_str());
		}catch(...){
				sprintf(m_clog,"redis insert exception!key:%s	value:%s",sKey.c_str(),sValue.c_str());
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"InsertSet")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"InsertSet error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisHsetOneField(string sKey,string sField,string sValue){
	int itrynum=4;
	while(itrynum--){
		try{
			iret=m_RedisFilter.Insert(sKey.c_str(),sField.c_str(),sValue.c_str());
		}catch(...){
				sprintf(m_clog,"redis insert exception!key:%s	value:%s",sKey.c_str(),sValue.c_str());
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"HsetOneField")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"HsetOneField error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisHgetOneField(string sKey,string sField,char* cRet){
	int itrynum=4;
	while(itrynum--){
		try{	
			iret=m_RedisFilter.HgetOneField(sKey.c_str(),sField.c_str(),cRet);
		}catch(...){
				sprintf(m_clog,"redis RedisHgetOneField exception!key:%s	field:%s",sKey.c_str(),sField.c_str());
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"HgetOneField")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"HgetOneField error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisInsertList(string sKey,string sValue){
	int itrynum=4;
	while(itrynum--){
		try{
			iret=m_RedisFilter.InsertList(sKey.c_str(),sValue.c_str());
		}catch(...){
				LOG4CXX_ERROR(m_logger,"redis exception!");
		}
		if(iret){
			if(RedisErrorHandler(iret,"InsertList")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"InsertList error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisDelKey(string sKey){
	int itrynum=4;
	while(itrynum--){
		try{
			iret=m_RedisFilter.delkey(sKey.c_str());
		}catch(...){
				LOG4CXX_ERROR(m_logger,"redis exception!");
		}
		if(iret){
			if(RedisErrorHandler(iret,"DelKey")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"DelKey error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisListPop(string sKey,char* cRet){
	int itrynum=4;
	while(itrynum--){
		try{	
			iret=m_RedisFilter.ListPop	(sKey.c_str(),cRet);
		}catch(...){
				sprintf(m_clog,"redis exception!");
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"ListPop")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"ListPop error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisListRange(string sKey,int iIndex,char* cRet){
	int itrynum=4;
	while(itrynum--){
		try{	
			iret=m_RedisFilter.ListRange(sKey.c_str(),iIndex,cRet);
		}catch(...){
				sprintf(m_clog,"redis exception!");
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"ListRange")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"ListRange error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisListRange(string sKey,int iBegin,int iEnd,vector<string> &svReturn){
	int itrynum=4;
	while(itrynum--){
		try{	
			iret=m_RedisFilter.ListRange(sKey.c_str(),iBegin,iEnd,svReturn);
		}catch(...){
				sprintf(m_clog,"redis exception!");
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"ListRange")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"ListRange error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisListLen(string sKey,int &iLen){
	int itrynum=4;
	while(itrynum--){
		try{	
			iret=m_RedisFilter.ListLen(sKey.c_str(),iLen);
		}catch(...){
				sprintf(m_clog,"redis exception!");
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"ListLen")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"ListLen error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisSetDel(string sKey,string sValue){
	int itrynum=4;
	while(itrynum--){
		try{	
			iret=m_RedisFilter.MembersDelValue(sKey.c_str(),sValue.c_str());
		}catch(...){
				sprintf(m_clog,"redis exception!");
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"SetDel")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"SetDel error!");
		return false;
	}
	return true;
}

bool FilterTask::RedisSetIsMember(string sKey,string sValue,bool& bRet){
	int itrynum=4;
	while(itrynum--){
		try{	
			iret=m_RedisFilter.SIsMemeber(sKey.c_str(),sValue.c_str(),bRet);
		}catch(...){
				sprintf(m_clog,"redis exception!");
				LOG4CXX_ERROR(m_logger,m_clog);
		}
		if(iret){
			if(RedisErrorHandler(iret,"SetDel")){
				itrynum--;
			}else{
				//redis error
			}
		}else{
			break;
		}
	}
	if(iret){
		LOG4CXX_ERROR(m_logger,"SetDel error!");
		return false;
	}
	return true;
}
