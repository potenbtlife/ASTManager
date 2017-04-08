#include "common.h"
#include "BuyDialog.h"
#include "main.h"

//sqlite���ݿ��������
#define gSqlite Runtime::getInstance()->sqlite


//ע�����봰�ڵ��¼�������
BEGIN_EVENT_TABLE(BuyDialog, wxDialog)    
	EVT_BUTTON(ID_BUYCTRL_OK,   BuyDialog::OnOK)
	EVT_BUTTON(ID_BUYCTRL_CANCEL,  BuyDialog::OnCancel)
	EVT_TEXT(ID_BUYCODE_CTRL, OnGetCurPrice)
	EVT_TEXT(ID_BUYPRICE_CTRL, UpdateYongJin)
	EVT_TEXT(ID_BUYNUM_CTRL, UpdateYongJin)
END_EVENT_TABLE()

//���봰�ڹ��캯��
BuyDialog::BuyDialog(wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxDialog(parent,id,title,pos,size,style,name)
{
	wxStaticText *buyCodeText = new wxStaticText(this, -1, "����:",wxPoint(20, 10),wxSize(30, 20));
	buyCodeCtrl = new wxTextCtrl(this, ID_BUYCODE_CTRL, "00",wxPoint(90, 10),wxSize(80, 20));
	wxStaticText *buyNameText = new wxStaticText(this, -1, "��Ʊ����:",wxPoint(180, 10),wxSize(60, 20));
	buyNameCtrl = new wxTextCtrl(this,ID_BUYNANME_CTRL, "",wxPoint(240, 10),wxSize(80, 20));

	wxStaticText *buyPriceText = new wxStaticText(this, -1, "����۸�:",wxPoint(20, 35),wxSize(60, 20));
	buyPriceCtrl = new wxTextCtrl(this,ID_BUYPRICE_CTRL, "0.0",wxPoint(90, 35),wxSize(80, 20));
	wxStaticText *buyNumText = new wxStaticText(this, -1, "����:",wxPoint(200, 35),wxSize(30, 20));
	buyNumCtrl = new wxTextCtrl(this, ID_BUYNUM_CTRL, "100",wxPoint(240, 35),wxSize(80, 20));

	wxStaticText *buyTimeText = new wxStaticText(this, -1, "����ʱ��:",wxPoint(20, 60),wxSize(60, 20));
	//wxTextCtrl *buyTimeCtrl = new wxTextCtrl(this, ID_BUYTIME_CTRL, "",wxPoint(90, 60),wxSize(80, 20));
	buyTimeCtrl = new wxDatePickerCtrl(this,ID_BUYTIME_CTRL,wxDefaultDateTime,
		wxPoint(90, 60),wxSize(80, 20), wxDP_DROPDOWN,wxDefaultValidator,"timectrl");

    new wxStaticText(this, -1, "Ӷ��:",wxPoint(200, 60),wxSize(60, 20));
    YongJinCtrl = new wxTextCtrl(this, ID_YONGJIN_CTRL, "",wxPoint(240, 60),wxSize(80, 20));

	wxStaticText *buyReasonText = new wxStaticText(this, -1, "����ԭ��:",wxPoint(20, 85),wxSize(60, 20));
	buyReasonCtrl = new wxTextCtrl(this, ID_BUYREASON_CTRL, "",wxPoint(90, 85),wxSize(230, 50));
	
	wxButton* okButton = new wxButton(this, ID_BUYCTRL_OK,"ȷ��", wxPoint(120,150), wxSize(60,30));
	wxButton* cancelButton = new wxButton(this, ID_BUYCTRL_CANCEL,"ȡ��", wxPoint(210,150), wxSize(60,30));

}

void BuyDialog::OnOK(wxCommandEvent& event)//release ģʽ�£����������ȡ�����ؼ���ֵ
{
	string stockName = buyNameCtrl->GetValue();
    stringTrim(stockName);

	string stockCode = buyCodeCtrl->GetValue();
    stringTrim(stockCode);

	string buyPrice = buyPriceCtrl->GetValue();
	string buyNum = buyNumCtrl->GetValue();

    double yongjin=0;
    if( getTradingFee(stockCode, atoi(buyNum.c_str()), atof(buyPrice.c_str()) * atoi(buyNum.c_str()), 0, yongjin) < 0 ){
        wxMessageBox("����Ӷ��ʧ�ܣ�����Ӷ�����ã�");
    }

    double buy_allAmt = atof(buyPrice.c_str()) * atoi(buyNum.c_str()) + yongjin;

    char cyongjin[16] = {0};
    sprintf(cyongjin, "%.2f", yongjin);
    YongJinCtrl->SetValue(cyongjin);

	string buyTime = buyTimeCtrl->GetValue().Format("%Y-%m-%d");

	string buyReason = buyReasonCtrl->GetValue();
	if( stockName =="" || stockCode=="" || buyPrice=="" || buyNum=="" || buyTime=="" || buyReason=="" )
	{
		wxMessageBox( "����ѡ�����Ϊ�գ����������룡","��ֵ����", wxOK | wxICON_ERROR );
		return;
	}

    if(BuyUpdateCash(buy_allAmt) < 0){
        return;
    }

	//gSqlite.open(); //����open�ᵼ�����ݿⱻ��
	gSqlite.setSql(gInsertLogSql);
	gSqlite.prepare();
    gSqlite.bindInt(1, Runtime::getInstance()->CurComposeID);
	gSqlite.bindString(2, stockName.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(3, stockCode.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(4, buyTime.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(5, buyPrice.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(6, buyNum.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(7, buy_allAmt);
	gSqlite.bindString(8, buyReason.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(9, "", -1, SQLITE_STATIC);
	gSqlite.bindString(10, "", -1, SQLITE_STATIC);
	gSqlite.bindString(11, "", -1, SQLITE_STATIC);
	gSqlite.bindString(12, "", -1, SQLITE_STATIC);
	gSqlite.bindString(13, "", -1, SQLITE_STATIC);
	gSqlite.bindString(14, "", -1, SQLITE_STATIC);
	gSqlite.bindString(15, "", -1, SQLITE_STATIC);
    gSqlite.bindString(16, "0", -1, SQLITE_STATIC);

	if(gSqlite.step() < 0){
        wxMessageBox(gSqlite.errString);
		return;
	};

	//ˢ��չ�ֽ����е�����
	//ReLoadLogData(Runtime::getInstance()->vLogDataDet, dataSourceLog,gSqlite);
    gSqlite.finalize();
	
    //������ʾҳ
	//setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);
	//this->GetParent()->Refresh();


    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshLogInfo();
    myFrame->RefreshGoodsAndValue();

	Destroy();
	//Close(false);
}

void BuyDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

//������ʱ�ֽ�ݶ��
int BuyDialog::BuyUpdateCash(double changeCash){

    double curCash=0, debet=0, curShare=0;
    qryCashAndShare(Runtime::getInstance()->CurComposeID, curCash, debet, curShare);
    double allCash = curCash - changeCash;
    if (allCash < -0.005) {
        wxMessageBox("�ֽ𲻹������ȵ����ֽ�");
        return -1;
    }

    string stockName = buyNameCtrl->GetValue();
    string reasonStr = "����:" + trim(stockName);
    InsertCashRecord(Runtime::getInstance()->CurComposeID, buyStockType, -1*changeCash, allCash, debet, curShare, reasonStr);

    //���½�����ʾ
    wxTextCtrl* cashCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(ID_CASH_TEXTCTRL);
    char cAllCash[16] = {0};
    sprintf(cAllCash, "%.2f", allCash);
    cashCtrl->SetValue(cAllCash);

    return 1;
}

//��ȡ��Ĵ���ؼ���id�����ݴ�id�����¼۸�ؼ��ļ۸�
void BuyDialog::OnGetCurPrice(wxCommandEvent& event)
{
	//��ȡ�۸񣬲�����
	string stockId = buyCodeCtrl->GetValue();
	double price = getPriceByStockId(stockId);
	string stockName = getNameByStockId(stockId);

	char strPrice[32];
	sprintf(strPrice,"%.3f", price);
	buyPriceCtrl->SetValue(strPrice);

	buyNameCtrl->SetValue(stockName.c_str());

	//����Ӷ��ؼ�
	UpdateYongJin(event);

}

void BuyDialog::UpdateYongJin(wxCommandEvent& event)
{
	//��ȡӶ�𣬲�������ʾ
	string stockCode = buyCodeCtrl->GetValue();
	stringTrim(stockCode);

	string buyPrice = buyPriceCtrl->GetValue();
	string buyNum = buyNumCtrl->GetValue();

	double yongjin=0;
	if( getTradingFee(stockCode, atoi(buyNum.c_str()), atof(buyPrice.c_str()) * atoi(buyNum.c_str()), 0, yongjin) < 0 ){
		wxMessageBox("����Ӷ��ʧ�ܣ�����Ӷ�����ã�");
	}

	stringstream ss;
	ss<<yongjin;
	YongJinCtrl->SetValue(ss.str().c_str());
}