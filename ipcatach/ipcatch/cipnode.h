#ifndef CIPNODE_H
#define CIPNODE_H

class CIPNode
{
public:
    CIPNode(unsigned long dwSourIP, unsigned long dwDestIP, unsigned char chPro);

    CIPNode *pNext;
    void addCount();
    unsigned long getDestIPAddr();
    unsigned long getCount();
    unsigned long getSourIPAddr();
    unsigned char getProtocol();

private:
    unsigned long m_dwSourceIPAddr;  //源地址
    unsigned long m_dwDestIPAddr;    //目的地址
    unsigned char m_chProtocal;      //IP包协议类型
    unsigned long m_dwCouter;        //数据包的数量
};

#endif // CIPNODE_H
