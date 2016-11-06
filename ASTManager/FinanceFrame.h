#ifndef AST_FINANCEFRAME_H
#define AST_FINANCEFRAME_H
#include <string>
#include "common.h"
#include "Runtime.h"
#include "wx/wxfreechartdefs.h"
#include "wx/chartpanel.h"
#include "CRTData.h"
#include <wx/aui/aui.h>
#include <wx/arrstr.h>

using namespace std;

struct FinanceIndexData{
    string stock_id;
    string report_time;
    string report_type;
    double cashdidept;
    double maolilv;
    double yingyelirunlv;
    double jinglilv;
    double roe;
    double roa;
    double yingshougrowthrate;
    double yingshoulvrungrowthrate;
    double zongzicangrowthrate;
    double jingzicangrowthrate;
    double yingshouzzl;
    double cunhuozzl;
    double gudingzicanzzl;
    double zongzicanzzl;
    double gangganlv;
    double ziyouxijgz;
    double guxi;
    double sjl; //�о���
    double curPrice;
};

int getOneBalance(string stock_id, vector<BalanceData>& vecBalanceData, string& reportType);
int getOneSunYi(string stock_id, vector<SunYiData>& vecSunYiData, string& reportType);
int getOneCashFlow(string stock_id, vector<CashFlowData>& vecCashFlowData, string& reportType);
int getOneFinanceIndex(string stock_id, vector<FinanceIndexData>& vecFinanceIndexData, string& reportType);

class BalanceFrame;
class SunYiFrame;
class CashFlowFrame;

class FinanceFrame: public wxFrame
{
public:
    FinanceFrame(string stockId = "");
    virtual ~FinanceFrame();
    
    //�������б�Ĳ���ָ��
    static void FinanceFrame::CalcAllFinanceIndex(string reportType, string stockId = "");

private:

    //����һ����ĵĲ���ָ��
    static void CalcOneFinanceIndexToDb(string stock_id, vector<BalanceData>& vecOneAllBalance, vector<SunYiData>& vecOneAllSunYi, vector<CashFlowData> vecOneAllCashFlow);

	//��ȡ���ݿ������еı������ݣ����ػ�ȡ�ļ�¼��
	static int getAllReportData(string& reportType, map<string, vector<BalanceData> >& mapBalance, map<string, vector<SunYiData> >& mapSunyi, map<string, vector<CashFlowData> >& mapCashFlow);

    void RefreshFinanceIndexGT( vector<FinanceIndexData> &vecFinanceIndexData); //ˢ�²���ָ������Դ
	void OnSelectedChange(wxCommandEvent& event);
    void ShowYearThreeTbl(wxCommandEvent& event);
    void OnShowBalance(wxCommandEvent& event);
    void OnShowSunYi(wxCommandEvent& event);
    void OnShowCashFlow(wxCommandEvent& event);
    void OnQueryCode(wxCommandEvent& event);
    void Switch2Year(wxCommandEvent& event);
    void Switch2Mid(wxCommandEvent& event);
    void ShowThreeTbl(wxCommandEvent& event);

    wxAuiManager* m_auiMan;
    wxComboBox* m_stockIdNameCB;
    wxButton * m_threeTblBtn;
    wxButton * m_balanceTblBtn;
    wxButton * m_sunYiTblBtn;
    wxButton * m_cashFlowTblBtn;
    wxGrid* financeIndexGrid;
    wxGridStringTable *financeIndexGT;

    BalanceFrame *m_balanceFrame;
    SunYiFrame *m_sunYiFrame;
    CashFlowFrame *m_cashFlowFrame;

    static wxArrayString m_arrayAllIDName; //�������й�Ʊ�б�
    string m_reportType;
    
    DECLARE_EVENT_TABLE();
    wxFont m_fontCell;
    wxFont m_fontLabel;
	string _stockId; //��ǰ�����stockid

    void ReCreateThreeTbl( string stock_id, string idName );
};

class BalanceFrame: public wxFrame
{
public:
    BalanceFrame(string stockId, string caption, string reportType);
    virtual ~BalanceFrame(){};

    DECLARE_EVENT_TABLE();

private:
    string m_stockid;
    string m_reportType;
    wxGrid* balanceGrid;
    wxGridStringTable *balanceGT;
    void OnClose(wxCloseEvent& event);

};

class SunYiFrame: public wxFrame
{
public:
    SunYiFrame(string stockId, string caption, string reportType);
    virtual ~SunYiFrame(){};

    DECLARE_EVENT_TABLE();

private:
    string m_stockid;
    string m_reportType;
    void OnClose(wxCloseEvent& event);
};

class CashFlowFrame: public wxFrame
{
public:
    CashFlowFrame(string stockId, string caption, string reportType);
    virtual ~CashFlowFrame(){};

private:
    string m_stockid;
    string m_reportType;
    wxGrid* cashFlowGrid;
    wxGridStringTable *cashFlowGT;
    void OnClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE();
};


#endif //AST_FINANCEFRAME_H