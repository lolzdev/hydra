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

#ifndef EFI_GRAPHICS_OUTPUT
#define EFI_GRAPHICS_OUTPUT

#include <efi/types.h>

#define EFI_EDID_DISCOVERED_PROTOCOL_GUID \
 {0x1c0c34f6,0xd380,0x41fa,\
  {0xa0,0x49,0x8a,0xd0,0x6c,0x1a,0x66,0xaa}}

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
 {0x9042a9de,0x23dc,0x4a38,\
  {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}

#define EFI_EDID_ACTIVE_PROTOCOL_GUID \
 {0xbd8c1056,0x9f36,0x44ec,\
  {0x92,0xa8,0xa6,0x33,0x7f,0x81,0x79,0x86}}  

#define EFI_EDID_OVERRIDE_PROTOCOL_GUID \
 {0x48ecb431,0xfb72,0x45c0,\
  {0xa9,0x22,0xf4,0x58,0xfe,0x04,0x0b,0xd5}}

typedef struct _EFI_EDID_OVERRIDE_PROTOCOL EFI_EID_OVERRIDE_PROTOCOL;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef struct {
	UINT32 RedMask;
	UINT32 GreenMask;
	UINT32 BlueMask;
	UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
	PixelRedGreenBlueReserved8BitPerColor,
	PixelBlueGreenRedReserved8BitPerColor,
	PixelBitMask,
	PixelBltOnly,
	PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
	UINT32 Version;
	UINT32 HorizontalResolution;
	UINT32 VerticalResolution;
	EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
	EFI_PIXEL_BITMASK PixelInformation;
	UINT32 PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
	UINT32 MaxMode;
	UINT32 Mode;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
	UINTN SizeOfInfo;
	EFI_PHYSICAL_ADDRESS FrameBufferBase;
	UINTN FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
	EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
	UINT32 ModeNumber,
	UINTN *SizeOfInfo,
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
);

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
	EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
	UINT32 ModeNumber
);

typedef struct {
	UINT8 Blue;
	UINT8 Green;
	UINT8 Red;
	UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
	EfiBltVideoFill,
	EfiBltVideoToBltBuffer,
	EfiBltBufferToVideo,
	EfiBltVideoToVideo,
	EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef EFI_STATUS (*EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
	EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
	EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
	UINTN SourceX,
	UINTN SourceY,
	UINTN DestinationX,
	UINTN DestinationY,
	UINTN Width,
	UINTN Height,
	UINTN Delta
);

typedef struct {
	UINT32 SizeOfEdid;
	UINT8 *Edid;
} EFI_EDID_DISCOVERED_PROTOCOL;

typedef struct {
	UINT32 SizeOfEdid;
	UINT8 *Edid;
} EFI_EDID_ACTIVE_PROTOCOL;

typedef EFI_STATUS (*EFI_EDID_OVERRIDE_PROTOCOL_GET_EDID) (
	struct _EFI_EDID_OVERRIDE_PROTOCOL *This,
	EFI_HANDLE *ChildHandle,
	UINT32 *Attributes,
	UINTN *EdidSize,
	UINT8 **Edid
);

typedef struct _EFI_EDID_OVERRIDE_PROTOCOL {
	EFI_EDID_OVERRIDE_PROTOCOL_GET_EDID GetEdid;
} EFI_EDID_OVERRIDE_PROTOCOL;

typedef struct _EFI_GRAPHICS_OUTPUT_PROTCOL {
	EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE     QueryMode;
	EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE       SetMode;
	EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT            Blt;
	EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE           *Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL;

#endif
