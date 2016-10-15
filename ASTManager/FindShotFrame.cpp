#include "FindShotFrame.h"
#include "Runtime.h"
#include <algorithm>

//ֻ���걨������ɸѡ
string qryGuZhiData="select b.stock_id, b.stock_name,b.curprice,a.ziyouxijgz, \
                    (a.ziyouxijgz-b.curprice)/a.ziyouxijgz from finance_index a, stock_list b \
                    where report_type='�걨' and a.stock_id = b.stock_id and a.ziyouxijgz>0 and b.curprice>0 and (a.ziyouxijgz-b.curprice)/a.ziyouxijgz>? and substr(report_time,0,5)=? ";

BEGIN_EVENT_TABLE(FindShotFrame, wxFrame)    
    EVT_BUTTON(ID_OK_BUTTON, OnOk)
END_EVENT_TABLE()

wxString diGuLvName[]={"�͹�0%����","�͹�10%����","�͹�20%����","�͹�40%����"}; //��diGuLvId����ʹ��
double diGuLvId[] ={0, 0.1, 0.2, 0.4}; //��diGuLvName����ʹ��

FindShotFrame::~FindShotFrame(){
    m_auiMan->UnInit();
}

FindShotFrame::FindShotFrame() : wxFrame(NULL, wxID_ANY, "find a shot!", wxDefaultPosition, wxSize(600, 400)) {
    m_auiMan = new wxAuiManager(this);

    wxPanel* panel1 = new wxPanel(this, wxID_ANY);

    new wxStaticText(panel1, wxID_ANY, "���:", wxPoint(3,3), wxSize(40,20));
    yearTextCtrl = new wxTextCtrl(panel1, ID_YEAR_TEXTCTRL, wxString::Format("%d",wxDateTime::Now().GetYear()), wxPoint(50,0),wxSize(100,20));

    new wxStaticText(panel1, wxID_ANY, "�͹���:", wxPoint(3,23), wxSize(40,20));
    wxString diGuLv[]={"0%����","10%����","20%����","40%����"};
    m_diGuLvComboBox = new wxComboBox(panel1, ID_DIGULV_COMBOBOX,"",wxPoint(50,20), wxSize(100,20),4,diGuLv,wxTE_PROCESS_ENTER);
    m_diGuLvComboBox->SetSelection(0);

    m_okButton = new wxButton(panel1, ID_OK_BUTTON, "ȷ��", wxPoint(170, 0), wxSize(60,28));
    m_auiMan->AddPane(panel1, wxAuiPaneInfo().CloseButton(false).CaptionVisible(false).Top().BestSize(600,60));


    DataGrid = new wxGrid(this,ID_FIND_RESULE_GRID,wxDefaultPosition,wxDefaultSize,wxWANTS_CHARS, "�б�");
    DataGT = new wxGridStringTable(1, 5);
    DataGT->SetColLabelValue(0,"��Ĵ���");  
    DataGT->SetColLabelValue(1,"�������");
    DataGT->SetColLabelValue(2,"��ǰ�۸�");
    DataGT->SetColLabelValue(3,"��ֵ�۸�");
    DataGT->SetColLabelValue(4,"�͹�����");
    
    DataGrid->SetTable(DataGT, true);
    DataGrid->AutoSizeRows();
    DataGrid->AutoSizeColumns();
    //DataGrid->SetRowLabelSize(180);
    m_auiMan->AddPane(DataGrid, wxAuiPaneInfo().CloseButton(false).CaptionVisible(false).Centre().BestSize(600,60));

    m_auiMan->Update();
    Centre();
}

bool guZhiGreater(GuZhiData lo, GuZhiData ro){
    if(lo.diGuRate > ro.diGuRate){
        return true;
    }

    if(lo.diGuRate < ro.diGuRate){
        return false;
    }

    return false;
}

void FindShotFrame::OnOk(wxCommandEvent& event){
    double diguLv = diGuLvId[m_diGuLvComboBox->GetSelection()];
    string year = yearTextCtrl->GetValue();

    vector<GuZhiData> vecGuZhiData;
    GetGuZhiData(vecGuZhiData, diguLv,year);
    sort(vecGuZhiData.begin(), vecGuZhiData.end(), guZhiGreater);
    RefreshGuZhiGrid(vecGuZhiData);

    return;
}

void FindShotFrame::RefreshGuZhiGrid(vector<GuZhiData> &vecGuZhiData){
    DataGT->Clear();
    int curRowNum = DataGT->GetNumberRows();
    if(curRowNum > 1 && (vecGuZhiData.size() < curRowNum) ) { //����>1Ԥ����ֻ��һ�б�ɾʱ��/�к��ж����� 0
        DataGT->DeleteRows(0, curRowNum - vecGuZhiData.size()); //�к��ж����� 0

    }else if( vecGuZhiData.size() > curRowNum){
        DataGT->AppendRows(vecGuZhiData.size() - curRowNum);
    }

    for(int i=0; i<vecGuZhiData.size(); ++i){
        DataGT->SetValue(i,0,vecGuZhiData[i].stockId);
        DataGT->SetValue(i,1,vecGuZhiData[i].stockName);
        DataGT->SetValue(i,2,wxString::Format("%.2f",vecGuZhiData[i].price));
        DataGT->SetValue(i,3,wxString::Format("%.2f",vecGuZhiData[i].standPrice));
        DataGT->SetValue(i,4,wxString::Format("%.2f%%",vecGuZhiData[i].diGuRate*100));
    }

}

void FindShotFrame::GetGuZhiData(vector<GuZhiData> &vecGuZhiData, double diguRate, string year){

    Runtime::getInstance()->sqlite.setSql(qryGuZhiData);
    if( Runtime::getInstance()->sqlite.prepare() < 0 ){
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return;
    }

    Runtime::getInstance()->sqlite.bindDouble(1, diguRate);
    Runtime::getInstance()->sqlite.bindString(2,year.c_str(), -1, SQLITE_STATIC);

    GuZhiData tmpDiGuData;
    while ( 1 == Runtime::getInstance()->sqlite.step() )
    {
        tmpDiGuData.stockId = Runtime::getInstance()->sqlite.getColumnString(0); 
        tmpDiGuData.stockName = Runtime::getInstance()->sqlite.getColumnString(1); 
        tmpDiGuData.price = Runtime::getInstance()->sqlite.getColumnDouble(2); 
        tmpDiGuData.standPrice = Runtime::getInstance()->sqlite.getColumnDouble(3); 
        tmpDiGuData.diGuRate = Runtime::getInstance()->sqlite.getColumnDouble(4); 

        vecGuZhiData.push_back(tmpDiGuData);
    }

    return;
}