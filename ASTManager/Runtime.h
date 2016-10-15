#ifndef ASTMANAGER_RUNTIME_H
#define ASTMANAGER_RUNTIME_H

#include "common.h"
#include "CDBSqlite.h"
#include "tray.h"
#include "CRTData.h"
#include <vector>
#include <map>
#include <string>
#include "Timer.h"
#include "CConfig.h"

using namespace std;


//�����࣬���汾�����һЩ��������
class Runtime
{
public:
    static Runtime* getInstance(){
        if( 0 ==_instance ){
            _instance = new Runtime();
        }
        return _instance;
    }

    CDBSqlite sqlite; //sqlite���ݿ��������
    int curPage; //��ǰչ��ҳ
    int rowNumPage; //һҳչ�ֵ�����
    vector<LogDataDet> vLogDataDet; //������־��ϸ���ݵ���������Ҫʱʱ��֤�����ݿ������һ�£�
    vector<LogDataDet> vSearchResultData; //����������Ľ������
    //map<string, GoodsDet, greater<std::string> > mapGoodsDet; //�����������
    
    map<int, map<string, GoodsDet> > Compose2CurStock; //�ִ����Ϻ�stockid��������Ϣ��Ӧ��ϵ������ÿ��Ǽ�������ϵľ�ֵ
    map<string, LogDataDet> mapRecentBuy; //���������0�ı������������¼

    CRTData* rtData;//ʵʱ���ݶ���
    Timer myTimer; //��ʱ��������ʵʱˢ��ʵʱ����
    MyTray myTray;//����ͼ��
    CConfig* configObj;

    int CurComposeID;

    inline int initSqlite(string dbname) {
        sqlite = CDBSqlite(dbname);
        sqlite.open();

        return 1;
    }

    //����־��¼�л�ȡ��ǰ���������浽mapCurGoods��
    void RefreshCurrentGoods();

    void RefreshRecentBuyRecord();//������������¼��

    //����RT�����е�ʵʱˢ�µĴ���
    inline void SetRtCode(){
        vector<std::string> vTmpCode;

        map<int, map<string, GoodsDet> >::iterator iter=Compose2CurStock.begin();
        for(; iter != Compose2CurStock.end(); ++iter){

            vTmpCode.push_back(qryCompseIndex(iter->first));
            for(map<string, GoodsDet>::iterator iter1 = iter->second.begin(); iter1 != iter->second.end(); ++iter1){
                vTmpCode.push_back(iter1->second.stock_id);
            }
        }

        rtData->setAllCode(vTmpCode);
    }

    //�����ݿ��ȡ��ǰ���id
    int qryCurCompseID();
    std::string qryCurCompseName();
    std::string qryCompseIndex(int composeId);
    std::string qryCompseChild(int composeId);
    int qryAllCompseID(std::vector<int>& vec);

protected:
    Runtime():curPage(1), rowNumPage(10),CurComposeID(1){};

private:
    static Runtime* _instance;

};

#endif //ASTMANAGER_RUNTIME_H