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

#include "mongo/db/fts/unicode/codepoints.h"
#include "mongo/unittest/unittest.h"

namespace mongo {
namespace unicode {

TEST(UnicodeCodepoints, Diacritics) {
    const char32_t marks[] = {0x0301, 0x0339, 0x1AB4, 0x1DC5, 0xA69D};
    const char32_t not_marks[] = {U'-', U'.', U'\'', U'*', U'm'};

    for (auto i = 0; i < 5; ++i) {
        ASSERT(codepointIsDiacritic(marks[i]));
        ASSERT_FALSE(codepointIsDiacritic(not_marks[i]));
    }
}

TEST(UnicodeCodepoints, Delimiters) {
    const char32_t delimiters[] = {U'-', U'.', U'"', U'¿', U'«'};
    const char32_t not_delimiters[] = {U'a', U'ê', U'π', U'Ω', U'å'};

    for (auto i = 0; i < 5; ++i) {
        ASSERT(codepointIsDelimiter(delimiters[i], DelimiterListLanguage::kEnglish));
        ASSERT(codepointIsDelimiter(delimiters[i], DelimiterListLanguage::kNotEnglish));
        ASSERT_FALSE(codepointIsDelimiter(not_delimiters[i], DelimiterListLanguage::kEnglish));
        ASSERT_FALSE(codepointIsDelimiter(not_delimiters[i], DelimiterListLanguage::kNotEnglish));
    }

    // Special case for English.
    ASSERT(codepointIsDelimiter(U'\'', DelimiterListLanguage::kNotEnglish));
    ASSERT_FALSE(codepointIsDelimiter(U'\'', DelimiterListLanguage::kEnglish));
}

TEST(UnicodeCodepoints, RemoveDiacritics) {
    const char32_t originals[] = {U'á', U'ê', U'ñ', U'å', U'ç'};
    const char32_t clean[] = {U'a', U'e', U'n', U'a', U'c'};

    for (auto i = 0; i < 5; ++i) {
        ASSERT_EQUALS(clean[i], codepointRemoveDiacritics(originals[i]));
    }
}

TEST(UnicodeCodepoints, ToLower) {
    const char32_t upper[] = {U'Á', U'Ê', U'Ñ', U'Å', U'Ç'};
    const char32_t lower[] = {U'á', U'ê', U'ñ', U'å', U'ç'};

    for (auto i = 0; i < 5; ++i) {
        ASSERT_EQUALS(lower[i], codepointToLower(upper[i]));
    }
}

}  // namespace unicode
}  // namespace mongo
