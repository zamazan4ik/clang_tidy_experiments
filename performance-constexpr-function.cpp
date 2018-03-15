/*
// RUN: %check_clang_tidy %s performance-constexpr-function %t

// FIXME: Add something that triggers the check here.
constexpr void f() {}
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: function 'f' is insufficiently awesome [performance-constexpr-function]

// FIXME: Verify the applied fix.
//   * Make the CHECK patterns specific enough and try to make verified lines
//     unique to avoid incorrect matches.
//   * Use {{}} for regular expressions.
// CHECK-FIXES: {{^}}void awesome_f();{{$}}

// FIXME: Add something that doesn't trigger the check here.
void f2() {}

struct A
{
    // Error: try...catch statement in ctor
    A() try {}
        catch (...)
        {}

    // Error: virtual method
    virtual void f() {}
};

struct B : virtual public A
{
    // Error: virtual inheritance
    B(){}
};

struct NonTrivialStruct
{
    NonTrivialStruct()
    {
        ptr = new int[10];
    }
    ~NonTrivialStruct()
    {
        delete[] ptr;
    }
    int* ptr;
};

struct C
{
    // Error: ASM isn't allowed
    C()
    {
        __asm__ volatile ("" : :);
    }

    //Error: goto statement isn't allowed
    C(int a)
    {
        goto label;
        label:
        int temp;
    }

    // Error: try...catch() statement isn't allowed
    C(int a, int b)
    {
        try {} catch (...){}
    }

    // Error: static storage
    C(int a, int b, int c)
    {
        static int d;
    }

    // Error: thread_local storage
    C(int a, int b, int c, int d)
    {
        thread_local int e;
    }

    C(int a, int b, int c, int d, int e, int f)
    {
        NonTrivialStruct n;
    }
    // Ok
    C(int a, int& b, int c[]) {}
};

struct D
{
    D()
    {
        int a = 42;
        switch(a)
        {
            case 1:
            default:
                break;
        }
    }
};*/

struct NonTrivialStruct
{
    NonTrivialStruct()
    {
        ptr = new int[10];
    }
    ~NonTrivialStruct()
    {
        delete[] ptr;
    }
    int* ptr;
};

void empty_function() {}

constexpr void constexpr_function() {}

void func_with_exception()
{
    try {} catch(...) {}
}

void func_with_label()
{
    label:
    int a;

    goto label;
}

void func_with_static()
{
    static int a;
}

void func_with_thread()
{
    thread_local int a;
}

void func_non_trivial()
{
    NonTrivialStruct nonTrivialStruct;
}

void func_with_asm()
{
    __asm__ volatile ("" : :);
}

void func_with_no_init()
{
    int a = 5;
}

NonTrivialStruct func_with_nonliteral_return()
{
    return NonTrivialStruct();
}