
#include "CppScript/Boxed.h"

#include <QtTest>
#include <QFlags>

#include <typeinfo>
#include <iostream>

class TestBoxed : public QObject
{
    Q_OBJECT

public:
    TestBoxed();

private slots:
    void anyTypeOf_data();
    void anyTypeOf();

    void boxedType_data();
    void boxedType();

    void testInt();
    void testDouble();
    void testChar();
    void testUint8();
    void testInt64();
    void testBool();
    void testStdString();

    void testPointer();
};

#include "TestBoxed.moc"

using namespace CppScript;

TestBoxed::TestBoxed()
{
    std::cout << "sizeof(AnyType) = " << sizeof(AnyType) << "\n";
}

enum TypeDetail {
    TYPE_NORMAL = 0,
    TYPE_VOID = 1,
    TYPE_CONST = 2,
};
using TypeDetails = QFlags<TypeDetail>;

Q_DECLARE_METATYPE(AnyType);
Q_DECLARE_METATYPE(const std::type_info*);
Q_DECLARE_METATYPE(TypeDetails);
Q_DECLARE_METATYPE(AnyPtr);

void TestBoxed::anyTypeOf_data()
{
    QTest::addColumn< AnyType >( "type" );
    QTest::addColumn< const std::type_info* >( "type_info" );
    QTest::addColumn< TypeDetails >("details");
    QTest::addColumn< AnyPtr >("ptr_type");

    QTest::newRow("int") << AnyTypeOf<int>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::none;
    QTest::newRow("void") << AnyTypeOf<void>::anyType() << &typeid(void) << TypeDetails(TYPE_VOID) << AnyPtr::none;
    QTest::newRow("<empty>") << AnyType() << &typeid(void) << TypeDetails(TYPE_VOID) << AnyPtr::none;
    //    QTest::newRow("const int") << AnyTypeOf<const int>::anyType() << &typeid(int) << TypeDetails(TYPE_CONST) << AnyPtr::none;
    //    QTest::newRow("int&") << AnyTypeOf<int&>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::none;
    QTest::newRow("int*") << AnyTypeOf<int*>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::raw;
    //    QTest::newRow("const int&") << AnyTypeOf<const int&>::anyType() << &typeid(int) << TypeDetails(TYPE_CONST) << AnyPtr::ref;
    QTest::newRow("const int*") << AnyTypeOf<const int*>::anyType() << &typeid(int) << TypeDetails(TYPE_CONST) << AnyPtr::raw;

    QTest::newRow("int**") << AnyTypeOf<int**>::anyType() << &typeid(int*) << TypeDetails() << AnyPtr::raw;
    //    QTest::newRow("int*&") << AnyTypeOf<int*&>::anyType() << &typeid(int*) << TypeDetails() << AnyPtr::raw;

    QTest::newRow("std::reference_wrapper<int>") << AnyTypeOf<std::reference_wrapper<int>>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::ref;
    //    QTest::newRow("std::reference_wrapper<int>&") << AnyTypeOf<std::reference_wrapper<int>&>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::ref;
    //    QTest::newRow("const std::reference_wrapper<int>&") << AnyTypeOf<const std::reference_wrapper<int>&>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::ref;

    QTest::newRow("std::shared_ptr<int>") << AnyTypeOf<std::shared_ptr<int>>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::shared;
    //    QTest::newRow("std::shared_ptr<int>&") << AnyTypeOf<std::shared_ptr<int>&>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::shared;
    //    QTest::newRow("const std::shared_ptr<int>&") << AnyTypeOf<const std::shared_ptr<int>&>::anyType() << &typeid(int) << TypeDetails() << AnyPtr::shared;
}

void TestBoxed::anyTypeOf()
{
    QFETCH(AnyType, type);
    QFETCH(const std::type_info*, type_info);
    QFETCH(TypeDetails, details);
    QFETCH(AnyPtr, ptr_type);

    QVERIFY2(*type.type_info == *type_info, type.type_info->name());
    QVERIFY(type.is_void == details.testFlag(TYPE_VOID));
    QVERIFY(type.is_const == details.testFlag(TYPE_CONST));
    QVERIFY(type.ptr_type == ptr_type);
}

Q_DECLARE_METATYPE(Boxed);

void TestBoxed::boxedType_data()
{
    QTest::addColumn< Boxed >( "boxed" );
    QTest::addColumn< const std::type_info* >( "store" );
    QTest::addColumn< const std::type_info* >( "type_info" );
    QTest::addColumn< TypeDetails >("details");
    QTest::addColumn< AnyPtr >("ptr_type");

    int i = 5; // used for references
    QTest::newRow("int") << Boxed::make(5) << &typeid(int) << &typeid(int) << TypeDetails() << AnyPtr::none;
    QTest::newRow("const int") << Boxed::make_const(5) << &typeid(int) << &typeid(int) << TypeDetails(TYPE_CONST) << AnyPtr::none;
    QTest::newRow("int&") << Boxed::make(std::ref(i)) << &typeid(std::reference_wrapper<int>) << &typeid(int) << TypeDetails() << AnyPtr::ref;
    QTest::newRow("int*") << Boxed::make(&i) << &typeid(int*) << &typeid(int) << TypeDetails() << AnyPtr::raw;
    QTest::newRow("const int&") << Boxed::make(std::cref(i)) << &typeid(std::reference_wrapper<int>) << &typeid(int) << TypeDetails(TYPE_CONST) << AnyPtr::ref;
    QTest::newRow("const int*") << Boxed::make((const int*)&i) << &typeid(int*) << &typeid(int) << TypeDetails(TYPE_CONST) << AnyPtr::raw;

    int *p = &i;
    QTest::newRow("int**") << Boxed::make(&p) << &typeid(int**) << &typeid(int*) << TypeDetails() << AnyPtr::raw;
    QTest::newRow("int*&") << Boxed::make(std::ref(p)) << &typeid(std::reference_wrapper<int*>) << &typeid(int*) << TypeDetails() << AnyPtr::ref;

    std::shared_ptr<int> sh = std::make_shared<int>(5);
    QTest::newRow("std::shared_ptr<int>") << Boxed::make(sh) << &typeid(std::shared_ptr<int>) << &typeid(int) << TypeDetails() << AnyPtr::shared;
    QTest::newRow("std::shared_ptr<int>&") << Boxed::make(std::ref(sh)) << &typeid(std::reference_wrapper<std::shared_ptr<int>>) << &typeid(std::shared_ptr<int>) << TypeDetails() << AnyPtr::ref;
    QTest::newRow("const std::shared_ptr<int>&") << Boxed::make(std::cref(sh)) << &typeid(std::reference_wrapper<const std::shared_ptr<int>>) << &typeid(std::shared_ptr<int>) << TypeDetails(TYPE_CONST) << AnyPtr::ref;
}

void TestBoxed::boxedType()
{
    QFETCH(Boxed, boxed);
    QFETCH(const std::type_info*, store);
    QFETCH(const std::type_info*, type_info);
    QFETCH(TypeDetails, details);
    QFETCH(AnyPtr, ptr_type);

    Q_UNUSED(store); // only for reference

    auto type = boxed.type();
    QVERIFY2(*type.type_info == *type_info, type.type_info->name());
    QVERIFY(type.is_void == details.testFlag(TYPE_VOID));
    QVERIFY(type.is_const == details.testFlag(TYPE_CONST));
    QVERIFY(type.ptr_type == ptr_type);
}

template<typename To>
void testBoxedConversion(const Boxed &value, bool expectSuccess)
{
    if (expectSuccess) {
        bool can = value.is<To>();
        QVERIFY2(can, typeid(To).name());
        if (can) {
            try {
                auto x = value.get<To>();
                (void)x;
            }
            catch(...) {
                QFAIL("exception");
            }
        }
    }
    else {
        QVERIFY2(! value.is<To>(), typeid(To).name());
    }
}

enum Conversion {
    CONV_NONE = 0,
    CONV_TYPE = 1,
    CONV_CONST = CONV_TYPE << 1,
    CONV_REF = CONV_CONST << 1,
    CONV_CONST_REF = CONV_REF << 1,
    CONV_PTR = CONV_CONST_REF << 1,
    CONV_CONST_PTR = CONV_PTR << 1,
    CONV_PTR_CONST = CONV_CONST_PTR << 1,
    CONV_CONST_PTR_CONST = CONV_PTR_CONST << 1,
    CONV_PTR_CONST_REF = CONV_CONST_PTR_CONST << 1,
    CONV_CONST_PTR_CONST_REF = CONV_PTR_CONST_REF << 1,
    CONV_SHARED_PTR = CONV_CONST_PTR_CONST_REF << 1,
    CONV_SHARED_CONST_PTR = CONV_SHARED_PTR << 1,
    CONV_CONST_SHARED_PTR = CONV_SHARED_CONST_PTR << 1,
    CONV_CONST_SHARED_CONST_PTR = CONV_CONST_SHARED_PTR << 1,
    CONV_CONST_SHARED_PTR_REF = CONV_CONST_SHARED_CONST_PTR << 1,
    CONV_CONST_SHARED_PTR_CONST_REF = CONV_CONST_SHARED_PTR_REF << 1,
    CONV_WRAPPED_REF = CONV_CONST_SHARED_PTR_CONST_REF << 1,
    CONV_WRAPPED_CONST_REF = CONV_WRAPPED_REF << 1,
    CONV_CONST_WRAPPED_REF = CONV_WRAPPED_CONST_REF << 1,
    CONV_CONST_WRAPPED_CONST_REF = CONV_CONST_WRAPPED_REF << 1,
    CONV_CONST_WRAPPED_REF_REF = CONV_CONST_WRAPPED_CONST_REF << 1,
    CONV_CONST_WRAPPED_CONST_REF_REF = CONV_CONST_WRAPPED_REF_REF << 1,
    CONV_NUMBER = CONV_CONST_WRAPPED_CONST_REF_REF << 1,
    CONV_CONST_NUMBER = CONV_NUMBER << 1,
    CONV_CONST_NUMBER_REF = CONV_CONST_NUMBER << 1
};
using ConversionFlags = QFlags<Conversion>;

template<typename Type>
void testBoxed(const Boxed &value, ConversionFlags flags)
{
    testBoxedConversion<Type>(value, flags.testFlag(CONV_TYPE));
    testBoxedConversion<const Type>(value, flags.testFlag(CONV_CONST));
    testBoxedConversion<Type&>(value, flags.testFlag(CONV_REF));
    testBoxedConversion<const Type&>(value, flags.testFlag(CONV_CONST_REF));
    testBoxedConversion<Type*>(value, flags.testFlag(CONV_PTR));
    testBoxedConversion<const Type*>(value, flags.testFlag(CONV_CONST_PTR));
    testBoxedConversion<Type* const>(value, flags.testFlag(CONV_PTR_CONST));
    testBoxedConversion<const Type* const>(value, flags.testFlag(CONV_CONST_PTR_CONST));
    testBoxedConversion<Type * const&>(value, flags.testFlag(CONV_PTR_CONST_REF));
    testBoxedConversion<const Type * const&>(value, flags.testFlag(CONV_CONST_PTR_CONST_REF));
    testBoxedConversion<Type*&>(value, false);
    testBoxedConversion<const Type*&>(value, false);
    testBoxedConversion<Type* const&>(value, flags.testFlag(CONV_PTR_CONST_REF));
    testBoxedConversion<const Type* const&>(value, flags.testFlag(CONV_CONST_PTR_CONST_REF));

    testBoxedConversion<std::shared_ptr<Type>>(value, flags.testFlag(CONV_SHARED_PTR));
    testBoxedConversion<std::shared_ptr<const Type>>(value, flags.testFlag(CONV_SHARED_CONST_PTR));
    testBoxedConversion<std::shared_ptr<Type>&>(value, false);
    testBoxedConversion<std::shared_ptr<const Type>&>(value, false);
    testBoxedConversion<const std::shared_ptr<Type>>(value, flags.testFlag(CONV_CONST_SHARED_PTR));
    testBoxedConversion<const std::shared_ptr<const Type>>(value, flags.testFlag(CONV_CONST_SHARED_CONST_PTR));
    testBoxedConversion<const std::shared_ptr<Type>&>(value, flags.testFlag(CONV_CONST_SHARED_PTR_REF));
    testBoxedConversion<const std::shared_ptr<const Type>&>(value, flags.testFlag(CONV_CONST_SHARED_PTR_CONST_REF));

    testBoxedConversion<std::reference_wrapper<Type>>(value, flags.testFlag(CONV_WRAPPED_REF));
    testBoxedConversion<std::reference_wrapper<const Type>>(value, flags.testFlag(CONV_WRAPPED_CONST_REF));
    testBoxedConversion<std::reference_wrapper<Type>&>(value, false);
    testBoxedConversion<std::reference_wrapper<const Type>&>(value, false);
    testBoxedConversion<const std::reference_wrapper<Type>>(value, flags.testFlag(CONV_CONST_WRAPPED_REF));
    testBoxedConversion<const std::reference_wrapper<const Type>>(value, flags.testFlag(CONV_CONST_WRAPPED_CONST_REF));
    testBoxedConversion<const std::reference_wrapper<Type>&>(value, flags.testFlag(CONV_CONST_WRAPPED_REF_REF));
    testBoxedConversion<const std::reference_wrapper<const Type>&>(value, flags.testFlag(CONV_CONST_WRAPPED_CONST_REF_REF));

    // TODO: BoxedNumber
//    testBoxedConversion<BoxedNumber>(value, flags.testFlag(CONV_NUMBER));
//    testBoxedConversion<const BoxedNumber>(value, flags.testFlag(CONV_CONST_NUMBER));
//    testBoxedConversion<BoxedNumber&>(value, false);
//    testBoxedConversion<const BoxedNumber&>(value, flags.testFlag(CONV_CONST_NUMBER_REF));
//    testBoxedConversion<BoxedNumber*>(value, false);
//    testBoxedConversion<const BoxedNumber*>(value, false);
//    testBoxedConversion<BoxedNumber* const>(value, false);
//    testBoxedConversion<const BoxedNumber* const>(value, false);

    testBoxedConversion<Boxed>(value, true);
    testBoxedConversion<const Boxed>(value, true);
    testBoxedConversion<const Boxed&>(value, true);
}

template<typename T>
void testType(const T &initial, bool ispod)
{
    T value = initial;
    testBoxed<T>(Boxed::make(value), ConversionFlags(
                     CONV_TYPE|CONV_CONST|CONV_REF|CONV_CONST_REF|
                     CONV_PTR|CONV_CONST_PTR|CONV_PTR_CONST|CONV_CONST_PTR_CONST|
                     CONV_PTR_CONST_REF|CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     CONV_WRAPPED_REF|CONV_WRAPPED_CONST_REF|CONV_CONST_WRAPPED_REF|CONV_CONST_WRAPPED_CONST_REF|
                     CONV_CONST_WRAPPED_REF_REF|CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make_const(value), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make(&value), ConversionFlags(
                     CONV_TYPE|CONV_CONST|CONV_REF|CONV_CONST_REF|
                     CONV_PTR|CONV_CONST_PTR|CONV_PTR_CONST|CONV_CONST_PTR_CONST|
                     CONV_PTR_CONST_REF|CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     CONV_WRAPPED_REF|CONV_WRAPPED_CONST_REF|CONV_CONST_WRAPPED_REF|CONV_CONST_WRAPPED_CONST_REF|
                     CONV_CONST_WRAPPED_REF_REF|CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make_const(&value), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make(std::ref(value)), ConversionFlags(
                     CONV_TYPE|CONV_CONST|CONV_REF|CONV_CONST_REF|
                     CONV_PTR|CONV_CONST_PTR|CONV_PTR_CONST|CONV_CONST_PTR_CONST|
                     CONV_PTR_CONST_REF|CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     CONV_WRAPPED_REF|CONV_WRAPPED_CONST_REF|CONV_CONST_WRAPPED_REF|CONV_CONST_WRAPPED_CONST_REF|
                     CONV_CONST_WRAPPED_REF_REF|CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make(std::cref(value)), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    const T &ref = value;
    testBoxed<T>(Boxed::make(ref), ConversionFlags(
                     CONV_TYPE|CONV_CONST|CONV_REF|CONV_CONST_REF|
                     CONV_PTR|CONV_CONST_PTR|CONV_PTR_CONST|CONV_CONST_PTR_CONST|
                     CONV_PTR_CONST_REF|CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     CONV_WRAPPED_REF|CONV_WRAPPED_CONST_REF|CONV_CONST_WRAPPED_REF|CONV_CONST_WRAPPED_CONST_REF|
                     CONV_CONST_WRAPPED_REF_REF|CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    const T* ptr = &value;
    testBoxed<T>(Boxed::make(ptr), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make_const(ptr), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|*/
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    std::shared_ptr<T> shared = std::make_shared<T>(initial);
    testBoxed<T>(Boxed::make(shared), ConversionFlags(
                     CONV_TYPE|CONV_CONST|CONV_REF|CONV_CONST_REF|
                     CONV_PTR|CONV_CONST_PTR|CONV_PTR_CONST|CONV_CONST_PTR_CONST|
                     CONV_PTR_CONST_REF|CONV_CONST_PTR_CONST_REF|
                     CONV_SHARED_PTR|CONV_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR|
                     CONV_CONST_SHARED_CONST_PTR|CONV_CONST_SHARED_PTR_REF|CONV_CONST_SHARED_PTR_CONST_REF|
                     CONV_WRAPPED_REF|CONV_WRAPPED_CONST_REF|CONV_CONST_WRAPPED_REF|CONV_CONST_WRAPPED_CONST_REF|
                     CONV_CONST_WRAPPED_REF_REF|CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make_const(shared), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|*/CONV_SHARED_CONST_PTR|/*CONV_CONST_SHARED_PTR|*/
                     CONV_CONST_SHARED_CONST_PTR|/*CONV_CONST_SHARED_PTR_REF|*/CONV_CONST_SHARED_PTR_CONST_REF|
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    std::shared_ptr<const T> shared_const = std::make_shared<T>(initial);
    testBoxed<T>(Boxed::make(shared_const), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|*/CONV_SHARED_CONST_PTR|/*CONV_CONST_SHARED_PTR|*/
                     CONV_CONST_SHARED_CONST_PTR|/*CONV_CONST_SHARED_PTR_REF|*/CONV_CONST_SHARED_PTR_CONST_REF|
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));

    testBoxed<T>(Boxed::make_const(shared_const), ConversionFlags(
                     CONV_TYPE|CONV_CONST|/*CONV_REF|*/CONV_CONST_REF|
                     /*CONV_PTR|*/CONV_CONST_PTR|/*CONV_PTR_CONST|*/CONV_CONST_PTR_CONST|
                     /*CONV_PTR_CONST_REF|*/CONV_CONST_PTR_CONST_REF|
                     /*CONV_SHARED_PTR|*/CONV_SHARED_CONST_PTR|/*CONV_CONST_SHARED_PTR|*/
                     CONV_CONST_SHARED_CONST_PTR|/*CONV_CONST_SHARED_PTR_REF|*/CONV_CONST_SHARED_PTR_CONST_REF|
                     /*CONV_WRAPPED_REF|*/CONV_WRAPPED_CONST_REF|/*CONV_CONST_WRAPPED_REF|*/CONV_CONST_WRAPPED_CONST_REF|
                     /*CONV_CONST_WRAPPED_REF_REF|*/CONV_CONST_WRAPPED_CONST_REF_REF|
                     (ispod ? CONV_NUMBER|CONV_CONST_NUMBER|CONV_CONST_NUMBER_REF : CONV_NONE)));
}

void TestBoxed::testInt()
{
    testType<int>(5, true);
}

void TestBoxed::testDouble()
{
    testType<double>(1.1, true);
}

void TestBoxed::testChar()
{
    testType<char>('a', true);
}

void TestBoxed::testUint8()
{
    testType<uint8_t>(5, true);
}

void TestBoxed::testInt64()
{
    testType<int64_t>(5, true);
}

void TestBoxed::testBool()
{
    testType<bool>(false, false);
}

void TestBoxed::testStdString()
{
    testType<std::string>("Hello World", false);
}

void TestBoxed::testPointer()
{
    using T = int;
    T* p = new T(1);

    T **x = Boxed::make(&p).get<T**>();
    QVERIFY(p == *x);
    QVERIFY(*p == **x);

    **x = 2;
    QVERIFY(*p == **x);
    delete p;
}

QTEST_GUILESS_MAIN(TestBoxed)
