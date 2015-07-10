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

#include "mongo/db/fts/fts_unicode_string_normalizer.h"

#include "mongo/db/fts/fts_language.h"
#include "mongo/db/fts/unicode_utils/u_string.h"


namespace mongo {
namespace fts {

using unicode::UString;

UnicodeFTSStringNormalizer::UnicodeFTSStringNormalizer(const FTSLanguage* language) {
    _turkish = language->str() == "turkish";
}

void UnicodeFTSStringNormalizer::reset(Options options) {
    _options = options;
}

string UnicodeFTSStringNormalizer::normalizeString(StringData str) const {

    UString result = UString(str.toString().c_str());

    result = result.toNoDiacritics();

    if(_options & Options::FoldCase) {
        if(!_turkish) {
            result = result.toLower();
        }
        else {
            result = result.toLower(true);
        }
    }

    return result.toString();

    /*
    if(_caseSensitive && !_removeDiacritics) {
    	return str.toString();
    }

    UString result = UString(str.toString().c_str());

    if(!_caseSensitive) {
    	if(!_turkish) {
    		result = result.toLower();
    	}
    	else {
    		result = result.toLower(true);
    	}
    }
    if(_removeDiacritics) {
    	result = result.toNoDiacritics();
    }

    return result.toString();*/
}

}  // namespace fts
}  // namespace mongo
