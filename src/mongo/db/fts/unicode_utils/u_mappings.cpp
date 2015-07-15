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
