#include "common.h"

vector<DataContainer> gVecAllData; //�������е���ϸ����
const unsigned int lineNumEachPage = 12; //ÿҳ������
unsigned int curPage = 1; //��ǰҳ��

void fillPageFromVector2GridTable(vector<DataContainer>& vSrc, wxGridStringTable& gridStrTable, unsigned int numInOnePage, unsigned int pageNum) {
    
    if( vSrc.size() < numInOnePage * (pageNum - 1) ){ //��¼��С����һҳ����������ֱ�ӷ���
        return;
    }

    gridStrTable.Clear();
    //����gridStringTable�е�����ΪnumInOnePage
    if(gridStrTable.GetNumberRows() > getRowNumInOnePage(vSrc, numInOnePage, pageNum)){
        gridStrTable.DeleteRows(0, gridStrTable.GetNumberRows() - getRowNumInOnePage(vSrc, numInOnePage, pageNum));
    }else{
        gridStrTable.AppendRows(getRowNumInOnePage(vSrc, numInOnePage, pageNum) - gridStrTable.GetNumberRows());
    }

    vector<DataContainer>::iterator iter = vSrc.begin();
    iter += numInOnePage * (pageNum - 1);// ��ת��pageNumҳ�ļ�¼
    for(int i = 0; i < getRowNumInOnePage(vSrc, numInOnePage, pageNum) && iter != vSrc.end(); ++i, ++iter){

        char cWeekOfYear[16];
        memset(cWeekOfYear,'0',sizeof(cWeekOfYear));
        sprintf(cWeekOfYear,"��%d��",iter->iWeekofYear);
        gridStrTable.SetValue(i,0,cWeekOfYear);
        gridStrTable.SetValue(i,1,GetDayOfWeek(iter->idayOfWeek));
        gridStrTable.SetValue(i,2,iter->startTime);
        gridStrTable.SetValue(i,3,iter->endTime);
        gridStrTable.SetValue(i,4,timeConvert(iter->usedTimeSec));
        gridStrTable.SetValue(i,5,iter->doTings);
        gridStrTable.SetValue(i,6,iter->priority);
    }
}
