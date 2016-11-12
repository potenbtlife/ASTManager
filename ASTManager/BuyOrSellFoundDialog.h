#ifndef ASM_BUGORSELLFOUNDDIALOG_H
#define ASM_BUGORSELLFOUNDDIALOG_H

#include <wx/dialog.h>
#include "Runtime.h"

enum BUYORSELLFOUNDDIALOGID{
    ID_AMT_TEXTCTRL,
    ID_ADVICE_TEXTCTRL,
    ID_BUYORSELLFOUNDDIALOGID_OK_BUTTON
};

class BuyOrSellFoundDialog : public wxDialog{
public:

    //���캯��,type:0 �깺��1�����
    BuyOrSellFoundDialog(int type, wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr) : wxDialog(parent,id,title,pos,size,style,name), m_type(type){

        if(m_type == 0){
            new wxStaticText(this, -1, "�깺���:",wxPoint(10, 10),wxSize(60, 20));
        }else {
            new wxStaticText(this, -1, "��ؽ��:",wxPoint(10, 10),wxSize(60, 20));
        }

        amtCtrl = new wxTextCtrl(this, ID_AMT_TEXTCTRL, "", wxPoint(70,10),wxSize(120,20));

        new wxStaticText(this, -1, "ԭ��:",wxPoint(10, 40),wxSize(60, 20));
        adviceCtrl = new wxTextCtrl(this, ID_ADVICE_TEXTCTRL, "", wxPoint(70,40),wxSize(120,60), wxTE_MULTILINE);

        okbutton = new wxButton(this, ID_BUYORSELLFOUNDDIALOGID_OK_BUTTON, "ȷ��", wxPoint(80, 120), wxSize(60,30));
    }


    void OnOk(wxCommandEvent& event){
        string amtStr = amtCtrl->GetValue();
        string reason = adviceCtrl->GetValue();
        double changeCash = atof(amtStr.c_str());
        
        if((changeCash > -0.005 && changeCash < 0.005) || reason == "") {
            wxMessageBox("������󣬲���Ϊ��!");
            return;
        }

        double curCash=0, curShare=0;
        qryCashAndShare(Runtime::getInstance()->CurComposeID, curCash, curShare);

        string datetime,value_advice, detailInfo;
        double fundShare=0, fundValue=0,marketvalue=0,cash=0;
        qryValueInfo(Runtime::getInstance()->CurComposeID, datetime, value_advice, detailInfo, fundShare, fundValue,marketvalue,cash);//��ȡvalue_info���ݣ���Ҫ��Ϊ�˻�ȡ��ֵ����

        string reasonStr;
        double newShare=0, newCash=0;
        if (m_type == 0){
            newCash = curCash + changeCash;
            double addShare = changeCash/fundValue;
            newShare = curShare + addShare;
            reasonStr = "�깺��" + trim(reason);
        }else{
			changeCash = -1*changeCash;
            newCash = curCash + changeCash;
            double reduceShare = changeCash/fundValue;
            newShare = curShare - reduceShare;
            reasonStr = "��أ�" + trim(reason);
        }
        
        InsertCashRecord(Runtime::getInstance()->CurComposeID, changeCash, newCash, newShare, reasonStr);

        Destroy();
    }

    wxTextCtrl* amtCtrl;
    wxTextCtrl* adviceCtrl;
    wxButton* okbutton;
    int m_type; //���ͣ�0���깺�� 1�����

    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(BuyOrSellFoundDialog, wxDialog)    
EVT_BUTTON(ID_OK_BUTTON, OnOk)
END_EVENT_TABLE()

#endif //ASM_BuyOrSellFoundDialog_H