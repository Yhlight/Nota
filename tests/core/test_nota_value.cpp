#include <gtest/gtest.h>
#include "core/NotaValue.hpp"

using namespace nota::core;

TEST(NotaValueTest, DefaultConstructor) {
    NotaValue value;
    EXPECT_EQ(value.GetType(), ValueType::NIL);
    EXPECT_TRUE(value.IsNil());
}

TEST(NotaValueTest, BoolValue) {
    NotaValue value(true);
    EXPECT_EQ(value.GetType(), ValueType::BOOL);
    EXPECT_TRUE(value.IsBool());
    EXPECT_EQ(value.AsBool(), true);
}

TEST(NotaValueTest, IntValue) {
    NotaValue value(123);
    EXPECT_EQ(value.GetType(), ValueType::INT);
    EXPECT_TRUE(value.IsInt());
    EXPECT_EQ(value.AsInt(), 123);
}

TEST(NotaValueTest, FloatValue) {
    NotaValue value(123.456);
    EXPECT_EQ(value.GetType(), ValueType::FLOAT);
    EXPECT_TRUE(value.IsFloat());
    EXPECT_EQ(value.AsFloat(), 123.456);
}

TEST(NotaValueTest, ObjectValue) {
    NotaObject obj(ObjectType::STRING);
    NotaValue value(&obj);
    EXPECT_EQ(value.GetType(), ValueType::OBJECT);
    EXPECT_TRUE(value.IsObject());
    EXPECT_EQ(value.AsObject(), &obj);
}
