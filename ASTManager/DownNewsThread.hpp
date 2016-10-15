//�߳���ش��룬�������cpp��main.cpp��ʹ�ã�Ϊ�˾��������ļ���������

#ifndef AST_DOWNNEWSTHREAD_H
#define  AST_DOWNNEWSTHREAD_H

#include "CRTData.h"
#include "wx/thread.h"

class LoadNewsThread : public wxThread
{
public:
    LoadNewsThread(wxDateTime& beginDate, wxDateTime& endDate, string& dir, int beginNum=0, int endNum=999): wxThread() {
        m_beginDate = beginDate;
        m_endDate = endDate;
        m_dir = dir;
        m_beginNum = beginNum;
        m_endNum = endNum;
    };

    // thread execution starts here
    virtual void *Entry(){
        wxLogDebug("begin LoadNewsThread::Entry");

        //Runtime::getInstance()->myTimer.Stop();//��ֹͣ��ѯ�������������ռ�������ݿ⣬���³����⡣

        CRTData* tmpRtData;
        createRtDataObj(&tmpRtData);

        wxDateSpan dateSpan(0,0,0,1);//��1��

        for(; m_endDate>=m_beginDate; m_endDate.Subtract(dateSpan)){
            tmpRtData->LoadOneDayNews( m_endDate, m_dir, m_beginNum, m_endNum);
        }

        delete tmpRtData;

        //Runtime::getInstance()->myTimer.Start(1000*60);//1���Ӵ���һ��
        wxLogDebug("end LoadNewsThread::Entry");
        wxMessageBox("����news����.");
        return NULL;
    };

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit(){};

public:
    wxDateTime m_beginDate;
    wxDateTime m_endDate;
    string m_dir;
    int m_beginNum; //��ʼ��ţ�Ϊ��֧�ֶ����ͬʱ����
    int m_endNum;  //������ţ�Ϊ��֧�ֶ����ͬʱ����
};


#endif //AST_DOWNNEWSTHREAD_H