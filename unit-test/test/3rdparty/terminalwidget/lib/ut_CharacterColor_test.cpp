#include "ut_CharacterColor_test.h"

//Qt单元测试相关头文件
#include <QTest>
#include <QtGui>
#include <QDebug>

Q_DECLARE_METATYPE(ColorEntry)

const ColorEntry UT_CharacterColor_Test::DefaultColorTable[TABLE_COLORS] =
{
  // normal
  ColorEntry(QColor(0x00,0x00,0x00), false), ColorEntry( QColor(0xB2,0xB2,0xB2), true), // Dfore, Dback
  ColorEntry(QColor(0x00,0x00,0x00), false), ColorEntry( QColor(0xB2,0x18,0x18), false), // Black, Red
  ColorEntry(QColor(0x18,0xB2,0x18), false), ColorEntry( QColor(0xB2,0x68,0x18), false), // Green, Yellow
  ColorEntry(QColor(0x18,0x18,0xB2), false), ColorEntry( QColor(0xB2,0x18,0xB2), false), // Blue, Magenta
  ColorEntry(QColor(0x18,0xB2,0xB2), false), ColorEntry( QColor(0xB2,0xB2,0xB2), false), // Cyan, White
  // intensiv
  ColorEntry(QColor(0x00,0x00,0x00), false), ColorEntry( QColor(0xFF,0xFF,0xFF), true),
  ColorEntry(QColor(0x68,0x68,0x68), false), ColorEntry( QColor(0xFF,0x54,0x54), false),
  ColorEntry(QColor(0x54,0xFF,0x54), false), ColorEntry( QColor(0xFF,0xFF,0x54), false),
  ColorEntry(QColor(0x54,0x54,0xFF), false), ColorEntry( QColor(0xFF,0x54,0xFF), false),
  ColorEntry(QColor(0x54,0xFF,0xFF), false), ColorEntry( QColor(0xFF,0xFF,0xFF), false)
};

UT_CharacterColor_Test::UT_CharacterColor_Test()
{
}

void UT_CharacterColor_Test::SetUp()
{
}

void UT_CharacterColor_Test::TearDown()
{
}

#ifdef UT_CHARACTERCOLOR_TEST
TEST_F(UT_CharacterColor_Test, ColorEntryTest)
{
    ColorEntry black = ColorEntry(QColor(0x00, 0x00, 0x00), false);
    ColorEntry white = ColorEntry(QColor(0xFF, 0xFF, 0xFF), false);
    ColorEntry red = ColorEntry(QColor(0xB2, 0x18, 0x18), false);
    ColorEntry green = ColorEntry(QColor(0x18, 0xB2, 0x18), false);

    // Test operator== operator!=
    EXPECT_EQ(black.color == white.color, false);
    EXPECT_EQ(black.color != white.color, true);
    EXPECT_EQ(red.color == green.color, false);
    EXPECT_EQ(red.color != green.color, true);

    EXPECT_EQ(red.color == green.color, false);
    EXPECT_EQ(red.color != green.color, true);

    // Test operator=
    ColorEntry tmpColorEntry;
    tmpColorEntry = red;
    EXPECT_EQ(tmpColorEntry.color == red.color, true);
    EXPECT_EQ(red.color == tmpColorEntry.color, true);

    // Test ColorEntry()
    ColorEntry defaultColorEntry = ColorEntry();
    EXPECT_EQ(defaultColorEntry.color != green.color, true);
    EXPECT_EQ(defaultColorEntry.color != black.color, true);
    EXPECT_EQ(defaultColorEntry.color.isValid(), false);
}

TEST_F(UT_CharacterColor_Test, DummyConstructorTest)
{
    CharacterColor charColor;
    EXPECT_EQ(charColor.isValid(), false);
}

TEST_F(UT_CharacterColor_Test, ColorSpaceDefaultTest_data)
{
    CharacterColor charColor(COLOR_SPACE_DEFAULT, 1);
    const QColor result = charColor.color(DefaultColorTable);

    EXPECT_EQ(result, DefaultColorTable[1].color);
}

#endif
