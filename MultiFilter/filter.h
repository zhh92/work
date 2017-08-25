#include<iostream>
#include<string>
#include<fstream>
#include<vector>
using namespace std;

//for showDesignationFiles
#include <unistd.h>  
#include <dirent.h>  
#include <stdlib.h>  
#include <sys/stat.h> 

#include <boost/format.hpp>  
#include <boost/tokenizer.hpp>
#include<boost/algorithm/string.hpp>//for boost::split

#include <boost/program_options.hpp>
namespace BPO = boost::program_options;


//for Process
//#include "Process.h"
//for m_RedisFilter
#include "C_RedisOption.h"
//for Config
#include "config.h"

class FilterTask/*:public Process*/{
	public:
		FilterTask(int argc,char** argv):g_argc(argc),g_argv(argv){}
		~FilterTask();
		bool prepare();
		int run();
		int test();
		void printManual();
	private:
		enum{STRING_LEN=1000,LINE_LENGTH=1000};
		bool processDir(string sDir);
		bool processFile(string sdir,string sfilename,int iFlowFlag=1);
		enum nReturn{SUCCESS=0,WRONGVALUE=-1,LOCKERROR=-2,PROCESSERROR=-3};
		int processOneData(map<string,string> &mapData,string sBatchNum,int iFlowFlag=1);
		bool update(const char* chkey,map<string,string> &mapData,string sBatchNum,long lUpdateValue);
		bool update(const char* chkey,map<string,string> &mapData,string sBatchNum);
		bool rollBack(const char* chkey,map<string,string> &mapData,string sBatchNum);
		
		bool IsDirectory(char* pchPath);
		bool CheckPath(char *pchPath);
		bool GetRedisConfigInfo(ConnParam *pstParam);
		void showDesignationFiles( const char * dir_name ,vector<string> &svecFiles);
		bool construction(string& strData,vector<string>& svecFiledName,map<string,string> &mapData);
		void RedisErrorHandler(int iErrCode,string sMsg);
		
		
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
		//格式错误
		ofstream m_ofWrongData;
		//处理错误
		ofstream m_ofErrData;
		//输出正确数据
		ofstream m_ofData;
};