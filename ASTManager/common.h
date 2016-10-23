#ifndef COMMON_H
#define COMMON_H

#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/datectrl.h>

#ifndef WX_PRECOMP    
	#include <wx/wx.h>
#endif
#include "CDBSqlite.h"
//#include "CRTData.h"
#include <vector>
#include <map>

static string gQryLogSql="select compose_id,stock_id,stock_name,buy_time,buy_price,buy_num,buy_allAmt,buy_reason,\
                         sell_time,sell_price,sell_num,sell_allAmt,sell_reason,trade_sumamt,trade_review,keyid,type \
                         , (buy_time<ifnull(sell_time,'1970-01-01')),case (buy_time<ifnull(sell_time,'1970-01-01')) when 1 then sell_time else buy_time end as sortid \
				 from trade_info order by compose_id,sortid desc";
static string gInsertLogSql="insert into trade_info(compose_id,stock_name,stock_id,buy_time,buy_price,buy_num,buy_allAmt,buy_reason,\
				 sell_time,sell_price,sell_num,sell_allAmt,sell_reason,trade_sumamt,trade_review,type)\
				  values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
static string gFindCanSell="select keyid,buy_num,sell_num,sell_allAmt,buy_allAmt,sell_price,sell_time,\
				 sell_reason,trade_sumamt,sell_price from trade_info where ((buy_num-sell_num)>0 or sell_num is null) \
                 and stock_id=? and compose_id=? order by buy_time desc";

static string gSellUpdate="update trade_info set sell_time=?,sell_price=?,sell_num=?,sell_allAmt=?, \
                          sell_reason=?,trade_sumamt=? where rowid=?";

static string gDeleteLog="delete from trade_info where keyid=?";
static string gUpdateReview="update trade_info set trade_review=?  where rowid=?";


//��ѯ����ķݶ�
const std::string qryCashSql = "select cash, fund_share from cash_flow a where a.compose_id=? and a.change_time=(select max(change_time) from cash_flow where compose_id=a.compose_id)";
const std::string insertCashSql = "insert into cash_flow(compose_id,cash,fund_share,change_time,change_reason) values(?,?,?,datetime('now'),?)";

//sqlite���ݿ��������
//extern CDBSqlite gSqlite;

enum MainID
{
    ID_MAIN_FRAME=999,
    ID_COMMON_REVIEW_BUTTON=1000,
    ID_DELETE_BUTTON=1001,
    ID_PREVIOUS_BUTTON=1002,
    ID_NEXTPAGE_BUTTON=1003,
    ID_SEARCH_LOG_BUTTON=1004,
    ID_STOCKNAME_COMBOBOX=1005,
    ID_VALUESHOW_PANEL=1006,
    ID_MARKETVAL_TEXTCTRL=1007,
    ID_CASH_TEXTCTRL=1008,
    ID_STOCKRATIO_TEXTCTRL=1009,
    ID_TOTALASSET_TEXTCTRL=1010,
    ID_RATIOADVICE_TEXTCTRL=1011,
    ID_NETVALUE_TEXTCTRL,
    ID_SHARE_TEXTCTRL,
    ID_HKCOM,
    ID_ACOM,
    ID_FIXEDCOM,
    ID_ALLCOM,
    ID_BUY ,
    ID_SELL,
    ID_ADJUSTCASH,
    ID_BUG_FOUND,
    ID_SELL_FOUND,
    ID_REPORT_MENU,
    ID_DOWNLOAD_YEAR_THREETBL,
    ID_DOWNLOAD_MID_THREETBL,
    ID_DOWNLOAD_ONE_THREETBL,
    ID_UPDATE_ALLPRICE,
    ID_SHOW_THREE_TABLE,
    ID_FIND_ASHOT,
    ID_CALCINDEX_TABLE,
    ID_LOG_GRID,
    ID_GOODS_GRID,
    ID_LOAD_NEWS
};

struct LogDataDet
{
	int keyid;
    int composeid;
	string stock_name;
	string stock_id;
	string buy_time;
	double buy_price; 
	int buy_num;
	double buy_allAmt;
	string buy_reason;
	string sell_time; 
	double sell_price;
	int sell_num;
	double sell_allAmt;
	string sell_reason;
	double trade_sumamt;
	string trade_review;
    string type;//�������ͣ�1Ϊ���գ�0Ϊ������
};

struct GoodsDet{
    string stock_name;
    string stock_id;
    int num;
    double price; 
    double amt;
    bool isBear; //�Ƿ����գ�true�ǣ�false��

    friend bool operator <(const GoodsDet& la, const GoodsDet& rb);
};

/*ˢ�µ�Nҳ������
*@vSrc������Դ������ds��grid��������Դ��numInOnePage��һҳ�е�������pageNum����䵽ds��ҳ��
*/
void setPageFromVector2GridTable(vector<LogDataDet>& vSrc, wxGridStringTable& gridStrTab, int numInOnePage, int pageNum);

inline int getRowNumInOnePage(vector<LogDataDet>& vSrc, int numInOnePage, int pageNum)
{
	/*int test5 = (numInOnePage * (pageNum - 1));
	int test6 = vSrc.size();
	bool test7 = ((test6 - test5) > 0);
	int test8 = vSrc.size() - test5;
	bool test2 = ((vSrc.size() - test5) > 0); //��vSrc.size()Ϊ6�� test5Ϊ10 ʱ���˴�Ϊʲô������true������
	bool test4 = test1 > 0;
	int test3 = (vSrc.size() - numInOnePage * (pageNum - 1));*/

	int intervalRowNum = (vSrc.size() - numInOnePage * (pageNum - 1));//�Ƚ�������������������ĵ�ҳ���� �� ǰһҳ�����������Ƚϣ����С��0������0
	int VecRowNum = (intervalRowNum > 0) ? intervalRowNum : 0;
	
	return VecRowNum < numInOnePage ? VecRowNum : numInOnePage;
}

/*ȥ��string���ߵĿո񡢻��з�*/
string& stringTrim(string& src);

//�ַ���ת��Ϊʱ���ʽ
static time_t StringToDatetime(const char *str)  
{  
    tm tm_;  
    int year, month, day, hour=0, minute=0,second=0;  
    sscanf(str,"%d-%d-%d", &year, &month, &day);
    tm_.tm_year  = year-1900;  
    tm_.tm_mon   = month-1;  
    tm_.tm_mday  = day;  
    tm_.tm_hour  = hour;  
    tm_.tm_min   = minute;  
    tm_.tm_sec   = second;  
    tm_.tm_isdst = 0;

    time_t t_ = mktime(&tm_); //�Ѿ�����8��ʱ��  
    return t_; //��ʱ��  
}  

inline std::string& rtrim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    std::string::size_type pos = str.find_last_not_of(whitespaces);

    if (pos != std::string::npos) {
        str.erase(pos + 1);

    } else {
        str.clear();
    }

    return str;
}

inline std::string& ltrim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    return str.erase(0, str.find_first_not_of(whitespaces));
}

inline
std::string& trim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    return ltrim(rtrim(str, whitespaces), whitespaces);
}


inline int splitString(const std::string& str, std::vector<std::string >& vec, const std::string spChs, bool onlyFirst, bool multiChar = false) {
    vec.clear();
    std::string workStr = str;
    std::string tmpStr;
    
    if (trim(workStr, spChs.c_str()).empty()) {
        return 0;
    }

    int curPos = 0, bakPos = 0;

    bool isfirst = true;
    do {

        if (multiChar) { //���Ҷ��ַ��ķָ���
            if(!isfirst) {
                bakPos = curPos + spChs.size();
            }
            isfirst = false;
            curPos = workStr.find(spChs.c_str(), bakPos);
            
        }else{
            bakPos = workStr.find_first_not_of(spChs.c_str(), curPos);
            curPos = workStr.find_first_of(spChs.c_str(), bakPos);
        }

        if (curPos != std::string::npos) {  //lint !e650 !e737  l00294687 pclint 2015.6.4 
            vec.push_back(workStr.substr(bakPos, curPos - bakPos));

            if (onlyFirst) {
                tmpStr = workStr.substr(curPos + 1, workStr.length() - curPos - 1); //lint !e737 by wWX238163
                trim(tmpStr);
                vec.push_back(tmpStr);
                break;
            }

        } else {
            tmpStr = workStr.substr(bakPos, workStr.length() - bakPos); //lint !e737 by wWX238163
            trim(tmpStr);
            vec.push_back(tmpStr);
            break;
        }

        //curPos += 1;
    } while (true);

    return 1;
}

//����һ�����ڣ������month��0��ʼ������������µ����һ��
//���أ������µ����һ�죻�����-1������
inline int GetLastDate(int month,int year)
{
    if(year<=1) {
        return -1;
    }
    if(month<1||month>12){
        return -1;
    }

    int months[]={31,0,31,30,31,30,31,31,30,31,30,31};
    if(year % 4 ==0 && year % 100 !=0 || year % 400 ==0)
        months[1]=29;
    else months[2]=28;

    return months[month];
}

/*���㽻�������� 
* param[in] stockid ���id��tradeNum ���׹����� tradeAmt ������; type �������ͣ�0 ��1 ��
* param[out] tax, ���Σ���Ӷ��
* return: <0, ����Ӷ��ʧ��
*/
int getTradingFee(std::string stockid, int tradeNum, double tradeAmt, int type, double& tax);

//����۹ɵĽ��������ѣ�tradeAmt,�ܳɽ��return ����������
int getHKTradingFee(double tradeAmt, double& tax);

/*����a�������� 
* param[in] stockid ���id��tradeNum ���׹����� tradeAmt ������; type �������ͣ�0 ��1 ��
* param[out] tax, ���Σ���Ӷ��
* return: <0, ����Ӷ��ʧ��
*/
int getATradingFee(std::string stockid, int tradeNum, double tradeAmt, int type, double& tax);

/*���ݶ̵�stockid����������url�����code
* return url�����code
*/
std::string genUrlRequestCode(std::string shortCode);

//genUrlRequestCode �����ķ�����
std::string reverseUrlCode(std::string urlCode);

//������ݿ����Ƿ��Ѿ�������,���recordtime:ʱ�䣻return��true �������ݣ�false δ������
bool QryUniqueRptData(string querySql, string stock_id, string recordtime);

//��ȡ���еı���б�
int getAllStockIdName(vector<string>& vecStockIdName);

int getAllStockId(vector<string>& vecStockId); //����ȡ����stockid

void qryCashAndShare(int composeId, double& cashVaule, double& curShare); //��ѯ���µ��ֽ�ͷֶ�
void InsertCashRecord(int composeId, double cash, double share, string &reasonStr );//�����¼

void qryValueInfo(int composeId, string& datetime,string& value_advice,string& detailInfo,double& fundShare,double& fundValue, double& marketvalue,double& cash);

class CRTData;
void createRtDataObj(CRTData** destObj);//���� CRTData ����

double getMarketValue(int composeId);//��ȡָ����ϵ���ֵ

#endif