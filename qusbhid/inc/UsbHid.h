#pragma once

#include "hidsdi++.h"
#include "setupapi.h"
#include "usb100.h"
#include "WinIoCtl.h"

#pragma pack(push,1)
struct CDeviceSerial
{
	wchar_t serial[13];			/// Device serial number
	WORD	bcdDevVersion;		/// Device hardware version
	WORD	keySign;			/// Device key signature
};
#pragma pack(pop)

#include <vector>
#include <string>
using std::vector;
using std::string;
using std::wstring;
#ifdef	UNICODE
typedef		wstring	xstring;
#else
typedef		string	xstring;
#endif

struct	CHidDevice{
	xstring	m_path;
	xstring	m_desc;
};

xstring		toXString(LPCWSTR str);
xstring		toXString(LPCSTR str);
inline xstring		toXString(const string& str){return toXString(str.c_str());}
inline xstring		toXString(const wstring& str){return toXString(str.c_str());}

/// when pid is 0x0000, ignore it
size_t	EnumHidDevice(WORD vid, WORD pid, vector<CHidDevice>& devs, LPCTSTR lpFilter = NULL);
size_t	EnumHidDevice(WORD vid, WORD pid, vector<CHidDevice>& devs, USAGE usagePage, USAGE usage);
size_t  EnumGuidDevice(LPGUID  pGuid, vector<CHidDevice>& devs);
BOOL    SearchGuidBySN(const LPGUID  pGuid, CHidDevice& dev,const wchar_t* SN);
BOOL	CheckUsage(xstring &path, USAGE usagePage, USAGE usage);

class	CUsbHid
{
public:
	CUsbHid():m_hFile(NULL),m_hListenerThread(NULL)
		,m_fStopping(false),m_hevtOverlappedWorkerThread(NULL)
		,m_pid(0),m_vid(0),m_filter(NULL),m_hParent(NULL),m_bConnect(FALSE)
		,m_curDeviceVer(0),m_usagePage(0),m_usage(0)
		,m_firmwareVer(0),m_firmwareBuild(0){
            memset(m_deviceSN,0,sizeof(m_deviceSN));
            memset(m_devicePath,0,sizeof(m_devicePath)); }
	~CUsbHid(){Close();}
	LONG	Open(LPCTSTR lpDevice);
	LONG	Close();
	LONG	StartListener();
	void	SetStopFlag(){m_fStopping = true;}
	LONG	StopListener(DWORD dwTimeout = INFINITE);
	LONG	Write(const void* pData, size_t len, DWORD* realWrite);
	LONG	Read(void* pData, size_t len, DWORD* realRead);
	LONG	IoControl(void* pData, size_t len, DWORD *realRead);
	LONG	SetFeature(void* pData, size_t len, DWORD *realRead);

	BOOL	ReadUntil(void* pData, DWORD len, DWORD dwTimeout = 1000);
	virtual	void	OnHidData(void* pData,size_t dataLen){}
	virtual	void	OnConnect(LPCTSTR path){}
	virtual	void	OnDisconnect(LPCTSTR path){}
    virtual void    OnBulkData(void* pData,size_t dataLen){}
    virtual void    OnBulkConnect(LPCTSTR path){}
    virtual void    OnBulkDisconnect(LPCTSTR path){}

	LONG	Open(WORD vid, WORD pid, HWND hParent, LPCTSTR filter = NULL);
	LONG	Monitor(WORD vid, WORD pid, HWND hParent, LPCTSTR filter = NULL);

	LONG	Open(WORD vid, WORD pid, HWND hParent, USAGE usagePage, USAGE usage, LPCTSTR filter = NULL);
	LONG	Monitor(WORD vid, WORD pid, HWND hParent, USAGE usagePage, USAGE usage, LPCTSTR filter = NULL);

	LRESULT	OnDeviceChange(WPARAM wParam, LPARAM lParam);

	/// HidD_ functions
public:
	BOOL	GetAttributes(PHIDD_ATTRIBUTES  Attributes);
	BOOL	GetSerialString(void * p, ULONG len);
		/// Get device serial number
	BOOL	GetDeviceSerial(CDeviceSerial& serial);
	/// If success, return the firmware version
	BOOL	GetFirmwareInfo(LPWSTR version);

protected:
	static DWORD WINAPI ThreadProc (LPVOID lpArg);
	DWORD ThreadProc (void);

	LONG	RegisterDevice();
public:
	WORD	m_pid;
	WORD	m_vid;
	LPCTSTR	m_filter;
	HWND	m_hParent;
	BOOL	m_bConnect;

	DWORD	m_lLastError;
	HANDLE	m_hFile;
	HANDLE	m_hListenerThread;
	HANDLE	m_hevtOverlappedWorkerThread;
	bool	m_fStopping;
	
	/// Current device version, the length of supported data per package
	WORD	m_curDeviceVer;
	/// Firmware version
	WORD	m_firmwareVer;
	/// Firmware build number
	WORD	m_firmwareBuild;
	USAGE	m_usagePage;
	USAGE	m_usage;
public:
	TCHAR	m_devicePath[MAX_PATH];
    wchar_t m_deviceSN[16];

    CRITICAL_SECTION    m_criticalSection;
};

void	memcpy_decode(void* des,const void* src,size_t cnt);
void	memcpy_encode(void* des,const void* src,size_t cnt);

class CUsbHidPipe : public CUsbHid
{
public:
	CUsbHidPipe();
	~CUsbHidPipe();
protected:
	static DWORD WINAPI PipeProc(LPVOID p){
		return ((CUsbHidPipe*)p)->PipeProc();
	}
	DWORD PipeProc();
	HANDLE  m_hPipeThread;
	HANDLE  m_hPipe;
};