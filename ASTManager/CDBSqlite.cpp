#include "CDBSqlite.h"

//ȱʡ���캯��
CDBSqlite::CDBSqlite() {
}

/**
*@Description: ���캯������ʼ��dbname��Ա����
*@param[IN] dbname�����ݿ�����
*/
CDBSqlite::CDBSqlite(string& dbname) : m_pdb(NULL), m_pStmt(NULL) {
	m_dbname = dbname;
}


//���캯��
CDBSqlite::CDBSqlite(string& dbname, string& sql) : m_pdb(NULL), m_pStmt(NULL) {
	m_dbname = dbname;
	m_sql = sql;
}

//��������
CDBSqlite::~CDBSqlite()
{
    if (m_pStmt) {
        sqlite3_finalize(m_pStmt);
    }
	
    if (m_pdb) {
        sqlite3_close(m_pdb);
    }
}


//�����ݿ�����
int CDBSqlite::open()
{
	if( m_dbname.empty() )
	{
		cerr<<"m_dbname isn't been initialised!"<<endl;
		return RET_FAIL;
	}

	int rt = sqlite3_open(m_dbname.c_str(), &m_pdb);
	if( rt != SQLITE_OK )
	{
		string tmp = sqlite3_errmsg(m_pdb);
		cout<<"Can't open database["<<m_dbname<<"],err_msg["<<sqlite3_errmsg(m_pdb)<<endl;
		sqlite3_close(m_pdb);
		return RET_FAIL;
	}
	
	return RET_OK;
}

//�����ݿ�����
int CDBSqlite::open(string& dbname)
{
	int rt = sqlite3_open(dbname.c_str(), &m_pdb);
	if( rt != SQLITE_OK )
	{
		//cerr<<"Can't open database["<<dbname<<"],err_msg["<<sqlite3_errmsg(m_pdb)<<endl;
		errString=sqlite3_errstr(rt);
		sqlite3_close(m_pdb);
		return RET_FAIL;
	}

	return RET_OK;
}

//Description�����׼������ʼ�������ã����������Ա������ʹ�ó�Ա����m_pdb��m_sql����Ҫ����Ƿ��Ѿ���ʼ��
int CDBSqlite::prepare()
{
	if( !m_pdb )
	{
		cerr<<"m_pdb isn't init!, call open frist!"<<endl;
		return RET_FAIL;
	}
	if( m_sql.empty() )
	{
		cerr<<"m_sql isn't initialise!"<<endl;
		return RET_FAIL;
	}

	const char *zLeftover;
	m_pStmt=0;
	retCode = sqlite3_prepare_v2(m_pdb, m_sql.c_str(), -1, &m_pStmt, &zLeftover);
	if( retCode != SQLITE_OK )
	{
		errString = sqlite3_errstr(retCode);
		return RET_FAIL;
	}

	return RET_OK;
}

//���׼������ʼ�������ã����������Ա����
int CDBSqlite::prepare(string& sql, int length)
{
	if( m_pdb )
	{
		cerr<<"m_pdb isn't init!, call open frist!"<<endl;
		return RET_FAIL;
	}
	if( sql=="" )
	{
		cerr<<"sql is null!"<<endl;
		return RET_FAIL;
	}

	retCode = sqlite3_prepare_v2(m_pdb, sql.c_str(), -1, &m_pStmt, NULL);
	if( retCode != SQLITE_OK )
	{
		errString=sqlite3_errstr(retCode);
		return RET_FAIL;
	}

	return RET_OK;
}

/**
*@Description: ���ַ�������
*@param[in] index : ��Ҫ��ֵ��sql������ţ���1��ʼ
*@param[in] value : �󶨵�ֵ
*@param[in] length : value ռ�õ��ֽ���������Ǹ�������ȡ����������
*@param[in] dtfunc : value��������������Ϊ SQLITE_TRANSIENT �� SQLITE_STATIC
*/
int CDBSqlite::bindString(int index, const char* value, int length, void dtfunc(void*) )
{
	int rt = sqlite3_bind_text(m_pStmt, index, value, length, dtfunc);
	if( rt != SQLITE_OK )
	{
		string errmsg = sqlite3_errstr(rt);
		cerr<<"bind failed! ["<<sqlite3_errstr(rt)<<"]"<<endl;
		return RET_FAIL;
	}
	
	return RET_OK;
}

/**
*@Description: ��double�ͱ���
*@param[in] index : ��Ҫ��ֵ��sql������ţ���1��ʼ
*@param[in] value : �󶨵�ֵ
*@param[in] length : value ռ�õ��ֽ���������Ǹ�������ȡ����������
*@param[in] dtfunc : value��������������Ϊ SQLITE_TRANSIENT �� SQLITE_STATIC
*/
int CDBSqlite::bindDouble(int index, double value)
{
	int rt = sqlite3_bind_double(m_pStmt, index, value);
	if( rt != SQLITE_OK )
	{
		string errmsg = sqlite3_errstr(rt);
		cerr<<"bind failed! ["<<sqlite3_errstr(rt)<<"]"<<endl;
		return RET_FAIL;
	}
	
	return RET_OK;
}

/**
*@Description: ��int�ͱ���
*@param[in] index : ��Ҫ��ֵ��sql������ţ���1��ʼ
*@param[in] value : �󶨵�ֵ
*/
int CDBSqlite::bindInt(int index, int value)
{
	int rt = sqlite3_bind_int(m_pStmt, index, value);
	if( rt != SQLITE_OK )
	{
		string errmsg = sqlite3_errstr(rt);
		cerr<<"bind failed! ["<<sqlite3_errstr(rt)<<"]"<<endl;
		return RET_FAIL;
	}
	
	return RET_OK;
}


/**
*@Description:ִ�г����������Ҫ����������Ƿ���Ч��������з���������Ĳ�ѯ���ظ�ִ�����λ�ȡ�����
*@return 1:������һ����¼��0���ɹ�ִ����ϣ� -1 ��ִ�д���
*/
int CDBSqlite::step()
{
	if( !m_pStmt )
	{
		errString="m_pStmt isn't init, call prepare first!";
		return RET_FAIL;
	}
	retCode = sqlite3_step(m_pStmt);

    if ( retCode == SQLITE_ROW ) {
        return 1;

    } else if( retCode == SQLITE_DONE ) {
        return 0;

    } else if( retCode == SQLITE_BUSY) {
		errString="database is busy! can't benn written!";
		return RET_FAIL;

	}else if( retCode == SQLITE_MISUSE ) {
		errString="uses an interface that is undefined or unsupported!";
		return RET_FAIL;

	}else {
		errString=sqlite3_errstr(retCode);
		return RET_FAIL;
	}
}

/**
*@Description����ȡĳһ�У�char���ͣ���ֵ��ת��Ϊstring����
*@param[IN] index: �е������ţ���0��ʼ
*@return: ��ȡ����ֵ
*/
string CDBSqlite::getColumnString(int index)
{
	const unsigned char* pTmpBuf = sqlite3_column_text(m_pStmt, index);
	char buf[2048];
	sprintf(buf,"%s\n",pTmpBuf);
	string colStr=buf;

	return colStr;
}

/**
*@Description����ȡĳһ�У�int���ͣ���ֵ
*@param[IN] index: �е������ţ���0��ʼ
*@return: ��ȡ����ֵ
*/
int CDBSqlite::getColumnInt(int index)
{
	return sqlite3_column_int(m_pStmt, index);
}

/**
*@Description����ȡĳһ�У�double���ͣ���ֵ
*@param[IN] index: �е������ţ���0��ʼ
*@return: ��ȡ����ֵ
*/
double CDBSqlite::getColumnDouble(int index)
{
	return sqlite3_column_double(m_pStmt, index);
}


/**
*@Description: ��ȡ�������ĳһ�е�����
*@param[IN] index: �е������ţ���0��ʼ
*@return ��ȡ�������ͣ�SQLITE_INTEGER : 1; SQLITE_FLOAT : 2;SQLITE_TEXT : 3; SQLITE3_TEXT : 3; SQLITE_BLOB : 4; SQLITE_NULL : 5��
*/
int getColumnType(int index)
{
	return 0;
}

/**
*@Description: ��ȡ�������ĳһ�е�������
*@param[IN] index: �е������ţ���0��ʼ
*@return ������
*/
string CDBSqlite::getColumnName(int index)
{
	char buf[256];
	const char* pValue = sqlite3_column_name(m_pStmt, index);
	sprintf(buf,"%s\n",pValue);
	
	return string(buf);
}

/*ֱ��ִ��һ��sql
*/
int CDBSqlite::exec(const char *sql,                           /* SQL to be evaluated */
					int (*callback)(void*,int,char**,char**),  /* Callback function */
					void * arg,                                    /* 1st argument to callback */
					char **errmsg                              /* Error msg written here */
					){

	return sqlite3_exec( m_pdb, sql,callback,arg,errmsg);

}

/*����ʼ���ر��Զ��ύģʽ
*/
int CDBSqlite::begin(){

	char * errMsg = (char*) sqlite3_malloc(100);
	int rc = exec("BEGIN;", 0, 0, &errMsg);
	if (rc != 0 && errMsg) {
		errString = string(errMsg);
		sqlite3_free(errMsg);
	}
	

	return rc;
}

/*���������ͬʱ�����Զ��ύģʽ
*/
int CDBSqlite::end(){

	char * errMsg = (char*) sqlite3_malloc(100);
	int rc = exec("COMMIT;", 0, 0, &errMsg);
	if (rc != 0 && errMsg) {
		errString = string(errMsg);
		sqlite3_free(errMsg);
	}

	return rc;
}