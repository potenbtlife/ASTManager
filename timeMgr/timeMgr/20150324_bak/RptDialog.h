#ifndef INCLUDED_RPTDIALOG_H
#define INCLUDED_RPTDIALOG_H

//�Բ�֧��Ԥ���������Ҫ����"wx/wx.h"
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
       #include "wx/wx.h"
#endif

#include "common.h"
#include <map>

enum{
	ID_REPORT_GRID,
	ID_PREVIOUS_BUTTON,
	ID_NEXTPAGE_BUTTON
};

class RptDialog: public wxDialog{
public:
	RptDialog(wxFrame *parent, wxWindowID id, const wxString &title, wxPoint& point, wxSize& size);
    ~RptDialog();

    DECLARE_EVENT_TABLE();

private:
    void OnNextPage(wxCommandEvent& event);
    void OnPrePage(wxCommandEvent& event);

    //����ÿ�����޵�ռ��ʱ�����
    void RptDialog::calcRatioOfEachPriority(vector<DataContainer>& vecAllData, wxGridStringTable& statisticDS, int numInOnePage, int pageNum);

    wxGrid *rptGrid; //������
    wxGridStringTable *dataSource; //����������Դ
    wxGrid *statisticGrid; //ͳ����Ϣ���
    wxGridStringTable *statisticDS; //ͳ����Ϣ����Դ
};

#endif