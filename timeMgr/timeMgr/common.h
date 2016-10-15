#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H

#include "wx/wxprec.h"
#include <wx/grid.h>
#ifndef WX_PRECOMP
       #include "wx/wx.h"
#endif
#include "wx/datetime.h"
#include "CDBSqlite.h"
#include <string>
#include <vector>

static wxDateTime startTime;
static wxDateTime endTime;

static std::string gDBName = "./timeMgrData.db"; //���ݿ�·��������
//sqlite���ݿ��������
static CDBSqlite gSqlite(gDBName);

static std::string gInsertSql = "insert into things_set(start_time,end_time,do_things,priority) values(?,?,?,?)";

static string gQryHistory = "SELECT do_things, priority from things_set order by keyid desc";

static string gAllQrySql = "SELECT keyid,start_time,end_time,strftime('%s',end_time)-strftime('%s',start_time),do_things,priority,"
	"strftime('%w',start_time),strftime('%W',start_time) from things_set order by start_time desc";

static string gGetObject = "select things from object_plan";//��������£��˱�ֻ��һ����¼
static string gUpdateObject = "update object_plan set things=?";//��������£��˱�ֻ��һ����¼


//������ʱ��ʹ�����ݵļ���ά��
class DataContainer{
public:
    int keyid;
	int iWeekofYear; //һ���еĵڼ�������
	int idayOfWeek; //һ�������еĵڼ���
	std::string startTime;
	std::string endTime;
	int usedTimeSec; //ʹ��ʱ�䣬��λΪ��
	std::string doTings;
	std::string priority;
};

extern vector<DataContainer> gVecAllData; //�������е���ϸ����
extern const unsigned int lineNumEachPage; //ÿҳ������
extern unsigned int curPage; //��ǰҳ��

//ʱ��ת�����������밴�մ�Сת��Ϊ���ʵĵ�λ��������ת�����string����
inline string timeConvert(int sec){
	char cTmp[32];
	string targetTime;
	memset(cTmp,'0',sizeof(cTmp));
	if( sec < 60){ //ʱ�䵥λ�жϣ�С��60��
		sprintf(cTmp,"%d",sec);
		targetTime = std::string(cTmp) + "��";
	}else if( sec < 60 * 60 ){ //С��60����
		sprintf(cTmp,"%.2f",static_cast<double>(sec)/60);
		targetTime = std::string(cTmp) + "����";
	}else if( sec < 60*60*24){ //С��24Сʱ
		sprintf(cTmp,"%.2f",static_cast<double>(sec)/(60*60));
		targetTime = std::string(cTmp) + "Сʱ";		
	}else{
		sprintf(cTmp,"%.2f",static_cast<double>(sec)/(60*60*24));
		targetTime = std::string(cTmp) + "��";
	}

	return targetTime;
}

//�������ִ�0��ʼ������������
inline string GetDayOfWeek(int index){
	string target;
	switch(index){
		case 0:
			target = "������";
			break;
		case 1:
			target = "����һ";
			break;
		case 2:
			target = "���ڶ�";
			break;
		case 3:
			target = "������";
			break;
		case 4:
			target = "������";
			break;
		case 5:
			target = "������";
			break;
		case 6:
			target = "������";
			break;
	}
	
	return target;
}

void fillPageFromVector2GridTable(vector<DataContainer>& vSrc, wxGridStringTable& gridStrTable, unsigned int numInOnePage, unsigned int pageNum);

inline int getRowNumInOnePage(vector<DataContainer>& vSrc, int numInOnePage, int pageNum)
{
	int intervalRowNum = (vSrc.size() - numInOnePage * (pageNum - 1));//�Ƚ�������������������ĵ�ҳ���� �� ǰһҳ�����������Ƚϣ����С��0������0
	int VecRowNum = (intervalRowNum > 0) ? intervalRowNum : 0;
	
	return VecRowNum < numInOnePage ? VecRowNum : numInOnePage;
}


#endif