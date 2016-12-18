#include "common.h"
#include "OperHistoryDialog.h"
#include "main.h"

const string QRY_CASH_FLOW="select keyid,compose_id, oper_type,change_cash,after_cash,fund_share,change_time,change_reason from cash_flow where compose_id=? order by keyid desc";
const string DEL_CASH_FLOW="delete from cash_flow where keyid=?";
const string DEL_TRADE_INFO="delete from trade_info where keyid in (select max(keyid) from trade_info)";

//ע�����봰�ڵ��¼�������
BEGIN_EVENT_TABLE(OperHistoryDialog, wxFrame)
	EVT_BUTTON(ID_OPER_HISTORY_NEXT,  OnNext)
	EVT_BUTTON(ID_OPER_HISTORY_PREVIOUS,  OnPrevious)
	EVT_BUTTON(ID_OPER_HISTORY_ROLLBACK,  OnRollback)
	EVT_BUTTON(ID_OPER_HISTORY_CANCEL,  OnCancel)
END_EVENT_TABLE()

//������ʷ���ڹ��캯��
OperHistoryDialog::OperHistoryDialog(wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxFrame(parent,id,title,pos,size,style,name),curPage(1),numOnePage(8)
{

	//���grid�ؼ���չʾ������־
	_pOperHisgrid = new wxGrid(this, ID_OPER_HISTORY_GRID, wxPoint(0, 0), size);
	
	_pDS = new wxGridStringTable(1, 8);
	_pDS->SetColLabelValue(0, "����ʱ��");
	_pDS->SetColLabelValue(1, "�������");
	_pDS->SetColLabelValue(2, "��������");
	_pDS->SetColLabelValue(3, "��ǰ���");
	_pDS->SetColLabelValue(4, "�䶯���");
	_pDS->SetColLabelValue(5, "�����");
	_pDS->SetColLabelValue(6, "����ݶ�");
	_pDS->SetColLabelValue(7, "�䶯ԭ��");

	GetCashFlowData(_vecCashFlowData);

	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);

	_pOperHisgrid->SetTable(_pDS, true);
	_pOperHisgrid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
	_pOperHisgrid->AutoSizeRows();
	_pOperHisgrid->AutoSizeColumns();
	_pOperHisgrid->SetRowLabelSize(30);

	wxButton* nextButton = new wxButton(this, ID_OPER_HISTORY_NEXT,"��һҳ");
	wxButton* previousButton = new wxButton(this, ID_OPER_HISTORY_PREVIOUS,"��һҳ");
	wxButton* cancelButton = new wxButton(this, ID_OPER_HISTORY_CANCEL,"ȡ��");
	wxButton* rollbackButton = new wxButton(this, ID_OPER_HISTORY_ROLLBACK,"�ع����һ������");

	//���ֵ�����
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);  //��������Ĳ�����
	bottomSizer->AddSpacer(20);
	bottomSizer->Add(previousButton, 0, wxALL|wxALIGN_CENTRE, 10);
	bottomSizer->Add(nextButton, 0, wxALL|wxALIGN_CENTRE, 10);
	bottomSizer->Add(cancelButton, 0, wxALL|wxALIGN_CENTRE, 10);
	bottomSizer->AddSpacer(300);
	bottomSizer->Add(rollbackButton, 0, wxFIXED_MINSIZE|wxALIGN_CENTRE, 10);

	topSizer->Add(_pOperHisgrid, 0, wxEXPAND | wxALL, 2);
	topSizer->AddSpacer(2);
	topSizer->Add(bottomSizer, 0, wxEXPAND | wxALL, 2);


	SetSizerAndFit(topSizer);
}

//��ȡ�ֽ�䶯��ļ�¼
int OperHistoryDialog::GetCashFlowData(vector<CASHFLOW>& vecOut){
	
	//��ѯ�ֽ�
	Runtime::getInstance()->sqlite.setSql(QRY_CASH_FLOW);
	Runtime::getInstance()->sqlite.prepare();
	Runtime::getInstance()->sqlite.bindInt(1, Runtime::getInstance()->CurComposeID);

	while ( 1 == Runtime::getInstance()->sqlite.step() ) {
		CASHFLOW tmpCashFlow;
		tmpCashFlow.keyid = Runtime::getInstance()->sqlite.getColumnInt(0);
		tmpCashFlow.composeid = Runtime::getInstance()->sqlite.getColumnInt(1);
		tmpCashFlow.operType = Runtime::getInstance()->sqlite.getColumnInt(2);
		tmpCashFlow.changeCash = Runtime::getInstance()->sqlite.getColumnDouble(3);
		tmpCashFlow.afterCash = Runtime::getInstance()->sqlite.getColumnDouble(4);
		tmpCashFlow.fundShare = Runtime::getInstance()->sqlite.getColumnDouble(5);
		tmpCashFlow.changeTime = Runtime::getInstance()->sqlite.getColumnString(6);
		tmpCashFlow.reason = Runtime::getInstance()->sqlite.getColumnString(7);

		vecOut.push_back(tmpCashFlow);
	}

	return vecOut.size();
}

void OperHistoryDialog::OnRollback(wxCommandEvent& event)//release ģʽ�£����������ȡ�����ؼ���ֵ
{
	CASHFLOW rollbackData = *_vecCashFlowData.begin();

	if(rollbackData.operType == sellStockType){
		wxMessageBox("�����ݲ�֧�ֻع������ֹ��ع��ֽ��ͽ��ױ�");//��Ϊ����ع�����ɾ���������׼�¼������ͬ��ǰ�������¼һ��ɾ��
		return;
	}

	//ֻ�ܻع�����Ĳ���������������ѵǼ���ֵ���ݲ�֧�ֻع�
	wxDateTime datetime(wxDateTime::GetTimeNow());
	string todayStr = datetime.Format("%Y-%m-%d");
	string rollDay = rollbackData.changeTime.substr(0,10);
	if(todayStr != rollDay){
		wxMessageBox("ֻ֧�ֻع�����Ĳ���");
		return;
	}

	wxMessageBox("ע�⣺ֻ�ܻع����һ������ֻ�����ڼ�¼��ֵ֮ǰ��������ǣ����˳�����");

	if(rollbackData.operType == buyStockType){//����Ҫɾ�����׼�¼
		Runtime::getInstance()->sqlite.setSql(DEL_TRADE_INFO);
		Runtime::getInstance()->sqlite.prepare();

		if (Runtime::getInstance()->sqlite.step() < 0) {
			wxMessageBox(Runtime::getInstance()->sqlite.errString);
			return;
		}

		MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
		myFrame->RefreshLogInfo();
		myFrame->RefreshGoodsAndValue();
	}


	Runtime::getInstance()->sqlite.setSql(DEL_CASH_FLOW);
	Runtime::getInstance()->sqlite.prepare();
	Runtime::getInstance()->sqlite.bindInt(1, rollbackData.keyid);

	if (Runtime::getInstance()->sqlite.step() < 0) {
		wxMessageBox(Runtime::getInstance()->sqlite.errString);
		return;
	}

	_vecCashFlowData.clear();
	GetCashFlowData(_vecCashFlowData);
	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);
	_pOperHisgrid->Refresh();
	wxMessageBox("�ع��ɹ���");
}

void OperHistoryDialog::OnNext(wxCommandEvent& event)
{
	if(_vecCashFlowData.size() <= curPage*numOnePage ){
		return;
	}
	++curPage;
	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);

	_pOperHisgrid->AutoSizeRows();
	_pOperHisgrid->AutoSizeColumns();
}

void OperHistoryDialog::OnPrevious(wxCommandEvent& event)
{
	if(curPage==1){
		return;
	}

	--curPage;
	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);

	_pOperHisgrid->AutoSizeRows();
	_pOperHisgrid->AutoSizeColumns();

}

void OperHistoryDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

/*��ȡ��Nҳ������
*@vSrc������Դ������gridStrTab��grid��������Դ��numInOnePage��һҳ�е�������pageNum����䵽grid����ҳ��
*/
void OperHistoryDialog::setPageFromVector2GridTable(vector<CASHFLOW>& vSrc, wxGridStringTable* gridStrTab,
													int numInOnePage, int pageNum)
{	
	vector<CASHFLOW> tmpSrc;
	for(int i=0; i<vSrc.size(); ++i) { //���˵����ǵ�ǰ��ϵ���Ϣ
		if(vSrc[i].composeid == Runtime::getInstance()->CurComposeID){
			tmpSrc.push_back(vSrc[i]);
		}
	}

	if (tmpSrc.size() == 0 && gridStrTab->GetNumberRows()<=1) { //�Է���־��¼Ϊ��ʱ��û�����ݵ��½��汻Ť�������
		gridStrTab->AppendRows(8);
		return;
	}

	if( tmpSrc.size() < numInOnePage * (pageNum - 1) ){ //��¼��С����һҳ����������ֱ�ӷ���
		return;
	}

	gridStrTab->Clear();
	//����gridStringTable�е�����ΪnumInOnePage
	if(gridStrTab->GetNumberRows() > getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum)){
		gridStrTab->DeleteRows(0, gridStrTab->GetNumberRows() - getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum));

	}else{
		gridStrTab->AppendRows(getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum) - gridStrTab->GetNumberRows());
	}

	vector<CASHFLOW>::iterator iter = tmpSrc.begin();
	iter += numInOnePage * (pageNum - 1);// ��ת��pageNumҳ�ļ�¼
	for(int i = 0; i < getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum) && iter != tmpSrc.end(); ++i, ++iter){
		gridStrTab->SetValue(i,0,iter->changeTime);
		gridStrTab->SetValue(i,1,wxString::Format("%d", iter->composeid));
		gridStrTab->SetValue(i,2,GetNameOfOperType(iter->operType));
		gridStrTab->SetValue(i,3,wxString::Format("%g", (iter->afterCash - iter->changeCash)));
		gridStrTab->SetValue(i,4,wxString::Format("%g", (iter->changeCash)));
		gridStrTab->SetValue(i,5,wxString::Format("%g", (iter->afterCash)));
		gridStrTab->SetValue(i,6,wxString::Format("%g", (iter->fundShare)));
		gridStrTab->SetValue(i,7,iter->reason);
	}
}

