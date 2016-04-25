#include "Registry.h"

NTSTATUS Reg_OpenKey(OUT PHANDLE phKey, IN LPCWSTR pszValueName)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING RegistryPath;
	OBJECT_ATTRIBUTES objectAttributes;
	RtlInitUnicodeString(&RegistryPath, pszValueName);
	InitializeObjectAttributes(&objectAttributes, &RegistryPath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwOpenKey(phKey, KEY_ALL_ACCESS, &objectAttributes);
	return status;
}

NTSTATUS Reg_GetDwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PULONG32 pValue)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	struct {
		KEY_VALUE_PARTIAL_INFORMATION Info;
		ULONG32 Extra;
	} Buffer;
	ULONG ResultLength;
	status = ZwQueryValueKey(hKey, &ValueName, KeyValuePartialInformation, &Buffer, sizeof(Buffer), &ResultLength);
	if (NT_SUCCESS(status))
	{
		if (REG_DWORD == Buffer.Info.Type)
		{
			ASSERT(Buffer.Info.DataLength == sizeof(ULONG32));
			*pValue = *(PULONG32)Buffer.Info.Data;
		}
		else
		{
			status = STATUS_INVALID_BUFFER_SIZE;
		}
	}
	return status;
}

NTSTATUS Reg_SetDwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN ULONG32 Value)
{
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	return ZwSetValueKey(hKey, &ValueName, 0, REG_DWORD, &Value, sizeof(ULONG32));
}

NTSTATUS Reg_GetQwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PULONG64 pValue)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	struct {
		KEY_VALUE_PARTIAL_INFORMATION Info;
		ULONG64 Extra;
	} Buffer;
	ULONG ResultLength;
	status = ZwQueryValueKey(hKey, &ValueName, KeyValuePartialInformation, &Buffer, sizeof(Buffer), &ResultLength);
	if (NT_SUCCESS(status))
	{
		if (REG_QWORD == Buffer.Info.Type) {
			ASSERT(Buffer.Info.DataLength == sizeof(ULONG64));
			*pValue = *(PULONG64)Buffer.Info.Data;
		}
		else
			status = STATUS_INVALID_BUFFER_SIZE;
	}
	return status;
}

NTSTATUS Reg_SetQwordValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN ULONG64 Value)
{
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	return ZwSetValueKey(hKey, &ValueName, 0, REG_QWORD, &Value, sizeof(ULONG64));
}

NTSTATUS Reg_GetStringValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PWSTR *ppValue)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	ULONG ResultLength = 0;
	status = ZwQueryValueKey(hKey, &ValueName, KeyValuePartialInformation, NULL, 0, &ResultLength);

	if (status == STATUS_OBJECT_NAME_NOT_FOUND || ResultLength == 0)
	{
		KdPrint(("注册表键值不存在！\n"));
		return CUSTOM_STATUS_REGISTRY_KEY_NOTEXIST;
	}
	KEY_VALUE_PARTIAL_INFORMATION *pBuffer = 
		(PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool, ResultLength);
	if (pBuffer)
	{
		status = ZwQueryValueKey(hKey, &ValueName, KeyValuePartialInformation, pBuffer, ResultLength, &ResultLength);
		if (pBuffer->Type != REG_SZ)
		{
			KdPrint(("注册表键值类型不匹配！\n"));
			return CUSTOM_STATUS_REGISTRY_KEYVALUE_TYPEMISMATCH;
		}
		*ppValue = (PWSTR)ExAllocatePool(PagedPool, pBuffer->DataLength);
		if (*ppValue)
		{
			RtlMoveMemory(*ppValue, pBuffer->Data, pBuffer->DataLength);
		}
		else
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		ExFreePool(pBuffer);
	}

	return STATUS_SUCCESS;
}

NTSTATUS Reg_SetStringValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN LPCWSTR pszValue)
{
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	return ZwSetValueKey(hKey, &ValueName, 0, REG_SZ, (PVOID)pszValue, wcslen(pszValue) * 2 + 2);
}

NTSTATUS Reg_GetBinaryValue(IN HANDLE hKey, IN LPCWSTR pszValueName, OUT PVOID *ppValue, OUT PULONG ValueLength)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG ResultLength = 0;
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	status = ZwQueryValueKey(hKey, &ValueName, KeyValuePartialInformation, NULL, 0, &ResultLength);

	if (status == STATUS_OBJECT_NAME_NOT_FOUND || ResultLength == 0)
	{
		KdPrint(("注册表键值不存在！\n"));
		return CUSTOM_STATUS_REGISTRY_KEY_NOTEXIST;
	}

	KEY_VALUE_PARTIAL_INFORMATION *pBuffer =
		(PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePool(PagedPool, ResultLength);
	if (pBuffer)
	{
		status = ZwQueryValueKey(hKey, &ValueName, KeyValuePartialInformation, pBuffer, ResultLength, &ResultLength);
		if (pBuffer->Type != REG_BINARY)
		{
			KdPrint(("注册表键值类型不匹配！\n"));
			return CUSTOM_STATUS_REGISTRY_KEYVALUE_TYPEMISMATCH;
		}
		*ppValue = (PWSTR)ExAllocatePool(PagedPool, pBuffer->DataLength);
		if (*ppValue)
		{
			RtlMoveMemory(*ppValue, pBuffer->Data, pBuffer->DataLength);
			*ValueLength = pBuffer->DataLength;
		}
		else
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		ExFreePool(pBuffer);
	}
	return STATUS_SUCCESS;
}

NTSTATUS Reg_SetBinaryValue(IN HANDLE hKey, IN LPCWSTR pszValueName, IN PVOID pValue, IN ULONG ValueLength)
{
	UNICODE_STRING ValueName;
	RtlInitUnicodeString(&ValueName, pszValueName);
	return ZwSetValueKey(hKey, &ValueName, 0, REG_BINARY, pValue, ValueLength);
}
