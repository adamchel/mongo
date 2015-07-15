/**
 *    Copyright (C) 2015 MongoDB Inc.
 *
 *    This program is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/db/fts/unicode/string.h"
#include "mongo/unittest/unittest.h"
#include "mongo/util/text.h"

#ifdef MSC_VER
// Microsoft VS 2013 does not handle UTF-8 strings in char literal strings, error C4566
// The Microsoft compiler can be tricked into using UTF-8 strings as follows:
// 1. The file has a UTF-8 BOM
// 2. The string literal is a wide character string literal (ie, prefixed with L)
// at this point.
#define UTF8(x) toUtf8String(L##x)
#else
#define UTF8(x) x
#endif

namespace mongo {
namespace unicode {

TEST(UnicodeString, RemoveDiacritics) {
    // NFC Normalized Text
    String test1 = String(UTF8("¿CUÁNTOS AÑOS TIENES TÚ?"));

    // NFD Normalized Text ("Café")
    const char test2[] = {'C', 'a', 'f', 'e', static_cast<char>(0xcc), static_cast<char>(0x81), 0};

    ASSERT_EQUALS(UTF8("¿CUANTOS ANOS TIENES TU?"), test1.removeDiacritics().toString());
    ASSERT_EQUALS(UTF8("Cafe"), String(test2).removeDiacritics().toString());
}

TEST(UnicodeString, CaseFolding) {
    String test1 = String(UTF8("СКОЛЬКО ТЕБЕ ЛЕТ?"));
    String test2 = String(UTF8("¿CUÁNTOS AÑOS TIENES TÚ?"));

    ASSERT_EQUALS(UTF8("сколько тебе лет?"), test1.toLower().toString());
    ASSERT_EQUALS(UTF8("¿cuántos años tienes tú?"), test2.toLower().toString());
}

TEST(UnicodeString, CaseFoldingTurkish) {
    String test1 = String(UTF8("KAC YASINDASINIZ"));
    String test2 = String(UTF8("KAC YASİNDASİNİZ"));

    ASSERT_EQUALS(UTF8("kac yasındasınız"), test1.toLower(CaseFoldMode::kTurkish).toString());
    ASSERT_EQUALS(UTF8("kac yasindasiniz"), test2.toLower(CaseFoldMode::kTurkish).toString());
}

TEST(UnicodeString, CaseFoldingAndRemoveDiacritics) {
    // NFC Normalized Text
    String test1 = String(UTF8("Πόσο χρονών είσαι?"));
    String test2 = String(UTF8("¿CUÁNTOS AÑOS TIENES TÚ?"));

    // NFD Normalized Text ("CAFÉ")
    const char test3[] = {'C', 'A', 'F', 'E', static_cast<char>(0xcc), static_cast<char>(0x81), 0};

    ASSERT_EQUALS(UTF8("ποσο χρονων εισαι?"), test1.toLower().removeDiacritics().toString());
    ASSERT_EQUALS(UTF8("¿cuantos anos tienes tu?"), test2.toLower().removeDiacritics().toString());
    ASSERT_EQUALS(UTF8("cafe"), String(test3).toLower().removeDiacritics().toString());
}

TEST(UnicodeString, SubstringMatch) {
    String str = String(UTF8("Одумайся! Престол свой сохрани; И ярость укроти."));

    // Case insensitive & diacritic insensitive
    ASSERT(String::substrMatch(str, String(UTF8("престол свои")), String::kNone));
    ASSERT_FALSE(String::substrMatch(str, String(UTF8("Престол сохрани")), String::kNone));

    // Case sensitive & diacritic insensitive
    ASSERT(String::substrMatch(str, String(UTF8("Одумаися!")), String::kCaseSensitive));
    ASSERT_FALSE(String::substrMatch(str, String(UTF8("одумайся!")), String::kCaseSensitive));

    // Case insensitive & diacritic sensitive
    ASSERT(String::substrMatch(str, String(UTF8("одумайся!")), String::kDiacriticSensitive));
    ASSERT_FALSE(String::substrMatch(str, String(UTF8("Одумаися!")), String::kDiacriticSensitive));

    // Case sensitive & diacritic sensitive
    ASSERT(String::substrMatch(
        str, String(UTF8("Одумайся!")), String::kDiacriticSensitive | String::kCaseSensitive));
    ASSERT_FALSE(String::substrMatch(
        str, String(UTF8("Одумаися!")), String::kDiacriticSensitive | String::kCaseSensitive));
}

TEST(UnicodeString, SubstringMatchTurkish) {
    String str = String(UTF8("KAÇ YAŞINDASINIZ?"));

    // Case insensitive & diacritic insensitive
    ASSERT(String::substrMatch(
        str, String(UTF8("yasındasınız")), String::kNone, CaseFoldMode::kTurkish));
    ASSERT_FALSE(String::substrMatch(
        str, String(UTF8("yasindasiniz")), String::kNone, CaseFoldMode::kTurkish));

    // Case insensitive & diacritic sensitive
    ASSERT(String::substrMatch(
        str, String(UTF8("yaşındasınız")), String::kDiacriticSensitive, CaseFoldMode::kTurkish));
    ASSERT_FALSE(String::substrMatch(
        str, String(UTF8("yaşindasiniz")), String::kDiacriticSensitive, CaseFoldMode::kTurkish));
}

TEST(UnicodeString, BadUTF8) {
    // Overlong
    const char invalid1[] = {static_cast<char>(0xc0), static_cast<char>(0xaf), 0};

    // Invalid code positions
    const char invalid2[] = {
        static_cast<char>(0xed), static_cast<char>(0xa0), static_cast<char>(0x80), 0};

    ASSERT_THROWS(String test1(invalid1), AssertionException);
    ASSERT_THROWS(String test2(invalid2), AssertionException);
}

}  // namespace unicode
}  // namespace mongo
