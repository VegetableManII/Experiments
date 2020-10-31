#ifndef DEF_H
#define DEF_H
#pragma once
#include <Windows.h>
#include <iostream>
#include <iomanip>

using namespace std;

typedef struct {
    unsigned char hdrlen_ver;
    unsigned char tos;
    unsigned short total_len;
    unsigned short identifier;
    unsigned short frag_and_flags;
    unsigned char ttl;
    unsigned char service;
    unsigned short checksum;
    unsigned long sourceIP;
    unsigned long destIP;
}IP_HEADER;

typedef struct {
    BYTE type;
    BYTE code;
    USHORT cksum;
    USHORT id;
    USHORT seq;
}ICMP_HEADER;
USHORT checksum(USHORT* pBuf, int iSize)
{
    unsigned long cksum = 0;
    while (iSize > 1)
    {
        cksum += *pBuf++;
        iSize -= sizeof(USHORT);
    }
    if (iSize)
    {
        cksum += *(UCHAR *)pBuf;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT)(~cksum);
}
const BYTE ICMP_ECHO_REQUEST = 8;
const BYTE ICMP_ECHO_REPLY = 0;
const BYTE ICMP_TIMEOUT = 11;
const int DEF_ICMP_DATA_SIZE = 32;
const int MAX_ICMP_PACKET_SIZE = 1024;
const DWORD DEF_ICMP_TIMEOUT = 3000;
const int DEF_MAX_HOP = 30;

typedef struct {
    USHORT usSeqNo;
    DWORD dwRoundTripTime;
    IN_ADDR dwIPaddr;
}DECODE_RESULT;

BOOL DecodeIcmpResponse(char *pBuf, int iPacketSize, DECODE_RESULT&DecodeResult)
{
    int iIpHdrLen = (((IP_HEADER *)pBuf)->hdrlen_ver & 0x0f) * 4;
    IP_HEADER *pIpHdr = (IP_HEADER *)pBuf;
    //根据ICMP报文类型提取ID字段和序列号字段
    ICMP_HEADER* pIcmpHdr = (ICMP_HEADER *)(pBuf + iIpHdrLen);
    USHORT usID, usSquNo;
    if (pIcmpHdr->type == ICMP_ECHO_REPLY)
    {
        usID = pIcmpHdr->id;
        usSquNo = pIcmpHdr->seq;
    }
    else if (pIcmpHdr->type == ICMP_TIMEOUT)
    {
        char * pInnerIpHdr = pBuf + iIpHdrLen + sizeof(ICMP_HEADER);
        int iInnerIPHdrLen = (((IP_HEADER *)pInnerIpHdr)->hdrlen_ver & 0x0f) * 4;
        ICMP_HEADER * pInnerIcmpHdr = (ICMP_HEADER *)(pInnerIpHdr + iInnerIPHdrLen);
        usID = pInnerIcmpHdr->id;
        usSquNo = pInnerIcmpHdr->seq;
    }
    else
    {
        return FALSE;
    }

    if (usID != (USHORT)GetCurrentProcessId() || usSquNo != DecodeResult.usSeqNo)
    {
        return FALSE;
    }
    DecodeResult.dwIPaddr.S_un.S_addr = pIpHdr->sourceIP;
    DecodeResult.dwRoundTripTime = GetTickCount() - DecodeResult.dwRoundTripTime;

    //打印往返时间

    if (DecodeResult.dwRoundTripTime)
    {
        cout << setw(6) << DecodeResult.dwRoundTripTime << "ms" << flush;
    }
    else
    {
        cout << setw(6) << "<1" << "ms" << flush;
    }
    return TRUE;
}
#endif // DEF_H
