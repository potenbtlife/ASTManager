#include "common.h"
#include "Runtime.h"
#include <math.h>
std::string qryStockIdName = "select distinct stock_id, stock_name from stock_list order by stock_id";
std::string qryStockId = "select distinct stock_id from stock_list order by stock_id";
std::string qryPriceByStockid = "select curprice from stock_list where stock_id=?";
std::string qryNameByStockid = "select stock_name from stock_list where stock_id=?";
const std::string m_qryValueSql = "select a.recordtime, a.value_advice, a.detail_info, a.fund_share, a.fund_value, a.marketvalue, \
                                  a.cash from value_info a  where a.recordtime=(select max(recordtime) from value_info \
                                  where compose_id=a.compose_id) and compose_id=?";

Runtime* Runtime::_instance = 0;

bool operator <(const GoodsDet& la, const GoodsDet& rb){ //�����յ����룬�ٰ��Ӵ�С����
    if(la.isBear) {
        if (!rb.isBear) {
            return true;
        }else if(la.num*la.price > rb.num*rb.price) {
            return true;

        }else if(la.num*la.price < rb.num*rb.price) {
            return false;

        }else{  //������һ�£��ٰ���id����
            return (la.stock_id > rb.stock_id);
        }

    }else{
        if (la.num*la.price > rb.num*rb.price) {
            return true;

        }else if(la.num*la.price < rb.num*rb.price) {
            return false;

        }else{  //������һ�£��ٰ���id����
            return (la.stock_id > rb.stock_id);
        }
    }

    return false;
}


//����۹ɵĽ��������ѣ�tradeAmt,�ܳɽ��return ����������
/*
���׷���
1. ����Ӷ��
0.0188%  ���18.8

2. ���׷�
ÿ�ڽ��׶�֮0.005%

3. ӡ��˰
ÿ�ڽ��׶�֮0.1% (����һԪ����һԪ����)
ÿ�ڽ��׶�1,000Ԫ, �ո۱�1Ԫ

4. ��������
ÿ�ڽ��׶�֮0.0027%

5. �������ɷݽ��շ�
ÿ�ڽ��׶�֮0.002%
��͸۱�2Ԫ
*/

/*���������� 
* param[in] stockid ���id��tradeNum ���׹����� tradeAmt ������; type �������ͣ�0 ��1 ��
* param[out] tax, ���Σ���Ӷ��
* return: <0, ����Ӷ��ʧ��
*/
int getTradingFee(std::string stockid, int tradeNum, double tradeAmt, int type, double& tax){
    
    std::string sockidAdd = genUrlRequestCode(stockid);

    if (sockidAdd.substr(0,2) == "hk" || sockidAdd.substr(0,2) == "HK") {
        return getHKTradingFee(tradeAmt, tax);

    }else if(sockidAdd.substr(0,2) == "sh" || sockidAdd.substr(0,2) == "SH" ||
        sockidAdd.substr(0,2) == "sz" || sockidAdd.substr(0,2) == "SZ") {

        return getATradingFee(stockid, tradeNum, tradeAmt, type, tax);
    }
    
    return 0;
}

/*����۹�������
* param[in] tradeAmt, ������
* param[out] tax, ���Σ���Ӷ��
* return: <0, ����Ӷ��ʧ��
*/
int getHKTradingFee(double tradeAmt, double& tax){
    
    string yongjinStr = Runtime::getInstance()->configObj->find("HK_YONGJIN");
    double lowprice=0, ratio=0;
    sscanf(yongjinStr.c_str(), "%lf:%lf", &lowprice, &ratio);

    if(ratio < -0.0005){ //С��0�������ô���
        return -1;
    }
    
    double yongjin = tradeAmt * ratio > lowprice ? tradeAmt * ratio : lowprice;
    double jiaoyifei = tradeAmt * 0.005 * 0.01;
    double yinhuashui = ceil(tradeAmt * 0.1 * 0.01);
    double jiaoyizhengfei = tradeAmt * 0.0027 * 0.01;
    double zhongyangjiesuanfei = (tradeAmt*0.002 *0.01) > 10 ? (tradeAmt*0.002 *0.01):10;
    
    //�������뵽��λС��
    jiaoyifei = (int)((jiaoyifei + 0.005) * 100)/100.0;
    jiaoyizhengfei = (int)((jiaoyizhengfei + 0.005) * 100)/100.0;
    tax = yongjin + jiaoyifei + yinhuashui + jiaoyizhengfei + zhongyangjiesuanfei;

    return 1;
}

/*����a�������� 
* param[in] stockid ���id��tradeNum ���׹����� tradeAmt ������; type �������ͣ�0 ��1 ��
* param[out] tax, ���Σ���Ӷ��
* return: <0, ����Ӷ��ʧ��
*/
int getATradingFee(std::string stockid, int tradeNum, double tradeAmt, int type, double& tax){
    
    string yongjinStr = Runtime::getInstance()->configObj->find("A_YONGJIN");
    double lowprice=0, ratio=0;
    sscanf(yongjinStr.c_str(), "%lf:%lf", &lowprice, &ratio);

    if(ratio < -0.0005){ //С��0�������ô���
        return -1;
    }

    double yongjinFee = tradeAmt * ratio > lowprice ? tradeAmt * ratio : lowprice;
    
    double yinghuaFee = 0;
    if ( 1==type ) {
        string yinghuaStr = Runtime::getInstance()->configObj->find("A_YINGHUA");
        double yinghuaRatio = 0;
        sscanf(yinghuaStr.c_str(), "%lf", &yinghuaRatio);
        yinghuaFee = tradeAmt * yinghuaRatio;
    }

    double guohuFee = 0;
    if (stockid.substr(0,2) == "sh" || stockid.substr(0,2) == "SH") {
        string guohuStr = Runtime::getInstance()->configObj->find("A_GUOHU");
        double guohuRatio = 0;
        sscanf(guohuStr.c_str(), "%lf", &guohuRatio);

        guohuFee = ceil(tradeNum/guohuRatio);
    }

    tax = yongjinFee + yinghuaFee + guohuFee;

    return 1;
}


/*��ȡ��Nҳ������
*@vSrc������Դ������gridStrTab��grid��������Դ��numInOnePage��һҳ�е�������pageNum����䵽grid����ҳ��
*/
void setPageFromVector2GridTable(vector<LogDataDet>& vSrc, wxGridStringTable& gridStrTab, int numInOnePage, int pageNum)
{	
    vector<LogDataDet> tmpSrc;
    for(int i=0; i<vSrc.size(); ++i) { //���˵����ǵ�ǰ��ϵ���Ϣ
        if(vSrc[i].composeid == Runtime::getInstance()->CurComposeID){
            tmpSrc.push_back(vSrc[i]);
        }
    }

	gridStrTab.Clear();
    if (tmpSrc.size() == 0 && gridStrTab.GetNumberRows()<=1) { //�Է���־��¼Ϊ��ʱ��û�����ݵ��½��汻Ť�������
        gridStrTab.AppendRows(8);
        return;
    }

	if( tmpSrc.size() < numInOnePage * (pageNum - 1) ){ //��¼��С����һҳ����������ֱ�ӷ���
		return;
	}

	//����gridStringTable�е�����ΪnumInOnePage
	if(gridStrTab.GetNumberRows() > getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum)){
		gridStrTab.DeleteRows(0, gridStrTab.GetNumberRows() - getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum));

	}else{
		gridStrTab.AppendRows(getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum) - gridStrTab.GetNumberRows());
	}

	vector<LogDataDet>::iterator iter = tmpSrc.begin();
	iter += numInOnePage * (pageNum - 1);// ��ת��pageNumҳ�ļ�¼
	for(int i = 0; i < getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum) && iter != tmpSrc.end(); ++i, ++iter){
		gridStrTab.SetValue(i,0,iter->stock_name);
		gridStrTab.SetValue(i,1,iter->stock_id);
		gridStrTab.SetValue(i,2,iter->buy_time);
		gridStrTab.SetValue(i,3,wxString::Format("%g", iter->buy_price) );
		gridStrTab.SetValue(i,4,wxString::Format("%d", iter->buy_num));
		gridStrTab.SetValue(i,5,wxString::Format("%g", iter->buy_allAmt));
		gridStrTab.SetValue(i,6,iter->buy_reason);
		gridStrTab.SetValue(i,7,iter->sell_time);
		gridStrTab.SetValue(i,8,wxString::Format("%g", iter->sell_price));
		gridStrTab.SetValue(i,9,wxString::Format("%d", iter->sell_num));
		gridStrTab.SetValue(i,10,wxString::Format("%g", iter->sell_allAmt));
		gridStrTab.SetValue(i,11,iter->sell_reason);
		gridStrTab.SetValue(i,12,wxString::Format("%g", iter->trade_sumamt));
		gridStrTab.SetValue(i,13,iter->trade_review);
	}
}


/*ȥ��string���ߵĿո񡢻��з�*/
string& stringTrim(string& src) {
    
    int begIndex = 0;
    while( begIndex < src.length() ) {
        if( src[begIndex] == ' ' || src[begIndex] == '\n' || src[begIndex] == 10 ){
            cout<<"match!"<<endl;
            ++begIndex;

        }else{
            cout<<"not match"<<endl;
            break;
        }        

    }
    src.erase(0, begIndex);

    int endIndex = src.length() - 1;
    while( endIndex >= 0 ) {
        if( src[endIndex] == ' ' || src[endIndex] == '\n' || src[endIndex] == 10 ){
            cout<<"match!"<<endl;
            --endIndex;

        }else{
            cout<<"not match"<<endl;
            break;
        }        

    }
    src.erase(endIndex + 1);

    return src;
}

/*���ݶ̵�stockid����������url�����code
* return url�����code
*/
std::string genUrlRequestCode(std::string shortCode) {
    trim(shortCode);

    std::string res = shortCode;
    if (shortCode.size()<=5){
        res = "hk" + shortCode;

    }else if(shortCode.size() == 6) {

        if( shortCode.substr(0,1) == "6" ){
            res = "sh" + shortCode;

        }else {
            res = "sz" + shortCode;
        }
    }

    return res;
}

//genUrlRequestCode �����ķ�����
std::string reverseUrlCode(std::string urlCode) {
    
    if (urlCode.find("hk",0)!=string::npos || urlCode.find("sz0",0)!=string::npos || urlCode.find("sh6",0)!=string::npos){
        return urlCode.substr(2,-1);
    }

    return urlCode;
}

//������ݿ����Ƿ��Ѿ��е�������
//���recordtime:ʱ�䣻return��true �������ݣ�false δ������
bool QryUniqueRptData(string querySql, string stock_id, string recordtime){
    Runtime::getInstance()->sqlite.setSql(querySql);

    Runtime::getInstance()->sqlite.prepare();
    Runtime::getInstance()->sqlite.bindString(1, stock_id.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindString(2, recordtime.c_str(), -1, SQLITE_STATIC);

    bool ret = false;
    if ( 1 == Runtime::getInstance()->sqlite.step() ) {
        ret = true;
    }

    //Runtime::getInstance()->sqlite.finalize();

    return ret;
}


int getAllStockIdName(vector<string>& vecStockIdName){

    Runtime::getInstance()->sqlite.setSql(qryStockIdName);
    if( Runtime::getInstance()->sqlite.prepare() < 0 ){
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    while ( 1 == Runtime::getInstance()->sqlite.step() )
    {
        string strIdName = Runtime::getInstance()->sqlite.getColumnString(0);
        trim(strIdName);
        strIdName += "_";
        strIdName += Runtime::getInstance()->sqlite.getColumnString(1);
        vecStockIdName.push_back(strIdName);
    }

    return 1;
}

int getAllStockId(vector<string>& vecStockId){

	Runtime::getInstance()->sqlite.setSql(qryStockId);
	if( Runtime::getInstance()->sqlite.prepare() < 0 ){
		wxMessageBox(Runtime::getInstance()->sqlite.errString);
		return -1;
	}

	while ( 1 == Runtime::getInstance()->sqlite.step() )
	{
		string strId = Runtime::getInstance()->sqlite.getColumnString(0);
		vecStockId.push_back(trim(strId));
	}

	return 1;
}

//����stock_id ��ȡ���¼۸�
double getPriceByStockId(string& stockId){

	Runtime::getInstance()->sqlite.setSql(qryPriceByStockid);
	if( Runtime::getInstance()->sqlite.prepare() < 0 ){
		wxMessageBox(Runtime::getInstance()->sqlite.errString);
		return -1;
	}
	Runtime::getInstance()->sqlite.bindString(1, stockId.c_str(), -1, SQLITE_STATIC);

	double retPrice = 0.0;
	if ( 1 == Runtime::getInstance()->sqlite.step() )
	{
		retPrice = Runtime::getInstance()->sqlite.getColumnDouble(0);
	}

	return retPrice;
}

//����stock_id ��ȡ����
string getNameByStockId(string& stockId){

	Runtime::getInstance()->sqlite.setSql(qryNameByStockid);
	if( Runtime::getInstance()->sqlite.prepare() < 0 ){
		wxMessageBox(Runtime::getInstance()->sqlite.errString);
		return "";
	}
	Runtime::getInstance()->sqlite.bindString(1, stockId.c_str(), -1, SQLITE_STATIC);

	string stockName;
	if ( 1 == Runtime::getInstance()->sqlite.step() )
	{
		stockName = Runtime::getInstance()->sqlite.getColumnString(0);
	}

	return stockName;
}


void qryCashAndShare(int composeId, double& cashVaule, double& debet, double& curShare)
{
    //��ѯ�ֽ�
    Runtime::getInstance()->sqlite.setSql(qryCashSql);
    Runtime::getInstance()->sqlite.prepare();
    Runtime::getInstance()->sqlite.bindInt(1, composeId);

    if ( 1 == Runtime::getInstance()->sqlite.step() ) {
        cashVaule = Runtime::getInstance()->sqlite.getColumnDouble(0);
		debet = Runtime::getInstance()->sqlite.getColumnDouble(1);
        curShare = Runtime::getInstance()->sqlite.getColumnDouble(2);
    }/*else{
        wxMessageBox("��ȡ��ǰ�ֽ�ֵʧ�ܣ�"+ Runtime::getInstance()->sqlite.errString);
    }*/
}


void InsertCashRecord(int composeId, int operType, double changeCash, double afterCash, double debet, double share, string &reasonStr )
{
    //�����ֽ�ֵ
    Runtime::getInstance()->sqlite.setSql(insertCashSql);
    Runtime::getInstance()->sqlite.prepare();
    Runtime::getInstance()->sqlite.bindInt(1, composeId);
	Runtime::getInstance()->sqlite.bindInt(2, operType);
	Runtime::getInstance()->sqlite.bindDouble(3, changeCash);
    Runtime::getInstance()->sqlite.bindDouble(4, afterCash);
	Runtime::getInstance()->sqlite.bindDouble(5, debet);
    Runtime::getInstance()->sqlite.bindDouble(6, share);
    Runtime::getInstance()->sqlite.bindString(7, reasonStr.c_str(), -1, SQLITE_STATIC);

    if(Runtime::getInstance()->sqlite.step() < 0){
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
		Runtime::getInstance()->sqlite.finalize();
        return;
    }
}

void qryValueInfo(int composeId, string& datetime,string& value_advice,string& detailInfo,double& fundShare,double& fundValue, double& marketvalue,double& cash)
{
    Runtime::getInstance()->sqlite.setSql(m_qryValueSql);
    if( Runtime::getInstance()->sqlite.prepare() < 0 ){
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return;
    }
    Runtime::getInstance()->sqlite.bindInt(1, composeId);

    if ( 1 == Runtime::getInstance()->sqlite.step() )
    {
        datetime = Runtime::getInstance()->sqlite.getColumnString(0);
        value_advice = Runtime::getInstance()->sqlite.getColumnString(1);
        detailInfo = Runtime::getInstance()->sqlite.getColumnString(2);
        fundShare = Runtime::getInstance()->sqlite.getColumnDouble(3);
        fundValue = Runtime::getInstance()->sqlite.getColumnDouble(4);
        marketvalue = Runtime::getInstance()->sqlite.getColumnDouble(5);
        cash = Runtime::getInstance()->sqlite.getColumnDouble(6);
    }

    return;
}

class CRTDataNoProxy;
class CRTDataProxy;
void createRtDataObj(CRTData** destObj) 
{
    string proxyAddr = Runtime::getInstance()->configObj->find("ProxyAddr");
    string proxyPort = Runtime::getInstance()->configObj->find("ProxyPort");
    string userName = Runtime::getInstance()->configObj->find("ProxyUser");
    string passWord = Runtime::getInstance()->configObj->find("ProxyPwd");

    if(proxyAddr == ""){ //���ô�������
        *destObj = new CRTDataNoProxy(80);

    }else{
        char ipaddr[20] = {0};
        Domain2Ip(proxyAddr.c_str(), ipaddr);
        *destObj = new CRTDataProxy(ipaddr, atoi(proxyPort.c_str()), userName.c_str(), passWord.c_str());
    }
}

//��ȡ��ֵ
double getMarketValue(int composeId) {
    double marketValue = 0;
    map<string, GoodsDet>::iterator iter = Runtime::getInstance()->Compose2CurStock[composeId].begin();

    for (; iter != Runtime::getInstance()->Compose2CurStock[composeId].end(); ++iter) {
        marketValue += iter->second.num * iter->second.price;
    }

    return marketValue;
}
