#ifndef ASTMANAGER_CRTDATA_H
#define ASTMANAGER_CRTDATA_H

#include <winsock.h>
#include "common.h"
#include <string>
#include <vector>
#include <map>
//#include <WinSock2.h>
//#include <ws2def.h>
#include "base64_algo.h"
#include <assert.h>
#include <math.h>

using namespace std;

//������������ȡip��ַ
int Domain2Ip(const char * url, char* ipaddr);
int getHttpHeadLength(const char* src);
//��ȡhttpЭ������ݳ����ֶ�,-1:can not find�������chunked���͵����ݣ�����1
int getHttpDataLength(const char* src, int srcLen);
//��ȡ��һ��chunk��ĳ��ȣ�src�����������ݿ飬�����Ѿ�ȥ��httpͷ����
int getHttpChunkLength(const char* dataSrc);
int hex_char_value(char c);
int hex_to_decimal(const char* szHex, int len);
int checkHttpHead(const char* src, int& code);

////��ʽ��MIKO INTL,�׸���ʿع�,0.640,0.630,0.640,0.610,0.630,0.000,0.000,0.610,0.630,247620,402000,3.520,5.079,1.280,0.600,2015/09/25,16:01
//PARKSON GROUP,��ʢ����,1.060,1.030,1.080,1.020,1.060,0.030,2.913,1.060,1.070,6042972,5714165,0.000,4.717,2.380,0.990,2015/09/25,16:01
struct RTInfo{
    std::string name;
    float price; //�ּ�
    //time_t datetime; //�۸�ʱ��
};

/*
�ʲ���ծ��
������
��������
�������ʲ�
�����ʲ�
������ծ
�������ʲ�/(��ծ)
��������ծ
�����ɶ�Ȩ�� - (��)/��
���ʲ�/(��ծ)
�ѷ��йɱ�
����
�ɶ�Ȩ��/(����)
�����ʲ�(�������ʲ�)
��ҵ���������豸(�������ʲ�)
������˾Ȩ��(�������ʲ�)
��Ӫ��˾Ȩ�� (�������ʲ�)
����Ͷ��(�������ʲ�)
Ӧ���˿�(�����ʲ�)
���(�����ʲ�)
�ֽ����н��(�����ʲ�)
Ӧ���ʿ�(������ծ)
���д���(������ծ)
���������д���
���ʲ�
�ܸ�ծ
�ɷ���Ŀ
����
*/
struct BalanceData{
    string stock_id;
    string report_time;
    string report_type;
    double static_asset;
    double flow_asset;
    double flow_debt;
    double netflow_asset;
    double static_debt;
    double fewholder_asset;
    double net_asset;
    double guben;
    double chubei;
    double holder_asset; //��λΪ����
    double invisible_asset;
    double building_device;
    double childcort_asset;
    double sibing_asset;
    double other_invest;
    double yingshou;
    double cunhuo;
    double cash;
    double yingfu;
    double flow_bankdept;
    double static_bankdept;
    double all_asset;
    double all_dept;
    double stock_share;
    string bizhong;
    double bond; //���е�ծȯ
};

/*
�ۺ������
������
��������
Ӫҵ��
��˰ǰӯ��/(����)
˰��
��˰��ӯ��/(����)
�����ɶ�Ȩ��
�ɶ�Ӧռӯ��/(����)
��Ϣ
��˰����Ϣ��ӯ��/(����)
����ÿ��ӯ��(��)
̯��ÿ��ӯ��(��)
ÿ�ɹ�Ϣ(��)
���۳ɱ�
�۾�
���ۼ���������
һ�㼰��������
��Ϣ����/���ʳɱ�
ë��
��Ӫӯ��
Ӧռ��Ӫ��˾ӯ��
����
*/
struct SunYiData{
    string stock_id;
    string report_time;
    string report_type;
    double yingyee;
    double untax_profit;
    double tax;
    double tax_profit;
    double fewholder_profit;
    double holder_profit;
    double dividend;
    double net_profit;
    double basic_one_profit;
    double tanxiao_one_profit;
    double ont_dividend;
    double cost;
    double depreciation;
    double sell_fee;
    double admin_fee;
    double lixi_fee;
    double gross_profit;
    double jingying_profit;
    double othercorp_profit;
    string bizhong;
};

/*
�ֽ�������
������
��������
��Ӫҵ������֮�ֽ����뾻��
Ͷ�ʻ֮�ֽ����뾻��
���ʻ֮�ֽ����뾻��
�ֽ��ֽ�ȼ�������
������֮�ֽ��ֽ�ȼ���
�������֮�ֽ��ֽ�ȼ���
���һ��ʱ䶯֮Ӱ��
���ù̶��ʲ�����
����
*/
struct CashFlowData{
    string stock_id;
    string report_time;
    string report_type;
    double jingying_net;
    double invest_net;
    double rongzi_net;
    double cash_add;
    double cash_begin;
    double cash_end;
    double waihui_profit;
    double buy_device;
    string bizhong;
};

//ʵʱ���ݳ�����
class CRTData {
public:
   virtual int RefreshRtData(); //��ȡ���й�Ʊ�۸�

   //��ȡ����stock�۸�return 1:����ȡ�������У� 0: ����ȡ�ꣻ-1���쳣����
   int GetAllStockPrice(std::map<std::string, RTInfo>& mapOut, int& curpos);

   int GetRtDataFromHttp(string& allCode, std::map<std::string, RTInfo>& mOutData);

   virtual int GetHttpData(std::string url, std::string& allGetData, FILE* flstm = NULL) = 0;
   virtual int getOneReportData(string url, string& allGetData){return 0;}
   
   //get balance report, param[in] reportType : 0: �걨�� 1���б�
   int GetBalanceReport(string stock_id, int reportType, vector<BalanceData>& vecOneAllBalance);
   //get sun yi report, param[in] reportType : 0: �걨�� 1���б�
   int GetSunYiReport(string stock_id, int reportType,vector<SunYiData>& vecOneAllSunYi);
   //get cash flow report, param[in] reportType : 0: �걨�� 1���б�
   int GetCashFlowReport(string stock_id, int reportType,vector<CashFlowData>& vecOneAllCashFlow);

   virtual int ConnectSepSocket(string& hostName) = 0;//����ʵ������socket����

   //never use, use dnews process
   void LoadOneDayNews( wxDateTime &datetime, string outDir, int beginNum=0, int endNum=999);//����һ��ָ����Χ������

   std::map<std::string, RTInfo>& getData(){
       return m_mOutData;
   }

   void setAllCode(std::vector<std::string>& data){
       m_vCode = data;
   }

   void addOneCode(std::string stockCode){
       if(stockCode != ""){
            m_vCode.push_back(stockCode);
       }
   }

   virtual int resetSocket(string& hostName){
       closesocket(m_Socket);
       WSACleanup();
       return ConnectSepSocket(hostName);
   }

protected:

    int ConnectSocket(std::string proxyIP, int proxyPort); //����socket���Ӻ���

    //��ȡsocket�׽ӿ������н��ջ����������ݣ����ػ�ȡ�����ֽ���
    int Recv(int sockfd, string& allGetData, int& code, FILE* flstm = NULL);

    SOCKET m_Socket; //��Ҫ���ӵ���������sock���ӣ��д�����Ǵ����������û�������host�ķ�����
    std::vector<std::string> m_vCode;

    std::map<std::string, RTInfo> m_mOutData;
};

//ʵʱ�����д���
class CRTDataProxy : public CRTData{
public:
    CRTDataProxy(char* proxyIP, int proxyPort,const char* username, const char* pwd);

    virtual int GetHttpData(std::string url, std::string& allGetData, FILE* flstm = NULL);//����http��Ϣ

    int ConnectSepSocket(string& hostName){ //��ʱ��hostNameû�ã���ȫΪ�˼��ø���ӿ�
        return ConnectSocket(m_proxyIP, m_proxyPort);
    }
protected:

    std::string m_proxyIP;//�����������ip��ַ
    int m_proxyPort; //����������Ķ˿ں�
    std::string m_proxyUserName; //������������û���
    std::string m_proxyPassword; //���������������
};

//ʵʱ���� ��ʹ�ô���������汾
class CRTDataNoProxy : public CRTData {
public:

    CRTDataNoProxy(int port);
    virtual int GetHttpData(std::string stockCode, std::string& allGetData, FILE* flstm = NULL);

    int ConnectSepSocket(string& hostName);

    string m_hostIp;
    int m_hostPort;
};

#endif //ASTMANAGER_CRTDATA_H