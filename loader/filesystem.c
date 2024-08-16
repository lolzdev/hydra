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

#include <filesystem.h>
#include <efi/types.h>
#include <string.h>
#include <alloc.h>

extern EFI_HANDLE *image_handle;
extern EFI_SYSTEM_TABLE *system_table;

EFI_GUID efi_loaded_image_protocol_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
EFI_GUID efi_simple_filesystem_protocol_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
EFI_GUID efi_file_info_guid = EFI_FILE_INFO_GUID;

EFI_STATUS sfs_get_root(EFI_FILE_PROTOCOL **root)
{
	EFI_STATUS status;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfs_protocol;
	EFI_LOADED_IMAGE_PROTOCOL *loaded_image_protocol;

    status = system_table->BootServices->HandleProtocol(*image_handle, &efi_loaded_image_protocol_guid, (void **)&loaded_image_protocol);

	status = system_table->BootServices->HandleProtocol(loaded_image_protocol->DeviceHandle, &efi_simple_filesystem_protocol_guid, (void **) &sfs_protocol);

	if (status != 0) {
		return status;
	}

	return sfs_protocol->OpenVolume(sfs_protocol, root);
}

file_t sfs_open(CHAR16 *path, UINT64 mode)
{
	file_t file = {0};
	EFI_STATUS status;
	EFI_FILE_PROTOCOL *root;
	status = sfs_get_root(&root);

	if (status != 0) {
		file.status = status;
		return file;
	}

	EFI_FILE_PROTOCOL *file_protocol;	

	status = root->Open(root, &file_protocol, path, mode, 0);

	if (status != 0) {
		file.status = status;
		return file;
	}

	file.file = file_protocol;

	UINTN info_size = sizeof(EFI_FILE_INFO);
	status = file_protocol->GetInfo(file_protocol, &efi_file_info_guid, &info_size, &file.info);	

	if (status != 0) {
		file.status = status;
		return file;
	}

	return file;
}

EFI_STATUS sfs_read(file_t *file, void *buffer)
{
	EFI_STATUS status = 0;

	file->file->Read(file->file, &file->info.PhysicalSize, buffer);
	file->status = status;

	return status;
}

EFI_STATUS sfs_close(file_t *file)
{
	EFI_STATUS status;
	status = file->file->Close(file->file);
	file->status = status;
	return status;
}
