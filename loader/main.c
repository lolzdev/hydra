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

#include <efi/types.h>
#include <efi/sys_table.h>
#include <string.h>
#include <loader.h>

EFI_HANDLE *image_handle;
EFI_SYSTEM_TABLE *system_table;

EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *stdout;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *stdin;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *stderr;

EFI_STATUS efi_main(EFI_HANDLE _image_handle, EFI_SYSTEM_TABLE *_system_table)
{
	system_table = _system_table;
	image_handle = &_image_handle;
    stdout = system_table->ConOut;
    stdin = system_table->ConIn;
    stderr = system_table->StdErr;

	stdout->SetAttribute(stdout, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
    stdout->SetCursorPosition(stdout, 0, 0);
    stdout->ClearScreen(stdout);

	CHAR16 *menu[] = {
			L"                                        \r\n",
			L"                                        \r\n",
			L"╔═══════════Welcome to Hydra═══════════╗\r\n",
			L"║                                      ║\r\n",
			L"║ 1. Boot Hydra Normally (Enter)       ║\r\n",
			L"║ 2. Reboot (r)                        ║\r\n",
			L"║                                      ║\r\n",
			L"║ 3. Enter Firmware Settings (Esc)     ║\r\n",
			L"║                                      ║\r\n",
			L"║                                      ║\r\n",
			L"║                                      ║\r\n",
			L"║ Hydra 0.0.1 (dev)                    ║\r\n",
			L"╚══════════════════════════════════════╝\r\n",
			L"                                        \r\n",
    };

	for (int i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
		CHAR16 *str = menu[i];
		stdout->OutputString(stdout, str);
    }

	UINTN index = 0;
	EFI_INPUT_KEY key;

	while(1) {
		EFI_EVENT key_event[1] = {system_table->ConIn->WaitForKey};
		EFI_STATUS status = system_table->BootServices->WaitForEvent(1, key_event, &index);

		if (status != 0) {
			break;
		}

		if (index == 0) {
			stdin->ReadKeyStroke(stdin, &key);
			if (key.ScanCode == EFI_SCANCODE_ESC) 
				return 0;
			switch (key.UnicodeChar) {
				case L'\r':
				case L'b':
					kernel_load(L"/boot/kernel/hydra");
					while(1);
					return 0;
				case L'r':
					system_table->RuntimeServices->ResetSystem(EfiResetCold, 0, 0, NULL);
					break;
				default:
					break;
			}
		}
	}


	return 0;
}



