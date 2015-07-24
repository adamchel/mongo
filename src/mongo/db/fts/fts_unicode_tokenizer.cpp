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

#include "mongo/platform/basic.h"

#include "mongo/db/fts/fts_unicode_tokenizer.h"

#include "mongo/db/fts/fts_query.h"
#include "mongo/db/fts/fts_spec.h"
#include "mongo/db/fts/stemmer.h"
#include "mongo/db/fts/stop_words.h"
#include "mongo/db/fts/tokenizer.h"
#include "mongo/stdx/memory.h"
#include "mongo/util/mongoutils/str.h"
#include "mongo/util/stringutils.h"

namespace mongo {
namespace fts {

using std::string;

UnicodeFTSTokenizer::UnicodeFTSTokenizer(const FTSLanguage* language)
    : _language(language), _stemmer(language), _stopWords(StopWords::getStopWords(language)) {
    if (_language->str() == "english") {
        _delimListLanguage = unicode::DelimiterListLanguage::kEnglish;
    } else {
        _delimListLanguage = unicode::DelimiterListLanguage::kNotEnglish;
    }

    if (_language->str() == "turkish") {
        _caseFoldMode = unicode::CaseFoldMode::kTurkish;
    } else {
        _caseFoldMode = unicode::CaseFoldMode::kNormal;
    }
}

void UnicodeFTSTokenizer::reset(StringData document, FTSTokenizerOptions options) {
    _options = options;
    _pos = 0;
    _document = unicode::String(document);

    _skipDelimiters();
}

bool UnicodeFTSTokenizer::moveNext() {
    while (true) {
        if (_pos >= _document.size()) {
            _stem = "";
            return false;
        }

        // Find next word
        size_t start = _pos++;
        while (_pos < _document.size() &&
               (!unicode::codepointIsDelimiter(_document[_pos], _delimListLanguage))) {
            ++_pos;
        }
        unicode::String token = _document.substr(start, _pos - start);
        _skipDelimiters();

        unicode::String word = token.toLower(_caseFoldMode);

        // Stop words are case-sensitive and diacritic sensitive, so we need them to be lower cased
        // but with diacritics not removed to check against the stop word list.
        if ((_options & kFilterStopWords) && _stopWords->isStopWord(word.toString())) {
            continue;
        }

        if (_options & kGenerateCaseSensitiveTokens) {
            word = token;
        }

        if (!(_options & kGenerateDiacriticSensitiveTokens)) {
            word = word.removeDiacritics();
        }

        _stem = _stemmer.stem(word.toString());
        return true;
    }
}

StringData UnicodeFTSTokenizer::get() const {
    return _stem;
}

bool UnicodeFTSTokenizer::_skipDelimiters() {
    size_t start = _pos;
    while (_pos < _document.size() &&
           unicode::codepointIsDelimiter(_document[_pos], _delimListLanguage)) {
        ++_pos;
    }
    return _pos > start;
}

}  // namespace fts
}  // namespace mongo
