#ifndef SELLDIALOG_H
#define SELLDIALOG_H

#include "common.h"
#include <wx/datectrl.h>
#include "Runtime.h"
#include "CDBSqlite.h"
using namespace std;

extern wxGridStringTable *dataSourceLog;

enum
{
	ID_SELLCTRL_OK = 500, //SellDialogȷ����ť��ʶ
	ID_SELLCTRL_CANCEL = 501,
	ID_SELLNANME_CTRL = 502,
	ID_SELLCODE_CTRL = 503,
	ID_SELLPRICE_CTRL = 504,
	ID_SELLNUM_CTRL = 505,
	ID_SELLTIME_CTRL = 506,
	ID_SELLREASON_CTRL = 507,
    ID_SELLYONGJIN_CTRL,
    ID_SELLKONG_CHOICE //�Ƿ�����ѡ���
};

//���� SellDialog ����صĶ���
class SellDialog: public wxDialog
{
public:
	//���캯��
	SellDialog(wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

	//����һ��������¼�����ݿ�
	void insertSellLog(LogDataDet& insertData);

	//�������м�¼��������Ϣ
	void updateSellLog(LogDataDet& upLogData);

	//��ȡһ����¼���Գɹ�����������
	int getSucessSellNum(int toSell, int canSell);

    //����Ƿ񹻿�������return: false: û���㹻�Ŀ�������true У��ɹ� 
    bool checkSellCondition(int sellNum, vector<LogDataDet>& vAllCanSellLogData, int& allCanSellNum);

    wxDatePickerCtrl *sellTimeCtrl; //ʱ��ؼ�

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnGetCurPrice(wxCommandEvent& event);//����Ĵ����䶯ʱ����ȡ�˴���ĵ�ǰ�۸�
    int updateCash(double cash); //������ʱ�ֽ�ݶ������Ϊ��������Ϊ��
	
	DECLARE_EVENT_TABLE();

private:
    wxTextCtrl *sellNameCtrl;
    wxTextCtrl *sellCodeCtrl;
    wxTextCtrl *sellPriceCtrl;
    wxTextCtrl *sellNumCtrl;
    wxTextCtrl *sellReasonCtrl;
    wxTextCtrl *YongJinCtrl;
    wxChoice *sellkongChoice;
};


#endif