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

#include "mongo/db/fts/unicode_utils/u_string.h"

#include "mongo/db/fts/unicode_utils/invalid_utf8_exception.h"
#include "mongo/db/fts/unicode_utils/u_mappings.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace unicode {

using std::codecvt;
using std::codecvt_base;
using std::mbstate_t;
using std::use_facet;

using std::locale;

UString::UString() : UString(std::vector<char32_t>()) {}

UString::UString(const char utf8_src[]) {
    auto& f = use_facet<codecvt<char32_t, char, mbstate_t>>(locale());

    mbstate_t mb = mbstate_t();

    size_t utf8_src_size = strlen(utf8_src);

    _data.resize(utf8_src_size);

    const char* from_next;
    char32_t* to_next;

    codecvt_base::result result = f.in(mb,
                                       &utf8_src[0],
                                       &utf8_src[utf8_src_size],
                                       from_next,
                                       &_data[0],
                                       &_data[_data.size()],
                                       to_next);

    switch (result) {
        case codecvt_base::ok:
            // Success
            break;
        case codecvt_base::partial:
        case codecvt_base::error:
            // Failure
            throw InvalidUTF8Exception();
            break;
        case codecvt_base::noconv:
            invariant(false);
            break;
    }

    _data.resize(to_next - &_data[0]);
}

UString::UString(const std::vector<char32_t> udata_src) : _data(udata_src) {}

std::string UString::toString() const {
    auto& f = use_facet<codecvt<char32_t, char, mbstate_t>>(locale());

    mbstate_t mb = mbstate_t();
    std::string output(_data.size() * f.max_length(), '\0');

    const char32_t* from_next;
    char* to_next;

    codecvt_base::result result = f.out(mb,
                                        &_data[0],
                                        &_data[_data.size()],
                                        from_next,
                                        &output[0],
                                        &output[output.size()],
                                        to_next);

    switch (result) {
        case codecvt_base::ok:
            // Success!
            break;
        case codecvt_base::partial:
        case codecvt_base::error:
        case codecvt_base::noconv:
            // Failure
            invariant(false);
            break;
    }

    output.resize(to_next - &output[0]);
    return output;
}

UString UString::substr(size_t begin, size_t end) const {
    if (begin >= _data.size()) {
        return UString("");
    } else if (end >= _data.size()) {
        return UString(std::vector<char32_t>(_data.begin() + begin, _data.end()));
    } else {
        return UString(std::vector<char32_t>(_data.begin() + begin, _data.begin() + end));
    }
}

UString UString::toLower(bool turkish) const {
    std::vector<char32_t> newdata;
    for (auto codepoint : _data) {
        newdata.push_back(codepointToLower(codepoint, turkish));
    }
    return UString(newdata);
}

UString UString::toNoDiacritics() const {
    std::vector<char32_t> newdata;
    for (auto codepoint : _data) {
        newdata.push_back(codepointRemoveDiacritics(codepoint));
    }
    return UString(newdata);
}

ostream& operator<<(ostream& os, const UString& str) {
    os << str.toString();
    return os;
}

bool UString::needleInHaystack(const UString& haystack,
                               const UString& needle,
                               bool caseSensitive,
                               bool ignoreDiacritics,
                               bool turkish) {
    if (caseSensitive == false && ignoreDiacritics == true) {
        return std::search(haystack._data.cbegin(),
                           haystack._data.cend(),
                           needle._data.cbegin(),
                           needle._data.cend(),
                           [&](char32_t c1, char32_t c2) {
                               return (codepointToLower(codepointRemoveDiacritics(c1), turkish) ==
                                       codepointToLower(codepointRemoveDiacritics(c1), turkish));
                           }) != haystack._data.cend();
    } else if (caseSensitive == true && ignoreDiacritics == true) {
        return std::search(haystack._data.cbegin(),
                           haystack._data.cend(),
                           needle._data.cbegin(),
                           needle._data.cend(),
                           [&](char32_t c1, char32_t c2) {
                               return (codepointRemoveDiacritics(c1) ==
                                       codepointRemoveDiacritics(c2));
                           }) != haystack._data.cend();
    } else if (caseSensitive == true && ignoreDiacritics == false) {
        return std::search(haystack._data.cbegin(),
                           haystack._data.cend(),
                           needle._data.cbegin(),
                           needle._data.cend(),
                           [&](char32_t c1, char32_t c2) { return (c1 == c2); }) !=
            haystack._data.cend();
    } else {  // caseSensitive == false && ignoreDiacritics == false
        return std::search(haystack._data.cbegin(),
                           haystack._data.cend(),
                           needle._data.cbegin(),
                           needle._data.cend(),
                           [&](char32_t c1, char32_t c2) {
                               return (codepointToLower(c1, turkish) ==
                                       codepointToLower(c2, turkish));
                           }) != haystack._data.cend();
    }
}

}  // namespace unicode
}  // namespace mongo
