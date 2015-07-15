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

#include <algorithm>
#include <codecvt>
#include <locale>

#include "mongo/util/assert_util.h"

namespace mongo {
namespace unicode {

using std::codecvt;
using std::codecvt_base;
using std::locale;
using std::mbstate_t;
using std::string;
using std::u32string;
using std::use_facet;

String::String(const StringData utf8_src) {
    auto& conversion_facet = use_facet<codecvt<char32_t, char, mbstate_t>>(locale::classic());
    mbstate_t mb = mbstate_t();

    // _data is the target, resize it so that it's guaranteed to fit all of the input characters.
    _data.resize(utf8_src.size());

    // Apply the UTF-8 -> UTF-32 conversion.
    const char* from_next;
    char32_t* to_next;
    codecvt_base::result result = conversion_facet.in(mb,
                                                      &utf8_src.rawData()[0],
                                                      &utf8_src.rawData()[utf8_src.size()],
                                                      from_next,
                                                      &_data[0],
                                                      &_data[_data.size()],
                                                      to_next);

    // Handle invalid UTF-8.
    switch (result) {
        case codecvt_base::ok:
            // Success
            break;
        case codecvt_base::partial:
        case codecvt_base::error:
            // Failure
            uasserted(28732, "text contains invalid UTF-8");
            break;
        case codecvt_base::noconv:
            invariant(false);
            break;
    }

    // Resize _data so it is only as big as what it contains.
    _data.resize(to_next - &_data[0]);
}

String::String(String&& src) : _data(std::move(src._data)) {}

String::String(u32string&& src) : _data(src) {}

std::string String::toString() const {
    auto& conversion_facet = use_facet<codecvt<char32_t, char, mbstate_t>>(locale::classic());
    mbstate_t mb = mbstate_t();

    // output is the target, size it so that it's guaranteed to fit all of the input characters.
    std::string output(_data.size() * conversion_facet.max_length(), '\0');

    // Apply the UTF-32 -> UTF-8 conversion.
    const char32_t* from_next;
    char* to_next;
    codecvt_base::result result = conversion_facet.out(mb,
                                                       &_data[0],
                                                       &_data[_data.size()],
                                                       from_next,
                                                       &output[0],
                                                       &output[output.size()],
                                                       to_next);

    // Ensure that there is no invalid UTF-32.
    switch (result) {
        case codecvt_base::ok:
            break;
        case codecvt_base::partial:
        case codecvt_base::error:
        case codecvt_base::noconv:
            // If this invariant is triggered, it means that for some reason, one of the char32_t
            // components of the u32string is greater than 0x10ffff, which is outside the range of
            // Unicode code points. Check your mappings and ensure they are all valid codepoints.
            invariant(false);
            break;
    }

    // Resize output so it is only as large as what it contains.
    output.resize(to_next - &output[0]);
    return output;
}

size_t String::size() const {
    return _data.size();
}

const char32_t& String::operator[](int i) const {
    return _data[i];
}

String String::substr(size_t pos, size_t len) const {
    return String(_data.substr(pos, len));
}

String String::toLower(CaseFoldMode mode) const {
    u32string newdata(_data.size(), 0);
    auto index = 0;
    for (auto codepoint : _data) {
        newdata[index++] = (codepointToLower(codepoint, mode));
    }
    return String(std::move(newdata));
}

String String::removeDiacritics() const {
    u32string newdata(_data.size(), 0);
    auto index = 0;
    for (auto codepoint : _data) {
        if (!codepointIsCombiningDiacriticalMark(codepoint)) {
            newdata[index++] = (codepointRemoveDiacritics(codepoint));
        }
    }
    newdata.resize(index);
    return String(std::move(newdata));
}

bool String::substrMatch(const String& str,
                         const String& find,
                         SubstrMatchOptions options,
                         CaseFoldMode cfMode) {
    if (options & kDiacriticSensitive) {
        if (options & kCaseSensitive) {
            // Case sensitive and diacritic sensitive
            return std::search(str._data.cbegin(),
                               str._data.cend(),
                               find._data.cbegin(),
                               find._data.cend(),
                               [&](char32_t c1, char32_t c2) { return (c1 == c2); }) !=
                str._data.cend();
        } else {
            // Case insensitive and diacritic sensitive
            return std::search(str._data.cbegin(),
                               str._data.cend(),
                               find._data.cbegin(),
                               find._data.cend(),
                               [&](char32_t c1, char32_t c2) {
                                   return (codepointToLower(c1, cfMode) ==
                                           codepointToLower(c2, cfMode));
                               }) != str._data.cend();
        }

    } else {
        String cleanStr = str.removeDiacritics();
        String cleanFind = find.removeDiacritics();
        if (options & kCaseSensitive) {
            // Case sensitive and diacritic insensitive
            return std::search(cleanStr._data.cbegin(),
                               cleanStr._data.cend(),
                               cleanFind._data.cbegin(),
                               cleanFind._data.cend(),
                               [&](char32_t c1, char32_t c2) { return (c1 == c2); }) !=
                cleanStr._data.cend();
        } else {
            // Case insensitive and diacritic insensitive
            return std::search(cleanStr._data.cbegin(),
                               cleanStr._data.cend(),
                               cleanFind._data.cbegin(),
                               cleanFind._data.cend(),
                               [&](char32_t c1, char32_t c2) {
                                   return (codepointToLower(c1, cfMode) ==
                                           codepointToLower(c2, cfMode));
                               }) != cleanStr._data.cend();
        }
    }
}

}  // namespace unicode
}  // namespace mongo
