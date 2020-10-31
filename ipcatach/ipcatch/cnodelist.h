#ifndef CNODELIST_H
#define CNODELIST_H

#include "cipnode.h"

class CNodeList
{
public:
    CNodeList();
    ~CNodeList();
    void addNode(unsigned long dwSourIP, unsigned long dwDestIP, unsigned char chPro);
private:
    CIPNode *pHead;
    CIPNode *pTail;
};

#endif // CNODELIST_H
