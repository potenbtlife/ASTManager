#ifndef DOWNLOADONETBL_H
#define DOWNLOADONETBL_H

using namespace std;


enum
{
	ID_DLOT_OK,
	ID_DLOT_CANCEL
};

//���� BuyDialog ����صĶ���
class DownLoadOneTblDialog: public wxDialog
{
public:
	//���캯��
	DownLoadOneTblDialog(string& stockId,
        wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

    
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();

private:
    wxTextCtrl *_codeCtrl;
    string& _stockId;
};


#endif //DOWNLOADONETBL_H