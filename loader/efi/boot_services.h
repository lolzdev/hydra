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
	 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUINTORS “AS IS” AND ANY
	 * EXPRESS OR IMPLIED WARRANTIES, CLUDG, BUINT NOT LIMITED TO, THE IMPLIED
	 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	 * DISCLAIMED.  NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUINTORS BE LIABLE FOR ANY
	 * DIRECT, DIRECT, CIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	 * (CLUDG, BUINT NOT LIMITED TO, PROCUREMENT OF SUBSTITUINTE GOODS OR SERVICES;
	 * LOSS OF USE, DATA, OR PROFITS; OR BUSESS TERRUPTION) HOWEVER CAUSED AND
	 * ON ANY THEORY OF LIABILITY, WHETHER  CONTRACT, STRICT LIABILITY, OR TORT
	 * (CLUDG NEGLIGENCE OR OTHERWISE) ARISG  ANY WAY  OF THE USE OF THIS
	 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	 */

#ifndef _EFI_BOOT_SERVICES
#define _EFI_BOOT_SERVICES

#include <efi/protocols/device_path.h>

#define EFI_BOOT_SERVICES_SIGNATURE 0x56524553544f4f42
#define EFI_BOOT_SERVICES_REVISION EFI_SPECIFICATION_VERSION

#define EFI_BOOT_SERVICES_SIGNATURE 0x56524553544f4f42
#define EFI_BOOT_SERVICES_REVISION EFI_SPECIFICATION_VERSION

#define EVT_TIMER												0x80000000
#define EVT_RUNTIME												0x40000000

#define EVT_NOTIFY_WAIT											0x00000100
#define EVT_NOTIFY_SIGNAL										0x00000200

#define EVT_SIGNAL_EXIT_BOOT_SERVICES							0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE						0x60000202

#define EFI_EVENT_GROUP_EXIT_BOOT_SERVICES {0x27abf055,0xb1b8,0x4c26, {0x80,0x48,0x74,0x8f,0x37,0xba,0xa2,0xdf}}
#define EFI_EVENT_GROUP_BEFORE_EXIT_BOOT_SERVICES {0x8be0e274,0x3970,0x4b44, {0x80,0xc5,0x1a,0xb9,0x50,0x2f,0x3b,0xfc}}
#define EFI_EVENT_GROUP_VIRTUAL_ADDRESS_CHANGE {0x13fa7698,0xc831,0x49c7, {0x87,0xea,0x8f,0x43,0xfc,0xc2,0x51,0x96}}
#define EFI_EVENT_GROUP_MEMORY_MAP_CHANGE {0x78bee926,0x692f,0x48fd, {0x9e,0xdb,0x1,0x42,0x2e, 0xf0,0xd7,0xab}}
#define EFI_EVENT_GROUP_READY_TO_BOOT {0x7ce88fb3,0x4bd7,0x4679, {0x87,0xa8,0xa8,0xd8,0xde,0xe5,0xd,0x2b}}
#define EFI_EVENT_GROUP_AFTER_READY_TO_BOOT {0x3a2a00ad,0x98b9,0x4cdf, {0xa4,0x78,0x70,0x27,0x77,0xf1,0xc1,0xb}}
#define EFI_EVENT_GROUP_RESET_SYSTEM {0x62da6a56,0x13fb,0x485a, {0xa8,0xda,0xa3,0xdd,0x79,0x12,0xcb,0x6b}}

#define TPL_APPLICATION 4
#define TPL_CALLBACK 8
#define TPL_NOTIFY 16
#define TPL_HIGH_LEVEL 31

#define EFI_MEMORY_UC 0x0000000000000001
#define EFI_MEMORY_WC 0x0000000000000002
#define EFI_MEMORY_WT 0x0000000000000004
#define EFI_MEMORY_WB 0x0000000000000008
#define EFI_MEMORY_UCE 0x0000000000000010
#define EFI_MEMORY_WP 0x0000000000001000
#define EFI_MEMORY_RP 0x0000000000002000
#define EFI_MEMORY_XP 0x0000000000004000
#define EFI_MEMORY_NV 0x0000000000008000
#define EFI_MEMORY_MORE_RELIABLE 0x0000000000010000
#define EFI_MEMORY_RO 0x0000000000020000
#define EFI_MEMORY_SP 0x0000000000040000
#define EFI_MEMORY_CPU_CRYPTO 0x0000000000080000
#define EFI_MEMORY_RUNTIME 0x8000000000000000
#define EFI_MEMORY_ISA_VALID 0x4000000000000000
#define EFI_MEMORY_ISA_MASK 0x0FFFF00000000000

#define EFI_MEMORY_DESCRIPTOR_VERSION 1

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

typedef enum
{
    TimerCancel,
    TimerPeriodic,
    TimerRelative
} EFI_TIMER_DELAY;

typedef enum
{
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE;

typedef enum
{
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiUnacceptedMemoryType,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum
{
    EFI_NATIVE_TERFACE
} EFI_TERFACE_TYPE;

typedef enum
{
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE;

typedef struct
{
    EFI_HANDLE AgentHandle;
    EFI_HANDLE ControllerHandle;
    UINT32 Attributes;
    UINT32 OpenCount;
} EFI_OPEN_PROTOCOL_FORMATION_ENTRY;

typedef VOID(*EFI_EVENT_NOTIFY) (EFI_EVENT Event, VOID * Context);

typedef EFI_TPL(*EFI_RAISE_TPL) (EFI_TPL NewTpl);

typedef EFI_TPL(*EFI_RESTORE_TPL) (EFI_TPL OldTpl);

typedef EFI_STATUS (*EFI_ALLOCATE_PAGES) (
			EFI_ALLOCATE_TYPE Type,
			EFI_MEMORY_TYPE MemoryType,
			UINTN Pages, EFI_PHYSICAL_ADDRESS * Memory);

typedef EFI_STATUS(*EFI_FREE_PAGES) (EFI_PHYSICAL_ADDRESS Memory, UINTN Pages);

typedef EFI_STATUS (*EFI_GET_MEMORY_MAP) (
			UINTN * MemoryMapSize,
			EFI_MEMORY_DESCRIPTOR * MemoryMap,
			UINTN * MapKey,
			UINTN * DescriptorSize,
			UINT32 * DescriptorVersion);

typedef
    EFI_STATUS
    (*EFI_ALLOCATE_POOL) (EFI_MEMORY_TYPE PoolType,
			  UINTN Size, VOID ** Buffer);

typedef EFI_STATUS(*EFI_FREE_POOL) (VOID * Buffer);

typedef
    VOID
    (*EFI_CREATE_EVENT) (UINT32 Type,
			 EFI_TPL NotifyTPL,
			 EFI_EVENT_NOTIFY NotifyFunction,
			 VOID * NotifyContext, EFI_EVENT * Event);

typedef
    EFI_STATUS
    (*EFI_SET_TIMER) (EFI_EVENT Event,
		      EFI_TIMER_DELAY Type, UINT64 TriggerTime);

typedef
    EFI_STATUS
    (*EFI_WAIT_FOR_EVENT) (UINTN NumberOfEvents,
			   EFI_EVENT * Event, UINTN * Index);

typedef EFI_STATUS(*EFI_SIGNAL_EVENT) (EFI_EVENT Event);

typedef EFI_STATUS(*EFI_CLOSE_EVENT) (EFI_EVENT Event);

typedef EFI_STATUS(*EFI_CHECK_EVENT) (EFI_EVENT Event);

typedef
    EFI_STATUS
    (*EFI_STALL_PROTOCOL_TERFACE) (EFI_HANDLE * Handle,
				   EFI_GUID * Protocol,
				   EFI_TERFACE_TYPE InterfaceType,
				   VOID * Interface);

typedef
    EFI_STATUS
    (*EFI_RESTALL_PROTOCOL_TERFACE) (EFI_HANDLE Handle,
				     EFI_GUID * Protocol,
				     VOID * OldInterface,
				     VOID * NewInterface);

typedef
    EFI_STATUS
    (*EFI_UNSTALL_PROTOCOL_TERFACE) (EFI_HANDLE Handle,
				     EFI_GUID * Protocol, VOID * Interface);

typedef
    EFI_STATUS
    (*EFI_HANDLE_PROTOCOL) (EFI_HANDLE Handle,
			    EFI_GUID * Protocol, VOID ** Interface);

typedef
    EFI_STATUS
    (*EFI_REGISTER_PROTOCOL_NOTIFY) (EFI_GUID * Protocol,
				     EFI_EVENT Event, VOID ** Registration);

typedef
    EFI_STATUS
    (*EFI_LOCATE_HANDLE) (EFI_LOCATE_SEARCH_TYPE SearchType,
			  EFI_GUID * Protocol,
			  VOID * SearchKey,
			  UINTN * BufferSize, EFI_HANDLE * Buffer);

typedef
    EFI_STATUS
    (*EFI_LOCATE_DEVICE_PATH) (EFI_GUID * Protocol,
			       EFI_DEVICE_PATH_PROTOCOL ** DevicePath,
			       EFI_HANDLE * Device);

typedef
    EFI_STATUS
    (*EFI_STALL_CONFIGURATION_TABLE) (EFI_GUID * Guid, VOID * Table);

typedef
    EFI_STATUS
    (*EFI_IMAGE_LOAD) (BOOLEAN BootPolicy,
		       EFI_HANDLE ParentImageHandle,
		       EFI_DEVICE_PATH_PROTOCOL * DevicePath,
		       VOID * SourceBuffer,
		       UINTN SourceSize, EFI_HANDLE * ImageHandle);

typedef
    EFI_STATUS
    (*EFI_IMAGE_START) (EFI_HANDLE ImageHandle,
			UINTN * ExitDataSize, CHAR16 ** ExitData);

typedef
    EFI_STATUS
    (*EFI_EXIT) (EFI_HANDLE ImageHandle,
		 EFI_STATUS ExitStatus,
		 UINTN ExitDataSize, CHAR16 * ExitData);

typedef EFI_STATUS(*EFI_IMAGE_UNLOAD) (EFI_HANDLE ImageHandle);

typedef
    EFI_STATUS
    (*EFI_EXIT_BOOT_SERVICES) (EFI_HANDLE ImageHandle, UINTN MapKey);

typedef EFI_STATUS(*EFI_GET_NEXT_MONOTONIC_COUNT) (UINT64 * Count);

typedef EFI_STATUS(*EFI_STALL) (UINTN Microseconds);

typedef
    EFI_STATUS
    (*EFI_SET_WATCHDOG_TIMER) (UINTN Timeout,
			       UINT64 WatchDogCode,
			       UINTN DataSize, CHAR16 * WatchdogData);

typedef
    EFI_STATUS
    (*EFI_CONNECT_CONTROLLER) (EFI_HANDLE ControllerHandle,
			       EFI_HANDLE * DriverImageHandle,
			       EFI_DEVICE_PATH_PROTOCOL *
			       RemainingDevicePath, BOOLEAN Recursive);

typedef
    EFI_STATUS
    (*EFI_DISCONNECT_CONTROLLER) (EFI_HANDLE ControllerHandle,
				  EFI_HANDLE DriverImageHandle,
				  EFI_HANDLE ChildHandle);

typedef
    EFI_STATUS
    (*EFI_OPEN_PROTOCOL_FORMATION) (EFI_HANDLE Handle,
				    EFI_GUID * Protocol,
				    EFI_OPEN_PROTOCOL_FORMATION_ENTRY **
				    EntryBuffer, UINTN * EntryCount);

typedef
    EFI_STATUS
    (*EFI_OPEN_PROTOCOL) (EFI_HANDLE Handle,
			  EFI_GUID * Protocol,
			  VOID ** Interface,
			  EFI_HANDLE AgentHandle,
			  EFI_HANDLE ControllerHandle, UINT32 Attributes);

typedef
    EFI_STATUS
    (*EFI_CLOSE_PROTOCOL) (EFI_HANDLE Handle,
			   EFI_GUID * Protocol,
			   EFI_HANDLE AgentHandle,
			   EFI_HANDLE ControllerHandle);

typedef
    EFI_STATUS
    (*EFI_PROTOCOLS_PER_HANDLE) (EFI_HANDLE Handle,
				 EFI_GUID *** ProtocolBuffer,
				 UINTN * ProtocolBufferCount);

typedef
    EFI_STATUS
    (*EFI_LOCATE_HANDLE_BUFFER) (EFI_LOCATE_SEARCH_TYPE SearchType,
				 EFI_GUID * Protocol,
				 VOID * SearchKey,
				 UINTN * NoHandles, EFI_HANDLE ** Buffer);

typedef
    EFI_STATUS
    (*EFI_LOCATE_PROTOCOL) (EFI_GUID * Protocol,
			    VOID * Registration, VOID ** Interface);

typedef
    EFI_STATUS
    (*EFI_STALL_MULTIPLE_PROTOCOL_TERFACES) (EFI_HANDLE * Handle, ...);

typedef
    EFI_STATUS
    (*EFI_UNSTALL_MULTIPLE_PROTOCOL_TERFACES) (EFI_HANDLE * Handle, ...);

typedef
    EFI_STATUS
    (*EFI_CALCULATE_CRC32) (VOID * Data, UINTN DataSize, UINT32 * Crc32);

typedef
    VOID(*EFI_COPY_MEM) (VOID * Destination, VOID * Source, UINTN Length);

typedef VOID(*EFI_SET_MEM) (VOID * Buffer, UINTN Size, UINT8 Value);

typedef
    EFI_STATUS
    (*EFI_CREATE_EVENT_EX) (UINT32 Type,
			    EFI_TPL NotifyTpl,
			    EFI_EVENT_NOTIFY NotifyFunction,
			    VOID * NotifyContext,
			    EFI_GUID * EventGroup, EFI_EVENT * Event);

typedef struct
{
    EFI_TABLE_HEADER Hdr;
    // 
    // Task Priority Services
    // 
    EFI_RAISE_TPL RaiseTPL;	// EFI 1.0+
    EFI_RESTORE_TPL RestoreTPL;	// EFI 1.0+
    // 
    // Memory Services
    // 
    EFI_ALLOCATE_PAGES AllocatePages;	// EFI 1.0+
    EFI_FREE_PAGES FreePages;	// EFI 1.0+
    EFI_GET_MEMORY_MAP GetMemoryMap;	// EFI 1.0+
    EFI_ALLOCATE_POOL AllocatePool;	// EFI 1.0+
    EFI_FREE_POOL FreePool;	// EFI 1.0+
    // 
    // Event & Timer Services
    // 
    EFI_CREATE_EVENT CreateEvent;	// EFI 1.0+
    EFI_SET_TIMER SetTimer;	// EFI 1.0+
    EFI_WAIT_FOR_EVENT WaitForEvent;	// EFI 1.0+
    EFI_SIGNAL_EVENT SignalEvent;	// EFI 1.0+
    EFI_CLOSE_EVENT CloseEvent;	// EFI 1.0+
    EFI_CHECK_EVENT CheckEvent;	// EFI 1.0+
    // 
    // Protocol Handler Services
    // 
    EFI_STALL_PROTOCOL_TERFACE InstallProtocolInterface;	// EFI 1.0
    EFI_RESTALL_PROTOCOL_TERFACE ReinstallProtocolInterface;	// EFI 1.0
    EFI_UNSTALL_PROTOCOL_TERFACE UninstallProtocolInterface;	// EFI 1.0
    EFI_HANDLE_PROTOCOL HandleProtocol;	// EFI 1.0
    VOID *Reserved;		// EFI 1.0+
    EFI_REGISTER_PROTOCOL_NOTIFY RegisterProtocolNotify;	// EFI 1.0
    EFI_LOCATE_HANDLE LocateHandle;	// EFI 1.0
    EFI_LOCATE_DEVICE_PATH LocateDevicePath;	// EFI 1.0
    EFI_STALL_CONFIGURATION_TABLE InstallConfigurationTable;	// EFI 1.0
    // 
    // Image Services
    // 
    EFI_IMAGE_UNLOAD LoadImage;	// EFI 1.0+
    EFI_IMAGE_START StartImage;	// EFI 1.0+
    EFI_EXIT Exit;		// EFI 1.0+
    EFI_IMAGE_UNLOAD UnloadImage;	// EFI 1.0+
    EFI_EXIT_BOOT_SERVICES ExitBootServices;	// EFI 1.0+
    // 
    // Miscellaneous Services
    // 
    EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;	// EFI 1.0+
    EFI_STALL Stall;		// EFI 1.0+
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;	// EFI 1.0+
    // 
    // DriverSupport Services
    // 
    EFI_CONNECT_CONTROLLER ConnectController;	// EFI 1.1
    EFI_DISCONNECT_CONTROLLER DisconnectController;	// EFI 1.1+
    // 
    // Open and Close Protocol Services
    // 
    EFI_OPEN_PROTOCOL OpenProtocol;	// EFI 1.1+
    EFI_CLOSE_PROTOCOL CloseProtocol;	// EFI 1.1+
    EFI_OPEN_PROTOCOL_FORMATION OpenProtocolInformation;	// EFI
    // 1.1+
    // 
    // Library Services
    // 
    EFI_PROTOCOLS_PER_HANDLE ProtocolsPerHandle;	// EFI 1.1+
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;	// EFI 1.1+
    EFI_LOCATE_PROTOCOL LocateProtocol;	// EFI 1.1+
    EFI_UNSTALL_MULTIPLE_PROTOCOL_TERFACES InstallMultipleProtocolInterfaces;	// EFI 
    // 1.1+
    EFI_UNSTALL_MULTIPLE_PROTOCOL_TERFACES UninstallMultipleProtocolInterfaces;	// EFI 
    // 1.1+*
    // 
    // 32-bit CRC Services
    // 
    EFI_CALCULATE_CRC32 CalculateCrc32;	// EFI 1.1+
    // 
    // Miscellaneous Services
    // 
    EFI_COPY_MEM CopyMem;	// EFI 1.1+
    EFI_SET_MEM SetMem;		// EFI 1.1+
    EFI_CREATE_EVENT_EX CreateEventEx;	// UEFI 2.0+
} EFI_BOOT_SERVICES;

#endif
