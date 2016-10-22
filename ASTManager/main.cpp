#include "main.h"
#include <time.h>
#include "tray.h"
#include "Timer.h"
#include "ValueReportFrm.h"
#include "FinanceFrame.h"
#include <wx/utils.h>
#include "DownLoadNewsDialog.h"
#include "BuyOrSellFoundDialog.h"
#include "FindShotFrame.h"
#include "DownLoadOneTbl.h"

IMPLEMENT_APP(MyApp);

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_BUY,   MyFrame::OnBuy)
    EVT_MENU(ID_SELL, MyFrame::OnSell)
    EVT_MENU(ID_ADJUSTCASH, MyFrame::OnAdjustCash)
    EVT_MENU(ID_BUG_FOUND, MyFrame::OnBuyFound)
    EVT_MENU(ID_SELL_FOUND, MyFrame::OnSellFound)
    EVT_MENU(ID_REPORT_MENU, MyFrame::OnShowValueReport)
    EVT_MENU(ID_DOWNLOAD_YEAR_THREETBL, MyFrame::DownLoadYearThreeTbl)
    EVT_MENU(ID_DOWNLOAD_MID_THREETBL, MyFrame::DownLoadMidThreeTbl)
    EVT_MENU(ID_DOWNLOAD_ONE_THREETBL, MyFrame::DownLoadOneThreeTbl)
    EVT_MENU(ID_LOAD_NEWS, MyFrame::DownLoadNews)
    EVT_MENU(ID_UPDATE_ALLPRICE, MyFrame::UpdateAllPrice)
    EVT_MENU(ID_SHOW_THREE_TABLE, MyFrame::OnShowThreeTable)
    EVT_MENU(ID_FIND_ASHOT, MyFrame::OnFindAShot)
    EVT_MENU(ID_CALCINDEX_TABLE, MyFrame::OnReCalcFinanceIndex)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(ID_HKCOM,   MyFrame::OnHKCompose)
    EVT_MENU(ID_ACOM,   MyFrame::OnACompose)
    //EVT_MENU(ID_FIXEDCOM, MyFrame::OnFixedCompose)
    EVT_MENU(ID_ALLCOM,   MyFrame::OnAllCompose)
    EVT_BUTTON(ID_DELETE_BUTTON, OnDeleteLog)
    EVT_BUTTON(ID_PREVIOUS_BUTTON, OnPresLog)
    EVT_BUTTON(ID_NEXTPAGE_BUTTON, OnNextLog)
    EVT_GRID_CMD_CELL_CHANGED(ID_LOG_GRID, OnGetReviewData)
    //EVT_GRID_CMD_CELL_LEFT_CLICK(ID_LOG_GRID, OnGridTest)
    EVT_BUTTON(ID_COMMON_REVIEW_BUTTON, OnCommitReview)
    EVT_BUTTON(ID_SEARCH_LOG_BUTTON, OnSearchName)
	EVT_GRID_CMD_CELL_LEFT_DCLICK(ID_GOODS_GRID, OnShowOneFinance)
    //EVT_GRID_CMD_CELL_LEFT_CLICK(ID_GOODS_GRID, OnShowBuyCondition)
    EVT_CLOSE(OnClose)
END_EVENT_TABLE()

const std::string m_qryCashSql = "select cash from cash_flow a where a.compose_id=? and a.change_time=(select max(change_time) from cash_flow where compose_id=a.compose_id)";
const std::string m_qryAdviceSql = "select opt_advice from tmp_info where compose_id=?";
const std::string m_updateAdviceSql = "update tmp_info set opt_advice = ? where compose_id=?";

std::string chekcUniqueBalanceRptSql = "select 1 from balance_report where stock_id=? and report_time=?";
std::string insertBalanceRptSql = "insert into balance_report(stock_id,report_time,report_type,static_asset,flow_asset, \
                                  flow_debt,netflow_asset,static_debt,fewholder_asset,net_asset,guben,chubei,holder_asset,\
                                  invisible_asset,building_device,childcort_asset,sibing_asset,other_invest,yingshou,\
                                  cunhuo,cash,yingfu,flow_bankdept,static_bankdept,all_asset,all_dept,stock_share,bizhong,bond) \
                                  values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
std::string chekcUniqueSunYiRptSql = "select 1 from sunyi_report where stock_id=? and report_time=?";
std::string insertSunYiRptSql = "insert into sunyi_report(stock_id,report_time,report_type,yingyee,untax_profit,tax,\
                                tax_profit,fewholder_profit,holder_profit,dividend,net_profit,basic_one_profit,\
                                tanxiao_one_profit,ont_dividend,cost,depreciation,sell_fee,admin_fee,lixi_fee,\
                                gross_profit,jingying_profit,othercorp_profit,bizhong) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

std::string chekcUniqueCashFlowRptSql = "select 1 from cashflow_report where stock_id=? and report_time=?";
std::string insertCashFlowSql = "insert into cashflow_report(stock_id,report_time,report_type,jingying_net,invest_net,\
                                rongzi_net,cash_add,cash_begin,cash_end,waihui_profit,buy_device,bizhong) \
                                values(?,?,?,?,?,?,?,?,?,?,?,?)";

//std::string getUnRegStockIdSql = "select stock_id from stock_list  where stock_id not in(select distinct a.stock_id from balance_report a, sunyi_report b, cashflow_report c where a.stock_id=b.stock_id and a.stock_id=c.stock_id)";
std::string getUnRegStockIdSql = "select stock_id from stock_list";
std::string deleteFinanceIndexSql = "delete from finance_index where report_type=?";
std::string deleteOneFinanceIndexSql = "delete from finance_index where report_type=? and stock_id=?";

const std::string resetCurAllSql = "update compose_info set iscurrent = 0";
const std::string updateCurSql = "update compose_info set iscurrent = ? where compose_id = ?";

wxGridStringTable* dataSourceLog;
wxGridStringTable* dataSourceGoods;

//���ݿ��е�composeid �� �˵����е����id��Ӧ��ϵ������Ϊ���ݿ���compose_info��composeid�������е�ֵΪ�˵����е����id
const int compostID2menuID[10] = { -1, ID_HKCOM, ID_ACOM, ID_ALLCOM};

bool MyApp::OnInit() {

    m_checker = new wxSingleInstanceChecker;
    m_checker->Create(wxApp::GetAppName(), wxGetUserId());

    if (m_checker->IsAnotherRunning()) {
        wxLogError(_("����һ��AST�����������У����Ҽ�����ͼ���Ƚ���رգ�"));
        delete m_checker; // OnExit() won't be called if we return false
        m_checker = NULL;
        return false;
    }

    FILE* fstream = fopen("log.log", "a+");

    if (fstream == NULL) {
        wxMessageBox(string("open log file err:") + strerror(errno));
    }

    wxLog* logger = new wxLogStderr(fstream);
    wxLog::SetActiveTarget(logger);
    //wxLogWarning("my log test %s","cdq");

    Runtime::getInstance()->configObj = new CConfig(configName);

    Runtime::getInstance()->initSqlite(Runtime::getInstance()->configObj->find(key));
    Runtime::getInstance()->sqlite.open();
    Runtime::getInstance()->qryCurCompseID();

    MyFrame* frame = new MyFrame(ID_MAIN_FRAME, "ASTManager", wxDefaultPosition, wxDefaultSize);

    Runtime::getInstance()->myTray.setFrame(frame); //��������������ͼ������Ŀ�ܶ���
    createRtDataObj(&Runtime::getInstance()->rtData);//����ʵʱ���ݶ���

    Runtime::getInstance()->SetRtCode();//����ʵʱ����ˢ�µĴ���

    frame->Centre();
    frame->Show(true);

    //for test
    /*wxCommandEvent* eventObj = new wxCommandEvent();
    frame->OnFindAShot(*eventObj);*/

    Runtime::getInstance()->myTimer.Start(1000 * 60); //1���Ӵ���һ��
    Runtime::getInstance()->myTimer.Notify();

    return true;
}

MyFrame::MyFrame(wxWindowID frameID, const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, frameID, title, pos, size), buyDialog(NULL), sellDialog(NULL), m_inSearch(false) {
	//�ļ��˵�
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_BUY, "&����(BUY)...\tCtrl-B", "Buy stock");
	menuFile->Append(ID_SELL, "&����(SELL)...\tCtrl-S", "Sell stock");
	menuFile->AppendSeparator();
	menuFile->Append(ID_ADJUSTCASH, "&�����ֽ����...\tCtrl-M", "adjust cash");
	menuFile->AppendSeparator();
	menuFile->Append(ID_BUG_FOUND, "&�깺", "�������");
	menuFile->Append(ID_SELL_FOUND, "&���", "��������");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);


	//��ϲ˵�
	//int compostID2menuID[10]={-1,ID_HKCOM,ID_ACOM,ID_ALLCOM};
	composeMenu = new wxMenu;
	composeMenu->AppendRadioItem(ID_HKCOM, "&HK���\tCtrl-Q");
	composeMenu->AppendRadioItem(ID_ACOM, "&A�����\tCtrl-W");
	//composeMenu->AppendRadioItem(ID_FIXEDCOM, "&�̶��������\tCtrl-A");
	composeMenu->AppendRadioItem(ID_ALLCOM, "���ʲ����\tCtrl-E");
	composeMenu->Check(compostID2menuID[Runtime::getInstance()->CurComposeID], true);

	//���񱨱�˵�
	wxMenu* financeFile = new wxMenu;
	financeFile->Append(ID_REPORT_MENU, "&��ֵ����\tCtrl-R", "show report");
	financeFile->Append(ID_SHOW_THREE_TABLE, "&�Ʊ�����\tCtrl-F");
	financeFile->Append(ID_FIND_ASHOT, "&����ɸѡ");


	//���񱨱�˵�
	wxMenu* newsMenu = new wxMenu;
	newsMenu->Append(ID_LOAD_NEWS, "&���ظ۽�������\tCtrl-D");
	newsMenu->AppendSeparator();
	newsMenu->Append(ID_UPDATE_ALLPRICE, "&�������м۸�\tCtrl-P");
	newsMenu->AppendSeparator();
	newsMenu->Append(ID_DOWNLOAD_YEAR_THREETBL, "&���������걨");
	newsMenu->Append(ID_DOWNLOAD_MID_THREETBL, "&���������б�");
	newsMenu->Append(ID_DOWNLOAD_ONE_THREETBL, "&����ĳ��ı���");
	newsMenu->AppendSeparator();
	newsMenu->Append(ID_CALCINDEX_TABLE, "&����Ʊ�ָ��");

	//�����˵�
	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar* menuBar = new wxMenuBar;

	//��ӵ��˵�����
	menuBar->Append(composeMenu, "���ѡ��");
	menuBar->Append(menuFile, "&���ײ���");
	menuBar->Append(financeFile, "&��ֵ���Ʊ�");
	menuBar->Append(newsMenu, "&������Ϣ");
	menuBar->Append(menuHelp, "&����");
	SetMenuBar(menuBar);
	CreateStatusBar(2);
	SetStatusText("��ǰ��ϣ�" + Runtime::getInstance()->qryCurCompseName() , 1);//��ʾ�������

	//���gridLog�ؼ���չʾ������־
	gridLog = new wxGrid(this, ID_LOG_GRID, wxPoint(0, 0), size);
	initDataSourceLog(); //��ʼ�� dataSourceLog

	gridLog->SetTable(dataSourceLog, true);
	gridLog->SetRowLabelSize(30);
	gridLog->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);

	RefreshLogInfo();
	//gridLog->SetDefaultCellOverflow(true);

	//���ֵ�����
	wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
	topsizer->Add(gridLog, 1, wxEXPAND | wxALL, 2);

	wxBoxSizer* bottom_Main_sizer = new wxBoxSizer(wxHORIZONTAL);  //�����������������
	wxBoxSizer* value_sizer = new wxBoxSizer(wxVERTICAL); //��λ����չ�ֲ�����
	wxBoxSizer* search_sizer = new wxBoxSizer(wxVERTICAL);   //�������沼����
	wxBoxSizer* button_sizer = new wxBoxSizer(wxVERTICAL);   //������ť������
	wxBoxSizer* modify_button_sizer = new wxBoxSizer(wxVERTICAL);   //�������沼����

	//��λ����չʾ��
	wxPanel* valuePanel = new wxPanel(this, ID_VALUESHOW_PANEL);
	wxBoxSizer* sizer1 = new wxBoxSizer(wxVERTICAL);
	sizer1->Add(new wxStaticText(valuePanel, wxID_ANY, "��ֵ:", wxPoint(0, 0), wxSize(60, 20)), 0, wxALL);
	marketValTextCtrl = new wxTextCtrl(valuePanel, ID_MARKETVAL_TEXTCTRL, "", wxPoint(60, 0), wxSize(100, 20), wxTE_READONLY);
	sizer1->Add(marketValTextCtrl, 0, wxALL);
	sizer1->Add(new wxStaticText(valuePanel, wxID_ANY, "�ֽ�:", wxPoint(0, 22), wxSize(60, 20)), 0, wxALL);
	cashTextCtrl = new wxTextCtrl(valuePanel, ID_CASH_TEXTCTRL, "", wxPoint(60, 22), wxSize(100, 20), wxTE_READONLY);
	sizer1->Add(cashTextCtrl, 0, wxALL);
	sizer1->Add(new wxStaticText(valuePanel, wxID_ANY, "��λռ��:", wxPoint(0, 44), wxSize(60, 20)), 0, wxALL);
	stockRatioTextCtrl = new wxTextCtrl(valuePanel, ID_STOCKRATIO_TEXTCTRL, "", wxPoint(60, 44), wxSize(100, 20), wxTE_READONLY);
	sizer1->Add(stockRatioTextCtrl, 0, wxALL);
	sizer1->Add(new wxStaticText(valuePanel, wxID_ANY, "���ʲ�:", wxPoint(0, 66), wxSize(60, 20)), 0, wxALL);
	totalAssetTextCtrl = new wxTextCtrl(valuePanel, ID_TOTALASSET_TEXTCTRL, "", wxPoint(60, 66), wxSize(100, 20), wxTE_READONLY);
	sizer1->Add(totalAssetTextCtrl, 0, wxALL);
	sizer1->Add(new wxStaticText(valuePanel, wxID_ANY, "��ֵ:", wxPoint(0, 88), wxSize(60, 20)), 0, wxALL);
	netValueTextCtrl = new wxTextCtrl(valuePanel, ID_NETVALUE_TEXTCTRL, "", wxPoint(60, 88), wxSize(100, 20), wxTE_READONLY);
	sizer1->Add(netValueTextCtrl, 0, wxALL);
	sizer1->Add(new wxStaticText(valuePanel, wxID_ANY, "�ݶ�:", wxPoint(0, 110), wxSize(60, 20)), 0, wxALL);
	shareTextCtrl = new wxTextCtrl(valuePanel, ID_SHARE_TEXTCTRL, "", wxPoint(60, 110), wxSize(100, 20), wxTE_READONLY);
	sizer1->Add(shareTextCtrl, 0, wxALL);
	sizer1->Add(new wxStaticText(valuePanel, wxID_ANY, "ʱ��:", wxPoint(0, 132), wxSize(60, 20)), 0, wxALL);
	timeTextCtrl = new wxTextCtrl(valuePanel, ID_TOTALASSET_TEXTCTRL, "", wxPoint(60, 132), wxSize(100, 20), wxTE_READONLY);
	sizer1->Add(timeTextCtrl, 0, wxALL);

	value_sizer->Add(valuePanel, 0, wxALL, 1);

	//�����ʲ�չʾ����
	gridGoods = new wxGrid(this, ID_GOODS_GRID, wxPoint(0, 0));

	//��ֵ��ֵ��Ϣ
	dataSourceGoods = new wxGridStringTable(1, 8);
	dataSourceGoods->SetColLabelValue(0, "�������");
	dataSourceGoods->SetColLabelValue(1, "��Ĵ���");
	dataSourceGoods->SetColLabelValue(2, "�����");
	dataSourceGoods->SetColLabelValue(3, "�ּ�");
	dataSourceGoods->SetColLabelValue(4, "��ֵ");
	dataSourceGoods->SetColLabelValue(5, "ռ��");
	dataSourceGoods->SetColLabelValue(6, "�Ӳ�����(Ԫ|��)");
	dataSourceGoods->SetColLabelValue(7, "��������(Ԫ*��)");

	//���ز��������õ�����
	for (int i = 0; i < Runtime::getInstance()->vLogDataDet.size(); ++i) {
		if (wxNOT_FOUND == filterArrayString.Index(Runtime::getInstance()->vLogDataDet[i].stock_name.c_str())) {
			filterArrayString.Insert(Runtime::getInstance()->vLogDataDet[i].stock_name, 0);
		}
	}

	stockName_comboBox = new wxComboBox(this, ID_STOCKNAME_COMBOBOX, "�������", wxDefaultPosition, wxDefaultSize, filterArrayString);

	search_sizer->Add(stockName_comboBox, 0, wxALIGN_LEFT | wxALL, 2);
	search_sizer->Add(new wxButton(this, ID_SEARCH_LOG_BUTTON, "��һ��"), 0, wxALIGN_LEFT | wxALL, 6);

	search_sizer->AddSpacer(5);
	search_sizer->Add(new wxStaticText(this, wxID_ANY, "��λ����:", wxPoint(0, 88), wxSize(60, 15)), 0, wxALL);
	adviceTextCtrl = new wxTextCtrl(this, ID_RATIOADVICE_TEXTCTRL, "", wxPoint(0, 88 + 15), wxSize(160, 62), wxTE_MULTILINE);
	search_sizer->Add(adviceTextCtrl, 0, wxALL);

	button_sizer->Add(new wxButton(this, ID_PREVIOUS_BUTTON, "��һҳ"), 0, wxALL, 6);
	button_sizer->Add(new wxButton(this, ID_NEXTPAGE_BUTTON, "��һҳ"), 0, wxALL, 6);
	button_sizer->Add(20, 0, 0);

	modify_button_sizer->Add(new wxButton(this, ID_COMMON_REVIEW_BUTTON, "�ύ�ܽ�"), 0, wxALL, 6);
	modify_button_sizer->Add(new wxButton(this, ID_DELETE_BUTTON, "ɾ��"), 0, wxALL, 6);
	//button_sizer->Add(100, 0, 0);

	bottom_Main_sizer->Add(gridGoods, 0, wxALIGN_LEFT | wxEXPAND);
	bottom_Main_sizer->Add(value_sizer, 0, wxALIGN_LEFT | wxEXPAND);
	bottom_Main_sizer->Add(search_sizer, 0, wxALIGN_LEFT | wxEXPAND);
	bottom_Main_sizer->Add(button_sizer, 0, wxEXPAND);
	bottom_Main_sizer->Add(modify_button_sizer, 0, wxEXPAND);

	topsizer->Add(bottom_Main_sizer, 0, wxEXPAND);


	RefreshGoodsAndValue(); //Ҫ����adviceTextCtrl����֮��
	gridGoods->SetTable(dataSourceGoods, true);
	gridGoods->AutoSizeColumns();
	gridGoods->EnableEditing(false);

	SetSizerAndFit(topsizer);
	SetBackgroundColour(*wxLIGHT_GREY);
}


#include "DownLoadThreeTblThread.h"
void MyFrame::DownLoadYearThreeTbl(wxCommandEvent& event) {
    //WriteThreeRptToDb(0);

	//Runtime::getInstance()->myTimer.Stop();//��ֹͣ��ѯ�������������ռ�������ݿ⣬���³����⡣
	
    DownLoadThreeTblThread* tblthread = new DownLoadThreeTblThread(this, 0);

    if (tblthread->Create() != wxTHREAD_NO_ERROR) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }

    tblthread->Run();

	//Runtime::getInstance()->myTimer.Start(1000*60);//1���Ӵ���һ��, û������, ��Ϊ�˾�Ҳ������ִ��
}


void MyFrame::DownLoadMidThreeTbl(wxCommandEvent& event) {

	//Runtime::getInstance()->myTimer.Stop();//��ֹͣ��ѯ�������������ռ�������ݿ⣬���³����⡣

    DownLoadThreeTblThread* tblthread = new DownLoadThreeTblThread(this, 1);

    if (tblthread->Create() != wxTHREAD_NO_ERROR) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }

    tblthread->Run();

	//Runtime::getInstance()->myTimer.Start(1000*60);//1���Ӵ���һ��, û������, ��Ϊ�˾�Ҳ������ִ��
}

void MyFrame::DownLoadOneThreeTbl(wxCommandEvent& event) {
    
    string stockIdTmp;
    DownLoadOneTblDialog* tmpObj = new DownLoadOneTblDialog(stockIdTmp, this, wxID_ANY, "���ر�ı��� �Ի���", wxPoint(0, 0), wxSize(200, 180), wxDEFAULT_DIALOG_STYLE, "");
    tmpObj->Centre();
    tmpObj->ShowModal();
    delete tmpObj;

    if (stockIdTmp == "") {
        return;
    }
    WriteThreeRptToDb(0,stockIdTmp);
    WriteThreeRptToDb(1,stockIdTmp);

    DeleteOneFinanceIndexFromDb("�걨",stockIdTmp);
    DeleteOneFinanceIndexFromDb("�б�",stockIdTmp);

    FinanceFrame::CalcAllFinanceIndex("�걨", stockIdTmp);
    FinanceFrame::CalcAllFinanceIndex("�б�", stockIdTmp);
}

void MyFrame::DownLoadNews(wxCommandEvent& event) {

	Runtime::getInstance()->myTimer.Stop();//��ֹͣ��ѯ�������������ռ�������ݿ⣬���³����⡣

    wxDateTime begtime;
    wxDateTime endtime;
    DownLoadNewsDialog* dialog1 = new DownLoadNewsDialog(this, wxID_ANY, wxString("����ʱ��ѡ��"), begtime, endtime, wxPoint(0, 0), wxSize(300, 200));
    dialog1->Centre();
    dialog1->ShowModal();
    delete dialog1;


    wxDateSpan dateSpan(0,0,0,1);//��1��

    for(; endtime>=begtime; endtime.Subtract(dateSpan)){
        //tmpRtData->LoadOneDayNews( endtime, m_dir, m_beginNum, m_endNum);

        string dateStr = endtime.Format("%Y%m%d");
        string cmd = Runtime::getInstance()->configObj->find("dNewsApp") + " ";
        cmd += Runtime::getInstance()->configObj->m_configFileName + " " + dateStr;
        //string cmd ="cmd.exe";
        /*if( wxExecute(cmd + " 0 499 dnews1.log", wxEXEC_HIDE_CONSOLE) <= 0 ){ //ִ�г���
            wxMessageBox("can not exec %s", cmd);
            return;
        }*/

		if( wxExecute(cmd + " 0 499 dnews1.log", wxEXEC_HIDE_CONSOLE) <= 0 ){ //ִ�г���
            wxMessageBox("can not exec %s", cmd);
            break;
        }

        if( wxExecute(cmd + " 500 999 dnews2.log", wxEXEC_HIDE_CONSOLE) <= 0 ){ //ִ�г���
            wxMessageBox("can not exec %s", cmd);
            break;
        }

    }




    /*string dir = Runtime::getInstance()->configObj->find("NewsDir") + "\\";

    lnThread1 = new LoadNewsThread(begtime, endtime, dir, 0,999);
    if (lnThread1->Create() != wxTHREAD_NO_ERROR) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }
    lnThread1->Run();*/

    /*LoadNewsThread* lnThread2 = new LoadNewsThread(begtime, endtime, dir, 500,999); //�������̻߳����
    if (lnThread2->Create() != wxTHREAD_NO_ERROR) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }
    lnThread2->Run();*/

	Runtime::getInstance()->myTimer.Start(1000*60);//1���Ӵ���һ��

    return;
}

void MyFrame::OnShowThreeTable(wxCommandEvent& event) {
    FinanceFrame* financeFrame = new FinanceFrame();
    financeFrame->Show(true);
}

void MyFrame::OnFindAShot(wxCommandEvent& event) {
    FindShotFrame* findAShotFrame = new FindShotFrame();
    findAShotFrame->Show(true);
}

void MyFrame::UpdateAllPrice(wxCommandEvent& event) {
    Runtime::getInstance()->myTimer.UpdateAllPrice();
}

#include "CalcFinIndexThread.hpp" //�߳���ش���ʵ��
void MyFrame::OnReCalcFinanceIndex(wxCommandEvent& event) {

    Runtime::getInstance()->myTimer.Stop();//��ֹͣ��ѯ�������������ռ�������ݿ⣬���³����⡣
    
    CalcFinanceIndexThread* thread = new CalcFinanceIndexThread();

    if (thread->Create() != wxTHREAD_NO_ERROR) {
        wxLogError(wxT("Can't create thread!"));
        return;
    }

    thread->Run();

    Runtime::getInstance()->myTimer.Start(1000*60);//1���Ӵ���һ��

}

//param[in] reportType : 0: �걨�� 1���б�;
void MyFrame::WriteThreeRptToDb(int reportType, string stock_id) {

    vector<string> vecStockList;

    if (stock_id == "") { //���Ϊ�գ��ӱ��л�ȡ�����б�
        gSqlite.setSql(getUnRegStockIdSql);

        if (gSqlite.prepare() < 0) {
            wxMessageBox(gSqlite.errString);
            return;
        }

        while (1 == gSqlite.step()) {
            string strId = gSqlite.getColumnString(0);
            vecStockList.push_back(strId);
        }

    }else{
        vecStockList.push_back(stock_id);
    }


    for (int i = 0; i < vecStockList.size(); ++i) {

        wxLogWarning("--begin get i[%d], stock_id[%s]", i, vecStockList[i].c_str());
		//����ʼ
		int rc = gSqlite.begin(); 
		if( rc !=0 ){
			wxMessageBox(gSqlite.errString);
			continue;
		}

        vector<BalanceData> vecOneAllBalance;
        WriteBalanceToDb(trim(vecStockList[i]), reportType, vecOneAllBalance);
        vector<SunYiData> vecOneAllSunYi;
        WriteSunYiToDb(trim(vecStockList[i]), reportType, vecOneAllSunYi);
        vector<CashFlowData> vecOneAllCashFlow;
        WriteCashFlowToDb(trim(vecStockList[i]), reportType, vecOneAllCashFlow);

		//�������
		rc = gSqlite.end(); 
		if( rc !=0 ){
			wxMessageBox(gSqlite.errString);
			continue;
		}

        wxLogWarning("--end get vecOneAllCashFlow.size[%d]", vecOneAllCashFlow.size());
    }

    if (stock_id != "") { //�������ز���������ָ��
        return;
    }

    if (reportType == 0) {
        DeleteFinanceIndexFromDb("�걨");
        FinanceFrame::CalcAllFinanceIndex("�걨");

    } else {
        DeleteFinanceIndexFromDb("�б�");
        FinanceFrame::CalcAllFinanceIndex("�б�");
    }

}

//param[in] reportType : 0: �걨�� 1���б�;
void MyFrame::WriteBalanceToDb(string stock_id, int reportType, vector<BalanceData>& vecOneAllBalance) {

    Runtime::getInstance()->rtData->GetBalanceReport(stock_id, reportType, vecOneAllBalance);

    for (int i = 0; i < vecOneAllBalance.size(); ++i) {
        if (QryUniqueRptData(chekcUniqueBalanceRptSql, vecOneAllBalance[i].stock_id, vecOneAllBalance[i].report_time)) { //����
            continue;
        }

        gSqlite.setSql(insertBalanceRptSql);

        if (gSqlite.prepare() < 0) {
            wxMessageBox(gSqlite.errString);
        }

        gSqlite.bindString(1, vecOneAllBalance[i].stock_id.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindString(2, vecOneAllBalance[i].report_time.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindString(3, vecOneAllBalance[i].report_type.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindDouble(4, vecOneAllBalance[i].static_asset);
        gSqlite.bindDouble(5, vecOneAllBalance[i].flow_asset);
        gSqlite.bindDouble(6, vecOneAllBalance[i].flow_debt);
        gSqlite.bindDouble(7, vecOneAllBalance[i].netflow_asset);
        gSqlite.bindDouble(8, vecOneAllBalance[i].static_debt);
        gSqlite.bindDouble(9, vecOneAllBalance[i].fewholder_asset);
        gSqlite.bindDouble(10, vecOneAllBalance[i].net_asset);
        gSqlite.bindDouble(11, vecOneAllBalance[i].guben);
        gSqlite.bindDouble(12, vecOneAllBalance[i].chubei);
        gSqlite.bindDouble(13, vecOneAllBalance[i].holder_asset);
        gSqlite.bindDouble(14, vecOneAllBalance[i].invisible_asset);
        gSqlite.bindDouble(15, vecOneAllBalance[i].building_device);
        gSqlite.bindDouble(16, vecOneAllBalance[i].childcort_asset);
        gSqlite.bindDouble(17, vecOneAllBalance[i].sibing_asset);
        gSqlite.bindDouble(18, vecOneAllBalance[i].other_invest);
        gSqlite.bindDouble(19, vecOneAllBalance[i].yingshou);
        gSqlite.bindDouble(20, vecOneAllBalance[i].cunhuo);
        gSqlite.bindDouble(21, vecOneAllBalance[i].cash);
        gSqlite.bindDouble(22, vecOneAllBalance[i].yingfu);
        gSqlite.bindDouble(23, vecOneAllBalance[i].flow_bankdept);
        gSqlite.bindDouble(24, vecOneAllBalance[i].static_bankdept);
        gSqlite.bindDouble(25, vecOneAllBalance[i].all_asset);
        gSqlite.bindDouble(26, vecOneAllBalance[i].all_dept);
        gSqlite.bindDouble(27, vecOneAllBalance[i].stock_share);
        gSqlite.bindString(28, vecOneAllBalance[i].bizhong.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindDouble(29, 0); //zhaiquan

        if (gSqlite.step() < 0) {
            wxMessageBox(gSqlite.errString);
            return;
        };

        gSqlite.finalize();
    }

}

void MyFrame::WriteSunYiToDb(string stock_id, int reportType, vector<SunYiData>& vecOneAllSunYi) {

    Runtime::getInstance()->rtData->GetSunYiReport(stock_id, reportType, vecOneAllSunYi);

    for (int i = 0; i < vecOneAllSunYi.size(); ++i) {
        if (QryUniqueRptData(chekcUniqueSunYiRptSql, vecOneAllSunYi[i].stock_id, vecOneAllSunYi[i].report_time)) { //����
            continue;;
        }

        gSqlite.setSql(insertSunYiRptSql);

        if (gSqlite.prepare() < 0) {
            wxMessageBox(gSqlite.errString);
        }

        gSqlite.bindString(1,  vecOneAllSunYi[i].stock_id.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindString(2,  vecOneAllSunYi[i].report_time.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindString(3,  vecOneAllSunYi[i].report_type.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindDouble(4,  vecOneAllSunYi[i].yingyee);
        gSqlite.bindDouble(5,  vecOneAllSunYi[i].untax_profit);
        gSqlite.bindDouble(6,  vecOneAllSunYi[i].tax);
        gSqlite.bindDouble(7,  vecOneAllSunYi[i].tax_profit);
        gSqlite.bindDouble(8,  vecOneAllSunYi[i].fewholder_profit);
        gSqlite.bindDouble(9,  vecOneAllSunYi[i].holder_profit);
        gSqlite.bindDouble(10, vecOneAllSunYi[i].dividend);
        gSqlite.bindDouble(11, vecOneAllSunYi[i].net_profit);
        gSqlite.bindDouble(12, vecOneAllSunYi[i].basic_one_profit);
        gSqlite.bindDouble(13, vecOneAllSunYi[i].tanxiao_one_profit);
        gSqlite.bindDouble(14, vecOneAllSunYi[i].ont_dividend);
        gSqlite.bindDouble(15, vecOneAllSunYi[i].cost);
        gSqlite.bindDouble(16, vecOneAllSunYi[i].depreciation);
        gSqlite.bindDouble(17, vecOneAllSunYi[i].sell_fee);
        gSqlite.bindDouble(18, vecOneAllSunYi[i].admin_fee);
        gSqlite.bindDouble(19, vecOneAllSunYi[i].lixi_fee);
        gSqlite.bindDouble(20, vecOneAllSunYi[i].gross_profit);
        gSqlite.bindDouble(21, vecOneAllSunYi[i].jingying_profit);
        gSqlite.bindDouble(22, vecOneAllSunYi[i].othercorp_profit);
        gSqlite.bindString(23, vecOneAllSunYi[i].bizhong.c_str(), -1, SQLITE_STATIC);

        if (gSqlite.step() < 0) {
            wxMessageBox(gSqlite.errString);
            return;
        };

        gSqlite.finalize();
    }

}

void MyFrame::WriteCashFlowToDb(string stock_id, int reportType, vector<CashFlowData>& vecOneAllCashFlow) {

    Runtime::getInstance()->rtData->GetCashFlowReport(stock_id, reportType, vecOneAllCashFlow);

    for (int i = 0; i < vecOneAllCashFlow.size(); ++i) {
        if (QryUniqueRptData(chekcUniqueCashFlowRptSql, vecOneAllCashFlow[i].stock_id, vecOneAllCashFlow[i].report_time)) { //����
            continue;;
        }

        gSqlite.setSql(insertCashFlowSql);

        if (gSqlite.prepare() < 0) {
            wxMessageBox(gSqlite.errString);
        }

        gSqlite.bindString(1,  vecOneAllCashFlow[i].stock_id.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindString(2,  vecOneAllCashFlow[i].report_time.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindString(3,  vecOneAllCashFlow[i].report_type.c_str(), -1, SQLITE_STATIC);
        gSqlite.bindDouble(4,  vecOneAllCashFlow[i].jingying_net);
        gSqlite.bindDouble(5,  vecOneAllCashFlow[i].invest_net);
        gSqlite.bindDouble(6,  vecOneAllCashFlow[i].rongzi_net);
        gSqlite.bindDouble(7,  vecOneAllCashFlow[i].cash_add);
        gSqlite.bindDouble(8,  vecOneAllCashFlow[i].cash_begin);
        gSqlite.bindDouble(9,  vecOneAllCashFlow[i].cash_end);
        gSqlite.bindDouble(10, vecOneAllCashFlow[i].waihui_profit);
        gSqlite.bindDouble(11, vecOneAllCashFlow[i].buy_device);
        gSqlite.bindString(12, vecOneAllCashFlow[i].bizhong.c_str(), -1, SQLITE_STATIC);

        if (gSqlite.step() < 0) {
            wxMessageBox(gSqlite.errString);
            return;
        };

        gSqlite.finalize();
    }

}

void MyFrame::OnExit(wxCommandEvent& event) {
    
    UpdateAdviceInfoInDb();

    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("This is a wxWidgets' Hello world sample",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

//�����¼��������
void MyFrame::OnBuy(wxCommandEvent& event) {
    buyDialog = new BuyDialog(this, wxID_ANY, "���� �Ի���", wxPoint(0, 0), wxSize(400, 220), wxDEFAULT_DIALOG_STYLE, "11111");
    buyDialog->Centre();
    buyDialog->Show();

    //wxLogMessage("Hello world from wxWidgets!");
}

//�����¼��������
void MyFrame::OnSell(wxCommandEvent& event) {
    sellDialog = new SellDialog(this, wxID_ANY, "���� �Ի���", wxPoint(0, 0), wxSize(400, 240), wxDEFAULT_DIALOG_STYLE, "11111");
    sellDialog->Centre();
    sellDialog->Show();

    //wxLogMessage("Hello world from wxWidgets!");
}

//�����ֽ����
void MyFrame::OnAdjustCash(wxCommandEvent& event) {
    adjustCashDialog = new AdjustCashDialog(this, wxID_ANY, "�ֽ���� �Ի���", wxPoint(0, 0), wxSize(300, 220), wxDEFAULT_DIALOG_STYLE, "11111");
    adjustCashDialog->Centre();
    adjustCashDialog->Show();

}

void MyFrame::OnBuyFound(wxCommandEvent& event) {
    BuyOrSellFoundDialog* bfoundDialog = new BuyOrSellFoundDialog(0, this, wxID_ANY, "�깺 �Ի���", wxPoint(0, 0), wxSize(300, 200), wxDEFAULT_DIALOG_STYLE);
    bfoundDialog->Centre();
    bfoundDialog->Show();
}

void MyFrame::OnSellFound(wxCommandEvent& event) {
    BuyOrSellFoundDialog* bfoundDialog = new BuyOrSellFoundDialog(1, this, wxID_ANY, "��� �Ի���", wxPoint(0, 0), wxSize(300, 200), wxDEFAULT_DIALOG_STYLE);
    bfoundDialog->Centre();
    bfoundDialog->Show();
}

void MyFrame::OnShowValueReport(wxCommandEvent& event) {
    //ValueReportDialog* report = new ValueReportDialog(this,wxID_ANY,"���� �Ի���",wxPoint(0, 0),wxSize(400, 400),wxDEFAULT_DIALOG_STYLE,"report");
    ValueReportFrm* rptFrame = new ValueReportFrm();
    rptFrame->Show(true);
}

void MyFrame::OnHKCompose(wxCommandEvent& event) {

    ResetAllCurrentCompose();
    Runtime::getInstance()->CurComposeID = 1;
    updateIsCurrentCompose(1, Runtime::getInstance()->CurComposeID);
    composeMenu->Check(compostID2menuID[Runtime::getInstance()->CurComposeID], true);
    SetStatusText("��ǰ��ϣ�" + Runtime::getInstance()->qryCurCompseName() , 1);//��ʾ�������

    RefreshLogInfo();

    RefreshGoodsAndValue();

}

void MyFrame::OnACompose(wxCommandEvent& event) {

    ResetAllCurrentCompose();
    Runtime::getInstance()->CurComposeID = 2;
    updateIsCurrentCompose(1, Runtime::getInstance()->CurComposeID);
    composeMenu->Check(compostID2menuID[Runtime::getInstance()->CurComposeID], true);
    SetStatusText("��ǰ��ϣ�" + Runtime::getInstance()->qryCurCompseName() , 1);//��ʾ�������

    RefreshLogInfo();
    RefreshGoodsAndValue();
}

//void MyFrame::OnFixedCompose(wxCommandEvent& event) {
//
//    ResetAllCurrentCompose();
//    Runtime::getInstance()->CurComposeID = 3;
//    updateIsCurrentCompose(1, Runtime::getInstance()->CurComposeID);
//    composeMenu->Check(compostID2menuID[Runtime::getInstance()->CurComposeID], true);
//    SetStatusText("��ǰ��ϣ�" + Runtime::getInstance()->qryCurCompseName() , 1);//��ʾ�������
//
//    RefreshLogInfo();
//    RefreshGoodsAndValue();
//}

void MyFrame::OnAllCompose(wxCommandEvent& event) {

    ResetAllCurrentCompose();
    Runtime::getInstance()->CurComposeID = 3;
    updateIsCurrentCompose(1, Runtime::getInstance()->CurComposeID);
    composeMenu->Check(compostID2menuID[Runtime::getInstance()->CurComposeID], true);
    SetStatusText("��ǰ��ϣ�" + Runtime::getInstance()->qryCurCompseName() , 1);//��ʾ�������

    RefreshLogInfo();
    RefreshGoodsAndValue();
}

//ɾ��������־����
void MyFrame::OnDeleteLog(wxCommandEvent& event) {
    wxArrayInt array1 = gridLog->GetSelectedRows();

    if (array1.GetCount() != 1) {
        wxMessageBox("û��ѡ��һ����¼������ѡ���˶�����¼��");
        return;
    }

    //gSqlite.open();
    gSqlite.setSql(gDeleteLog);
    gSqlite.prepare();
    int rowIndex = array1.Item(0); //gridLog��ȡ���������Ǵ�0��ʼ��

    int vecNum = Runtime::getInstance()->rowNumPage * (Runtime::getInstance()->curPage - 1) + rowIndex;
    int deleteKeyId = Runtime::getInstance()->vLogDataDet[vecNum].keyid; //vLogDataDet��˳��ģ���չ�ֵ�gridLog�ǵ���ģ��ʴ˴�Ҫ��vLogDataDet������ת��
    gSqlite.bindInt(1, deleteKeyId);

    if (gSqlite.step() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    };

    RefreshDataFromDB();

    //������ʾҳ
    setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);

    gridLog->Refresh();

    wxMessageBox("ɾ���ɹ���");
}

//���޸ĵ����ݱ�����vCiReview �����У����ύ��ť�����ύ�����ݿ�
void MyFrame::OnGetReviewData(wxGridEvent& event) {
    int editCol = event.GetCol();
    int editRow = event.GetRow();//gridLog���������Ǵ�0��ʼ��
    LogDataDet ciReview(Runtime::getInstance()->vLogDataDet[editRow]);

    ciReview.trade_review = gridLog->GetCellValue(editRow, editCol);

    vCiReview.clear();
    vCiReview.push_back(ciReview);
}

//��ʾĳ����ĵĲ���ָ��
void MyFrame::OnShowOneFinance(wxGridEvent& event) {
	int col = event.GetCol();
	int row = event.GetRow();//grid�������Ǵ�0��ʼ��
	string stockId = gridGoods->GetCellValue(row, 1); //��ȡ��ǰ�еĵڶ���

	FinanceFrame* financeFrame = new FinanceFrame(stockId);
	financeFrame->Show(true);

	return;
}

//�ύ�����ܽ�
void MyFrame::OnCommitReview(wxCommandEvent& event) {
    if (0 == vCiReview.size()) {
        return;
    }

    if (vCiReview.size() > 1) {
        wxMessageBox("ֻ��һ���ύһ����");
        return;
    }

    gSqlite.setSql(gUpdateReview);
    gSqlite.prepare();
    gSqlite.bindString(1, vCiReview[0].trade_review.c_str(), -1, SQLITE_STATIC);
    gSqlite.bindInt(2, vCiReview[0].keyid);

    if (gSqlite.step() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    };

    wxMessageBox("�ύ�����ܽ�ɹ���");

    RefreshDataFromDB();

    //������ʾҳ
    setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);
}

void MyFrame::OnNextLog(wxCommandEvent& event) {
    if (!m_inSearch) { //�����������״̬
        if (Runtime::getInstance()->vLogDataDet.size() <=
            (Runtime::getInstance()->rowNumPage * Runtime::getInstance()->curPage)) { //��һҳû�����ݣ�ֱ�ӷ���
            return;
        }

        setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, ++Runtime::getInstance()->curPage);

    } else { //���������״̬
        if (Runtime::getInstance()->vSearchResultData.size() <=
            (Runtime::getInstance()->rowNumPage * Runtime::getInstance()->curPage)) { //��һҳû�����ݣ�ֱ�ӷ���
            return;
        }

        setPageFromVector2GridTable(Runtime::getInstance()->vSearchResultData, *dataSourceLog, Runtime::getInstance()->rowNumPage, ++Runtime::getInstance()->curPage);
    }

    LogGridAutoSize();
}

void MyFrame::OnPresLog(wxCommandEvent& event) {
    if (Runtime::getInstance()->curPage < 2) {  //��һҳֱ�ӷ���
        return;
    }

    if (!m_inSearch) { //�����������״̬
        setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, --Runtime::getInstance()->curPage);

    } else { //���������״̬
        setPageFromVector2GridTable(Runtime::getInstance()->vSearchResultData, *dataSourceLog, Runtime::getInstance()->rowNumPage, --Runtime::getInstance()->curPage);
    }

    LogGridAutoSize();
}

void MyFrame::OnSearchName(wxCommandEvent& event) {
    string search_name = stockName_comboBox->GetStringSelection();

    if ("" == search_name) { //��������ַ���Ϊ��
        if (!m_inSearch) { //�����������״̬
            return;

        } else { //����Ѿ�������״̬���ָ�Ϊȫ������״̬
            Runtime::getInstance()->curPage = 1;//��ǰҳ����Ϊ1
            setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);
            m_inSearch = false;
            LogGridAutoSize();

            return;
        }
    }

    //��������״̬
    Runtime::getInstance()->vSearchResultData.clear();

    for (vector<LogDataDet>::iterator iterBeg = Runtime::getInstance()->vLogDataDet.begin();
         iterBeg != Runtime::getInstance()->vLogDataDet.end(); ++iterBeg) {

        if (iterBeg->stock_name == search_name) {
            Runtime::getInstance()->vSearchResultData.push_back(*iterBeg);
        }
    }

    Runtime::getInstance()->curPage = 1;//��ǰҳ����Ϊ1
    setPageFromVector2GridTable(Runtime::getInstance()->vSearchResultData, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);
    m_inSearch = true;
    LogGridAutoSize();

    return;
}

void MyFrame::LogGridAutoSize() {

    if (NULL != gridLog) {
        gridLog->AutoSizeRows();
        gridLog->AutoSizeColumns();
    }
}

MyFrame::~MyFrame() {
    UpdateAdviceInfoInDb();
}

void MyFrame::initDataSourceLog() {
    dataSourceLog = new wxGridStringTable(1, 14);

    dataSourceLog->SetColLabelValue(0, "�������");
    dataSourceLog->SetColLabelValue(1, "��Ĵ���");
    dataSourceLog->SetColLabelValue(2, "����ʱ��");
    dataSourceLog->SetColLabelValue(3, "����۸�");
    dataSourceLog->SetColLabelValue(4, "��������");
    dataSourceLog->SetColLabelValue(5, "���ѽ��");
    dataSourceLog->SetColLabelValue(6, "����ԭ��");
    dataSourceLog->SetColLabelValue(7, "����ʱ��");
    dataSourceLog->SetColLabelValue(8, "�����۸�");
    dataSourceLog->SetColLabelValue(9, "��������");
    dataSourceLog->SetColLabelValue(10, "�������");
    dataSourceLog->SetColLabelValue(11, "����ԭ��");
    dataSourceLog->SetColLabelValue(12, "����ӯ��");
    dataSourceLog->SetColLabelValue(13, "�����ܽ�");
    //dataSourceLog->SetRowLabelValue(0,"������");

}

//��ʼ��������б�
void MyFrame::SetCurGoodsPage(wxGridStringTable* goodsDS, double all_value) {

    map<string, GoodsDet>& mapCurGoods = Runtime::getInstance()->Compose2CurStock[Runtime::getInstance()->CurComposeID];
    map<string, LogDataDet>& mapRecentBuy = Runtime::getInstance()->mapRecentBuy;

    goodsDS->Clear();
    int curRowNum = goodsDS->GetNumberRows();

    if (curRowNum > 1 && (mapCurGoods.size() < curRowNum)) { //����>1Ԥ����ֻ��һ�б�ɾʱ��/�к��ж����� 0
        goodsDS->DeleteRows(0, curRowNum - mapCurGoods.size()); //�к��ж����� 0

    } else if (mapCurGoods.size() > curRowNum) {
        goodsDS->AppendRows(mapCurGoods.size() - curRowNum);
    }

    //���grid����GoodsDet�ṹ�������򣬽�key��value���������½�һ��map��ʵ�ַ�����
    std::map<GoodsDet, string> tmpMap;

    for (map<string, GoodsDet>::iterator iter = mapCurGoods.begin(); iter != mapCurGoods.end(); ++iter) {        
        tmpMap.insert( pair < GoodsDet, string > (iter->second, iter->first) );
    }
    
    std::map<GoodsDet, string>::iterator iter=tmpMap.begin();
    for (int i = 0; iter!=tmpMap.end(); ++iter, ++i) {

        goodsDS->SetValue(i, 0, iter->first.stock_name);
        goodsDS->SetValue(i, 1, iter->first.stock_id);
        goodsDS->SetValue(i, 2, wxString::Format("%d", iter->first.num));
        goodsDS->SetValue(i, 3, wxString::Format("%.2f", iter->first.price));

        double currAmt =  iter->first.num * iter->first.price;

        goodsDS->SetValue(i, 4, wxString::Format("%g", currAmt));
        goodsDS->SetValue(i, 5, wxString::Format("%.2f", currAmt / all_value * 100) + "%");

        double nextBuyPrice = mapRecentBuy[iter->second].buy_price * 0.9;
        time_t nextBuyTime = StringToDatetime(mapRecentBuy[iter->second].buy_time.c_str()) + 7 * 24 * 60 * 60; //һ�����ں�

        if (iter->first.isBear) {//���յ������ȡ�����۸��ʱ��
            nextBuyPrice = mapRecentBuy[iter->second].sell_price * 0.92; //���ճ���Ϊ8%
            nextBuyTime = StringToDatetime(mapRecentBuy[iter->second].sell_time.c_str()) + 14 * 24 * 60 * 60; //��������Ϊ��
        }

        struct tm tm2 = *localtime(&nextBuyTime);

        char showStr[128];

        sprintf(showStr, "%.2f|%d-%d-%d\n", nextBuyPrice, (1900 + tm2.tm_year), (1 + tm2.tm_mon), tm2.tm_mday);

        goodsDS->SetValue(i, 6, showStr);

        //sprintf(showStr, "%.2f|%s\n", nextBuyPrice,(1900+tm2.tm_year),(1+tm2.tm_mon),tm2.tm_mday);
        goodsDS->SetValue(i, 7, wxString::Format("%.2f*%d", mapRecentBuy[iter->second].buy_price * 1.1, mapRecentBuy[iter->second].buy_num)); //10%ӯ����������ӵĲ�λ
    }

}
void MyFrame::RefreshDataFromDB() {

    gSqlite.setSql(gQryLogSql);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    }

    LogDataDet tmpLogDataDet;
    vector<LogDataDet> vTmpLogDataDet;

    while (1 == gSqlite.step()) {
        tmpLogDataDet.composeid = gSqlite.getColumnInt(0);
        tmpLogDataDet.stock_id = gSqlite.getColumnString(1);
        tmpLogDataDet.stock_name = gSqlite.getColumnString(2);
        tmpLogDataDet.buy_time = gSqlite.getColumnString(3);
        tmpLogDataDet.buy_price = gSqlite.getColumnDouble(4);
        tmpLogDataDet.buy_num = gSqlite.getColumnInt(5);
        tmpLogDataDet.buy_allAmt = gSqlite.getColumnDouble(6);
        tmpLogDataDet.buy_reason = gSqlite.getColumnString(7);
        tmpLogDataDet.sell_time = gSqlite.getColumnString(8);
        tmpLogDataDet.sell_price = gSqlite.getColumnDouble(9);
        tmpLogDataDet.sell_num = gSqlite.getColumnInt(10);
        tmpLogDataDet.sell_allAmt = gSqlite.getColumnDouble(11);
        tmpLogDataDet.sell_reason = gSqlite.getColumnString(12);
        tmpLogDataDet.trade_sumamt = gSqlite.getColumnDouble(13);
        tmpLogDataDet.trade_review = gSqlite.getColumnString(14);
        tmpLogDataDet.keyid = gSqlite.getColumnInt(15);
        tmpLogDataDet.type = trim(gSqlite.getColumnString(16));

        vTmpLogDataDet.push_back(tmpLogDataDet);
    }

    //second sort��������ɵĽ��׷ŵ����
    Runtime::getInstance()->vLogDataDet.clear();
    vector<LogDataDet>::iterator iter = vTmpLogDataDet.begin();

    while (iter != vTmpLogDataDet.end()) { //��־��ʾ˳�����ա��������

        if (iter->type == "1" && (iter->buy_num - iter->sell_num) != 0) {  //��Ч���ռ�¼
            Runtime::getInstance()->vLogDataDet.insert(Runtime::getInstance()->vLogDataDet.begin(), *iter);
            iter = vTmpLogDataDet.erase(iter);

        } /*else if ((iter->buy_num - iter->sell_num) > 0) { //�����¼
            Runtime::getInstance()->vLogDataDet.push_back(*iter);
            iter = vTmpLogDataDet.erase(iter);

        } */else { //����ɽ���
            ++iter;
        }

    }

    Runtime::getInstance()->vLogDataDet.insert(Runtime::getInstance()->vLogDataDet.end(), vTmpLogDataDet.begin(), vTmpLogDataDet.end());

    //gSqlite.finalize(); //CDBSqlite ������������е��ã��˴��ٵ��ûᵼ�·��ʴ���
}

/*void MyFrame::OnShowBuyCondition(wxGridEvent& event) {
    int rowNum = event.GetRow(); //��ȡѡ�е���
    Runtime::getInstance()->mapRecentBuy;

    //TODO��չ���´�����ʱ�� ���� �´������λ
    return;
}*/

void MyFrame::OnClose(wxCloseEvent& event) {
    this->Show(false);
}

//�����ֵ��Ϣ��all_value�������
void MyFrame::GetValueFromDb(int composeId, double& marketvalue,double& cash,string& datetime,double& fundShare,double& fundValue) {

    string value_advice, detailInfo="";
    double marketvalue_nouse=0, cash_nouse=0;
    qryValueInfo(composeId, datetime, value_advice, detailInfo, fundShare, fundValue, marketvalue_nouse, cash_nouse);//��ȡvalue_info����

    //��ȡ�ֽ�ͷݶ�
    gSqlite.setSql(m_qryCashSql);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    }

    gSqlite.bindInt(1, composeId);

    if (1 == gSqlite.step()) {
        cash = gSqlite.getColumnDouble(0);
    }

    gSqlite.finalize();

    if (trim(detailInfo) == "") { //���ʲ�������
        marketvalue = marketvalue_nouse;
        
    }else {
        SetPrice2CurGoods(detailInfo); //���ϴμ�¼������ˢ��Runtime�����д������ݵļ۸�
        marketvalue = getMarketValue(Runtime::getInstance()->CurComposeID);
    }

    return;
}

//���´������ݼ���ֵ����
void MyFrame::RefreshGoodsAndValue() {

    //������ֵ��Ϣ
    double marketvalue=0, cash=0, fundShare=0, fundValue=0;
    string datetime;
    GetValueFromDb(Runtime::getInstance()->CurComposeID, marketvalue, cash, datetime, fundShare, fundValue);

    SetValueText(marketvalue, cash, datetime, fundShare, fundValue);

    RefreshAdvice(); //���½������

    //���´���չʾ����
    SetCurGoodsPage(dataSourceGoods, marketvalue+cash);
}

//������־չʾ����
void MyFrame::RefreshLogInfo() {
    RefreshDataFromDB();
    Runtime::getInstance()->RefreshCurrentGoods();
    Runtime::getInstance()->RefreshRecentBuyRecord();

    Runtime::getInstance()->curPage = 1;
    setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);

    LogGridAutoSize();
}

void MyFrame::SetPrice2CurGoods(string detailInfo) {
    //����һ��ļ۸���´���ļ۸�
    //�����ϸ��Ϣ
    vector<std::string> vTmp1;
    splitString(detailInfo, vTmp1, ";", false);
    map<std::string, GoodsDet> mapRecData;

    for (vector<std::string>::iterator iter = vTmp1.begin(); iter != vTmp1.end(); ++iter) {
        if (*iter == "") {
            continue;
        }

        vector<std::string> vTmp2;
        splitString(*iter, vTmp2, ":", false);
        GoodsDet tmpGoodsDet;
        tmpGoodsDet.stock_id = vTmp2[0];
        tmpGoodsDet.num = atoi(vTmp2[1].c_str());
        tmpGoodsDet.price = atof(vTmp2[2].c_str());

        mapRecData[tmpGoodsDet.stock_id] = tmpGoodsDet;
    }

    map<string, GoodsDet>::iterator iter = Runtime::getInstance()->Compose2CurStock[Runtime::getInstance()->CurComposeID].begin();

    for (int i = 0; iter != Runtime::getInstance()->Compose2CurStock[Runtime::getInstance()->CurComposeID].end(); ++iter, ++i) {

        map<std::string, GoodsDet>::iterator pos = mapRecData.find(stringTrim(iter->second.stock_id));

        if (pos != mapRecData.end() && !iter->second.isBear) { //���������ϸ�������ҵ�������ϸ���ݵļ۸����ղ����¼۸�
            iter->second.price = pos->second.price;
        }

    }
}


//���½����ı���
void MyFrame::RefreshAdvice()
{
    gSqlite.setSql(m_qryAdviceSql);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    }

    gSqlite.bindInt(1, Runtime::getInstance()->CurComposeID);

    string tmp_advice;
    if (1 == gSqlite.step()) {
        tmp_advice = gSqlite.getColumnString(0);
    }

    gSqlite.finalize();
    adviceTextCtrl->SetValue(tmp_advice);
}

void MyFrame::UpdateAdviceInfoInDb()
{
    gSqlite.setSql(m_updateAdviceSql);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
    }

    string tmpAdvice = adviceTextCtrl->GetValue();
    gSqlite.bindString(1, tmpAdvice.c_str(), -1, SQLITE_STATIC);
    gSqlite.bindInt(2, Runtime::getInstance()->CurComposeID);

    if (gSqlite.step() < 0) {
        wxMessageBox(gSqlite.errString);
    };
    gSqlite.finalize();
}

//�����Ƿ�ǰ���iscurrent �ֶ�
int MyFrame::updateIsCurrentCompose(int updateValue, int composeid) {

    gSqlite.setSql(updateCurSql);
    if( gSqlite.prepare() < 0 ){
        //wxMessageBox(sqlite.errString);
        return -1;
    }
    gSqlite.bindInt(1, updateValue);
    gSqlite.bindInt(2, composeid);

    if ( gSqlite.step() < 0) {
        return -2;
    }

    return 1;
}

//�������е�iscurrent �ֶ�Ϊ0
int MyFrame::ResetAllCurrentCompose() {

    gSqlite.setSql(resetCurAllSql);
    if( gSqlite.prepare() < 0 ){
        //wxMessageBox(sqlite.errString);
        return -1;
    }

    if ( gSqlite.step() < 0) {
        return -2;
    }

    return 1;
}

void MyFrame::SetValueText(double marketvalue, double cash, string datetime, double fundShare, double fundValue )
{
    double all_value = marketvalue + cash;

    double ratio = all_value == 0 ? all_value : marketvalue / all_value;
    char cRatio[16];
    sprintf(cRatio, "%.2f%s", ratio * 100, "%");

    char cMarketvalue[128];
    sprintf(cMarketvalue, "%.2f", marketvalue);
    char cCash[128];
    sprintf(cCash, "%.2f", cash);
    char cAllValue[128];
    sprintf(cAllValue, "%.2f", all_value);

    marketValTextCtrl->SetValue(cMarketvalue);
    cashTextCtrl->SetValue(cCash);
    stockRatioTextCtrl->SetValue(cRatio);
    totalAssetTextCtrl->SetValue(cAllValue);
    timeTextCtrl->SetValue(datetime);
    char cFundShare[128];
    sprintf(cFundShare, "%.2f", fundShare);
    shareTextCtrl->SetValue(cFundShare);
    char cFundValue[128];
    sprintf(cFundValue, "%.4f", fundValue);
    netValueTextCtrl->SetValue(cFundValue);
}

void DeleteFinanceIndexFromDb(string reportType) {

    gSqlite.setSql(deleteFinanceIndexSql);
    gSqlite.prepare();

    gSqlite.bindString(1, reportType.c_str(), -1, SQLITE_STATIC);

    if (gSqlite.step() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    };

    gSqlite.finalize();
}

void DeleteOneFinanceIndexFromDb(string reportType, string stockId) {

    gSqlite.setSql(deleteOneFinanceIndexSql);
    gSqlite.prepare();

    gSqlite.bindString(1, reportType.c_str(), -1, SQLITE_STATIC);
    gSqlite.bindString(2, stockId.c_str(), -1, SQLITE_STATIC);

    if (gSqlite.step() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    };

    gSqlite.finalize();
}
