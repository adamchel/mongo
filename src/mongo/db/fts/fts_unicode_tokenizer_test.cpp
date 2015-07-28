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

#include "mongo/db/fts/fts_spec.h"
#include "mongo/db/fts/fts_tokenizer.h"
#include "mongo/db/fts/fts_unicode_tokenizer.h"
#include "mongo/unittest/unittest.h"

namespace mongo {
namespace fts {

std::vector<std::string> tokenizeString(const char* str,
                                        const char* language,
                                        bool filterStopWords) {
    // TODO: in next patch set, make this with text index version 3, and get the tokenizer directly
    // from the FTSLanguage.
    StatusWithFTSLanguage swl = FTSLanguage::make(language, TEXT_INDEX_VERSION_2);
    ASSERT_OK(swl);

    UnicodeFTSTokenizer tokenizer(swl.getValue());

    FTSTokenizer::FTSTokenizerOptions options = FTSTokenizer::kNone;

    if (filterStopWords) {
        options |= FTSTokenizer::kFilterStopWords;
    }

    tokenizer.reset(str, options);

    std::vector<std::string> terms;

    while (tokenizer.moveNext()) {
        terms.push_back(tokenizer.get().toString());
    }

    return terms;
}

// Ensure punctuation is filtered out of the indexed document and the 's is not separated
TEST(FtsUnicodeTokenizer, English) {
    std::vector<std::string> terms = tokenizeString("Do you see Mark's dog running?", "english", false);

    ASSERT_EQUALS(6U, terms.size());
    ASSERT_EQUALS("do", terms[0]);
    ASSERT_EQUALS("you", terms[1]);
    ASSERT_EQUALS("see", terms[2]);
    ASSERT_EQUALS("mark", terms[3]);
    ASSERT_EQUALS("dog", terms[4]);
    ASSERT_EQUALS("run", terms[5]);
}

// Ensure punctuation is filtered out of the indexed document and the 'est is separated.
TEST(FtsUnicodeTokenizer, French) {
    std::vector<std::string> terms =
        tokenizeString("Voyez-vous «le chien» de Mark courante? C'est bien!", "french", false);

    ASSERT_EQUALS(10U, terms.size());
    ASSERT_EQUALS("voi", terms[0]);
    ASSERT_EQUALS("vous", terms[1]);
    ASSERT_EQUALS("le", terms[2]);
    ASSERT_EQUALS("chien", terms[3]);
    ASSERT_EQUALS("de", terms[4]);
    ASSERT_EQUALS("mark", terms[5]);
    ASSERT_EQUALS("cour", terms[6]);
    ASSERT_EQUALS("c", terms[7]);
    ASSERT_EQUALS("est", terms[8]);
    ASSERT_EQUALS("bien", terms[9]);
}

// Ensure punctuation is filtered out of the indexed document and that diacritics aren't in the
// resulting tokens.
TEST(FtsUnicodeTokenizer, Turkish) {
    std::vector<std::string> terms =
        tokenizeString("KAÇ YAŞINDASIN SEN, VE SEN NEREDEN VARDIR?", "turkish", false);

    ASSERT_EQUALS(7U, terms.size());
    ASSERT_EQUALS("kac", terms[0]);
    ASSERT_EQUALS("ya", terms[1]);
    ASSERT_EQUALS("sen", terms[2]);
    ASSERT_EQUALS("ve", terms[3]);
    ASSERT_EQUALS("sen", terms[4]);
    ASSERT_EQUALS("nere", terms[5]);
    ASSERT_EQUALS("var", terms[6]);
}

// Ensure that stop words are only removed if they contain the correct diacritics.
TEST(FtsUnicodeTokenizer, FrenchStopWords) {
    std::vector<std::string> terms =
        tokenizeString("Je ne vais pas etre heureux. Je vais être heureux.", "french", true);

    ASSERT_EQUALS(5U, terms.size());
    ASSERT_EQUALS("vais", terms[0]);
    ASSERT_EQUALS("etre", terms[1]);
    ASSERT_EQUALS("heureux", terms[2]);
    ASSERT_EQUALS("vais", terms[3]);
    ASSERT_EQUALS("heureux", terms[4]);
}

}  // namespace fts
}  // namespace mongo
