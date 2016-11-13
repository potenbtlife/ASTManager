#ifndef BUYDIALOG_H
#define BUYDIALOG_H

#include "common.h"
#include <wx/datectrl.h>
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

extern wxGridStringTable *dataSourceLog;

enum
{
	ID_BUYCTRL_OK = 400, //BuyDialogȷ����ť��ʶ
	ID_BUYCTRL_CANCEL = 401, //BuyDialogȡ����ť��ʶ
	ID_BUYNANME_CTRL = 402,
	ID_BUYCODE_CTRL = 403,
	ID_BUYPRICE_CTRL = 404,
	ID_BUYNUM_CTRL = 405,
	ID_BUYTIME_CTRL = 406,
	ID_BUYREASON_CTRL = 407,
    ID_YONGJIN_CTRL = 408
};

//���� BuyDialog ����صĶ���
class BuyDialog: public wxDialog
{
public:
	//���캯��
	BuyDialog(wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

    
    wxDatePickerCtrl *buyTimeCtrl; //ʱ��ؼ�

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

    int BuyUpdateCash(double cash); //������ʱ�ֽ�ݶ��
	void OnGetCurPrice(wxCommandEvent& event); //��ȡ��Ĵ���ؼ���id�����ݴ�id�����¼۸�ؼ��ļ۸�

	DECLARE_EVENT_TABLE();

private:
    wxTextCtrl *buyNameCtrl;
    wxTextCtrl *buyCodeCtrl;
    wxTextCtrl *buyPriceCtrl;
    wxTextCtrl *buyNumCtrl;
    wxTextCtrl *buyReasonCtrl;
    wxTextCtrl *YongJinCtrl;
};


#endif //BUYDIALOG_H