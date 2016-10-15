#ifndef ASTMANAGER_CDBSQLITE_H
#define ASTMANAGER_CDBSQLITE_H

#include "sqlite3.h"
#include<string>
#include<iostream>

using namespace std;

const int RET_OK = 0; //ִ�гɹ�����ֵ
const int RET_FAIL = -1; //ִ��ʧ�ܷ���ֵ

class CDBSqlite
{
public:
	//ȱʡ���캯��
	CDBSqlite();

	/**
	*@Description: ���캯������ʼ��dbname��Ա����
	*@param[IN] dbname�����ݿ�����
	*/
	CDBSqlite(string& dbname);

	/**
	*@Description: ���캯������ʼ��������Ա����
	*@param[IN] dbname�����ݿ�����
	*@parmm[IN] sql: ִ��sql
	*/
	CDBSqlite(string& dbname, string& sql);

	//��������
	~CDBSqlite();

	/**
	*@Description: ��sqlite3 ���ӣ�ʹ���ڲ���Ա����m_dbname����Ҫ�Ƿ��Ѿ���ʼ��
	*@return SQLITE_OK ���򿪳ɹ��� ��0���������
	*/
	int open();

	/**
	*@Description: ��sqlite3 ���ӣ�ʹ�ò����������ݿ�����
	*@return SQLITE_OK ���򿪳ɹ��� ��0���������
	*/
	int open(string& dbname);

	/**
	*@Description�����׼������ʼ�������ã����������Ա������ʹ�ó�Ա����m_pdb��m_sql����Ҫ����Ƿ��Ѿ���ʼ��
	*@return SQLITE_OK : �ɹ��� ��0���������
	*/
	int prepare();

	/**
	*@Descripton�����׼������ʼ�������ã����������Ա����
	*@param[IN] sql: ���׼����sql
	*@param[IN] length: sql�ĳ���
	*@return SQLITE_OK : �ɹ��� ��0���������
	*/
	int prepare(string& sql, int length);

	/**
	*@Description: ���ַ�������
	*@param[in] index : ��Ҫ��ֵ��sql������ţ���1��ʼ
	*@param[in] value : �󶨵�ֵ
	*@param[in] length : value ռ�õ��ֽ���������Ǹ�������ȡ����������
	*@param[in] dtfunc : value��������������Ϊ SQLITE_TRANSIENT �� SQLITE_STATIC
	*/
	int bindString(int index, const char* value, int length, void dtfunc(void*) );

	/**
	*@Description: ��double�ͱ���
	*@param[in] index : ��Ҫ��ֵ��sql������ţ���1��ʼ
	*@param[in] value : �󶨵�ֵ
	*/
	int bindDouble(int index, double value);

	/**
	*@Description: ��int�ͱ���
	*@param[in] index : ��Ҫ��ֵ��sql������ţ���1��ʼ
	*@param[in] value : �󶨵�ֵ
	*/
	int CDBSqlite::bindInt(int index, int value);

	/**
	*@Description:ִ�г����������Ҫ����������Ƿ���Ч��������з���������Ĳ�ѯ���ظ�ִ�����λ�ȡ�����
	*@return 1:������һ����¼��0���ɹ�ִ����ϣ� -1 ��ִ�д���
	*/
	int step();

	/**
	*@Description: ��ȡ�������ĳһ�е�����
	*@param[IN] index: �е������ţ���0��ʼ
	*@return ��ȡ�������ͣ�SQLITE_INTEGER : 1; SQLITE_FLOAT : 2;SQLITE_TEXT : 3; SQLITE3_TEXT : 3; SQLITE_BLOB : 4; SQLITE_NULL : 5��
	*/
	int getColumnType(int index);

	/**
	*@Description: ��ȡ�������ĳһ�е�������
	*@param[IN] index: �е������ţ���0��ʼ
	*@return ������
	*/
	string getColumnName(int index);

	/**
	*@Description����ȡĳһ�У�int���ͣ���ֵ
	*@param[IN] index: �е������ţ���0��ʼ
	*@return: ��ȡ����ֵ
	*/
	int getColumnInt(int index);

	/**
	*@Description����ȡĳһ�У�double���ͣ���ֵ
	*@param[IN] index: �е������ţ���0��ʼ
	*@return: ��ȡ����ֵ
	*/
	double getColumnDouble(int index);

	/**
	*@Description����ȡĳһ�У�char���ͣ���ֵ��ת��Ϊstring����
	*@param[IN] index: �е������ţ���0��ʼ
	*@return: ��ȡ����ֵ
	*/
	string getColumnString(int index);

	/**
	*@Description: ���ó�Ա����m_sql��ֵ
	*@param[in] sql : sql���
	*/
	void setSql(string sql)
	{
		m_sql = sql;
	}
	
	/**
	*@Description: ��ȡ��Ա����m_sql��ֵ
	*@return ����m_sql��ֵ
	*/
	string getSql()
	{
		return m_sql;
	}
	
	//�ͷ�sqlite3_stmt* ָ��Ķ���
	int finalize(){
		
		return sqlite3_finalize(m_pStmt);
	}

	int close(){
		return sqlite3_close(m_pdb);
	}

	int retCode; //������һ��ִ��sqlite�����ķ���ֵ��Ϊ���õ�����֪��������
	string errString; //������������ַ���

protected:
	sqlite3* m_pdb; //����sqlite3 ������
	sqlite3_stmt* m_pStmt; //����������
	string m_dbname; //����sqlite3���ݿ�����
	string m_sql; //����ִ��sql
};





#endif // ASTMANAGER_CDBSQLITE_H