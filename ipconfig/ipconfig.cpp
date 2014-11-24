/*
    This is a recreation of the Windows ipconfig.exe tool. I wrote this mostly for myself 
    to learn more about Windows command line tools and to maybe help others who want to 
    know how some of the Widows tools work.


***********************************
THIS CODE IS NOT FINISHED!!!!!!!
***********************************



*/

#include <stdio.h>
#include "ipconfig.h"
#include <WinSock2.h>
#include <Windows.h>
#include <Iphlpapi.h>


void ConvertIPToString(SOCKET_ADDRESS *sock, WCHAR **buffer, DWORD *bufferLen)
{
    /**
        Convert an IP address to a string and return it.

        @param sock      [IN]       Socket address to convert to a string.
        @param buffer    [IN/OUT]   Pointer to IP address string.
        @param bufferLen [IN/OUT]   Length of the provided buffer. 
    */
    DWORD addrToStrReturn = 0;  

    addrToStrReturn = WSAAddressToString(sock->lpSockaddr,
                                         sock->iSockaddrLength,
                                         NULL,
                                         *buffer,
                                         bufferLen);
    if (0 != addrToStrReturn)
    {
        VOID *savedPtr = *buffer; 
        *buffer = (WCHAR *)realloc(*buffer, *bufferLen * sizeof(WCHAR)); 
        if (NULL == *buffer)
        {
            free(savedPtr); 
            return; 
        }

        addrToStrReturn = WSAAddressToString(sock->lpSockaddr,
                                             sock->iSockaddrLength,
                                             NULL,
                                             *buffer,
                                             bufferLen);
        if (0 != addrToStrReturn)
        {
            printf("Failed to convert IP address to string.\n");
            free(*buffer);
            *buffer = NULL;
            bufferLen = 0; 
            return;                 
        }
    }
}


void PrintAdapterInfo(IP_ADAPTER_ADDRESSES *adapter)
{
    /**
    
    Print relevant information about the adapter given.

    @param  adapter [IN] IP_ADAPTER_ADDRESS structure to print information from. 

    */
    WSADATA wsaData = { 0 };
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    printf("%S\n", adapter->FriendlyName);
    printf("Description: %S\n", adapter->Description);

    IP_ADAPTER_UNICAST_ADDRESS *currAddr = adapter->FirstUnicastAddress; 

    while (NULL != currAddr)
    {
        WCHAR *ipAddress = (WCHAR *)calloc(1, sizeof(WCHAR)); 
        DWORD ipAddrLen = 1; 
        ConvertIPToString(&(currAddr->Address), &ipAddress, &ipAddrLen);
        printf("IPv4 Address: %S\n", ipAddress); 
        free(ipAddress); 
        currAddr = currAddr->Next; 
    }

    IP_ADAPTER_DNS_SERVER_ADDRESS *currDNS = adapter->FirstDnsServerAddress; 
    while (NULL != currDNS)
    {
        WCHAR *ipAddress = (WCHAR *)calloc(1, sizeof(WCHAR));
        DWORD ipAddrLen = 1;
        ConvertIPToString(&(currDNS->Address), &ipAddress, &ipAddrLen);
        printf("DNS Address: %S\n", ipAddress);
        free(ipAddress);
        currDNS = currDNS->Next;
    }

    printf("-------------------------------------------\n\n"); 

    WSACleanup();
}

void GetAdapterInfo()
{
    /*
        GetIPv4AdapterInfo
        Print relevant information about IPv4 adapters on the machine. 
    */
    IP_ADAPTER_ADDRESSES *adapterAddr = NULL;
    IP_ADAPTER_ADDRESSES *currAdapter = NULL;
    ULONG adapterAddrLen = 0;
    ULONG adapterAddrReturn = 0;

    adapterAddrReturn = GetAdaptersAddresses(AF_INET,
                                             0,
                                             NULL,
                                             adapterAddr,
                                             &adapterAddrLen);

    if (ERROR_BUFFER_OVERFLOW != adapterAddrReturn)
    {
        printf("GetAdaptersAddresses Error: %08x\n", adapterAddrLen);
        goto cleanup;
    }

    adapterAddr = (IP_ADAPTER_ADDRESSES *)malloc(adapterAddrLen);
    if (NULL == adapterAddr)
    {
        printf("Malloc error.\n");
        goto cleanup;
    }

    adapterAddrReturn = GetAdaptersAddresses(AF_INET,
                                             0,
                                             NULL,
                                             adapterAddr,
                                             &adapterAddrLen);

    if (ERROR_SUCCESS != adapterAddrReturn)
    {
        printf("GetAdaptersAddresses Error: %08x\n", adapterAddrReturn);
        goto cleanup;
    }

    currAdapter = adapterAddr;

    while (NULL != currAdapter)
    {

        PrintAdapterInfo(currAdapter);
        currAdapter = currAdapter->Next; 

    }


cleanup:

    free(adapterAddr);
}

int main()
{
    /*
        
    */
    GetAdapterInfo();
	return 0; 
}