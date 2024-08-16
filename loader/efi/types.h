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

#ifndef EFI_TYPES_H
#define EFI_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t BOOLEAN;
typedef uintptr_t UINTN;
typedef intptr_t INTN;
typedef uint8_t UINT8;
typedef int8_t INT8;
typedef uint16_t UINT16;
typedef int16_t INT16;
typedef uint32_t UINT32;
typedef int32_t INT32;
typedef uint64_t UINT64;
typedef int64_t INT64;
typedef char CHAR;
typedef uint16_t CHAR16;
typedef void VOID;
typedef UINTN EFI_STATUS;
typedef void *EFI_HANDLE;
typedef void *EFI_EVENT;
typedef uint64_t EFI_LBA;
typedef UINTN EFI_TPL;
typedef uint8_t EFI_MAC_ADDRESS[32];
typedef uint8_t EFI_IPV4_ADDRESS[4];
typedef uint8_t EFI_IPV6_ADDRESS[16];
typedef uint32_t EFI_IP_ADDRESS[2];

typedef struct {
	UINT32 Data1;
	UINT16 Data2;
	UINT16 Data3;
	UINT8 Data4[8];
} EFI_GUID;

typedef struct {
	UINT16 ScanCode;
	CHAR16 UnicodeChar;
} EFI_INPUT_KEY;

typedef struct {
	UINT8 BootIndicator;
	UINT8 StartHead;
	UINT8 StartSector;
	UINT8 StartTrack;
	UINT8 OSIndicator;
	UINT8 EndHead;
	UINT8 EndSector;
	UINT8 EndTrack;
	UINT8 StartingLBA[4];
	UINT8 SizeInLBA[4];
} MBR_PARTITION_RECORD;

// Master Boot Record
typedef struct {
	UINT8 BootStrapCode[440];
	UINT8 UniqueMbrSignature[4];
	UINT8 Unknown[2];
	MBR_PARTITION_RECORD Partition[4];
	UINT16 Signature;
} MASTER_BOOT_RECORD;

typedef struct {
	EFI_GUID PartitionTypeGUID;
	EFI_GUID UniquePartitionGUID;
	EFI_LBA StartingLBA;
	EFI_LBA EndingLBA;
	UINT64 Attributes;
	CHAR16 PartitionName[36];
} EFI_PARTITION_ENTRY;

#define EFI_SUCCESS					0

#define EFI_LOAD_ERROR				1
#define EFI_INVALID_PARAMETER		2
#define EFI_UNSUPPORTED				3
#define EFI_BAD_BUFFER_SIZE			4
#define EFI_BUFFER_TOO_SMALL		5
#define EFI_NOT_READY				6
#define EFI_DEVICE_ERROR			7
#define EFI_WRITE_PROTECTED			8
#define EFI_OUT_OF_RESOURCES		9
#define EFI_VOLUME_CORRUPTED		10
#define EFI_VOLUME_FULL				11
#define EFI_NO_MEDIA				12
#define EFI_MEDIA_CHANGED			13
#define EFI_NOT_FOUND				14
#define EFI_ACCESS_DENIED			15
#define EFI_NO_RESPONSE				16
#define EFI_NO_MAPPING				17
#define EFI_TIMEOUT					18
#define EFI_NOT_STARTED				19
#define EFI_ALREADY_STARTED			20
#define EFI_ABORTED					21
#define EFI_ICMP_ERROR				22
#define EFI_TFTP_ERROR				23
#define EFI_PROTOCOL_ERROR			24
#define EFI_INCOMPATIBLE_VERSION	25
#define EFI_SECURITY_VIOLATION		26
#define EFI_CRC_ERROR				27
#define EFI_END_OF_MEDIA			28
#define EFI_END_OF_FILE				29
#define EFI_INVALID_LANGUAGE		30
#define EFI_COMPROMISED_DATA		31
#define EFI_IP_ADDRESS_CONFLICT		32
#define EFI_HTTP_ERROR				33

#endif
