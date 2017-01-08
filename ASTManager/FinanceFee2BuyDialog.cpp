#include <sstream>
#include "FinanceFee2BuyDialog.h"
#include "main.h"

//ע�����봰�ڵ��¼�������
BEGIN_EVENT_TABLE(FinanceFee2BuyDialog, wxDialog)
EVT_TEXT(ID_DAYS_OFMONTH_CTRL, OnCalcFee)
EVT_BUTTON(ID_FEE2BUE_OK, FinanceFee2BuyDialog::OnOK)
EVT_BUTTON(ID_FEE2BUE_CANCEL, FinanceFee2BuyDialog::OnCancel)
END_EVENT_TABLE()

//���봰�ڹ��캯��
FinanceFee2BuyDialog::FinanceFee2BuyDialog(wxWindow *parent,
                     wxWindowID id, 
                     const wxString& title,
                     const wxPoint&  pos, 
                     const wxSize& size,
                     long  style,
                     const wxString& name)
                     :wxDialog(parent,id,title,pos,size,style,name)
{
    new wxStaticText(this, -1, "���и�ծ���:",wxPoint(20, 10),wxSize(100, 20));
    _curDebetCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 10),wxSize(100, 20), wxTE_READONLY);
    
	string rateDayStr = Runtime::getInstance()->configObj->find("DebetDayRate");
	new wxStaticText(this, -1, "��ծ������:",wxPoint(20, 35),wxSize(100, 20));
    _weekRateCtrl = new wxTextCtrl(this,wxID_ANY, rateDayStr.c_str(),wxPoint(120, 35),wxSize(100, 20));


	wxDateTime dateNow(wxDateTime::Now());
	int days = wxDateTime::GetNumberOfDays(dateNow.GetMonth(), dateNow.GetYear());

	char cdays[16] = {0};
	sprintf(cdays, "%d", days);
	new wxStaticText(this, -1, "��������:",wxPoint(20, 60),wxSize(100, 20));
	_daysOfMonthCtrl = new wxTextCtrl(this,ID_DAYS_OFMONTH_CTRL, cdays,wxPoint(120, 60),wxSize(100, 20));

    new wxStaticText(this, -1, "���²������:",wxPoint(20, 85),wxSize(100, 20));
    _feeCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 85),wxSize(120, 20));

    wxButton* okButton = new wxButton(this, ID_FEE2BUE_OK,"ȷ��", wxPoint(70,115), wxSize(60,30));
    wxButton* cancelButton = new wxButton(this, ID_FEE2BUE_CANCEL,"ȡ��", wxPoint(160,115), wxSize(60,30));

    qryCashAndShare(Runtime::getInstance()->CurComposeID, _curCash, _debet, _curShare);
    
    char ctmp[128] = {0};
    sprintf(ctmp, "%.2lf", _debet);
    _curDebetCtrl->SetValue(ctmp); //����ݶ�ؼ�
	
	_rateDay = atof(rateDayStr.c_str());

	double allFee = _debet * _rateDay * days;
	memset(ctmp,0,sizeof(ctmp));
	sprintf(ctmp, "%.2lf", allFee);
	_feeCtrl->SetValue(ctmp); //�ܲ�����ÿؼ�

}

void FinanceFee2BuyDialog::OnOK(wxCommandEvent& event)
{
	string strDebetFee = _feeCtrl->GetValue();

    double debetFee=0;
	sscanf(strDebetFee.c_str(), "%lf", &debetFee);

    if( debetFee<0.005 && debetFee>-0.005 ){
        wxMessageBox( "����ѡ���Ϊ�գ����������룡","�������", wxOK | wxICON_ERROR );
        return;
    }

	//�������ӵķݶ�
	double fundShare=0, fundValue=0,marketvalue=0,cash=0; //��ҪΪ�˻�ȡ����ÿ�ݾ�ֵ
	string datetime, value_advice, detailInfo;
	qryValueInfo(Runtime::getInstance()->CurComposeID, datetime, value_advice, detailInfo, fundShare, fundValue,marketvalue,cash);//��ȡvalue_info���ݣ���Ҫ��Ϊ�˻�ȡ��ֵ����

	double addShare = debetFee/fundValue;
	double newShare = _curShare + addShare;

	stringstream stream;
	stream << "ÿ�²������:["<<debetFee <<"], ת�깺[" <<addShare<<"]�ݶ";
	InsertCashRecord(Runtime::getInstance()->CurComposeID, buyFundType, 0,
		_curCash,_debet, newShare, stream.str());    

    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshGoodsAndValue();

    Destroy();
}

void FinanceFee2BuyDialog::OnCancel(wxCommandEvent& event)
{
    Destroy();
    //Close(false);
}

void FinanceFee2BuyDialog::OnCalcFee(wxCommandEvent& event)
{
	string daysStr = _daysOfMonthCtrl->GetValue();
	int days = atoi(daysStr.c_str());
	double allFee = _debet * _rateDay * days;

	char ctmp[64] = {0};
	sprintf(ctmp, "%.2lf", allFee);
	_feeCtrl->SetValue(ctmp); //�ܲ�����ÿؼ�
}

