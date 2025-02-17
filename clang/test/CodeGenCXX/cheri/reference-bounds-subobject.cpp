// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py
/// Check that we can set bounds on references
// REQUIRES: asserts
// RUN: %cheri_purecap_cc1 -cheri-bounds=references-only -O2 -std=c++17 -emit-llvm %s -o - -mllvm -debug-only=cheri-bounds -mllvm -stats 2>%t.dbg | FileCheck %s
// RUN: FileCheck -input-file %t.dbg %s -check-prefix DBG
/// Check that using hybrid codegen with -cheri-bounds= does not trigger assertions by incorrectly adding bounds
// RUN: %cheri_cc1 -cheri-bounds=references-only -O2 -std=c++17 -emit-llvm %s -o - | FileCheck %s --check-prefix=HYBRID

// DBG: Found record type 'Nested' -> is C-like struct type and is marked as final -> setting bounds for 'Nested' reference to 8
// DBG: Found scalar type -> setting bounds for 'int' reference to 4
// DBG: Found scalar type -> setting bounds for 'float' reference to 4

struct Nested final {
    int a;
    int b;
};

struct WithNested {
  float f1;
  Nested n;
  float f2;
};

void do_stuff_with_ref(int&);
void do_stuff_with_ref(float&);
void do_stuff_with_ref(Nested& nref);
void do_stuff_with_ptr(Nested* nptr);

// CHECK-LABEL: @_Z18test_subobject_refR10WithNested(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[N:%.*]] = getelementptr inbounds [[STRUCT_WITHNESTED:%.*]], ptr addrspace(200) [[S:%.*]], i64 0, i32 1
// CHECK-NEXT:    [[TMP0:%.*]] = tail call ptr addrspace(200) @llvm.cheri.cap.bounds.set.i64(ptr addrspace(200) nonnull [[N]], i64 8)
// CHECK-NEXT:    tail call void @_Z17do_stuff_with_refR6Nested(ptr addrspace(200) noundef nonnull align 4 dereferenceable(8) [[TMP0]]) #[[ATTR3:[0-9]+]]
// CHECK-NEXT:    [[TMP1:%.*]] = tail call ptr addrspace(200) @llvm.cheri.cap.bounds.set.i64(ptr addrspace(200) nonnull [[N]], i64 4)
// CHECK-NEXT:    tail call void @_Z17do_stuff_with_refRi(ptr addrspace(200) noundef nonnull align 4 dereferenceable(4) [[TMP1]]) #[[ATTR3]]
// CHECK-NEXT:    [[TMP2:%.*]] = tail call ptr addrspace(200) @llvm.cheri.cap.bounds.set.i64(ptr addrspace(200) nonnull [[S]], i64 4)
// CHECK-NEXT:    tail call void @_Z17do_stuff_with_refRf(ptr addrspace(200) noundef nonnull align 4 dereferenceable(4) [[TMP2]]) #[[ATTR3]]
// CHECK-NEXT:    ret void
//
// HYBRID-LABEL: @_Z18test_subobject_refR10WithNested(
// HYBRID-NEXT:  entry:
// HYBRID-NEXT:    [[N:%.*]] = getelementptr inbounds [[STRUCT_WITHNESTED:%.*]], ptr [[S:%.*]], i64 0, i32 1
// HYBRID-NEXT:    tail call void @_Z17do_stuff_with_refR6Nested(ptr noundef nonnull align 4 dereferenceable(8) [[N]]) #[[ATTR2:[0-9]+]]
// HYBRID-NEXT:    tail call void @_Z17do_stuff_with_refRi(ptr noundef nonnull align 4 dereferenceable(4) [[N]]) #[[ATTR2]]
// HYBRID-NEXT:    tail call void @_Z17do_stuff_with_refRf(ptr noundef nonnull align 4 dereferenceable(4) [[S]]) #[[ATTR2]]
// HYBRID-NEXT:    ret void
//
void test_subobject_ref(WithNested& s) {
  do_stuff_with_ref(s.n);
  do_stuff_with_ref(s.n.a);
  do_stuff_with_ref(s.f1);
}

// CHECK-LABEL: @_Z18test_subobject_ptrR10WithNested(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[N:%.*]] = getelementptr inbounds [[STRUCT_WITHNESTED:%.*]], ptr addrspace(200) [[S:%.*]], i64 0, i32 1
// CHECK-NEXT:    tail call void @_Z17do_stuff_with_ptrP6Nested(ptr addrspace(200) noundef nonnull [[N]]) #[[ATTR3]]
// CHECK-NEXT:    ret void
//
// HYBRID-LABEL: @_Z18test_subobject_ptrR10WithNested(
// HYBRID-NEXT:  entry:
// HYBRID-NEXT:    [[N:%.*]] = getelementptr inbounds [[STRUCT_WITHNESTED:%.*]], ptr [[S:%.*]], i64 0, i32 1
// HYBRID-NEXT:    tail call void @_Z17do_stuff_with_ptrP6Nested(ptr noundef nonnull [[N]]) #[[ATTR2]]
// HYBRID-NEXT:    ret void
//
void test_subobject_ptr(WithNested& s) {
// No bounds on the pointer here:
  do_stuff_with_ptr(&s.n);
}

// DBG: 3 cheri-bounds     - Number of references checked for tightening bounds
// DBG: 3 cheri-bounds     - Number of references where bounds were tightened
