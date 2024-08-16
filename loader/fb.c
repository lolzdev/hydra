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

#include <fb.h>
#include <alloc.h>
#include <efi/types.h>
#include <efi/sys_table.h>
#include <efi/protocols/media_access.h>
#include <efi/protocols/graphics_output.h>

extern EFI_HANDLE *image_handle;
extern EFI_SYSTEM_TABLE *system_table;
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *stdout;

static EFI_GUID efi_graphics_output_protocol_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

EFI_STATUS fb_get_all(hydra_framebuffer_t **fbs)
{
	EFI_STATUS status = EFI_SUCCESS;
	
	EFI_HANDLE *handles = NULL;
	uint64_t handles_no = 0;

	status = system_table->BootServices->LocateHandleBuffer(ByProtocol, &efi_graphics_output_protocol_guid, NULL, &handles_no, &handles);

	if (status != EFI_SUCCESS)
		return status;

	*fbs = (hydra_framebuffer_t *) malloc(handles_no * sizeof(hydra_framebuffer_t));

	uint32_t fb_count = 0;
	for (uint64_t i=0; i < handles_no; i++) {
		hydra_framebuffer_t fb = {0};

		EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
		UINTN info_size, native_mode;
		EFI_GRAPHICS_OUTPUT_PROTOCOL *graphics_protocol = NULL;
		status = system_table->BootServices->HandleProtocol(handles[i], &efi_graphics_output_protocol_guid, (void **) &graphics_protocol);

		if (status != EFI_SUCCESS) continue;

		status = graphics_protocol->QueryMode(graphics_protocol, graphics_protocol->Mode == NULL ? 0 : graphics_protocol->Mode->Mode, &info_size, &info);
		if (status == (UINTN)EFI_NOT_STARTED) {
			status = graphics_protocol->SetMode(graphics_protocol, 0);
		}

		if (status != EFI_SUCCESS) {
			return status;
		} else {
			native_mode = graphics_protocol->Mode->Mode;
		}

		status = graphics_protocol->QueryMode(graphics_protocol, native_mode, &info_size, &info);

		if (status != EFI_SUCCESS) return status;

		status = graphics_protocol->SetMode(graphics_protocol, native_mode);

		if (status != EFI_SUCCESS) return status;

		fb.address = (uint32_t *) graphics_protocol->Mode->FrameBufferBase;
		fb.size = graphics_protocol->Mode->FrameBufferSize;
		fb.width = graphics_protocol->Mode->Info->HorizontalResolution;
		fb.height = graphics_protocol->Mode->Info->VerticalResolution;

		(*fbs)[fb_count++] = fb;
	}

	system_table->BootServices->FreePool((void *) handles);

	return EFI_SUCCESS;
}
