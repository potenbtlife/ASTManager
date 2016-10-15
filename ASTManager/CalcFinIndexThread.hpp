//�߳���ش��룬�������cpp��main.cpp��ʹ�ã�Ϊ�˾��������ļ���������

#ifndef AST_MYTHREAD_H
#define  AST_MYTHREAD_H


#include "wx/thread.h"

class CalcFinanceIndexThread : public wxThread
{
public:
    CalcFinanceIndexThread(): wxThread() {
    };

    // thread execution starts here
    virtual void *Entry(){
        wxLogDebug("begin CalcFinanceIndexThread::Entry");

        DeleteFinanceIndexFromDb("�걨");
        FinanceFrame::CalcAllFinanceIndex("�걨");
        DeleteFinanceIndexFromDb("�б�");
        FinanceFrame::CalcAllFinanceIndex("�б�");
        //wxThread::Sleep(2000);

        wxLogDebug("end CalcFinanceIndexThread::Entry");
        wxMessageBox("����ָ�������");
        return NULL;
    };

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit(){};

};


#endif