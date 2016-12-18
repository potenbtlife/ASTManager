#ifndef OperHistoryDialog_H
#define OperHistoryDialog_H

#include "common.h"
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

struct CASHFLOW{
	int keyid;
	int composeid;
	int operType;
	double changeCash;
	double afterCash;
	double fundShare;
	string changeTime;
	string reason;
};

enum
{
	ID_OPER_HISTORY_ROLLBACK = 1400, //OperHistoryDialog �ع���ť��ʶ
	ID_OPER_HISTORY_CANCEL = 1401, //OperHistoryDialogȡ����ť��ʶ
	ID_OPER_HISTORY_GRID,
	ID_OPER_HISTORY_NEXT,
	ID_OPER_HISTORY_PREVIOUS
};

//���� OperHistoryDialog ����صĶ���
class OperHistoryDialog: public wxFrame
{
public:
	//���캯��
	OperHistoryDialog(wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

	int GetCashFlowData(vector<CASHFLOW>& vecOut); //��ȡ�ֽ�䶯��ļ�¼

	void OnRollback(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();

private:
	wxGrid* _pOperHisgrid; //չ�ֲ�����ʷ�б�grid����

	void setPageFromVector2GridTable(vector<CASHFLOW>& vSrc, wxGridStringTable* gridStrTab,
		int numInOnePage, int pageNum);

	void OnNext(wxCommandEvent& event);
	void OnPrevious(wxCommandEvent& event);

	int curPage;//��ǰҳ��
	int numOnePage;//һҳ������
	vector<CASHFLOW> _vecCashFlowData;
	wxGridStringTable* _pDS;
};


#endif //OperHistoryDialog_H