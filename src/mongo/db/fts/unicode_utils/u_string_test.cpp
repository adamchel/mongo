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

#include "mongo/db/fts/unicode_utils/invalid_utf8_exception.h"
#include "mongo/db/fts/unicode_utils/u_string.h"
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
    UString test1 = UTF8("¿CUÁNTOS AÑOS TIENES TÚ?");

    ASSERT_EQUALS(UTF8("¿CUANTOS ANOS TIENES TU?"), test1.toNoDiacritics().toString());
}

TEST(UnicodeString, CaseFolding) {
    UString test1 = UTF8("СКОЛЬКО ТЕБЕ ЛЕТ?");

    ASSERT_EQUALS(UTF8("сколько тебе лет?"), test1.toLower().toString());
}

TEST(UnicodeString, TurkishCaseFolding) {
    UString test1 = UTF8("KAC YASINDASINIZ");
    UString test2 = UTF8("KAC YASİNDASİNİZ");

    ASSERT_EQUALS(UTF8("kac yasındasınız"), test1.toLower(true).toString());
    ASSERT_EQUALS(UTF8("kac yasindasiniz"), test2.toLower(true).toString());
}

TEST(UnicodeString, CaseFoldingAndRemoveDiacritics) {
    UString test1 = UTF8("Πόσο χρονών είσαι?");

    ASSERT_EQUALS(UTF8("ποσο χρονων εισαι?"), test1.toLower().toNoDiacritics().toString());
}

TEST(UnicodeString, SubstringMatch) {
    UString str = UTF8("Kaç yaşındasınız?\n");
    UString find = UTF8("kac");

    // Case insensitive & diacritic insensitive
    ASSERT(UString::needleInHaystack(str, find));

    // Case sensitive & diacritic insensitive
    ASSERT_FALSE(UString::needleInHaystack(str, find, true));

    // Case insensitive & diacritic sensitive
    ASSERT_FALSE(UString::needleInHaystack(str, find, false, false));
}

TEST(UnicodeString, BadUTF8) {
    // Overlong
    const char invalid1[] = {static_cast<char>(0xc0), static_cast<char>(0xaf)};

    // Invalid code positions
    const char invalid2[] = {
        static_cast<char>(0xed), static_cast<char>(0xa0), static_cast<char>(0x80)};

    ASSERT_THROWS(UString test1(invalid1), InvalidUTF8Exception);
    ASSERT_THROWS(UString test2(invalid2), InvalidUTF8Exception);
}

} // namespace unicode
} // namespace mongo
