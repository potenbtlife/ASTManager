#include "common.h"
#include "SellDialog.h"
#include "main.h"

//sqlite���ݿ��������
#define gSqlite Runtime::getInstance()->sqlite

//ע���������ڵ��¼�������
BEGIN_EVENT_TABLE(SellDialog, wxDialog)    
	EVT_BUTTON(ID_SELLCTRL_OK,   SellDialog::OnOK)
	EVT_BUTTON(ID_SELLCTRL_CANCEL,  SellDialog::OnCancel)
END_EVENT_TABLE()

//�������ڹ��캯��
SellDialog::SellDialog(wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxDialog(parent,id,title,pos,size,style,name)
{
	wxStaticText *sellNameText = new wxStaticText(this, -1, "��Ʊ����:",wxPoint(20, 10),wxSize(60, 20));
	sellNameCtrl = new wxTextCtrl(this,ID_SELLNANME_CTRL, "",wxPoint(90, 10),wxSize(80, 20));
	wxStaticText *sellCodeText = new wxStaticText(this, -1, "����:",wxPoint(200, 10),wxSize(30, 20));
	sellCodeCtrl = new wxTextCtrl(this, ID_SELLCODE_CTRL, "000",wxPoint(240, 10),wxSize(80, 20));

	wxStaticText *sellPriceText = new wxStaticText(this, -1, "�����۸�:",wxPoint(20, 35),wxSize(60, 20));
	sellPriceCtrl = new wxTextCtrl(this,ID_SELLPRICE_CTRL, "0.0",wxPoint(90, 35),wxSize(80, 20));
	wxStaticText *sellNumText = new wxStaticText(this, -1, "����:",wxPoint(200, 35),wxSize(30, 20));
	sellNumCtrl = new wxTextCtrl(this, ID_SELLNUM_CTRL, "100",wxPoint(240, 35),wxSize(80, 20));

	wxStaticText *sellTimeText = new wxStaticText(this, -1, "����ʱ��:",wxPoint(20, 60),wxSize(60, 20));
	sellTimeCtrl = new wxDatePickerCtrl(this,ID_SELLTIME_CTRL,wxDefaultDateTime,
		wxPoint(90, 60),wxSize(80, 20), wxDP_DROPDOWN,wxDefaultValidator,"timectrl");

    new wxStaticText(this, -1, "Ӷ��:",wxPoint(200, 60),wxSize(60, 20));
    YongJinCtrl = new wxTextCtrl(this, ID_SELLYONGJIN_CTRL, "32.6",wxPoint(240, 60),wxSize(80, 20));

	wxStaticText *sellReasonText = new wxStaticText(this, -1, "����ԭ��:",wxPoint(20, 85),wxSize(60, 20));
	sellReasonCtrl = new wxTextCtrl(this, ID_SELLREASON_CTRL, "",wxPoint(90, 85),wxSize(230, 50));
	
    wxStaticText *sellKongText = new wxStaticText(this, -1, "�Ƿ�����:",wxPoint(20, 140),wxSize(60, 20));
    wxString strTest[2] = {"��","��"};
    sellkongChoice = new wxChoice(this, ID_SELLKONG_CHOICE, wxPoint(90, 140),wxSize(80, 20), 2, strTest);
    sellkongChoice->SetSelection(0);

	wxButton* okButton = new wxButton(this, ID_SELLCTRL_OK,"ȷ��", wxPoint(120,170), wxSize(60,30));
	wxButton* cancelButton = new wxButton(this, ID_SELLCTRL_CANCEL,"ȡ��", wxPoint(210,170), wxSize(60,30));

}

void SellDialog::OnOK(wxCommandEvent& event)
{
	LogDataDet sellLogData;//������������Ϣ
	sellLogData.stock_name = sellNameCtrl->GetValue();
    stringTrim(sellLogData.stock_name);

	sellLogData.stock_id = sellCodeCtrl->GetValue();
    stringTrim(sellLogData.stock_id);

	sellLogData.sell_price = strtod(sellPriceCtrl->GetValue(),NULL);
	sellLogData.sell_num = atoi(sellNumCtrl->GetValue().c_str());
	sellLogData.sell_time = sellTimeCtrl->GetValue().Format("%Y-%m-%d");

	sellLogData.sell_reason = sellReasonCtrl->GetValue();
	if("" == sellLogData.stock_name || "" == sellLogData.stock_id || (sellLogData.sell_price < 0.05 && sellLogData.sell_price > 0.05)
		|| 0 == sellLogData.sell_num || "" == sellLogData.sell_time || "" == sellLogData.sell_reason)
	{
		wxMessageBox( "����ѡ�����Ϊ�գ����������룡","��ֵ����", wxOK | wxICON_ERROR );
		return;
	}

	//gSqlite.open();
	gSqlite.setSql(gFindCanSell); //����stockname�����ݿ�ȥ�����е����ݣ��Ƿ񹻿������

	if(gSqlite.prepare() < 0){
		wxMessageBox(gSqlite.errString);
		return;
	}

	gSqlite.bindString(1, sellLogData.stock_id.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindInt(2, Runtime::getInstance()->CurComposeID);

	vector<LogDataDet> vAllCanSellLogData; //��������������������־��¼
	while( 1== gSqlite.step() ){
		LogDataDet tmpData;
		tmpData.keyid = gSqlite.getColumnInt(0);
		tmpData.buy_num = gSqlite.getColumnInt(1);
		tmpData.sell_num = gSqlite.getColumnInt(2);
		tmpData.sell_price = gSqlite.getColumnDouble(5);
		tmpData.sell_allAmt = gSqlite.getColumnInt(3);
		tmpData.buy_allAmt = gSqlite.getColumnDouble(4);

		vAllCanSellLogData.push_back(tmpData);
	};
	//gSqlite.finalize();

    int allCanSellNum = 0;
	if(!checkSellCondition(sellLogData.sell_num, vAllCanSellLogData, allCanSellNum)) {
        char cAllCanSellNum[16];
        itoa(allCanSellNum,cAllCanSellNum,10);
		wxMessageBox("��Ĵ���["+sellLogData.stock_id+"] ���["+cAllCanSellNum+"]��û���㹻���ɹ�������");
		return;
	}

    int sucessSellNumAll = 0;
    double yongjin = 0;
    if(sellkongChoice->GetSelection() ==0 ){ //���������
    
	    vector<LogDataDet>::iterator iter = vAllCanSellLogData.begin();
	    while(sellLogData.sell_num > 0 && iter != vAllCanSellLogData.end()){ //ѭ���ۼ�������־���
		    LogDataDet upLogData(*iter);
    		
		    int sucessSellNum = getSucessSellNum(sellLogData.sell_num, iter->buy_num - iter->sell_num);
		    upLogData.sell_num = iter->sell_num + sucessSellNum;
            
            upLogData.sell_allAmt = iter->sell_allAmt + sellLogData.sell_price * sucessSellNum;

            if (sellLogData.sell_num == sucessSellNum) { //���һ�����µļ�¼�����Ͻ��׷���
                //����Ӷ��
                if( getTradingFee(sellLogData.stock_id, sellLogData.sell_num, sellLogData.sell_price * sellLogData.sell_num, 1, yongjin) < 0 ){
                    wxMessageBox("����Ӷ��ʧ�ܣ�����Ӷ�����ã�");
                }

                upLogData.sell_allAmt = iter->sell_allAmt + sellLogData.sell_price * sucessSellNum - yongjin;
            }

		    upLogData.trade_sumamt = upLogData.sell_allAmt - iter->buy_allAmt;
		    upLogData.sell_price = (upLogData.sell_allAmt + yongjin)/upLogData.sell_num;
		    upLogData.sell_time = sellLogData.sell_time;
		    upLogData.sell_reason = sellLogData.sell_reason;

		    //������־��¼
		    updateSellLog(upLogData);

		    //�������µ���
		    sellLogData.sell_num -= sucessSellNum;
		    ++iter;

            sucessSellNumAll += sucessSellNum;
	    }

    }else{ //������������һ��������¼
        sellLogData.type = "1";
        sucessSellNumAll = sellLogData.sell_num;

        //����Ӷ��
        if( getTradingFee(sellLogData.stock_id, sellLogData.sell_num, sellLogData.sell_price * sellLogData.sell_num, 1, yongjin) < 0 ){
            wxMessageBox("����Ӷ��ʧ�ܣ�����Ӷ�����ã�");
        }

        insertSellLog(sellLogData);
    }
	
    char cyongjin[16] = {0};
    sprintf(cyongjin, "%.2f", yongjin);
    YongJinCtrl->SetValue(cyongjin);

    //�����ֽ�ֵ
    double changeCash = sellLogData.sell_price * sucessSellNumAll - yongjin;
    
    updateCash(changeCash); //������ʱ�ֽ�ݶ��,����Ϊ��������Ϊ��

    //TODO��delete, and check total can sell
	/*if(sellLogData.sell_num > 0){ //ȫ������־��涼�ù��ˣ���������������һ����¼
		//����һ��������¼
		insertSellLog(sellLogData);
	}*/

	//���¼�����־������
	//ReLoadLogData(Runtime::getInstance()->vLogDataDet, dataSourceLog,gSqlite);

	//������ʾҳ
	//setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);
    //this->GetParent()->Refresh();

    //Runtime::getInstance()->RefreshCurrentGoods();
    //Runtime::getInstance()->RefreshRecentBuyRecord();
    //Runtime::getInstance()->SetRtCode();//����ʵʱ����ˢ�µĴ���

    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshLogInfo();
    myFrame->RefreshGoodsAndValue();

	Destroy();
	//Close(false);
}

void SellDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

//����һ��������¼�����ݿ�
void SellDialog::insertSellLog(LogDataDet& insertData){

	gSqlite.setSql(gInsertLogSql);
	gSqlite.prepare();
    gSqlite.bindInt(1, Runtime::getInstance()->CurComposeID);
	gSqlite.bindString(2, insertData.stock_name.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(3, insertData.stock_id.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(4, "", -1, SQLITE_STATIC);
	gSqlite.bindString(5, "", -1, SQLITE_STATIC);
	gSqlite.bindString(6, "", -1, SQLITE_STATIC);
	gSqlite.bindString(7, "", -1, SQLITE_STATIC);
	gSqlite.bindString(8, "", -1, SQLITE_STATIC);
	gSqlite.bindString(9, insertData.sell_time.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(10, insertData.sell_price);
	gSqlite.bindInt(11, insertData.sell_num);
	gSqlite.bindDouble(12, insertData.sell_price * insertData.sell_num);
	gSqlite.bindString(13, insertData.sell_reason.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(14, "", -1, SQLITE_STATIC);
	gSqlite.bindString(15, "", -1, SQLITE_STATIC);
    gSqlite.bindString(16, insertData.type.c_str(), -1, SQLITE_STATIC);

	if(gSqlite.step() < 0 && gSqlite.finalize()){
		wxMessageBox(gSqlite.errString);
		return;
	}

	gSqlite.finalize();
}

//�������м�¼��������Ϣ
void SellDialog::updateSellLog(LogDataDet& upLogData){

	gSqlite.setSql(gSellUpdate);
	gSqlite.prepare();
	gSqlite.bindString(1, upLogData.sell_time.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(2, upLogData.sell_price);
	gSqlite.bindInt(3, upLogData.sell_num);
	gSqlite.bindDouble(4, upLogData.sell_allAmt);
	gSqlite.bindString(5, upLogData.sell_reason.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(6, upLogData.trade_sumamt);
	gSqlite.bindInt(7, upLogData.keyid);

	if(gSqlite.step() < 0 && gSqlite.finalize()){
		wxMessageBox(gSqlite.errString);
		return;
	};
	gSqlite.finalize();
}

//��ȡһ����¼���Գɹ�����������
int SellDialog::getSucessSellNum(int toSell, int canSell){
	if(toSell <= canSell){//������ɵ����˴�����
		return toSell;
	}else{
		return canSell; //�˴���������Ϊiter->buy_num - iter->sell_num
	}
}

//����Ƿ񹻿�������return: false: û���㹻�Ŀ�������true У��ɹ� 
bool SellDialog::checkSellCondition(int sellNum, vector<LogDataDet>& vAllCanSellLogData, int& allCanSellNum){
    allCanSellNum=0;
    for(int i=0; i<vAllCanSellLogData.size(); ++i) {
        allCanSellNum += (vAllCanSellLogData[i].buy_num - vAllCanSellLogData[i].sell_num);
    }

    if( allCanSellNum < sellNum ) { //û���㹻�Ŀ�����,����false
        return false;
    } else {
        return true;
    }
}

//������ʱ�ֽ�ݶ��,����Ϊ��������Ϊ��
int SellDialog::updateCash(double changeCash){

    double curCash=0, curShare=0;
    qryCashAndShare(Runtime::getInstance()->CurComposeID, curCash, curShare);

    double allCash = curCash + changeCash;
    string stockName = sellNameCtrl->GetValue();
    string reasonStr = "����:" + trim(stockName);

    InsertCashRecord(Runtime::getInstance()->CurComposeID, allCash, curShare, reasonStr);


    //���½�����ʾ
    wxTextCtrl* cashCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(1008);
    char cAllCash[16] = {0};
    sprintf(cAllCash, "%.2f", allCash);
    cashCtrl->SetValue(cAllCash);

    return 1;
}
