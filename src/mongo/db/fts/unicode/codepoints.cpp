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

namespace mongo {
namespace unicode {

bool codepointIsCombiningDiacriticalMark(char32_t codepoint) {
    /**
     * The ranges in this function come from the combining diacritical marks code blocks, which are
     * defined in ftp://unicode.org/Public/8.0.0/ucd/Blocks.txt, also see
     * https://en.wikipedia.org/wiki/Combining_character
     */

    // Combining Diacritical Marks
    if (codepoint >= 0x0300 && codepoint <= 0x036F) {
        return true;
    }

    // Combining Diacritical Marks Extended
    if (codepoint >= 0x1AB0 && codepoint <= 0x1AFF) {
        return true;
    }

    // Combining Diacritical Marks Supplement
    if (codepoint >= 0x1DC0 && codepoint <= 0x1DFF) {
        return true;
    }

    // Combining Diacritical Marks for Symbols
    if (codepoint >= 0x20D0 && codepoint <= 0x20FF) {
        return true;
    }

    return false;
}

/**
 * codepointIsDelimiter is defined in the generated codepoints_delimiter_list.cpp
 */

/**
 * codepointRemoveDiacritics is defined in the generated codepoints_diacritic_map.cpp
 */

 /**
 * codepointToLower is defined in the generated codepoints_casefold.cpp
 */

}  // namespace unicode
}  // namespace mongo