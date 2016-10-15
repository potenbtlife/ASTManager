// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#ifndef ASTMANAGER_MAIN_H
#define ASTMANAGER_MAIN_H

#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/datectrl.h>
#include <wx/snglinst.h>
#include <wx/file.h>
#include <wx/log.h>

#ifndef WX_PRECOMP    
	#include <wx/wx.h>
#endif

#include "CDBSqlite.h"
#include "common.h"
#include "Runtime.h"
#include "BuyDialog.h"
#include "SellDialog.h"
#include "AdjustCashDialog.h"
#include <vector>
#include <set>
#include "CConfig.h"
#include "DownNewsThread.hpp"


//�����ļ�����ֵ�Ե���ʽ��=Ϊ�ָ���
const string configName="conf";
const string key="DBPATH"; //���ݿ�·���ļ�

//sqlite���ݿ��������
#define gSqlite Runtime::getInstance()->sqlite

void DeleteFinanceIndexFromDb(string reportType);
void DeleteOneFinanceIndexFromDb(string reportType, string stockId);

class MyApp: public wxApp
{
public:
        virtual bool OnInit();

private:
    wxSingleInstanceChecker* m_checker;

};

class MyFrame: public wxFrame
{
	public:
		MyFrame(wxWindowID frameID, const wxString& title, const wxPoint& pos, const wxSize& size);
        ~MyFrame();
		
	//private:
        void initDataSourceLog();//��ʼ����־չʾ�б�
        void SetCurGoodsPage(wxGridStringTable *goodsDS, double all_value); //���ô������, all_value:����ֵ

        void SetPrice2CurGoods( string detailInfo );
        void OnBuy(wxCommandEvent& event);
		void OnSell(wxCommandEvent& event);
        void OnAdjustCash(wxCommandEvent& event);
        void OnBuyFound(wxCommandEvent& event);
        void OnSellFound(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);

        void UpdateAdviceInfoInDb();
        void OnClose(wxCloseEvent& event); //���ϽǵĹرհ�ť�����ز��˳�
		void OnAbout(wxCommandEvent& event);
		void OnReviewLog(wxCommandEvent& event);
		void OnDeleteLog(wxCommandEvent& event);
		void OnPresLog(wxCommandEvent& event);
		void OnNextLog(wxCommandEvent& event);
		void OnGetReviewData(wxGridEvent& event);
		void OnCommitReview(wxCommandEvent& event);
        void OnSearchName(wxCommandEvent& event);
		void OnShowOneFinance(wxGridEvent& event);//��ʾĳ����ĵĲ���ָ��
        void OnHKCompose(wxCommandEvent& event);
        void OnShowValueReport(wxCommandEvent& event);
        void DownLoadYearThreeTbl(wxCommandEvent& event);
        void DownLoadMidThreeTbl(wxCommandEvent& event);
        void DownLoadOneThreeTbl(wxCommandEvent& event);//����ĳһ��ı���
        void DownLoadNews(wxCommandEvent& event);

        void OnShowThreeTable(wxCommandEvent& event);
        void OnReCalcFinanceIndex(wxCommandEvent& event);
        void UpdateAllPrice(wxCommandEvent& event);
        void OnFindAShot(wxCommandEvent& event);

        void RefreshLogInfo(); //ˢ����־չ������
        void RefreshGoodsAndValue();
        void OnACompose(wxCommandEvent& event);
        void OnFixedCompose(wxCommandEvent& event);
        void OnAllCompose(wxCommandEvent& event);
        //void OnShowBuyCondition(wxGridEvent& event);

        void LogGridAutoSize(); //�Զ�������־�б�grid���к��еĿ�Ⱥ͸߶�
        void RefreshDataFromDB();

        //�����ֵ����, ���������
        void GetValueFromDb(int composeId, double& marketvalue,double& cash,string& datetime,double& fundShare,double& fundValue);

        void SetValueText(double marketvalue, double cash, string datetime, double fundShare, double fundValue );
        void RefreshAdvice();
        void WriteBalanceToDb(string stock_id, int reportType, vector<BalanceData>& vecOneAllBalance); //�����ݿ�Ǽ��ʲ���ծ������
        void WriteSunYiToDb(string stock_id, int reportType, vector<SunYiData>& vecOneAllSunYi); //�����ݿ�Ǽ����������
        void WriteCashFlowToDb(string stock_id, int reportType, vector<CashFlowData>& vecOneAllCashFlow); //�����ݿ�Ǽ��ֽ�����������
        void CalcFinanceIndexToDb(string stock_id, vector<BalanceData>& vecOneAllBalance, vector<SunYiData>& vecOneAllSunYi, vector<CashFlowData> vecOneAllCashFlow);

        void WriteThreeRptToDb(int reportType, string stock_id = ""); //��������ܽӿ�;param[in] reportType : 0: �걨�� 1���б�; stock_id: ���id�����û��ָ����ȡ����

		DECLARE_EVENT_TABLE();
	private:

		vector<LogDataDet> vCiReview;//�����ύ�ܽ������
        wxDialog *buyDialog; //����Ի���ָ��
        wxDialog *sellDialog; //�����Ի���ָ��
        wxDialog *adjustCashDialog; //�����ֽ�Ի���ָ��
        wxComboBox* stockName_comboBox; //������Ͽ�ؼ�

        wxArrayString filterArrayString;//����stock_name����������
        bool m_inSearch; //�Ƿ�������״̬

        wxGrid *gridLog; //չ����־�б�grid����
        wxGrid *gridGoods; //����б����

        wxTextCtrl* marketValTextCtrl;
        wxTextCtrl* cashTextCtrl;
        wxTextCtrl* stockRatioTextCtrl;
        wxTextCtrl* totalAssetTextCtrl;
        wxTextCtrl* netValueTextCtrl;
        wxTextCtrl* shareTextCtrl;
        wxTextCtrl* adviceTextCtrl;
        wxTextCtrl* timeTextCtrl;
        wxMenu *composeMenu; //�˵�����Ͽؼ�
        LoadNewsThread* lnThread1;

        int updateIsCurrentCompose(int updateValue, int composeid);
        int ResetAllCurrentCompose(); //�������е�iscurrent �ֶ�Ϊ0

};


#endif //ASTMANAGER_MAIN_H