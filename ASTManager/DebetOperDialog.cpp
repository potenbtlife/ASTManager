#include "DebetOperDialog.h"
#include "main.h"

//ע�����봰�ڵ��¼�������
BEGIN_EVENT_TABLE(DebetOperDialog, wxDialog)    
EVT_BUTTON(ID_ADJUSTCASHCTRL_OK, DebetOperDialog::OnOK)
EVT_BUTTON(ID_ADJUSTCASHCTRL_CANCEL, DebetOperDialog::OnCancel)
END_EVENT_TABLE()

//���봰�ڹ��캯��
DebetOperDialog::DebetOperDialog(DEBET_OPER_TYPE opertype, wxWindow *parent,
                     wxWindowID id, 
                     const wxString& title,
                     const wxPoint&  pos, 
                     const wxSize& size,
                     long  style,
                     const wxString& name)
                     :_type(opertype), wxDialog(parent,id,title,pos,size,style,name)
{
    new wxStaticText(this, -1, "���и�ծ���:",wxPoint(20, 10),wxSize(100, 20));
    wxTextCtrl* curDebetCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 10),wxSize(100, 20), wxTE_READONLY);
    
	string str;
	if (_type == ADD) {
		str="�Ӻ�ծ";
	}else{
		str="����ծ";
	}

    wxStaticText *buyPriceText = new wxStaticText(this, -1, str.c_str(),wxPoint(20, 35),wxSize(100, 20));
    _modifyDebetCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 35),wxSize(100, 20));

    new wxStaticText(this, -1, "����ԭ��:",wxPoint(20, 60),wxSize(100, 20));
    _reasonCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 60),wxSize(120, 60), wxTE_MULTILINE);

    wxButton* okButton = new wxButton(this, ID_ADJUSTCASHCTRL_OK,"ȷ��", wxPoint(70,120), wxSize(60,30));
    wxButton* cancelButton = new wxButton(this, ID_ADJUSTCASHCTRL_CANCEL,"ȡ��", wxPoint(160,120), wxSize(60,30));

    qryCashAndShare(Runtime::getInstance()->CurComposeID, _curCash, _debet, _curShare);
    
    char ctmp[128] = {0};
	memset(ctmp,0,sizeof(ctmp));
    sprintf(ctmp, "%.2lf", _debet);
    curDebetCtrl->SetValue(ctmp); //����ݶ�ؼ�
	
}

void DebetOperDialog::OnOK(wxCommandEvent& event)
{
	string strDebet = _modifyDebetCtrl->GetValue();
    string reasonStr = _reasonCtrl->GetValue();

    double modifyDebet=0;
	sscanf(strDebet.c_str(), "%lf", &modifyDebet);

    if( (modifyDebet<0.005 && modifyDebet>-0.005) || "" == reasonStr ){
        wxMessageBox( "����ѡ���Ϊ�գ����������룡","�������", wxOK | wxICON_ERROR );
        return;
    }

	if(_type == ADD){ // �Ӹ�ծ
		
		if (modifyDebet - _debet <=0 ) { //�Ӹܸˣ���ծ���ܼ���
			wxMessageBox( "�Ӹܸˣ���ծ���ܼ��٣����������룡","�������", wxOK | wxICON_ERROR );
			return;
		}
		
		InsertCashRecord(Runtime::getInstance()->CurComposeID, addDebet, (modifyDebet - _debet),
			(_curCash + modifyDebet - _debet),modifyDebet, _curShare, reasonStr);
	
	}else{ //����ծ

		if (_debet - modifyDebet <=0 ) { //���ܸˣ���ծ��������
			wxMessageBox( "�Ӹܸˣ���ծ���ܼ��٣����������룡","�������", wxOK | wxICON_ERROR );
			return;
		}

		InsertCashRecord(Runtime::getInstance()->CurComposeID, subDebet, (modifyDebet - _debet),
			(_curCash + modifyDebet - _debet),modifyDebet, _curShare, reasonStr);
	}
    

    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshGoodsAndValue();

    Destroy();
}

void DebetOperDialog::OnCancel(wxCommandEvent& event)
{
    Destroy();
    //Close(false);
}

