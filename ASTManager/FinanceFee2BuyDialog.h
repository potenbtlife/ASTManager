#ifndef FINANCEFEE2BUYDIALOG_H
#define FINANCEFEE2BUYDIALOG_H

#include "common.h"
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

enum
{
	ID_FEE2BUE_OK = 500, //ȷ����ť��ʶ
	ID_FEE2BUE_CANCEL,
	ID_DAYS_OFMONTH_CTRL
};

//���� BuyDialog ����صĶ���
class FinanceFee2BuyDialog: public wxDialog
{
public:
    //���캯��
    FinanceFee2BuyDialog(wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr);

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
	void OnCalcFee(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();

private:

	wxTextCtrl* _curDebetCtrl;
    wxTextCtrl* _weekRateCtrl; 
	wxTextCtrl* _daysOfMonthCtrl;
	wxTextCtrl* _feeCtrl;

	double _curCash, _debet, _curShare;
	double _rateDay; //��ծ������
};


#endif //FINANCEFEE2BUYDIALOG_H