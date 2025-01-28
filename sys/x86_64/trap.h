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

#ifndef TRAP_H 
#define TRAP_H

#include <stdint.h>
#include <stddef.h>

struct interrupt_frame {
	uintptr_t ip;
	uint64_t cs;
	uint64_t cpu_flags;
	uintptr_t sp;
	uint64_t ss;
} __attribute__((packed));

void int_division_by_zero(struct interrupt_frame *frame);
void int_breakpoint(struct interrupt_frame *frame);
void int_page_fault(struct interrupt_frame *frame, uint64_t error);
void int_gpf(struct interrupt_frame *frame);
void int_overflow(struct interrupt_frame *frame);
void int_bad_opcode(struct interrupt_frame *frame);
void int_double_fault(struct interrupt_frame *frame);
void int_invalid_tss(struct interrupt_frame *frame);
void int_no_segment(struct interrupt_frame *frame);
void int_stack_segment(struct interrupt_frame *frame);
void int_virtualization(struct interrupt_frame *frame);
void int_security(struct interrupt_frame *frame);
void int_systimer(struct interrupt_frame *frame);
void int_keyboard(struct interrupt_frame *frame);
void int_void(struct interrupt_frame *frame);

#endif
