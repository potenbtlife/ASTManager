#ifndef INCLUDED_HELLOWORLDAPP_H
#define INCLUDED_HELLOWORLDAPP_H

//�Բ�֧��Ԥ���������Ҫ����"wx/wx.h"
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/snglinst.h>
#include "wx/timectrl.h"
#include "wx/button.h"
#include <wx/grid.h>
#include "wx/taskbar.h"
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

private:
    wxSingleInstanceChecker* m_checker;
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
    void OnClose(wxCloseEvent& event);

    void refreshHistoryListBox();
    void refreshObjectText();
    void saveObjectDataToDb();

    DECLARE_EVENT_TABLE();

    struct HistoryInfo{
        wxString things;
        wxString priority;
    };

    vector<HistoryInfo> m_vecHistoryInfo; //������ʷ��¼��
    wxTaskBarIcon taskBarIcon; //����ͼ��
};


#endif // INCLUDED_HELLOWORLDAPP_H