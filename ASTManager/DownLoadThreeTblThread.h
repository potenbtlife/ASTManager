//�߳���ش��룬�������cpp��main.cpp��ʹ�ã�Ϊ�˾��������ļ���������

#ifndef AST_DOWNLOADTHREETBLTHREAD_H
#define  AST_DOWNLOADTHREETBLTHREAD_H


#include "wx/thread.h"

class DownLoadThreeTblThread : public wxThread
{
public:
    DownLoadThreeTblThread(MyFrame *frame, int reportType): wxThread(),m_reportType(reportType) {
        m_frame = frame;
    };

    // thread execution starts here
    virtual void *Entry(){
        //Runtime::getInstance()->myTimer.Stop();//��ֹͣ��ѯ�������������ռ�������ݿ⣬���³����⡣
        
        wxLogDebug("begin DownLoadThreadTblThread::Entry");
        m_frame->WriteThreeRptToDb(m_reportType);
        wxLogDebug("end DownLoadThreadTblThread::Entry");
        wxMessageBox("���ر��������");

        //Runtime::getInstance()->myTimer.Start(1000*60);//1���Ӵ���һ��

        return NULL;
    };

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit(){};

public:
    MyFrame *m_frame;
    int m_reportType;
};


#endif