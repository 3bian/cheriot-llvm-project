// RUN: %clang_cc1 -std=c++11 -triple x86_64-none-linux-gnu -fmerge-all-constants -emit-llvm -o - %s | FileCheck -check-prefixes=X86,CHECK %s
// RUN: %clang_cc1 -std=c++11 -triple amdgcn-amd-amdhsa -DNO_TLS -fmerge-all-constants -emit-llvm -o - %s | FileCheck -check-prefixes=AMDGCN,CHECK %s

namespace std {
  typedef decltype(sizeof(int)) size_t;

  // libc++'s implementation
  template <class _E>
  class initializer_list
  {
    const _E* __begin_;
    size_t    __size_;

    initializer_list(const _E* __b, size_t __s)
      : __begin_(__b),
        __size_(__s)
    {}

  public:
    typedef _E        value_type;
    typedef const _E& reference;
    typedef const _E& const_reference;
    typedef size_t    size_type;

    typedef const _E* iterator;
    typedef const _E* const_iterator;

    initializer_list() : __begin_(nullptr), __size_(0) {}

    size_t    size()  const {return __size_;}
    const _E* begin() const {return __begin_;}
    const _E* end()   const {return __begin_ + __size_;}
  };
}

struct destroyme1 {
  ~destroyme1();
};
struct destroyme2 {
  ~destroyme2();
};
struct witharg1 {
  witharg1(const destroyme1&);
  ~witharg1();
};
struct wantslist1 {
  wantslist1(std::initializer_list<destroyme1>);
  ~wantslist1();
};
// X86: @_ZGR15globalInitList1_ = internal constant [3 x i32] [i32 1, i32 2, i32 3]
// X86: @globalInitList1 ={{.*}} global %{{[^ ]+}} { ptr @_ZGR15globalInitList1_, i{{32|64}} 3 }
// AMDGCN: @_ZGR15globalInitList1_ = internal addrspace(1) constant [3 x i32] [i32 1, i32 2, i32 3]
// AMDGCN: @globalInitList1 ={{.*}} addrspace(1) global %{{[^ ]+}} { ptr addrspacecast (ptr addrspace(1) @_ZGR15globalInitList1_ to ptr), i{{32|64}} 3 }
std::initializer_list<int> globalInitList1 = {1, 2, 3};

#ifndef NO_TLS
namespace thread_local_global_array {
// FIXME: We should be able to constant-evaluate this even though the
// initializer is not a constant expression (pointers to thread_local
// objects aren't really a problem).
//
// X86: @_ZN25thread_local_global_array1xE ={{.*}} thread_local global
// X86: @_ZGRN25thread_local_global_array1xE_ = internal thread_local constant [4 x i32] [i32 1, i32 2, i32 3, i32 4]
std::initializer_list<int> thread_local x = {1, 2, 3, 4};
}
#endif

// X86: @globalInitList2 ={{.*}} global %{{[^ ]+}} zeroinitializer
// X86: @_ZGR15globalInitList2_ = internal global [2 x %[[WITHARG:[^ ]*]]] zeroinitializer
// AMDGCN: @globalInitList2 ={{.*}} addrspace(1) global %{{[^ ]+}} zeroinitializer
// AMDGCN: @_ZGR15globalInitList2_ = internal addrspace(1) global [2 x %[[WITHARG:[^ ]*]]] zeroinitializer

// X86: @_ZN15partly_constant1kE ={{.*}} global i32 0, align 4
// X86: @_ZN15partly_constant2ilE ={{.*}} global {{.*}} null, align 8
// X86: @[[PARTLY_CONSTANT_OUTER:_ZGRN15partly_constant2ilE_]] = internal global {{.*}} zeroinitializer, align 8
// X86: @[[PARTLY_CONSTANT_INNER:_ZGRN15partly_constant2ilE0_]] = internal global [3 x {{.*}}] zeroinitializer, align 8
// X86: @[[PARTLY_CONSTANT_FIRST:_ZGRN15partly_constant2ilE1_]] = internal constant [3 x i32] [i32 1, i32 2, i32 3], align 4
// X86: @[[PARTLY_CONSTANT_SECOND:_ZGRN15partly_constant2ilE2_]] = internal global [2 x i32] zeroinitializer, align 4
// X86: @[[PARTLY_CONSTANT_THIRD:_ZGRN15partly_constant2ilE3_]] = internal constant [4 x i32] [i32 5, i32 6, i32 7, i32 8], align 4
// AMDGCN: @_ZN15partly_constant1kE ={{.*}} addrspace(1) global i32 0, align 4
// AMDGCN: @_ZN15partly_constant2ilE ={{.*}} addrspace(1) global {{.*}} null, align 8
// AMDGCN: @[[PARTLY_CONSTANT_OUTER:_ZGRN15partly_constant2ilE_]] = internal addrspace(1) global {{.*}} zeroinitializer, align 8
// AMDGCN: @[[PARTLY_CONSTANT_INNER:_ZGRN15partly_constant2ilE0_]] = internal addrspace(1) global [3 x {{.*}}] zeroinitializer, align 8
// AMDGCN: @[[PARTLY_CONSTANT_FIRST:_ZGRN15partly_constant2ilE1_]] = internal addrspace(1) constant [3 x i32] [i32 1, i32 2, i32 3], align 4
// AMDGCN: @[[PARTLY_CONSTANT_SECOND:_ZGRN15partly_constant2ilE2_]] = internal addrspace(1) global [2 x i32] zeroinitializer, align 4
// AMDGCN: @[[PARTLY_CONSTANT_THIRD:_ZGRN15partly_constant2ilE3_]] = internal addrspace(1) constant [4 x i32] [i32 5, i32 6, i32 7, i32 8], align 4

// X86: @[[REFTMP1:.*]] = private constant [2 x i32] [i32 42, i32 43], align 4
// X86: @[[REFTMP2:.*]] = private constant [3 x %{{.*}}] [%{{.*}} { i32 1 }, %{{.*}} { i32 2 }, %{{.*}} { i32 3 }], align 4
// AMDGCN: @[[REFTMP1:.*]] = private addrspace(4) constant [2 x i32] [i32 42, i32 43], align 4
// AMDGCN: @[[REFTMP2:.*]] = private addrspace(4) constant [3 x %{{.*}}] [%{{.*}} { i32 1 }, %{{.*}} { i32 2 }, %{{.*}} { i32 3 }], align 4

// CHECK: appending {{(addrspace\(1\) )?}}global

// thread_local initializer:
// X86-LABEL: define internal void @__cxx_global_var_init
// X86: [[ADDR:%.*]] = call {{.*}} ptr @llvm.threadlocal.address.p0(ptr {{.*}} @_ZN25thread_local_global_array1xE)
// X86: store ptr @_ZGRN25thread_local_global_array1xE_, ptr [[ADDR]], align 8
// X86: store i64 4, ptr getelementptr inbounds ({{.*}}, ptr @_ZN25thread_local_global_array1xE, i32 0, i32 1), align 8

// CHECK-LABEL: define internal void @__cxx_global_var_init
// X86: call void @_ZN8witharg1C1ERK10destroyme1(ptr {{[^,]*}} @_ZGR15globalInitList2_
// X86: call void @_ZN8witharg1C1ERK10destroyme1(ptr {{[^,]*}} getelementptr inbounds (%[[WITHARG]], ptr @_ZGR15globalInitList2_, i{{32|64}} 1)
// AMDGCN: call void @_ZN8witharg1C1ERK10destroyme1(ptr {{[^,]*}} addrspacecast ({{[^@]+}} @_ZGR15globalInitList2_ {{[^)]+}}
// AMDGCN: call void @_ZN8witharg1C1ERK10destroyme1(ptr {{[^,]*}} getelementptr inbounds (%[[WITHARG]], ptr addrspacecast ({{[^@]+}} @_ZGR15globalInitList2_ {{[^)]+}}), i{{32|64}} 1
// CHECK: call i32 @__cxa_atexit
// X86: store ptr @_ZGR15globalInitList2_, ptr @globalInitList2, align 8
// X86: store i64 2, ptr getelementptr inbounds (%{{.*}}, ptr @globalInitList2, i32 0, i32 1), align 8
// AMDGCN: store ptr addrspacecast ({{[^@]+}} @_ZGR15globalInitList2_ {{[^)]+}}),
// AMDGCN:       ptr addrspacecast ({{[^@]+}} @globalInitList2 {{[^)]+}}), align 8
// AMDGCN: store i64 2, ptr getelementptr inbounds (%{{.*}}, ptr addrspacecast ({{[^@]+}} @globalInitList2 {{[^)]+}}), i32 0, i32 1), align 8
// CHECK: call void @_ZN10destroyme1D1Ev
// CHECK-NEXT: call void @_ZN10destroyme1D1Ev
// CHECK-NEXT: ret void
std::initializer_list<witharg1> globalInitList2 = {
  witharg1(destroyme1()), witharg1(destroyme1())
};

void fn1(int i) {
  // CHECK-LABEL: define{{.*}} void @_Z3fn1i
  // temporary array
  // X86: [[array:%[^ ]+]] = alloca [3 x i32]
  // AMDGCN: [[alloca:%[^ ]+]] = alloca [3 x i32], align 4, addrspace(5)
  // AMDGCN: [[array:%[^ ]+]] ={{.*}} addrspacecast ptr addrspace(5) [[alloca]] to ptr
  // CHECK: getelementptr inbounds [3 x i32], ptr [[array]], i{{32|64}} 0
  // CHECK-NEXT: store i32 1, ptr
  // CHECK-NEXT: getelementptr
  // CHECK-NEXT: store
  // CHECK-NEXT: getelementptr
  // CHECK-NEXT: load
  // CHECK-NEXT: store
  // init the list
  // CHECK-NEXT: getelementptr
  // CHECK-NEXT: getelementptr inbounds [3 x i32], ptr
  // CHECK-NEXT: store ptr
  // CHECK-NEXT: getelementptr
  // CHECK-NEXT: store i{{32|64}} 3
  std::initializer_list<int> intlist{1, 2, i};
}

void fn2() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn2v
  void target(std::initializer_list<destroyme1>);
  // objects should be destroyed before dm2, after call returns
  // CHECK: call void @_Z6targetSt16initializer_listI10destroyme1E
  target({ destroyme1(), destroyme1() });
  // CHECK: call void @_ZN10destroyme1D1Ev
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
}

void fn3() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn3v
  // objects should be destroyed after dm2
  auto list = { destroyme1(), destroyme1() };
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
  // CHECK: call void @_ZN10destroyme1D1Ev
}

void fn4() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn4v
  void target(std::initializer_list<witharg1>);
  // objects should be destroyed before dm2, after call returns
  // CHECK: call void @_ZN8witharg1C1ERK10destroyme1
  // CHECK: call void @_Z6targetSt16initializer_listI8witharg1E
  target({ witharg1(destroyme1()), witharg1(destroyme1()) });
  // CHECK: call void @_ZN8witharg1D1Ev
  // CHECK: call void @_ZN10destroyme1D1Ev
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
}

void fn5() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn5v
  // temps should be destroyed before dm2
  // objects should be destroyed after dm2
  // CHECK: call void @_ZN8witharg1C1ERK10destroyme1
  auto list = { witharg1(destroyme1()), witharg1(destroyme1()) };
  // CHECK: call void @_ZN10destroyme1D1Ev
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
  // CHECK: call void @_ZN8witharg1D1Ev
}

void fn6() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn6v
  void target(const wantslist1&);
  // objects should be destroyed before dm2, after call returns
  // CHECK: call void @_ZN10wantslist1C1ESt16initializer_listI10destroyme1E
  // CHECK: call void @_Z6targetRK10wantslist1
  target({ destroyme1(), destroyme1() });
  // CHECK: call void @_ZN10wantslist1D1Ev
  // CHECK: call void @_ZN10destroyme1D1Ev
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
}
void fn7() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn7v
  // temps should be destroyed before dm2
  // object should be destroyed after dm2
  // CHECK: call void @_ZN10wantslist1C1ESt16initializer_listI10destroyme1E
  wantslist1 wl = { destroyme1(), destroyme1() };
  // CHECK: call void @_ZN10destroyme1D1Ev
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
  // CHECK: call void @_ZN10wantslist1D1Ev
}

void fn8() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn8v
  void target(std::initializer_list<std::initializer_list<destroyme1>>);
  // objects should be destroyed before dm2, after call returns
  // CHECK: call void @_Z6targetSt16initializer_listIS_I10destroyme1EE
  std::initializer_list<destroyme1> inner;
  target({ inner, { destroyme1() } });
  // CHECK: call void @_ZN10destroyme1D1Ev
  // Only one destroy loop, since only one inner init list is directly inited.
  // CHECK-NOT: call void @_ZN10destroyme1D1Ev
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
}

void fn9() {
  // CHECK-LABEL: define{{.*}} void @_Z3fn9v
  // objects should be destroyed after dm2
  std::initializer_list<destroyme1> inner;
  std::initializer_list<std::initializer_list<destroyme1>> list =
      { inner, { destroyme1() } };
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
  // CHECK: call void @_ZN10destroyme1D1Ev
  // Only one destroy loop, since only one inner init list is directly inited.
  // CHECK-NOT: call void @_ZN10destroyme1D1Ev
  // CHECK: ret void
}

void fn10(int i) {
  // CHECK-LABEL: define{{.*}} void @_Z4fn10i
  // CHECK: alloca [3 x i32]
  // CHECK-X86: call noalias nonnull align 16 ptr @_Znw{{[jm]}}
  // CHECK-AMDGPU: call noalias nonnull align 8 ptr @_Znw{{[jm]}}
  // CHECK: store i32 %
  // CHECK: store i32 2
  // CHECK: store i32 3
  // CHECK: store ptr
  (void) new std::initializer_list<int> {i, 2, 3};
}

void fn11() {
  // CHECK-LABEL: define{{.*}} void @_Z4fn11v
  (void) new std::initializer_list<destroyme1> {destroyme1(), destroyme1()};
  // CHECK: call void @_ZN10destroyme1D1Ev
  destroyme2 dm2;
  // CHECK: call void @_ZN10destroyme2D1Ev
}

namespace PR12178 {
  struct string {
    string(int);
    ~string();
  };

  struct pair {
    string a;
    int b;
  };

  struct map {
    map(std::initializer_list<pair>);
  };

  map m{ {1, 2}, {3, 4} };
}

namespace rdar13325066 {
  struct X { ~X(); };

  // CHECK-LABEL: define{{.*}} void @_ZN12rdar133250664loopERNS_1XES1_
  void loop(X &x1, X &x2) {
    // CHECK: br label
    // CHECK: br i1
    // CHECK: br label
    // CHECK: call void @_ZN12rdar133250661XD1Ev
    // CHECK: br label
    // CHECK: br label
    // CHECK: call void @_ZN12rdar133250661XD1Ev
    // CHECK: br i1
    // CHECK: br label
    // CHECK: ret void
    for (X x : { x1, x2 }) { }
  }
}

namespace dtors {
  struct S {
    S();
    ~S();
  };
  void z();

  // CHECK-LABEL: define{{.*}} void @_ZN5dtors1fEv(
  void f() {
    // CHECK: call void @_ZN5dtors1SC1Ev(
    // CHECK: call void @_ZN5dtors1SC1Ev(
    std::initializer_list<S>{ S(), S() };

    // Destruction loop for underlying array.
    // CHECK: br label
    // CHECK: call void @_ZN5dtors1SD1Ev(
    // CHECK: br i1

    // CHECK: call void @_ZN5dtors1zEv(
    z();

    // CHECK-NOT: call void @_ZN5dtors1SD1Ev(
  }

  // CHECK-LABEL: define{{.*}} void @_ZN5dtors1gEv(
  void g() {
    // CHECK: call void @_ZN5dtors1SC1Ev(
    // CHECK: call void @_ZN5dtors1SC1Ev(
    auto x = std::initializer_list<S>{ S(), S() };

    // Destruction loop for underlying array.
    // CHECK: br label
    // CHECK: call void @_ZN5dtors1SD1Ev(
    // CHECK: br i1

    // CHECK: call void @_ZN5dtors1zEv(
    z();

    // CHECK-NOT: call void @_ZN5dtors1SD1Ev(
  }

  // CHECK-LABEL: define{{.*}} void @_ZN5dtors1hEv(
  void h() {
    // CHECK: call void @_ZN5dtors1SC1Ev(
    // CHECK: call void @_ZN5dtors1SC1Ev(
    std::initializer_list<S> x = { S(), S() };

    // CHECK-NOT: call void @_ZN5dtors1SD1Ev(

    // CHECK: call void @_ZN5dtors1zEv(
    z();

    // Destruction loop for underlying array.
    // CHECK: br label
    // CHECK: call void @_ZN5dtors1SD1Ev(
    // CHECK: br i1
  }
}

namespace partly_constant {
  int k;
  std::initializer_list<std::initializer_list<int>> &&il = { { 1, 2, 3 }, { 4, k }, { 5, 6, 7, 8 } };
  // First init list.
  // CHECK-NOT: @[[PARTLY_CONSTANT_FIRST]],
  // CHECK: store ptr {{.*}}@[[PARTLY_CONSTANT_FIRST]]{{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_INNER]]{{.*}}
  // CHECK: store i64 3, ptr getelementptr inbounds ({{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_INNER]]{{.*}}, i32 0, i32 1)
  // CHECK-NOT: @[[PARTLY_CONSTANT_FIRST]],
  //
  // Second init list array (non-constant).
  // CHECK: store i32 4, ptr {{.*}}@[[PARTLY_CONSTANT_SECOND]]{{.*}}
  // CHECK: load i32, ptr {{.*}}@_ZN15partly_constant1kE
  // CHECK: store i32 {{.*}}, ptr getelementptr inbounds ({{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_SECOND]]{{.*}}, i64 1)
  //
  // Second init list.
  // CHECK: store ptr {{.*}}@[[PARTLY_CONSTANT_SECOND]]{{.*}}, ptr getelementptr inbounds ({{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_INNER]]{{.*}}, i64 1)
  // CHECK: store i64 2, ptr getelementptr inbounds ({{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_INNER]]{{.*}}, i64 1, i32 1)
  //
  // Third init list.
  // CHECK-NOT: @[[PARTLY_CONSTANT_THIRD]],
  // CHECK: store ptr {{.*}}@[[PARTLY_CONSTANT_THIRD]]{{.*}}, ptr getelementptr inbounds ({{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_INNER]]{{.*}}, i64 2)
  // CHECK: store i64 4, ptr getelementptr inbounds ({{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_INNER]]{{.*}}, i64 2, i32 1)
  // CHECK-NOT: @[[PARTLY_CONSTANT_THIRD]],
  //
  // Outer init list.
  // CHECK: store ptr {{.*}}@[[PARTLY_CONSTANT_INNER]]{{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_OUTER]]{{.*}}
  // CHECK: store i64 3, ptr getelementptr inbounds ({{.*}}, ptr {{.*}}@[[PARTLY_CONSTANT_OUTER]]{{.*}}, i32 0, i32 1)
  //
  // 'il' reference.
  // CHECK: store ptr {{.*}}@[[PARTLY_CONSTANT_OUTER]]{{.*}}, ptr {{.*}}@_ZN15partly_constant2ilE{{.*}}, align 8
}
namespace nested {
  struct A { A(); ~A(); };
  struct B { const A &a; ~B(); };
  struct C { std::initializer_list<B> b; ~C(); };
  void f();
  // CHECK-LABEL: define{{.*}} void @_ZN6nested1gEv(
  void g() {
    // CHECK: call void @_ZN6nested1AC1Ev(
    // CHECK-NOT: call
    // CHECK: call void @_ZN6nested1AC1Ev(
    // CHECK-NOT: call
    const C &c { { { A() }, { A() } } };

    // CHECK: call void @_ZN6nested1fEv(
    // CHECK-NOT: call
    f();

    // CHECK: call void @_ZN6nested1CD1Ev(
    // CHECK-NOT: call

    // Destroy B[2] array.
    // FIXME: This isn't technically correct: reverse construction order would
    // destroy the second B then the second A then the first B then the first A.
    // CHECK: call void @_ZN6nested1BD1Ev(
    // CHECK-NOT: call
    // CHECK: br

    // CHECK-NOT: call
    // CHECK: call void @_ZN6nested1AD1Ev(
    // CHECK-NOT: call
    // CHECK: call void @_ZN6nested1AD1Ev(
    // CHECK-NOT: call
    // CHECK: }
  }
}

namespace DR1070 {
  struct A {
    A(std::initializer_list<int>);
  };
  struct B {
    int i;
    A a;
  };
  B b = {1};
  struct C {
    std::initializer_list<int> a;
    B b;
    std::initializer_list<double> c;
  };
  C c = {};
}

namespace ArrayOfInitList {
  struct S {
    S(std::initializer_list<int>);
  };
  S x[1] = {};
}

namespace PR20445 {
  struct vector { vector(std::initializer_list<int>); };
  struct MyClass { explicit MyClass(const vector &v); };
  template<int x> void f() { new MyClass({42, 43}); }
  template void f<0>();
  // CHECK-LABEL: define {{.*}} @_ZN7PR204451fILi0EEEvv(
  // CHECK: store ptr {{.*}}@[[REFTMP1]]{{.*}}
  // CHECK: call void @_ZN7PR204456vectorC1ESt16initializer_listIiE(
  // CHECK: call void @_ZN7PR204457MyClassC1ERKNS_6vectorE(
}

namespace ConstExpr {
  class C {
    int x;
  public:
    constexpr C(int x) : x(x) {}
  };
  void f(std::initializer_list<C>);
  void g() {
    // CHECK-LABEL: _ZN9ConstExpr1gEv
    // CHECK: store ptr {{.*}}@[[REFTMP2]]{{.*}}
    // CHECK: call void @_ZN9ConstExpr1fESt16initializer_listINS_1CEE
    f({C(1), C(2), C(3)});
  }
}

namespace B19773010 {
  template <class T1, class T2> struct pair {
    T1 first;
    T2 second;
    constexpr pair() : first(), second() {}
    constexpr pair(T1 a, T2 b) : first(a), second(b) {}
  };

  enum E { ENUM_CONSTANT };
  struct testcase {
    testcase(std::initializer_list<pair<const char *, E>>);
  };
  void f1() {
    // CHECK-LABEL: @_ZN9B197730102f1Ev
    testcase a{{"", ENUM_CONSTANT}};
    // X86: store ptr @.ref.tmp{{.*}}, ptr %{{.*}}, align 8
    // AMDGCN: store ptr addrspacecast{{.*}} @.ref.tmp{{.*}}{{.*}}, ptr %{{.*}}, align 8
  }
  void f2() {
    // CHECK-LABEL: @_ZN9B197730102f2Ev
    // X86: store ptr @_ZGRZN9B197730102f2EvE1p_, ptr getelementptr inbounds (%"class.std::initializer_list.10", ptr @_ZZN9B197730102f2EvE1p, i64 1), align 16
    // AMDGCN: store ptr addrspacecast{{.*}} @_ZGRZN9B197730102f2EvE1p_{{.*}}, ptr getelementptr inbounds (%"class.std::initializer_list.10", ptr addrspacecast{{.*}}@_ZZN9B197730102f2EvE1p{{.*}}, i64 1), align 8
    static std::initializer_list<pair<const char *, E>> a, p[2] =
        {a, {{"", ENUM_CONSTANT}}};
  }

  void PR22940_helper(const pair<void*, int>&) { }
  void PR22940() {
    // CHECK-LABEL: @_ZN9B197730107PR22940Ev
    // CHECK: call {{.*}} @_ZN9B197730104pairIPviEC{{.}}Ev(
    // CHECK: call {{.*}} @_ZN9B1977301014PR22940_helperERKNS_4pairIPviEE(
    PR22940_helper(pair<void*, int>());
  }
}
