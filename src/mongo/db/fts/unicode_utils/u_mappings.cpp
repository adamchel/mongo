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

#include "mongo/db/fts/unicode_utils/u_mappings.h"

namespace mongo {
namespace unicode {

bool isDelimiter(char32_t c, bool english) {
    // TODO: This is horribly inefficient, we need a better data structure here.

    // Whitespace
    if (c >= 0x0009 && c <= 0x000d) return true;
    if (c == 0x0020) return true;
    if (c == 0x0085) return true;
    if (c == 0x00A0) return true;
    if (c == 0x1680) return true;
    if (c >= 0x2000 && c <= 0x200A) return true;
    if (c == 0x2028) return true;
    if (c == 0x2029) return true;
    if (c == 0x202F) return true;
    if (c == 0x205F) return true;
    if (c == 0x3000) return true;

    // Quotation marks
    if (c == 0x0022) return true;
    if (!english && c == 0x0027) return true; // apostrophe
    if (c == 0x00AB) return true;
    if (c == 0x00BB) return true;
    if (c == 0x2018) return true;
    if (c == 0x2019) return true;
    if (c == 0x201A) return true;
    if (c >= 0x201B && c <= 0x201C) return true;
    if (c == 0x201D) return true;
    if (c == 0x201E) return true;
    if (c == 0x201F) return true;
    if (c == 0x2039) return true;
    if (c == 0x203A) return true;
    if (c == 0x2E42) return true;
    if (c == 0x300C) return true;
    if (c == 0x300D) return true;
    if (c == 0x300E) return true;
    if (c == 0x300F) return true;
    if (c == 0x301D) return true;
    if (c >= 0x301E && c <= 0x301F) return true;
    if (c == 0xFE41) return true;
    if (c == 0xFE42) return true;
    if (c == 0xFE43) return true;
    if (c == 0xFE44) return true;
    if (c == 0xFF02) return true;
    if (c == 0xFF07) return true;
    if (c == 0xFF62) return true;
    if (c == 0xFF63) return true;

    // Dashes
    if (c == 0x002D) return true;
    // TODO: the rest of the dashes

    // Hyphens
    // TODO: actually put hyphens

    // Terminal punctutation
    if (c == 0x0021) return true;
    if (c == 0x002C) return true;
    if (c == 0x002E) return true;
    if (c == 0x003F) return true;
    if (c == 0x037E) return true;
    if (c == 0x0387) return true;
    if (c == 0x0589) return true;
    if (c >= 0x003A && c <= 0x003B) return true;
    if (c == 0x05C3) return true;
    if (c == 0x060C) return true;
    if (c == 0x061B) return true;
    if (c == 0x061F) return true;
    if (c == 0x06D4) return true;
    if (c == 0x070C) return true;
    // TODO: the rest of the terminal punctuation

    // TODO: more characters that can be considered delimiters...
    // TODO: the characters we originally considered delimiters
    // TODO: read Unicode TR29 (UAX29) and apply a more compliant text segmentation algorithm

    return false;


}

char32_t codepointToLower(char32_t codepoint, bool turkish) {
    if (codepoint >= 0x10000) {
        return codepoint;
    }
    if (!turkish) {
        return caseFoldingMap[codepoint];
    } else {                       // turkish == true
        if (codepoint == 0x049) {  // I -> ı
            return 0x131;
        } else if (codepoint == 0x130) {  // İ -> i
            return 0x069;
        } else {
            return caseFoldingMap[codepoint];
        }
    }
}

char32_t codepointRemoveDiacritics(char32_t codepoint) {
    if (codepoint >= 0x2000) {
        return codepoint;
    }
    return ignoreDiacriticsMap[codepoint];
}

} // namespace unicode
} // namespace mongo
