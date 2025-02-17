; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; test that we zero $c13 on return from a variadic function or function with on-stack arguments
; If we do this we can always assume that any non-varidic function has a null $c13 on entry
; This is not strictly required but does ensure that all on-stack arguments are no longer reachable
; after the return.
; TODO: It might make more sense to do this in the caller since the stack is owned by the caller not the callee
; RUN: %cheri_purecap_llc -verify-machineinstrs -cheri-cap-table-abi=plt %s -o - | %cheri_FileCheck %s

@global = local_unnamed_addr addrspace(200) global i8 123, align 8

define ptr addrspace(200) @many_arg_fn(ptr addrspace(200) %arg1, ptr addrspace(200) %arg2, ptr addrspace(200) %arg3, ptr addrspace(200) %arg4, ptr addrspace(200) %arg5, ptr addrspace(200) %arg6, ptr addrspace(200) %arg7, ptr addrspace(200) %arg8, ptr addrspace(200) %arg9, ptr addrspace(200) %arg10, ptr addrspace(200) %arg11, ptr addrspace(200) %arg12) {
; CHECK-LABEL: many_arg_fn:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    clcbi $c3, %captab20(global)($c26)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    cgetnull $c13
entry:
  ret ptr addrspace(200) @global
}

define ptr addrspace(200) @variadic_fn(ptr addrspace(200) %in_arg1, ...) {
;; This cgetnull should remain here since we are calling a function without on-stack args from one with
; CHECK-LABEL: variadic_fn:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    clcbi $c3, %captab20(global)($c26)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    cgetnull $c13
entry:
  ret ptr addrspace(200) @global
}

define ptr addrspace(200) @no_onstack_args(ptr addrspace(200) %in_arg1) {
;; There should not be a cgetnull for a function with only one arg
; CHECK-LABEL: no_onstack_args:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    clcbi $c3, %captab20(global)($c26)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    nop
entry:
  ret ptr addrspace(200) @global
}

define ptr addrspace(200) @eight_arg_fn(ptr addrspace(200) %arg1, ptr addrspace(200) %arg2, ptr addrspace(200) %arg3, ptr addrspace(200) %arg4, ptr addrspace(200) %arg5, ptr addrspace(200) %arg6, ptr addrspace(200) %arg7, ptr addrspace(200) %arg8) {
;; There should not be a cgetnull for a function with 8 args (no-onstack args)
; CHECK-LABEL: eight_arg_fn:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    clcbi $c3, %captab20(global)($c26)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    nop
entry:
  ret ptr addrspace(200) @global
}

define ptr addrspace(200) @nine_arg_fn(ptr addrspace(200) %arg1, ptr addrspace(200) %arg2, ptr addrspace(200) %arg3, ptr addrspace(200) %arg4, ptr addrspace(200) %arg5, ptr addrspace(200) %arg6, ptr addrspace(200) %arg7, ptr addrspace(200) %arg8, ptr addrspace(200) %arg9) {
;; We should clear $c13 in a function with 9 args since that includes one on-stack arg
; CHECK-LABEL: nine_arg_fn:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    clcbi $c3, %captab20(global)($c26)
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    cgetnull $c13
entry:
  ret ptr addrspace(200) @global
}

define ptr addrspace(200) @no_onstack_args_call_variadic(ptr addrspace(200) %in_arg1) nounwind {
;; We should not need to clear $c13 after calling the variadic function since it will clear it prior to return
; CHECK-LABEL: no_onstack_args_call_variadic:
; CHECK:       # %bb.0: # %entry
; CHECK-NEXT:    cincoffset $c11, $c11, -48
; CHECK-NEXT:    csc $c18, $zero, 32($c11) # 16-byte Folded Spill
; CHECK-NEXT:    csc $c17, $zero, 16($c11) # 16-byte Folded Spill
; CHECK-NEXT:    cmove $c18, $c26
; CHECK-NEXT:    daddiu $1, $zero, 42
; CHECK-NEXT:    csd $1, $zero, 0($c11)
; CHECK-NEXT:    csetbounds $c1, $c11, 8
; CHECK-NEXT:    clcbi $c12, %capcall20(variadic_fn)($c18)
; CHECK-NEXT:    ori $1, $zero, 65495
; CHECK-NEXT:    cjalr $c12, $c17
; CHECK-NEXT:    candperm $c13, $c1, $1
; CHECK-NEXT:    clcbi $c3, %captab20(global)($c18)
; Restore $cgp
; CHECK-NEXT:    cmove $c26, $c18
; CHECK-NEXT:    clc $c17, $zero, 16($c11) # 16-byte Folded Reload
; CHECK-NEXT:    clc $c18, $zero, 32($c11) # 16-byte Folded Reload
; CHECK-NEXT:    cjr $c17
; CHECK-NEXT:    cincoffset $c11, $c11, 48
entry:
  %0 = call ptr addrspace(200) (ptr addrspace(200), ...) @variadic_fn(ptr addrspace(200) %in_arg1, i64 42)
  ret ptr addrspace(200) @global
}
