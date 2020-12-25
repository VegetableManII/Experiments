#include "cipnode.h"

CIPNode::CIPNode(unsigned long dwSourIP, unsigned long dwDestIP, unsigned char chPro)
    : m_dwSourceIPAddr(dwSourIP) , m_dwDestIPAddr(dwDestIP) , m_chProtocal(chPro)
{
    m_dwCouter = 1;
}
void CIPNode::addCount()
{
    m_dwCouter++;
}
unsigned long CIPNode::getCount()
{
    return m_dwCouter;
}
unsigned long CIPNode::getDestIPAddr()
{
    return m_dwDestIPAddr;
}
unsigned long CIPNode::getSourIPAddr()
{
    return m_dwSourceIPAddr;
}
unsigned char CIPNode::getProtocol()
{
    return m_chProtocal;
}
