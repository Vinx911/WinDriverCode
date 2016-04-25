#include <wdm.h>


#define CUSTOM_STATUS_REGISTRY_KEY_NOTEXIST ((NTSTATUS)0xE00E0000L) //注册表键值不存在
#define CUSTOM_STATUS_REGISTRY_KEYVALUE_TYPEMISMATCH ((NTSTATUS)0xE00E0001L) //注册表键值类型不匹配

NTSTATUS Reg_OpenKey(OUT PHANDLE phKey, IN LPCWSTR pszValueName);
NTSTATUS Reg_GetDwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PULONG32 pValue);
NTSTATUS Reg_SetDwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN ULONG32 Value);
NTSTATUS Reg_GetQwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PULONG64 pValue);
NTSTATUS Reg_SetQwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN ULONG64 Value);
NTSTATUS Reg_GetStringValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PWSTR *ppValue);
NTSTATUS Reg_SetStringValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN LPCWSTR pszValue);
NTSTATUS Reg_GetBinaryValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PVOID *ppValue, OUT PULONG ValueLength);
NTSTATUS Reg_SetBinaryValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN PVOID pValue, IN ULONG ValueLength);


