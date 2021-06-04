#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <thread>

#define REQUEST_READ_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x996, METHOD_OUT_DIRECT, FILE_ANY_ACCESS) //YOU MUST CHANGE IOCTL CODE
#define GET_PID_AND_BASE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x997, METHOD_OUT_DIRECT, FILE_ANY_ACCESS) //YOU MUST CHANGE IOCTL CODE
//���̶�д
typedef struct _ReadWrite {
	int SrcPid;
	uintptr_t SrcAddr;
	int DstPid;
	uintptr_t DstAddr;
	uintptr_t size;
} ReadWrite;
typedef struct _PidBase {
	wchar_t name[200];
	uintptr_t RetInfoPid;
	uintptr_t RetInfoAddr;
} PidBase;
struct PidBaseResponse {
	unsigned long long pid;
	unsigned long long baseAddr;
};

const HANDLE hDevice = CreateFileA("\\\\.\\Nvm", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

static bool connectDrv() {
	if (hDevice == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Disconnected", L"Error", 0);
		return false;
	}
	return true;
}
/*��ȡ����pid�ͻ���ַ*/
static DWORD64 GetPID(const wchar_t* pname) {
	PidBaseResponse result;
	PidBase cmd2;
	memcpy(cmd2.name, pname, wcslen(pname) * sizeof(wchar_t));
	cmd2.RetInfoPid = (uintptr_t)&result.pid;
	cmd2.RetInfoAddr = (uintptr_t)&result.baseAddr;

	DeviceIoControl(hDevice, GET_PID_AND_BASE, &cmd2, sizeof(PidBase), NULL, NULL, NULL, NULL);
	return result.pid;
}
static DWORD64 GetBase(const wchar_t* pname) {
	PidBaseResponse result;
	PidBase cmd2;
	memcpy(cmd2.name, pname, wcslen(pname) * sizeof(wchar_t));
	cmd2.RetInfoPid = (uintptr_t)&result.pid;
	cmd2.RetInfoAddr = (uintptr_t)&result.baseAddr;

	DeviceIoControl(hDevice, GET_PID_AND_BASE, &cmd2, sizeof(PidBase), NULL, NULL, NULL, NULL);
	return result.baseAddr;
}

/*��ȡ������ĳ����ַ��ָ����С�����ݵ�ָ����*/
static void readMem(int pid, DWORD64 addr1, int size, PVOID addr2) {
	int Pid = (uintptr_t)pid;
	_ReadWrite rw = { Pid ,(uintptr_t)addr1 ,(int)_getpid() ,(uintptr_t)addr2 ,(uintptr_t)size };

	//DWORD returned;
	DeviceIoControl(hDevice, REQUEST_READ_WRITE, &rw, sizeof(_ReadWrite), NULL, NULL, NULL, NULL);
}
/*��ָ���ϵ��ض���С��ֵд��ĳ��������ָ����ַ�ĺ���*/
static void writeMem(int pid, DWORD64 addr1, int size, PVOID addr2) {
	_ReadWrite rw = { (int)_getpid() ,(uintptr_t)addr2 , (int)pid,(uintptr_t)addr1 ,(uintptr_t)size };//���ǽ�һ������ĳ����ַ����Ϣ���Ƶ���һ�����̵ĵ�ַ��

	//DWORD returned;
	DeviceIoControl(hDevice, REQUEST_READ_WRITE, &rw, sizeof(_ReadWrite), NULL, NULL, NULL, NULL);
}

/*��ȡ����ָ����ַ����Ϣ*/
template <typename T>
static T Read(int pid, const uintptr_t addr) {
	T Buffer{ };
	_ReadWrite rw = { (int)pid ,addr ,(int)_getpid() ,(uintptr_t)&Buffer ,sizeof(T) };

	DWORD returned = 0;
	DeviceIoControl(hDevice, REQUEST_READ_WRITE, &rw, sizeof(_ReadWrite), NULL, NULL, NULL, NULL);
	return Buffer;
}
/*�ڽ���ָ���ĵ�ַ����д��ָ�����͵�ֵ*/
template <typename T>
static bool Write(int pid, const uintptr_t addr, const T& value) {
	_ReadWrite rw = { (int)_getpid() ,(uintptr_t)&value , pid,addr ,sizeof(T) };

	DWORD returned = 0;
	DeviceIoControl(hDevice, REQUEST_READ_WRITE, &rw, sizeof(_ReadWrite), NULL, NULL, NULL, NULL);
	return true;
}

/*����ֵbufferΪ��ȡ��ֵ*/
static uintptr_t Readmemory(int pid, uintptr_t addr, uintptr_t buffer, SIZE_T Size) {
	_ReadWrite rw = { pid ,addr ,(int)_getpid() ,buffer ,Size };

	DWORD returned = 0;
	DeviceIoControl(hDevice, REQUEST_READ_WRITE, &rw, sizeof(_ReadWrite), NULL, NULL, NULL, NULL);
	return buffer;
}
/*bool���ͣ�TΪ��ȡֵ�����ͣ�value��Ҫ�ȶ�������*/
template<typename T>
static inline bool read(int pid, uint64_t addr, T& value) {//&value����value�ĵ�ַ
	return Readmemory((uint32_t)pid, addr, (uint64_t)&value, sizeof(T));
}
/* ��ȡ���� */
template<typename T>
static inline bool read_array(int pid, uint64_t addr, T* array, size_t size) {
	return Readmemory((uint32_t)pid, addr, (uint64_t)array, sizeof(T) * size);
}