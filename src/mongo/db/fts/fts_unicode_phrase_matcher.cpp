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

#include "mongo/db/fts/fts_unicode_phrase_matcher.h"

#include "mongo/db/fts/fts_language.h"
#include "mongo/db/fts/unicode/string.h"

namespace mongo {
namespace fts {

using std::string;

using unicode::CaseFoldMode;

UnicodeFTSPhraseMatcher::UnicodeFTSPhraseMatcher(const string& language) {
    if (language == "turkish") {
        _caseFoldMode = unicode::CaseFoldMode::kTurkish;
    } else {
        _caseFoldMode = unicode::CaseFoldMode::kNormal;
    }
}

void UnicodeFTSPhraseMatcher::setPhrase(const string& phrase) {
    _masterPhraseBuffer = unicode::String(phrase);
}

bool UnicodeFTSPhraseMatcher::phraseMatches(const string& haystack, Options options) const {
    unicode::String::SubstrMatchOptions matchOptions = unicode::String::kNone;
    _haystackBuffer.resetData(haystack);

    if (options & kCaseSensitive) {
        matchOptions |= unicode::String::kCaseSensitive;
    }

    if (options & kDiacriticSensitive) {
        matchOptions |= unicode::String::kDiacriticSensitive;
    }

    _masterPhraseBuffer.copyToBuf(_phraseBuffer);
    return substrMatch(_haystackBuffer, _phraseBuffer, matchOptions, _caseFoldMode);
}

bool substrMatch(unicode::String& str, unicode::String& find, SubstrMatchOptions options, CaseFoldMode cfMode) {
    // In Turkish, lowercasing needs to be applied first because the letter İ has a different case
    // folding mapping than the letter I, but removing diacritics removes the dot from İ.
    if (cfMode == CaseFoldMode::kTurkish) {
        str.toLowerToBuf(str);
        find.toLowerToBuf(find);
        return substrMatch(str, find, options | kCaseSensitive, CaseFoldMode::kNormal);
    }

    if (options & kDiacriticSensitive) {
        if (options & kCaseSensitive) {
            // Case sensitive and diacritic sensitive.
            return std::search(str._data.cbegin(),
                               str._data.cend(),
                               find._data.cbegin(),
                               find._data.cend(),
                               [&](char32_t c1, char32_t c2) { return (c1 == c2); }) !=
                str._data.cend();
        }

        // Case insensitive and diacritic sensitive.
        return std::search(str._data.cbegin(),
                           str._data.cend(),
                           find._data.cbegin(),
                           find._data.cend(),
                           [&](char32_t c1, char32_t c2) {
                               return (codepointToLower(c1, cfMode) ==
                                       codepointToLower(c2, cfMode));
                           }) != str._data.cend();
    }

    str.removeDiacriticsToBuf(_diacriticFreeHaystackBuffer);
    find.removeDiacriticsToBuf(_diacriticFreePhraseBuffer);

    return substrMatch(_diacriticFreeHaystackBuffer,
                       _diacriticFreePhraseBuffer,
                       options | kDiacriticSensitive,
                       cfMode);
}

}  // namespace fts
}  // namespace mongo
