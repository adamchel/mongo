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

#pragma once

#include <string>

namespace mongo {
namespace fts {

typedef uint8_t PhraseMatcherOptions;

/**
 * FTSPhraseMatcher
 *
 * An interface for substring matching routines. Currently used by the FTSMatcher for finding
 * phrases within text documents. Implementations do not need to support all options, but the
 * options that are and aren't supported must be clearly documented.
 */
class FTSPhraseMatcher {
public:
    virtual ~FTSPhraseMatcher() = default;

    enum Options {
        /**
         * No options.
         */
        None = 0,

        /**
         * Lowercase strings as part of normalization.
         */
        CaseSensitive = 1 << 0,
    };

    /**
     * Does the string 'phrase' occur in the string 'haystack'?  Match is case-insensitive if the
     * CaseSensitive options is not set.
     */
    virtual bool phraseMatches(const std::string& phrase,
                               const std::string& haystack,
                               PhraseMatcherOptions Options) = 0;
};

}  // namespace fts
}  // namespace mongo
