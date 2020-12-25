#include "cnodelist.h"

CNodeList::CNodeList()
{
    pHead = pTail = nullptr;
}
CNodeList::~CNodeList()
{
    if(pHead != nullptr)
    {
        CIPNode *pTemp = pHead;
        pHead = pHead->pNext;
        delete pTemp;
    }
}
void CNodeList::addNode(unsigned long dwSourIP, unsigned long dwDestIP, unsigned char chPro)
{
    if(pHead == nullptr)
    {
        pTail = new CIPNode(dwSourIP,dwDestIP,chPro);
        pHead = pTail;
        pTail->pNext = nullptr;
    } else {
        CIPNode *pTemp;
        for(pTemp = pHead;pTemp;pTemp = pTemp->pNext)
        {
            if(pTemp->getSourIPAddr() == dwSourIP &&
               pTemp->getDestIPAddr() == dwDestIP &&
               pTemp->getProtocol() == chPro)
            {
                pTemp->addCount();
                break;
            }
        }

        if(pTemp == nullptr)
        {
            pTail->pNext = new CIPNode(dwSourIP,dwDestIP,chPro);
            pTail = pTail->pNext;
            pTail->pNext = nullptr;
        }
    }
}

