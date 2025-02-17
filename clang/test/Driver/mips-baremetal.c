// RUN: %clang -target mips64-none-elf %s -o %t -### 2>&1 | FileCheck %s -check-prefixes CHECK,NOSYSROOT,N64
// RUN: %clang --sysroot=/foo/bar -target mips64-none-elf %s -o %t -### 2>&1 | FileCheck %s -check-prefixes CHECK,SYSROOT,N64

// C++ has some more libraries and include dirs:
// RUN: %clangxx --sysroot=/foo/bar -xc++ -target mips64-none-elf %s -o %t -### 2>&1 | FileCheck %s -check-prefixes CHECK,SYSROOT,CXX,N64
// RUN: %clang -target mips64-none-elf -fPIC -mabi=purecap %s -o %t -### 2>&1 | FileCheck %s -check-prefixes CHECK,CHERIABI
// RUN: %clangxx -xc++ -target mips64-none-elf -fPIC -mabi=purecap %s -o %t -### 2>&1 | FileCheck %s -check-prefixes CHECK,CHERIABI

// N64: "-cc1" "-triple" "{{(mips64|mips64c128)}}-none-unknown-elf" "-emit-obj"
// CHERIABI: "-cc1" "-triple" "mips64-none-unknown-purecap" "-emit-obj"
// CHECK-NOT: "-no-integrated-as"
// CHECK-SAME: "-target-abi" "[[ABI:(n64|purecap)]]"
// CHECK-SAME: "-resource-dir" "[[RESOURCE_DIR:[^"]+]]"
// SYSROOT-SAME: "-isysroot" "[[SYSROOT:[^"]+]]"
// CXX-SAME: "-internal-isystem" "[[SYSROOT]]/include/c++/v1"
// CHECK-SAME: "-internal-isystem" "[[RESOURCE_DIR]]/include"
// SYSROOT-SAME: "-internal-isystem" "[[SYSROOT]]/include"
// NOSYSROOT-SAME: "-internal-isystem" "{{[^"]+}}./lib/clang-runtimes/mips64-none-elf/include"

// CHECK: "{{.*}}ld.lld" "{{.+}}.o" "-Bstatic"
// SYSROOT-SAME:   "-L[[SYSROOT]]/lib"
// NOSYSROOT-SAME: "-L{{.+}}/lib/clang-runtimes/mips64-none-elf/lib"
// CHERIABI-SAME: "-L{{.+}}/lib/clang-runtimes/mips64-none-elf/lib"
// CXX-SAME: "-lc++" "-lc++abi" "-lunwind"
// CHECK-SAME: "-lc" "-lm"
// CHECK-SAME: "-lclang_rt.builtins-mips64"
// CHECK-SAME: "-o" "{{.+}}/mips-baremetal.c.tmp"
