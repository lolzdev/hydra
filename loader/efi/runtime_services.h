	/*-
	 * Copyright 2024 Lorenzo Torres
 	 *
	 * Redistribution and use in source and binary forms, with or without
	 * modification, are permitted provided that the following conditions are met:
	 * 1. Redistributions of source code must retain the above copyright
	 *    notice, this list of conditions and the following disclaimer.
	 * 2. Redistributions in binary form must reproduce the above copyright
	 *    notice, this list of conditions and the following disclaimer in the
	 *    documentation and/or other materials provided with the distribution.
	 * 3. Neither the name of the copyright holder nor
	 *    the names of its contributors may be used to endorse or promote products
	 *    derived from this software without specific prior written permission.
	 *
	 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
	 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
	 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 */

#ifndef _EFI_RUNTIME_SERVICES
#define _EFI_RUNTIME_SERVICES

#include <efi/types.h>
#include <efi/table.h>
#include <efi/cert.h>

#define EFI_RUNTIME_SERVICES_SIGNATURE 0x56524553544e5552
#define EFI_RUNTIME_SERVICES_REVISION EFI_SPECIFICATION_VERSION

#define EFI_VARIABLE_NON_VOLATILE 0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS 0x00000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD 0x00000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS 0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define EFI_VARIABLE_APPEND_WRITE 0x00000040
#define EFI_VARIABLE_ENHANCED_AUTHENTICATED_ACCESS 0x00000080

typedef UINT64 EFI_VIRTUAL_ADDRESS;
typedef UINT64 EFI_PHYSICAL_ADDRESS;

typedef struct
{
    UINT32 Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    UINT64 NumberOfPages;
    UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef struct
{
    UINT16 Year;		// 1900 - 9999
    UINT8 Month;		// 1 - 12
    UINT8 Day;			// 1 - 31
    UINT8 Hour;			// 0 - 23
    UINT8 Minute;		// 0 - 59
    UINT8 Second;		// 0 - 59
    UINT8 Pad1;
    UINT32 Nanosecond;		// 0 - 999,999,999
    INT16 TimeZone;		// --1440 to 1440 or 2047
    UINT8 Daylight;
    UINT8 Pad2;
} EFI_TIME;

typedef EFI_STATUS(*EFI_GET_VARIABLE) (CHAR16 * VariableName,
				       EFI_GUID * VendorGuid,
				       UINT32 * Attributes,
				       UINTN * DataSize, VOID * Data);

typedef EFI_STATUS(*EFI_GET_NEXT_VARIABLE_NAME) (UINTN *
						 VariableNameSize,
						 CHAR16 *
						 VariableName,
						 EFI_GUID * VendorGuid);

typedef EFI_STATUS(*EFI_SET_VARIABLE) (CHAR16 * VariableName,
				       EFI_GUID * VendorGuid,
				       UINT32 Attributes,
				       UINTN DataSize, VOID * Data);

typedef struct
{
    UINT64 MonotonicCount;
    WIN_CERTIFICATE_UEFI_GUID AuthInfo;
} EFI_VARIABLE_AUTHENTICATION;

typedef struct
{
    EFI_TIME TimeStamp;
    WIN_CERTIFICATE_UEFI_GUID AuthInfo;
} EFI_VARIABLE_AUTHENTICATION_2;

#define EFI_VARIABLE_AUTHENTICATION_3_TIMESTAMP_TYPE 1
#define EFI_VARIABLE_AUTHENTICATION_3_NONCE_TYPE 2
typedef struct
{
    UINT8 Version;
    UINT8 Type;
    UINT32 MetadataSize;
    UINT32 Flags;
} EFI_VARIABLE_AUTHENTICATION_3;

typedef struct
{
    UINT32 NonceSize;
} EFI_VARIABLE_AUTHENTICATION_3_NONCE;

typedef EFI_STATUS(*EFI_QUERY_VARIABLE_INFO) (UINT32 Attributes,
					      UINT64 *
					      MaximumVariableStorageSize,
					      UINT64 *
					      RemainingVariableStorageSize,
					      UINT64 * MaximumVariableSize);



#define EFI_TIME_ADJUST_DAYLIGHT 0x01
#define EFI_TIME_IN_DAYLIGHT 0x02
#define EFI_UNSPECIFIED_TIMEZONE 0x07FF

typedef struct
{
    UINT32 Resolution;
    UINT32 Accuracy;
    BOOLEAN SetsToZero;
} EFI_TIME_CAPABILITIES;

typedef EFI_STATUS(*EFI_GET_TIME) (EFI_TIME * Time,
				   EFI_TIME_CAPABILITIES * Capabilities);

typedef EFI_STATUS(*EFI_SET_TIME) (EFI_TIME * Time);

typedef EFI_STATUS(*EFI_GET_WAKEUP_TIME) (BOOLEAN * Enabled,
					  BOOLEAN * Pending, EFI_TIME * Time);

typedef EFI_STATUS(*EFI_SET_WAKEUP_TIME) (BOOLEAN Enable, EFI_TIME * Time);

typedef EFI_STATUS(*EFI_SET_VIRTUAL_ADDRESS_MAP) (UINTN
						  MemoryMapSize,
						  UINTN
						  DescriptorSize,
						  UINT32
						  DescriptorVersion,
						  EFI_MEMORY_DESCRIPTOR
						  * VirtualMap);

#define EFI_OPTIONAL_PTR 0x00000001

typedef EFI_STATUS(*EFI_CONVERT_POINTER) (UINTN DebugDisposition,
					  VOID ** Address);

typedef enum
{
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown,
    EfiResetPlatformSpecific
} EFI_RESET_TYPE;

typedef VOID(*EFI_RESET_SYSTEM) (EFI_RESET_TYPE ResetType,
				 EFI_STATUS ResetStatus,
				 UINTN DataSize, VOID * ResetData);

typedef EFI_STATUS(*EFI_GET_NEXT_HIGH_MONO_COUNT) (UINT32 * HighCount);

typedef struct
{
    UINT64 Length;
    union
    {
	EFI_PHYSICAL_ADDRESS DataBlock;
	EFI_PHYSICAL_ADDRESS ContinuationPointer;
    } Union;
} EFI_CAPSULE_BLOCK_DESCRIPTOR;

typedef struct
{
    EFI_GUID CapsuleGuid;
    UINT32 HeaderSize;
    UINT32 Flags;
    UINT32 CapsuleImageSize;
} EFI_CAPSULE_HEADER;

#define CAPSULE_FLAGS_PERSIST_ACROSS_RESET 0x00010000
#define CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE 0x00020000
#define CAPSULE_FLAGS_INITIATE_RESET 0x00040000

typedef struct
{
    UINT32 CapsuleArrayNumber;
    VOID *CapsulePtr[1];
} EFI_CAPSULE_TABLE;

typedef EFI_STATUS(*EFI_UPDATE_CAPSULE) (EFI_CAPSULE_HEADER **
					 CapsuleHeaderArray,
					 UINTN CapsuleCount,
					 EFI_PHYSICAL_ADDRESS
					 ScatterGatherList);

typedef
EFI_STATUS(*EFI_QUERY_CAPSULE_CAPABILITIES) (EFI_CAPSULE_HEADER **
					     CapsuleHeaderArray,
					     UINTN CapsuleCount,
					     UINT64 * MaximumCapsuleSize,
					     EFI_RESET_TYPE * ResetType);

typedef struct
{
    EFI_TABLE_HEADER Hdr;
    // 
    // Time Services
    // 
    EFI_GET_TIME GetTime;
    EFI_SET_TIME SetTime;
    EFI_GET_WAKEUP_TIME GetWakeupTime;
    EFI_SET_WAKEUP_TIME SetWakeupTime;
    // 
    // Virtual Memory Services
    // 
    EFI_SET_VIRTUAL_ADDRESS_MAP SetVirtualAddressMap;
    EFI_CONVERT_POINTER ConvertPointer;
    // 
    // Variable Services
    // 
    EFI_GET_VARIABLE GetVariable;
    EFI_GET_NEXT_VARIABLE_NAME GetNextVariableName;
    EFI_SET_VARIABLE SetVariable;
    // 
    // Miscellaneous Services
    // 
    EFI_GET_NEXT_HIGH_MONO_COUNT GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM ResetSystem;
    // 
    // UEFI 2.0 Capsule Services
    // 
    EFI_UPDATE_CAPSULE UpdateCapsule;
    EFI_QUERY_CAPSULE_CAPABILITIES QueryCapsuleCapabilities;
    // 
    // Miscellaneous UEFI 2.0 Service
    // 
    EFI_QUERY_VARIABLE_INFO QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

#endif
