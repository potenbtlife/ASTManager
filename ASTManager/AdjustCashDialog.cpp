#include "AdjustCashDialog.h"
#include "main.h"

//ע�����봰�ڵ��¼�������
BEGIN_EVENT_TABLE(AdjustCashDialog, wxDialog)    
EVT_BUTTON(ID_ADJUSTCASHCTRL_OK, AdjustCashDialog::OnOK)
EVT_BUTTON(ID_ADJUSTCASHCTRL_CANCEL, AdjustCashDialog::OnCancel)
END_EVENT_TABLE()

//���봰�ڹ��캯��
AdjustCashDialog::AdjustCashDialog(wxWindow *parent,
                     wxWindowID id, 
                     const wxString& title,
                     const wxPoint&  pos, 
                     const wxSize& size,
                     long  style,
                     const wxString& name)
                     :wxDialog(parent,id,title,pos,size,style,name), m_curCashVaule(0), m_curShare(0)
{
    wxStaticText *buyNameText = new wxStaticText(this, -1, "�����ֽ����:",wxPoint(20, 10),wxSize(100, 20));
    wxTextCtrl* curCashCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 10),wxSize(100, 20), wxTE_READONLY);
    
    wxStaticText *buyPriceText = new wxStaticText(this, -1, "����������ֽ�:",wxPoint(20, 35),wxSize(100, 20));
    m_nowCashCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 35),wxSize(100, 20));

    wxStaticText *fundShareText = new wxStaticText(this, -1, "���������ݶ�:",wxPoint(20, 60),wxSize(100, 20));
    m_nowShareCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 60),wxSize(100, 20));
    
	new wxStaticText(this, -1, "������ծ��:",wxPoint(20, 85),wxSize(100, 20));
	m_debetCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 85),wxSize(100, 20));

    new wxStaticText(this, -1, "����ԭ��:",wxPoint(20, 110),wxSize(100, 20));
    m_reasonCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 110),wxSize(120, 60), wxTE_MULTILINE);

    wxButton* okButton = new wxButton(this, ID_ADJUSTCASHCTRL_OK,"ȷ��", wxPoint(70,190), wxSize(60,30));
    wxButton* cancelButton = new wxButton(this, ID_ADJUSTCASHCTRL_CANCEL,"ȡ��", wxPoint(160,190), wxSize(60,30));

    qryCashAndShare(Runtime::getInstance()->CurComposeID, m_curCashVaule, m_debet, m_curShare);
    
    char ctmp[128] = {0};
    sprintf(ctmp, "%.2lf", m_curCashVaule);
    curCashCtrl->SetValue(ctmp); //�ֽ�ؼ�

	memset(ctmp,0,sizeof(ctmp));
    sprintf(ctmp, "%.2lf", m_curShare);
    m_nowShareCtrl->SetValue(ctmp); //����ݶ�ؼ�
	
	memset(ctmp,0,sizeof(ctmp));
	sprintf(ctmp, "%.2lf", m_debet);
	m_debetCtrl->SetValue(ctmp); //ծ��ؼ�
}

void AdjustCashDialog::OnOK(wxCommandEvent& event)
{
    string modifyCashStr = m_nowCashCtrl->GetValue();
    string modifyShareStr = m_nowShareCtrl->GetValue();
    string reasonStr = m_reasonCtrl->GetValue();
	string strDebet = m_debetCtrl->GetValue();

    double modifyCash = 0, modifyShare = 0, modifyDebet=0;
    sscanf(modifyCashStr.c_str(), "%lf", &modifyCash);
    sscanf(modifyShareStr.c_str(), "%lf", &modifyShare);
	sscanf(strDebet.c_str(), "%lf", &modifyDebet);

    if( (modifyCash<0.005 && modifyCash>-0.005) || "" == reasonStr || (modifyShare < 0.05 && modifyShare > -0.05)){
        wxMessageBox( "����ѡ���Ϊ�գ����������룡","�������", wxOK | wxICON_ERROR );
        return;
    }

    double diffShare = modifyShare - m_curShare;
    if (diffShare < 0.5 && diffShare > -0.5) { //�������ת�����·ݶ�ľ��ȶ�ʧ��ʹ��ԭ���ķݶ�ֵ�Է�ֹ���ȶ�ʧ��
        modifyShare = m_curShare;
    }
    InsertCashRecord(Runtime::getInstance()->CurComposeID, AdjustBalance, (modifyCash-m_curCashVaule), modifyCash,modifyDebet, modifyShare, reasonStr);//����������Ϊ0���Ժ��ٲ���

    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshGoodsAndValue();

    Destroy();
}

void AdjustCashDialog::OnCancel(wxCommandEvent& event)
{
    Destroy();
    //Close(false);
}

//������ʱ�ֽ�ݶ��
int AdjustCashDialog::BuyUpdateCash(double changeCash){
/*
    //�������ݿ⣬�����ݿ��Ϊ���ձ�׼
    gSqlite.setSql(BuyUpdateTmpSql);
    gSqlite.prepare();
    gSqlite.bindDouble(1, changeCash);
    gSqlite.bindInt(2, Runtime::getInstance()->CurComposeID);

    if(gSqlite.step() < 0 && gSqlite.finalize()){ //дʧ�ܣ�
        wxMessageBox(gSqlite.errString);
        return -1;
    };
    gSqlite.finalize();

    //���½�����ʾ
    wxTextCtrl* cashCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(ID_CASH_TEXTCTRL);
    string sCash = cashCtrl->GetValue();
    double fCash = atof(sCash.c_str()); //�ֽ�
    double allCash = fCash - changeCash;
    char cAllCash[16] = {0};
    sprintf(cAllCash, "%.2f", allCash);
    cashCtrl->SetValue(cAllCash);
*/
    return 1;
}

