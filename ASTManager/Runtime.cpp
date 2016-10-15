#include "Runtime.h"

const std::string getCurrCompID="select compose_id from compose_info where iscurrent=1";
const std::string getAllCompID="select compose_id from compose_info order by compose_id";

const std::string getCurrCompName="select compose_name from compose_info where iscurrent=1";

const std::string getCompComparison="select comparison_code from compose_info where compose_id=?";
const std::string getCompChild="select child_compose from compose_info where compose_id=?";

//����־��¼�л�ȡ��ǰ���������浽mapCurGoods��
void Runtime::RefreshCurrentGoods() {
    Compose2CurStock.clear();

    GoodsDet tmpGoodsDet;
    for(int i=0; i<vLogDataDet.size(); ++i) {

        int leftNum = vLogDataDet[i].buy_num - vLogDataDet[i].sell_num;
        string findKey = vLogDataDet[i].type + vLogDataDet[i].stock_id;
        
        if(0 == leftNum) {
            continue;
        }

        map<int, map<string, GoodsDet> >::iterator iter = Compose2CurStock.find(vLogDataDet[i].composeid);
        
        if(iter == Compose2CurStock.end()) { //�Ҳ��������

            tmpGoodsDet.stock_id = vLogDataDet[i].stock_id;
            tmpGoodsDet.stock_name = vLogDataDet[i].type == "1" ? "br_"+vLogDataDet[i].stock_name : vLogDataDet[i].stock_name;
            tmpGoodsDet.num = leftNum;
            tmpGoodsDet.amt = vLogDataDet[i].buy_allAmt - vLogDataDet[i].sell_allAmt;
            tmpGoodsDet.price = tmpGoodsDet.amt/tmpGoodsDet.num;
            tmpGoodsDet.isBear = (vLogDataDet[i].type == "1");
            
            Compose2CurStock[vLogDataDet[i].composeid][findKey] = tmpGoodsDet;

        }else{
            map<string, GoodsDet>::iterator iter1 = iter->second.find(findKey); //�Ҳ�����key
            
            if(iter1 == iter->second.end()){
                tmpGoodsDet.stock_id = vLogDataDet[i].stock_id;
                tmpGoodsDet.stock_name = vLogDataDet[i].type == "1" ? "br_"+vLogDataDet[i].stock_name : vLogDataDet[i].stock_name;
                tmpGoodsDet.num = leftNum;
                tmpGoodsDet.amt = vLogDataDet[i].buy_allAmt - vLogDataDet[i].sell_allAmt;
                tmpGoodsDet.price = tmpGoodsDet.amt/tmpGoodsDet.num;
                tmpGoodsDet.isBear = (vLogDataDet[i].type == "1");
                
                iter->second[findKey] = tmpGoodsDet;//insert(pair<string, GoodsDet>(findKey,tmpGoodsDet));

            }else{
                iter1->second.num += leftNum;
                iter1->second.amt += (vLogDataDet[i].buy_allAmt - vLogDataDet[i].sell_allAmt);
                iter1->second.price = iter1->second.amt/(iter1->second.num);
            }
        }
    }

    return;
}


void Runtime::RefreshRecentBuyRecord() {
    
    mapRecentBuy.clear();
    for(int i=0; i<vLogDataDet.size(); ++i) {
        if((vLogDataDet[i].buy_num - vLogDataDet[i].sell_num) ==0 ) {//���û��ʣ��������������������¼
            continue;
        }

        string findKey = vLogDataDet[i].type + vLogDataDet[i].stock_id;

        map<string, LogDataDet>::iterator iterRecentBuy= mapRecentBuy.find(findKey);

        if(iterRecentBuy != mapRecentBuy.end()) { //����ҵ��˼�¼���Ƚ�ʱ�䣬����ʱ������һ����¼
            
            if(vLogDataDet[i].buy_time > iterRecentBuy->second.buy_time) {
                iterRecentBuy->second = vLogDataDet[i];
            }

        } else { //�Ҳ�����¼��ֱ�Ӳ���
            mapRecentBuy.insert(make_pair(findKey, vLogDataDet[i]));
        }

    }

    return;
}


//��ȡ��ǰ���id
int Runtime::qryCurCompseID() {

    sqlite.setSql(getCurrCompID);
    if( sqlite.prepare() < 0 ){
        //wxMessageBox(sqlite.errString);
        return -1;
    }

    if (1 == sqlite.step()) {
        CurComposeID = sqlite.getColumnInt(0);
    }

    return 1;
}

//��ȡ�������id
int Runtime::qryAllCompseID(std::vector<int>& vec) {

    sqlite.setSql(getAllCompID);
    if( sqlite.prepare() < 0 ){
        //wxMessageBox(sqlite.errString);
        return -1;
    }

    while (1 == sqlite.step()) {
        int composeID = sqlite.getColumnInt(0);
        vec.push_back(composeID);
    }

    return 1;
}


//��ȡ��ǰ�������
std::string Runtime::qryCurCompseName() {

    sqlite.setSql(getCurrCompName);
    if( sqlite.prepare() < 0 ){
        //wxMessageBox(sqlite.errString);
        return "��ѯ���ݿ����";
    }
    
    string CurComposeName;
    if (1 == sqlite.step()) {
        CurComposeName = sqlite.getColumnString(0);
    }

    return CurComposeName;
}

//��ȡ��ǰ��϶Աȴ���
std::string Runtime::qryCompseIndex(int composeId) {

    sqlite.setSql(getCompComparison);
    if( sqlite.prepare() < 0 ){
        wxLogError("sqlite.prepare failed! err[%s]",sqlite.errString);
        return "";
    }
    
    sqlite.bindInt(1, composeId);
    string index;
    if (1 == sqlite.step()) {
        index = sqlite.getColumnString(0);
    }

    return index;
}

//��ȡ��ǰ��϶Աȴ���
std::string Runtime::qryCompseChild(int composeId) {

    sqlite.setSql(getCompChild);
    if( sqlite.prepare() < 0 ){
        wxLogError("sqlite.prepare failed! err[%s]",sqlite.errString);
        return "";
    }

    sqlite.bindInt(1, composeId);
    string child;
    if (1 == sqlite.step()) {
        child = sqlite.getColumnString(0);
    }

    return child;
}
