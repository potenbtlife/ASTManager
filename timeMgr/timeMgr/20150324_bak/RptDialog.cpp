#include "RptDialog.h"

//ע�ᱨ���ڵ��¼�������
BEGIN_EVENT_TABLE(RptDialog, wxDialog)    
	EVT_BUTTON(ID_NEXTPAGE_BUTTON, RptDialog::OnNextPage)
    EVT_BUTTON(ID_PREVIOUS_BUTTON, OnPrePage)
END_EVENT_TABLE()

RptDialog::RptDialog(wxFrame *parent, wxWindowID id, const wxString &title, wxPoint& point, wxSize& size)
	:wxDialog(parent, id, title, point, size,wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER){
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	rptGrid = new wxGrid( this,ID_REPORT_GRID,wxPoint(0,0));
	topsizer->Add(rptGrid,1,// make vertically stretchable
			wxEXPAND |    // make horizontally stretchable        
			wxALL,        //   and make border all around        
			10 );         // set border width to 10

	dataSource = new wxGridStringTable(1,7);
	
	dataSource->SetColLabelValue(0,"����");
	dataSource->SetColLabelValue(1,"����");
	dataSource->SetColLabelValue(2,"��ʼʱ��");
	dataSource->SetColLabelValue(3,"����ʱ��");
	dataSource->SetColLabelValue(4,"��ʱ");
	dataSource->SetColLabelValue(5,"����");
	dataSource->SetColLabelValue(6,"���ȼ�");
	
	gSqlite.open();
	gSqlite.setSql(gAllQrySql);
	if( gSqlite.prepare() < 0 ){
		wxMessageBox(gSqlite.errString);
		return;
	}
	
    gVecAllData.clear();
	while ( 1 == gSqlite.step() ){
		DataContainer tmpDataCER;
		tmpDataCER.keyid = gSqlite.getColumnInt(0);
		tmpDataCER.startTime = gSqlite.getColumnString(1);
		tmpDataCER.endTime = gSqlite.getColumnString(2);
		tmpDataCER.usedTimeSec = gSqlite.getColumnInt(3);
		tmpDataCER.doTings = gSqlite.getColumnString(4);
		tmpDataCER.priority = gSqlite.getColumnString(5);
        tmpDataCER.idayOfWeek = gSqlite.getColumnInt(6);
		tmpDataCER.iWeekofYear = gSqlite.getColumnInt(7)+1;
		
		gVecAllData.push_back(tmpDataCER);

	}
	gSqlite.finalize();//�ͷ�pstmtָ��Ķ��󣬷�ֹ�ڴ�й¶�Ͷ��׼���������ݷ���æ״̬

	rptGrid->SetTable(dataSource,true);

	wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );

	statisticGrid = new wxGrid( this,wxID_ANY);
	statisticDS = new wxGridStringTable(4,3);
	statisticDS->SetColLabelValue(0,"����");
	statisticDS->SetColLabelValue(1,"���ȼ�");
	statisticDS->SetColLabelValue(2,"�ܰٷֱ�");
    statisticGrid->SetTable(statisticDS,true);

    fillPageFromVector2GridTable(gVecAllData, *dataSource, lineNumEachPage, curPage);
    rptGrid->AutoSize();
    calcRatioOfEachPriority(gVecAllData, *statisticDS, lineNumEachPage, curPage);
   
	//statisticGrid->AutoSize();

	button_sizer->Add(statisticGrid,0,wxALL,10 ); 

	button_sizer->Add(        
		new wxButton( this, ID_PREVIOUS_BUTTON, "��һҳ" ),
		0,           // make horizontally unstretchable
		wxALL,       // make border all around (implicit top alignment)
		10 );        // set border width to 10

    button_sizer->Add(
		new wxButton( this, ID_NEXTPAGE_BUTTON, "��һҳ" ),
		0,           // make horizontally unstretchable
		wxALL,       // make border all around (implicit top alignment)
		10 );        // set border width to 10

    topsizer->Add(
		button_sizer,
		0,                // make vertically unstretchable
		wxALIGN_CENTER ); // no border and centre horizontally

	SetSizerAndFit(topsizer);
}

RptDialog::~RptDialog() {
    
    if(rptGrid) {
        delete rptGrid; //�����������dataSourceһ���ͷŵ�
    }
    if(statisticGrid) {
        delete statisticGrid;
    }
}

void RptDialog::OnNextPage(wxCommandEvent& event) {

    if( gVecAllData.size() <= lineNumEachPage * curPage ) { //û����һҳ��ֱ�ӷ���
		return;
	}

    fillPageFromVector2GridTable(gVecAllData, *dataSource, lineNumEachPage, ++curPage);
    //rptGrid->AutoSize();

    calcRatioOfEachPriority(gVecAllData, *statisticDS, lineNumEachPage, curPage);
    statisticGrid->ForceRefresh();
}

void RptDialog::OnPrePage(wxCommandEvent& event) {
    
    if(2 > curPage) { //���Ϊ��һҳ��ֱ�ӷ���
        return;
    }

    fillPageFromVector2GridTable(gVecAllData, *dataSource, lineNumEachPage, --curPage);
    //rptGrid->AutoSize();

    calcRatioOfEachPriority(gVecAllData, *statisticDS, lineNumEachPage, curPage);
    statisticGrid->ForceRefresh();
}

//����ÿ�����޵�ռ��ʱ�����
void RptDialog::calcRatioOfEachPriority(vector<DataContainer>& vSrc, wxGridStringTable& statisticDS,
                                        int numInOnePage, int pageNum) {
    statisticDS.Clear();
    
    map<string,int> mapSepUsedTime;
    int totalUsedTime=0;
    int iWeekofYear=-1;
    
    vector<DataContainer>::iterator vIt = vSrc.begin();
    vIt += numInOnePage * (pageNum - 1); //�ƶ�����ǰҳ��

    if(vIt != vSrc.end()) {
        iWeekofYear = vIt->iWeekofYear; //������ȡ��һ����¼��Ϊ׼
    }

    for(int count=0; count < getRowNumInOnePage(vSrc, numInOnePage, pageNum) && vIt!=vSrc.end(); ++vIt, ++count){

	    map<string,int>::iterator mIt = mapSepUsedTime.find(vIt->priority);
	    if( mIt == mapSepUsedTime.end() ){ //�Ҳ�����ֵ
		    mapSepUsedTime.insert(pair<string,int>(vIt->priority,vIt->usedTimeSec));
	    }else{ //�ҵ��˼�ֵ
		    mIt->second += vIt->usedTimeSec;
	    }
		
	    totalUsedTime += vIt->usedTimeSec;
    }
	
    int index = 0;
    for(map<string,int>::iterator mIt = mapSepUsedTime.begin(); mIt!=mapSepUsedTime.end(); ++mIt){
	    char cWeekOfYear[16];
	    memset(cWeekOfYear,'0',sizeof(cWeekOfYear));
	    sprintf(cWeekOfYear,"��%d��",iWeekofYear);
	    statisticDS.SetValue(index,0,cWeekOfYear);  //ÿ�ζ��ڵ�0�в���ֵ
	    statisticDS.SetValue(index,1,mIt->first);  //���ȼ�
	    char cPecent[16];
	    memset(cPecent,'0',sizeof(cPecent));
	    double tmp1=static_cast<double>(mIt->second)/totalUsedTime*100;
	    sprintf(cPecent,"%.2f%%",tmp1);
	    statisticDS.SetValue(index,2,cPecent);  //���ȼ�
	    index++;
    }
}