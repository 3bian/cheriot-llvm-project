; RUN: llc < %s

; NVPTX can not select llvm.stacksave (dynamic_stackalloc) and llvm.stackrestore
; UNSUPPORTED: target=nvptx{{.*}}

declare ptr @llvm.stacksave.p0()

declare void @llvm.stackrestore.p0(ptr)

define ptr @test(i32 %N) {
        %tmp = call ptr @llvm.stacksave.p0( )              ; <ptr> [#uses=1]
        %P = alloca i32, i32 %N         ; <ptr> [#uses=1]
        call void @llvm.stackrestore.p0( ptr %tmp )
        %Q = alloca i32, i32 %N         ; <ptr> [#uses=0]
        ret ptr %P
}

