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

#ifndef EFI_CERT_H
#define EFI_CERT_H

#define WIN_CERT_TYPE_PKCS_SIGNED_DATA 0x0002
#define WIN_CERT_TYPE_EFI_PKCS115 0x0EF0
#define WIN_CERT_TYPE_EFI_GUID 0x0EF1

typedef struct _WIN_CERTIFICATE
{
    UINT32 dwLength;
    UINT16 wRevision;
    UINT16 wCertificateType;
} WIN_CERTIFICATE;

typedef struct _WIN_CERTIFICATE_UEFI_GUID
{
    WIN_CERTIFICATE Hdr;
    EFI_GUID CertType;
    UINT8 CertData[];
} WIN_CERTIFICATE_UEFI_GUID;

typedef struct _WIN_CERTIFICATE_EFI_PKCS1_15
{
    WIN_CERTIFICATE Hdr;
    EFI_GUID HashAlgorithm;
} WIN_CERTIFICATE_EFI_PKCS1_15;

#define EFI_CERT_TYPE_RSA2048_SHA256_GUID \
{0xa7717414, 0xc616, 0x4977, \
{0x94, 0x20, 0x84, 0x47, 0x12, 0xa7, 0x35, 0xbf}}
#define EFI_CERT_TYPE_PKCS7_GUID \
{0x4aafd29d, 0x68df, 0x49ee, \
{0x8a, 0xa9, 0x34, 0x7d, 0x37, 0x56, 0x65, 0xa7}}

typedef struct _EFI_CERT_BLOCK_RSA_2048_SHA256
{
    EFI_GUID HashType;
    UINT8 PublicKey[256];
    UINT8 Signature[256];
} EFI_CERT_BLOCK_RSA_2048_SHA256;

#endif
