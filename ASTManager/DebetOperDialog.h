#ifndef DEBETOPERDIALOG_H
#define DEBETOPERDIALOG_H

#include "common.h"
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

//���� BuyDialog ����صĶ���
class DebetOperDialog: public wxDialog
{
public:
    //���캯��
    DebetOperDialog(DEBET_OPER_TYPE opertype, wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr);

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();

private:

	DEBET_OPER_TYPE _type;

    wxTextCtrl* _modifyDebetCtrl; 
    wxTextCtrl* _reasonCtrl; 

	double _curCash, _debet, _curShare;
};


#endif //DEBETOPERDIALOG_H