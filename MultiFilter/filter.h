#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include <algorithm>
using namespace std;

//for showDesignationFiles
#include <unistd.h>  
#include <dirent.h>  
#include <stdlib.h>  
#include <sys/stat.h> 

#include <boost/format.hpp>  
#include <boost/tokenizer.hpp>
#include<boost/algorithm/string.hpp>//for boost::split
#include<boost/bind.hpp>

#include <boost/program_options.hpp>
namespace BPO = boost::program_options;

#include <log4cxx/logger.h>  
#include <log4cxx/basicconfigurator.h>  
#include <log4cxx/propertyconfigurator.h>  
#include <log4cxx/helpers/exception.h>
using namespace log4cxx;

//for Process
//#include "Process.h"
//for m_RedisFilter
#include "C_RedisOption.h"
//for Config
#include "config.h"


#ifdef	CLIENTINTERF
#include "redisparamintf.h"
#include "schedulerClient.h"
#endif

#ifdef YJJS
#include "Environment.h"
#endif


class FilterTask/*:public Process*/{
	public:
		FilterTask(int argc,char** argv):g_argc(argc),g_argv(argv){}
		~FilterTask();
		bool prepare();
		int run();
		void printManual();
	private:
		enum{STRING_LEN=1000,LINE_LENGTH=1000};
		bool processDir(string sDir);
		bool processFile(string sdir,string sfilename,int iFlowFlag=1);
		bool processOldDataIn(string fileFullPath,const string sbatchCode);
		bool processOldDataOut(const string sbatchCode);
		bool processOtherBatchData(const string sBatchCode);
		enum nReturn{SUCCESS=0,WRONGVALUE=-1,LOCKERROR=-2,PROCESSERROR=-3};
		int processOneData(map<string,string> &mapData,string sBatchNum,int iFlowFlag=1);
		bool update(const char* chkey,map<string,string> &mapData,string sBatchNum,long lUpdateValue);
		bool update(const char* chkey,map<string,string> &mapData,string sBatchNum);
		bool rollBack(const char* chkey,map<string,string> &mapData,string sBatchNum);
		
		bool IsDirectory(char* pchPath);
		bool CheckPath(char *pchPath);
		bool GetRedisConfigInfo(const char* cFieldName,ConnParam *pstParam);
		bool fileExists(const std::string& file);
		void showDesignationFiles( const char * dir_name ,vector<string> &svecFiles);
		void showDesignationDirs( const char * dir_name ,vector<string> &svecDirs);
		bool construction(string& strData,vector<string>& svecFiledName,map<string,string> &mapData);
		string constructionStr(map<string,string>& mapData,string sSeparator);
		bool RedisErrorHandler(const int iErrCode,string sMsg);
		bool RedisInsert(string sKey,string sValue);
		bool RedisHmset(string sKey,map<string,string> &mapData);
		bool RedisInsertSet(string sKey,string sValue);
		bool RedisHsetOneField(string sKey,string sField,string sValue);
		bool RedisHgetOneField(string sKey,string sField,char* cRet);
		bool RedisInsertList(string sKey,string sValue);
		bool RedisDelKey(string sKey);
		bool RedisListPop(string sKey,char* cRet);
		bool RedisListRange(string sKey,int iIndex,char* cRet);
		bool RedisListRange(string sKey,int iBegin,int iEnd,vector<string> &svReturn);
		bool RedisListLen(string sKey,int &iLen);
		bool RedisSetDel(string sKey,string sValue);
		bool RedisSetIsMember(string sKey,string sValue,bool& bRet);
		template<typename T> bool RedisOp(T t,string sMsg);
		bool outputFile(map<string,string>& mapData,ofstream& outfile);
		bool outputFinalFile(string sPath,string sFileName,int iCount);
		bool redisConnectDb(const int dbID);
		
		#ifdef CLIENTINTERF
		int NoCutJobCommit(int in_batchStatAfProcess);
		#endif
		
	private:
		//redis
		ConnParam m_stParam;
		C_RedisOption m_RedisFilter;
		int iret;
		//work info
		int m_iFlowFlag;
		string m_sDir;
		string m_sFile;
		int g_argc;
		char** g_argv;
		//输入输出字段名
		vector<string> m_svecFiledName;
		#ifdef CLIENTINTERF
		//redis 
		vector<string> m_svecRedisFileds;
		#endif
		//vector<string> m_svecRbKey;
		map<string,bool> m_mapOutFlag;
		//输出正确数据
		ofstream m_ofData;
		ofstream m_ofExclude;
		int m_ioutrows;
		int m_iexrows;
		//日志
		char m_clog[10240];
		LoggerPtr m_logger;
		
		#ifdef CLIENTINTERF
		//共享内存key
		int m_iKey;
		int m_iRnd;
		//流程调度
		C_SchedulerClient ClientInf;
		//
		string m_sPath;
		#endif
};
