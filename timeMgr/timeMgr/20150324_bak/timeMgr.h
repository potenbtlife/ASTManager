#ifndef INCLUDED_HELLOWORLDAPP_H
#define INCLUDED_HELLOWORLDAPP_H

//�Բ�֧��Ԥ���������Ҫ����"wx/wx.h"
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/timectrl.h"
#include "wx/button.h"
#include <wx/grid.h>
#include "common.h"
#include <string>
#include <vector>
#include <map>
#include "CDBSqlite.h"
#include "RptDialog.h"

using namespace std;

/**
 * TimeMgrApp�� * ������Ǳ������������
 */
class TimeMgrApp : public wxApp {
public:
    virtual bool OnInit();
};

enum {
    ID_RESET_BUTTON = 1000,
    ID_END_BUTTON,
    ID_MENU_REPORT,
    ID_MENU_START,
    ID_MENU_END,
    ID_HISTORY_LISTBOX
};

class MainFrame: public wxFrame {
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MainFrame();

    wxButton* resetButton; //��ʼ��ť
    wxButton* endButton; //������ť
    wxTextCtrl* dotextCtrl; //�����
    wxTextCtrl* objectCtrl; //Ŀ���
    wxChoice* priorityChoice; //���ȼ�ѡ���
    wxListBox* historyListBox; //��ʷ����ѡ���
    wxArrayString* arrayStr; //��ʷ��������Դ����

private:
    void OnReset(wxCommandEvent& event);
    void OnEnd(wxCommandEvent& event);
    void OnReport(wxCommandEvent& event);
    //void OnAbout(wxCommandEvent& event);

    void OnSelectHistory(wxCommandEvent& event);
    void refreshHistoryListBox();
    void refreshObjectText();
    void saveObjectDataToDb();
    DECLARE_EVENT_TABLE();

    struct HistoryInfo{
        wxString things;
        wxString priority;
    };

    vector<HistoryInfo> m_vecHistoryInfo; //������ʷ��¼��

};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(ID_RESET_BUTTON, OnReset) //������ʼ��ť���¼�������
    EVT_BUTTON(ID_END_BUTTON, OnEnd)
    //EVT_BUTTON(ID_NEXTPAGE_BUTTON, )
    EVT_MENU(ID_MENU_REPORT, OnReport)
    //EVT_MENU(wxID_EXIT,  MainFrame::OnExit)
    //EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_LISTBOX_DCLICK(ID_HISTORY_LISTBOX, OnSelectHistory)
END_EVENT_TABLE()



#endif // INCLUDED_HELLOWORLDAPP_H