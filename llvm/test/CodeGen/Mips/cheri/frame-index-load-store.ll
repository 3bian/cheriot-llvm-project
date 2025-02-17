; RUN: %cheri_purecap_llc -o - -O2 -verify-machineinstrs %s | FileCheck %s -check-prefixes CHECK,PURECAP
; Should be the same without the addressing mode folder
; RUN: %cheri_purecap_llc -o - -O2 -verify-machineinstrs %s -disable-cheri-addressing-mode-folder | FileCheck %s -check-prefixes CHECK,PURECAP
; RUN: sed 's/addrspace(200)/addrspace(0)/g' %s | %cheri_llc -o - -O2 -verify-machineinstrs | FileCheck %s -check-prefixes CHECK,MIPS

define ptr addrspace(200) @load_store_ptr_to_stack(ptr addrspace(200) %arg1, ptr addrspace(200) %arg2) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_ptr_to_stack:
  ; Store to stack slot 1
  tail call void asm sideeffect "", ""()
  %arg1.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  store volatile ptr addrspace(200) %arg1, ptr addrspace(200) %arg1.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, {{32|64}}($c11)
  ; MIPS-NEXT: sd $4, 32($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ; Store to stack slot 2
  %arg2.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  store volatile ptr addrspace(200) %arg2, ptr addrspace(200) %arg2.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c4, $zero, {{0|32}}($c11)
  ; MIPS-NEXT: sd $5, 0($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ; Load to stack slot 1
  %loaded1 = load volatile ptr addrspace(200), ptr addrspace(200) %arg1.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: clc $c3, $zero, {{32|64}}($c11)
  ; MIPS-NEXT: ld $2, 32($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ; Load from stack slot 1
  %loaded2 = load volatile ptr addrspace(200), ptr addrspace(200) %arg2.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: clc $c1, $zero, {{0|32}}($c11)
  ; MIPS-NEXT: ld $1, 0($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ret ptr addrspace(200) %loaded1
}

define i64 @load_store_stack_i64(i64 %arg, ptr addrspace(200) %padding) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_stack_i64:

  tail call void asm sideeffect "", ""()
  %padding.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  ; Store dummy cap to stack slot 1
  store volatile ptr addrspace(200) %padding, ptr addrspace(200) %padding.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, 32($c11)
  ; MIPS-NEXT:  sd $5, 32($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; Store the i64 to stack slot 2
  %arg.stack = alloca i64, align 8, addrspace(200)
  store volatile i64 %arg, ptr addrspace(200) %arg.stack, align 8
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csd $4, $zero, 24($c11)
  ; MIPS-NEXT: sd $4, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; And load it back
  %loaded = load volatile i64, ptr addrspace(200) %arg.stack, align 8
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: cld $2, $zero, 24($c11)
  ; MIPS-NEXT: ld $2, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ret i64 %loaded
}

define i32 @load_store_stack_i32(i32 %arg, ptr addrspace(200) %padding) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_stack_i32:

  tail call void asm sideeffect "", ""()
  %padding.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  ; Store dummy cap to stack slot 1
  store volatile ptr addrspace(200) %padding, ptr addrspace(200) %padding.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, 32($c11)
  ; MIPS-NEXT:  sd $5, 32($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; Store the i32 to stack slot 2
  %arg.stack = alloca i32, align 8, addrspace(200)
  store volatile i32 %arg, ptr addrspace(200) %arg.stack, align 4
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csw $4, $zero, 24($c11)
  ; MIPS-NEXT:  sw $4, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; And load it back
  %loaded = load volatile i32, ptr addrspace(200) %arg.stack, align 4
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: clw $2, $zero, 24($c11)
  ; MIPS-NEXT:  lw $2, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ret i32 %loaded
}

define i16 @load_store_stack_i16(i16 %arg, ptr addrspace(200) %padding) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_stack_i16:

  tail call void asm sideeffect "", ""()
  %padding.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  ; Store dummy cap to stack slot 1
  store volatile ptr addrspace(200) %padding, ptr addrspace(200) %padding.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, 32($c11)
  ; MIPS-NEXT:  sd $5, 32($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; Store the i16 to stack slot 2
  %arg.stack = alloca i16, align 8, addrspace(200)
  store volatile i16 %arg, ptr addrspace(200) %arg.stack, align 2
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csh $4, $zero, 24($c11)
  ; MIPS-NEXT: sh $4, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; And load it back
  %loaded = load volatile i16, ptr addrspace(200) %arg.stack, align 2
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: clhu $2, $zero, 24($c11)
  ; MIPS-NEXT: lhu $2, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ret i16 %loaded
}

; Function Attrs: nounwind
define i8 @load_store_stack_i8(i8 %arg, ptr addrspace(200) %padding) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_stack_i8:

  tail call void asm sideeffect "", ""()
  %padding.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  ; Store dummy cap to stack slot 1
  store volatile ptr addrspace(200) %padding, ptr addrspace(200) %padding.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, 32($c11)
  ; MIPS-NEXT:  sd $5, 32($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; Store the i8 to stack slot 2
  %arg.stack = alloca i8, align 8, addrspace(200)
  store volatile i8 %arg, ptr addrspace(200) %arg.stack, align 1
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csb $4, $zero, 24($c11)
  ; MIPS-NEXT: sb $4, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()

  ; And load it back
  %loaded = load volatile i8, ptr addrspace(200) %arg.stack, align 1
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: clbu $2, $zero, 24($c11)
  ; MIPS-NEXT: lbu $2, 24($sp)
  ; CHECK-NEXT: #APP
  tail call void asm sideeffect "", ""()
  ret i8 %loaded
}

define i64 @load_store_stack_i32_truncstore(i64 %arg, ptr addrspace(200) %padding) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_stack_i32_truncstore:
  tail call void asm sideeffect "", ""()
  ; Store dummy cap to stack slot 1
  %padding.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  store volatile ptr addrspace(200) %padding, ptr addrspace(200) %padding.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, 32($c11)
  ; MIPS-NEXT:  sd $5, 32($sp)
  ; CHECK-NEXT: #APP
  ; Store the i32 to stack slot 2
  %arg.stack = alloca i32, align 8, addrspace(200)
  tail call void asm sideeffect "", ""()
  %value = trunc i64 %arg to i32
  store volatile i32 %value, ptr addrspace(200) %arg.stack, align 4
  ; TODO: MIPS backend can't fold the trunc into the store?
  ; CHECK:      #NO_APP
  ; TODO: this sll is unncessary
  ; PURECAP-NEXT: csw	$4, $zero, 24($c11)
  ; MIPS-NEXT: sw	$4, 24($sp)
  ; CHECK-NEXT: #APP
  ; And load it back
  tail call void asm sideeffect "", ""()
  ret i64 %arg
}

define i64 @load_store_stack_i32_sext(i32 %arg, ptr addrspace(200) %padding) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_stack_i32_sext:
  tail call void asm sideeffect "", ""()
  ; Store dummy cap to stack slot 1
  %padding.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  store volatile ptr addrspace(200) %padding, ptr addrspace(200) %padding.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, 32($c11)
  ; MIPS-NEXT:  sd $5, 32($sp)
  ; CHECK-NEXT: #APP
  ; Store the i32 to stack slot 2
  %arg.stack = alloca i32, align 8, addrspace(200)
  tail call void asm sideeffect "", ""()
  store volatile i32 %arg, ptr addrspace(200) %arg.stack, align 4
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csw	$4, $zero, 24($c11)
  ; MIPS-NEXT: sw	$4, 24($sp)
  ; CHECK-NEXT: #APP
  ; And load it back
  tail call void asm sideeffect "", ""()
  %loaded = load volatile i32, ptr addrspace(200) %arg.stack, align 4
  %sext_val = sext i32 %loaded to i64
  tail call void asm sideeffect "", ""()
  ret i64 %sext_val
  ; CHECK: #NO_APP
  ; PURECAP-NEXT:  clw $1, $zero, 24($c11)
  ; MIPS-NEXT: lw $1, 24($sp)
  ; TODO: this sext is not folded into the load
  ; CHECK-NEXT: sll $2, $1, 0
  ; CHECK-NEXT: #APP
}

define i1 @load_store_stack_i1(i1 %arg, ptr addrspace(200) %padding) addrspace(200) nounwind {
  ; CHECK-LABEL: load_store_stack_i1:
  tail call void asm sideeffect "", ""()
  ; Store dummy cap to stack slot 1
  %padding.stack = alloca ptr addrspace(200), align 32, addrspace(200)
  store volatile ptr addrspace(200) %padding, ptr addrspace(200) %padding.stack, align 32
  ; CHECK:      #NO_APP
  ; PURECAP-NEXT: csc $c3, $zero, 32($c11)
  ; MIPS-NEXT:  sd $5, 32($sp)
  ; CHECK-NEXT: #APP
  ; Store the i32 to stack slot 2
  %arg.stack = alloca i1, align 8, addrspace(200)
  tail call void asm sideeffect "", ""()
  store volatile i1 %arg, ptr addrspace(200) %arg.stack, align 4
  ; CHECK:      #NO_APP
  ; CHECK-NEXT:   andi $1, $4, 1
  ; PURECAP-NEXT: csb	$1, $zero, 24($c11)
  ; MIPS-NEXT:    sb	$1, 24($sp)
  ; CHECK-NEXT: #APP
  ; And load it back
  tail call void asm sideeffect "", ""()
  %loaded = load volatile i1, ptr addrspace(200) %arg.stack, align 4
  tail call void asm sideeffect "", ""()
  ret i1 %loaded
  ; CHECK: #NO_APP
  ; PURECAP-NEXT:  clbu $2, $zero, 24($c11)
  ; MIPS-NEXT: lbu $2, 24($sp)
  ; CHECK-NEXT: #APP
}
