#ifndef ADJUSTCASHDIALOG_H
#define ADJUSTCASHDIALOG_H

#include "common.h"
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

enum
{
    ID_ADJUSTCASHCTRL_OK = 600,
    ID_ADJUSTCASHCTRL_CANCEL,
};

//���� BuyDialog ����صĶ���
class AdjustCashDialog: public wxDialog
{
public:
    //���캯��
    AdjustCashDialog(wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr);

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    int BuyUpdateCash(double cash); //������ʱ�ֽ�ݶ��
    DECLARE_EVENT_TABLE();

private:

    wxTextCtrl* m_nowCashCtrl; //�����ֽ�ֵ�Ŀؼ�
    wxTextCtrl* m_nowShareCtrl; //��������ݶ�Ŀؼ�
	wxTextCtrl* m_debetCtrl; //ծ��չʾ�ؼ�
    wxTextCtrl* m_reasonCtrl;

	double m_curCashVaule, m_curShare, m_debet;
};


#endif //ADJUSTCASHDIALOG_H