#include "Timer.h"
#include "Runtime.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/window.h>
#include "main.h"

#include "DownNewsThread.hpp" //�߳���ش���ʵ��

void Timer::Notify() {
    //wxMessageBox("in Timer::Notify");

    wxDateTime datetime(wxDateTime::GetTimeNow());
    int hour = datetime.GetHour();
    int minute = datetime.GetMinute();

    bool isRegInDb = false; //�Ƿ���Ҫ��¼���ݿ�

    wxDateTime::WeekDay todayWeekDay = datetime.GetWeekDay(); //Sat, Sun ��ȡ����

    if (todayWeekDay == 0 || todayWeekDay == 6) { //�������ĩ����ѯ��һ�������Ƿ��ѵǼǣ�δ�ǼǵĵǼǣ��ѵǼǾͷ���

        wxDateSpan dateSpan(0, 0, 0, 1); //������1��

        if (todayWeekDay == 0) {
            dateSpan = wxDateSpan(0, 0, 0, 2); //���ռ�2��
        }

        wxDateTime lastDate = datetime.Subtract(dateSpan);

        if (0 == QryUnique(string(lastDate.Format("%Y-%m-%d").c_str()))) {//֧�����ǰһ��û�еǼǣ��Ǽ�ǰһ�����ݵĹ���
            isRegInDb = true;

        } else {
            return;
        }

    } else if ((hour * 60 + minute) < 9 * 60 + 30) { //�����գ���С��9:30�����ǰһ���ѵǼ�ֱ�ӷ��أ�����Ǽ�

        wxDateSpan dateSpan(0, 0, 0, 1);

        if (todayWeekDay == 1) { //���������һ���ϣ�Ҫ������
            dateSpan = wxDateSpan(0, 0, 0, 3); //���ռ�3��
        }

        wxDateTime lastDate = datetime.Subtract(dateSpan);


        if (0 == QryUnique(string(lastDate.Format("%Y-%m-%d").c_str()))) {
            isRegInDb = true;

        } else {
            return;
        }

    } else if ((hour * 60 + minute) > 12 * 60 && (hour * 60 + minute) < 13 * 60) { //�������12:00ʱ��С��13:00ʱ��ֱ�ӷ���
        return;

    } else if ((hour * 60 + minute) > (17 * 60 + 30)) { //�����գ�ʱ�䳬��17:30����¼���ݿ�

        if (0 == QryUnique(string(datetime.Format("%Y-%m-%d").c_str()))) {
            isRegInDb = true;

        } else {
            return;
        }

    }

    //isRegInDb = true; //for test
	if (!isRegInDb && Runtime::getInstance()->configObj->find("isNotify") != "1") { //����������ݣ����عرգ�ֱ�ӷ���
		return;
	}

    Runtime::getInstance()->rtData->RefreshRtData();

    std::map<std::string, RTInfo>& rtInfo = Runtime::getInstance()->rtData->getData();
    map<string, LogDataDet>& curRecentGoods = Runtime::getInstance()->mapRecentBuy;

    if (0 == rtInfo.size()) { //��ȡ����ʧ�ܣ�����
        Runtime::getInstance()->myTray.ShowBalloon("Faild", "get rt data failed!");
        return;
    }


    if (Runtime::getInstance()->configObj->find("isNotify") == "1") { //����������ã�������

        //�����ʵʱ���������ҵ����Ƚ�ʵʱ�۸�������*1.1��������ھ���ʾ
        for (map<string, LogDataDet>::iterator iter = curRecentGoods.begin(); iter != curRecentGoods.end(); ++iter) {

            string type = iter->first.substr(0, 1); //type�ֶ�
            string findkey = iter->first.substr(1, iter->first.size() - 1); //ȥ��ǰ���type�ֶ�
            std::map<std::string, RTInfo>::iterator iter2 = rtInfo.find(trim(findkey));

            if (iter2 == rtInfo.end()) {
                //Runtime::getInstance()->myTray.ShowBalloon("Faild1","find rt info["+findkey+"] failed!");
                break;
            }

            float curPrice = iter2->second.price;

            if (type == "0") {
                std::string OxRemindLev = Runtime::getInstance()->configObj->find("OxRemindLev");
                double oxlev  = 0;
                sscanf(OxRemindLev.c_str(), "%lf", &oxlev);

                if (curPrice > (iter->second.buy_price * oxlev)) {
                    Runtime::getInstance()->myTray.ShowBalloon("SELL", "CAN SELL " + iter->second.stock_id);
                }

            } else { //����
                std::string BearRemindLev = Runtime::getInstance()->configObj->find("BearRemindLev");
                double bearlev  = 0;
                sscanf(BearRemindLev.c_str(), "%lf", &bearlev);

                if (curPrice < (iter->second.sell_price * bearlev)) {
                    Runtime::getInstance()->myTray.ShowBalloon("BUY", "CAN BUY " + iter->second.stock_id);
                }
            }

        }
    }



    if (isRegInDb) { //�Ǽ����ݿ⣬��ˢ�������ڵĴ�������ֵ��Ϣ

        //����ʵʱ���ݻ�ȡ���е����id
        std::vector<int> allComposeID;

        Runtime::getInstance()->qryAllCompseID(allComposeID);

        for (std::vector<int>::iterator iter = allComposeID.begin(); iter != allComposeID.end(); ++iter) {
            int composeId = *iter;

            //��ȡ��ǰ��ֵ��Ϣ
            double marketValue = 0;
            string detailInfo;
            map<string, GoodsDet>& curGoods = Runtime::getInstance()->Compose2CurStock[composeId];

            for (map<string, GoodsDet>::iterator iter = curGoods.begin(); iter != curGoods.end(); ++iter) {

                string type = iter->first.substr(0, 1); //type�ֶ�
                string findkey = iter->first.substr(1, iter->first.size() - 1); //ȥ��ǰ���type�ֶ�

                std::map<std::string, RTInfo>::iterator iter2 = rtInfo.find(trim(findkey));

                if (iter2 == rtInfo.end()) {
                    Runtime::getInstance()->myTray.ShowBalloon("Faild2", "find rt info failed!");
                    break;
                }

                marketValue += iter->second.num * iter2->second.price;
                char tmpDetail[64] = {0};
                sprintf(tmpDetail, "%s:%d:%.3f;", stringTrim(iter->second.stock_id).c_str(), iter->second.num, iter2->second.price);
                detailInfo += tmpDetail;
            }

            marketValue = (int)(marketValue * 100 + 0.5) / 100; //������λС����������

            //��ȡ����ָ��ֵ
            std::string indexCode = Runtime::getInstance()->qryCompseIndex(composeId);;

            double ref_index = 0;
            std::map<std::string, RTInfo>::iterator rtiter = rtInfo.find(reverseUrlCode(trim(indexCode)));
			
            if (rtiter != rtInfo.end()) {
                ref_index = int(rtiter->second.price * 100) / 100.0;
            }

            //��ȡ���飬���Ϊ��ǰ��ϣ�ȡ����ֵ������ȡ�ϴ�ֵ
            std::string advice;

            if (composeId == Runtime::getInstance()->CurComposeID) {
                wxTextCtrl* adviceTextCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(ID_RATIOADVICE_TEXTCTRL);
                advice = adviceTextCtrl->GetValue();

            } else {
                string tmpTime, detailInfo;
                double fundShare = 0, fundValue = 0, marketvalue = 0, cash = 0;
                qryValueInfo(composeId, tmpTime, advice, detailInfo, fundShare, fundValue, marketvalue, cash);//��ȡvalue_info����
            }

            RegValueInfo(composeId, string(datetime.Format("%Y-%m-%d").c_str()), marketValue, advice, detailInfo, ref_index);

        }

        MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
        myFrame->RefreshGoodsAndValue();

        //��ȡ����
        /*string dateStr = datetime.Format("%Y%m%d");
        string cmd = Runtime::getInstance()->configObj->find("dNewsApp") + " ";
        cmd += Runtime::getInstance()->configObj->m_configFileName + " " + dateStr + " 0 999 dnews.log";
        //string cmd ="cmd.exe";
        if( wxExecute(cmd, wxEXEC_HIDE_CONSOLE) <= 0 ){ //ִ�г���
            wxMessageBox("can not exec %s", cmd);
            return;
        }*/

        string dateStr = datetime.Format("%Y%m%d");
        string cmd = Runtime::getInstance()->configObj->find("dNewsApp") + " ";
        cmd += Runtime::getInstance()->configObj->m_configFileName + " " + dateStr;
        //string cmd ="cmd.exe";
        if( wxExecute(cmd + " 0 499 dnews1.log", wxEXEC_HIDE_CONSOLE) <= 0 ){ //ִ�г���
            wxMessageBox("can not exec %s", cmd);
            return;
        }

        if( wxExecute(cmd + " 500 999 dnews2.log", wxEXEC_HIDE_CONSOLE) <= 0 ){ //ִ�г���
            wxMessageBox("can not exec %s", cmd);
            return;
        }

    }

};

int Timer::RegValueInfo(int composeID, string recordtime, double marketvalue, string advice, string detailInfo, double ref_index) {

    //��ѯ�����ֽ𼰷ݶ���ڼ��㱾�ξ�ֵ
    double fCash = 0, fdebet=0, fund_share = 0;
    qryCashAndShare(composeID, fCash, fdebet, fund_share);


    //��������⴦��
    string childList = Runtime::getInstance()->qryCompseChild(composeID);
    if (childList.find("null") == string::npos && childList != "") {
        vector<string> vecChildCom;
        splitString(childList, vecChildCom, ";",false);
        for (vector<string>::iterator iter = vecChildCom.begin(); iter != vecChildCom.end(); ++iter) {
            
            vector<string> vechuilv;
            splitString(*iter, vechuilv, ":",false);

            int qryComposeId = atoi(vechuilv[0].c_str());
            if(qryComposeId == composeID){ //�Է���������Լ����Լ��������
                continue;
            }

            double fundShare1=0, fundValue1=0, marketvalue1=0, cash1=0;
            string datetime, value_advice, detailInfo;
            qryValueInfo(qryComposeId, datetime, value_advice, detailInfo, fundShare1, fundValue1,marketvalue1,cash1);//��ȡvalue_info����

            double huilv = 1;
            if( vechuilv.size() > 1 ){ //���������ʣ�ȡ���������еڶ�����
                huilv = atof(vechuilv[1].c_str());
            }

            marketvalue = marketvalue + huilv*(marketvalue1 + cash1);
        }
    }

    double all_value = marketvalue + fCash;//���ʲ�
	double net_value = all_value - fdebet;

    if (all_value > -0.05 && all_value < 0.05) {
        return 0;
    }

    double ratio = marketvalue / all_value;
    char stock_ratio[16];
    sprintf(stock_ratio, "%.2f%s", ratio * 100, "%"); //�ֲ�ռ��

    //���㱾�ξ�ֵ
    double fund_value = 0;

    //bool isFirst = !QryAllUnique(composeID);

    if ( fund_share <0.05) { //�������ݶ�С�ڵ���0������cash_flow��ķݶ������ֵ����Ϊ1��
        fund_value = 1;
        fund_share = all_value / fund_value;
        string reasonStr = "��һ�ε�����Ϸݶ�";
        InsertCashRecord(composeID,AdjustBalance, fCash, fCash, 0, fund_share, reasonStr);

    } else {
        fund_value = (int)((net_value) / fund_share * 10000 + 0.5) / 10000.00; //�������ֵ��������λС������������
    }

    //����db
    Runtime::getInstance()->sqlite.setSql(m_insertSql);

    if (Runtime::getInstance()->sqlite.prepare() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindInt(1, composeID);
    Runtime::getInstance()->sqlite.bindString(2, recordtime.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindDouble(3, marketvalue);
    Runtime::getInstance()->sqlite.bindDouble(4, fCash);
    Runtime::getInstance()->sqlite.bindString(5, stock_ratio, -1, SQLITE_STATIC);
	Runtime::getInstance()->sqlite.bindDouble(6, fdebet);
	Runtime::getInstance()->sqlite.bindDouble(7, all_value);
	Runtime::getInstance()->sqlite.bindDouble(8, net_value);
    Runtime::getInstance()->sqlite.bindString(9, stringTrim(advice).c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindString(10, detailInfo.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindDouble(11, fund_share);
    Runtime::getInstance()->sqlite.bindDouble(12, fund_value);
    Runtime::getInstance()->sqlite.bindDouble(13, ref_index);

    if (Runtime::getInstance()->sqlite.step() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        Runtime::getInstance()->sqlite.finalize();
        return -1;
    }

    Runtime::getInstance()->sqlite.finalize();

    return 1;
}

//������ݿ����Ƿ��Ѿ��е�������
//���recordtime:ʱ�䣻1:���ظ���¼��0��û���ظ���¼�� -1 ��ִ�д���
int Timer::QryUnique(string recordtime) {
    Runtime::getInstance()->sqlite.setSql(m_qryUnique);

    Runtime::getInstance()->sqlite.prepare();
    Runtime::getInstance()->sqlite.bindString(1, recordtime.c_str(), -1, SQLITE_STATIC);
    //Runtime::getInstance()->sqlite.bindInt(2, composeID);

    int ret = Runtime::getInstance()->sqlite.step();
    Runtime::getInstance()->sqlite.finalize();

    return ret;
}

//������ݿ����Ƿ����еǼǴ��������
//return��true �У�false δ��
bool Timer::QryAllUnique(int composeID) {
    Runtime::getInstance()->sqlite.setSql(m_qryComposefirst);

    Runtime::getInstance()->sqlite.prepare();
    Runtime::getInstance()->sqlite.bindInt(1, composeID);

    bool ret = false;

    if (1 == Runtime::getInstance()->sqlite.step()) {
        ret = true;
    }

    Runtime::getInstance()->sqlite.finalize();

    return ret;
}

bool Timer::UpdateAllPrice() {

    std::map<std::string, RTInfo> mAllStockInfo;

    int curPos = 0;

    while (1) {
        mAllStockInfo.clear();

        int ret = Runtime::getInstance()->rtData->GetAllStockPrice(mAllStockInfo, curPos);

        if (ret < 0) {
            wxLogError("GetAllStockPrice failed!");
            return false;
        }

        for (std::map<std::string, RTInfo>::iterator iter = mAllStockInfo.begin(); iter != mAllStockInfo.end(); ++iter) {

			if( iter->second.price <0.0001 && iter->second.price > -0.0001 ){ //�����ȡ����Ϊ0�������£���ֹͣ�Ƶ�ԭ���µ�Ϊ0�۸�
				continue;
			}

            Runtime::getInstance()->sqlite.setSql(updatePrice);

            if (Runtime::getInstance()->sqlite.prepare() < 0) {
                wxMessageBox(Runtime::getInstance()->sqlite.errString);
                return false;
            }

            Runtime::getInstance()->sqlite.bindDouble(1, iter->second.price);
            Runtime::getInstance()->sqlite.bindString(2, iter->first.c_str(), -1, SQLITE_STATIC);
            Runtime::getInstance()->sqlite.bindInt(3, 0); //hk��д��Ϊ0

            if (Runtime::getInstance()->sqlite.step() < 0) {
                wxMessageBox(Runtime::getInstance()->sqlite.errString);
                continue;
            }
        }

        if (ret == 0) {
            break;
        }
    }

    Runtime::getInstance()->sqlite.finalize();

    return true;
}