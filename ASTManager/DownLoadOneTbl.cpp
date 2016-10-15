#include "common.h"
#include "DownLoadOneTbl.h"

//ע�����봰�ڵ��¼�������
BEGIN_EVENT_TABLE(DownLoadOneTblDialog, wxDialog)    
	EVT_BUTTON(ID_DLOT_OK,   DownLoadOneTblDialog::OnOK)
	EVT_BUTTON(ID_DLOT_CANCEL,  DownLoadOneTblDialog::OnCancel)
END_EVENT_TABLE()

//���봰�ڹ��캯��
DownLoadOneTblDialog::DownLoadOneTblDialog(string& stockId,
                    wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxDialog(parent,id,title,pos,size,style,name),_stockId(stockId)
{
	wxStaticText *buyCodeText = new wxStaticText(this, -1, "��Ĵ���:",wxPoint(10, 10),wxSize(60, 20));
	_codeCtrl = new wxTextCtrl(this, wxID_ANY, "0",wxPoint(70, 10),wxSize(80, 20));
	
	wxButton* okButton = new wxButton(this, ID_DLOT_OK,"ȷ��", wxPoint(30,60), wxSize(60,30));
	wxButton* cancelButton = new wxButton(this, ID_DLOT_CANCEL,"ȡ��", wxPoint(120,60), wxSize(60,30));
}

void DownLoadOneTblDialog::OnOK(wxCommandEvent& event)//release ģʽ�£����������ȡ�����ؼ���ֵ
{
    _stockId = _codeCtrl->GetValue();
	Destroy();
	//Close(false);
}

void DownLoadOneTblDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

