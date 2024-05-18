//
// Created by Александр Лебедев on 21.10.2023.
//

#include <BeeEngine.h>
#include <Locale/Locale.h>
#include <gtest/gtest.h>
#include <unicode/udata.h>
#include <unicode/ulocdata.h>

using namespace BeeEngine;
using namespace BeeEngine::Locale;
const Localization locale = {"ru_RU"};
class TranslationTests : public ::testing::Test
{
protected:
    Domain domain{"test"};

    TranslationTests()
    {
        domain.SetLocale(locale);
        domain.AddLocaleKey(locale, "hello", "привет");
        domain.AddLocaleKey(locale, "greeting", "Привет, {name}!");
        domain.AddLocaleKey(
            locale,
            "apples_test",
            "У {name} есть {apples, plural, one {# яблоко} few {# яблока} many {# яблок} other {# яблока}}");
    }

    ~TranslationTests() override
    {
        // Тут можешь сделать деинициализацию после каждого теста, если требуется
    }
};

TEST_F(TranslationTests, BasicTranslationTest)
{
    // предположим, у тебя есть перевод для "hello" как "привет" на русском
    domain.SetLocale(locale);
    ASSERT_EQ(domain.Translate("hello"), "привет");
}

TEST_F(TranslationTests, FormatTranslationTest)
{
    // предположим, у тебя есть перевод для "greeting" как "Привет, {0}!" на русском
    domain.SetLocale(locale);
    ASSERT_EQ(domain.Translate("greeting", "name", "Денис"), "Привет, Денис!");
}

TEST_F(TranslationTests, PluralTranslationTest)
{
    // предположим, у тебя есть перевод для "apples" на русском с разными формами слова в зависимости от числа
    domain.SetLocale(locale);
    ASSERT_EQ(domain.Translate("apples_test", "name", "Саши", "apples", 1), "У Саши есть 1 яблоко");
    ASSERT_EQ(domain.Translate("apples_test", "name", "Саши", "apples", 2), "У Саши есть 2 яблока");
    ASSERT_EQ(domain.Translate("apples_test", "name", "Саши", "apples", 5), "У Саши есть 5 яблок");
}

/*
TEST_F(TranslationTests, ICUTest)
{
    UErrorCode status = U_ZERO_ERROR;
    icu::MessageFormat msgFmt(icu::UnicodeString::fromUTF8("У {name} есть {apples} яблок"), icu::Locale("ru_RU"),
status);

    icu::UnicodeString keys[] = { "name", "apples" };
    icu::Formattable values[] = { "Алексей", icu::Formattable(5) };

    icu::UnicodeString result;
    msgFmt.format(keys, values, 2, result, status);
    //msgFmt.format("apples", icu::Formattable(2), result, status);
    //icu::MessageFormat::format("У {name} есть {apples} яблок", values, 2, result, status);

    EXPECT_EQ(u_errorName(status), "U_ZERO_ERROR");
    EXPECT_EQ(result, "У Алексей есть 5 яблок");
}*/
