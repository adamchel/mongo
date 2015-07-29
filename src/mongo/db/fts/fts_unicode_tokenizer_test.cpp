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

#ifdef MSC_VER
// Microsoft VS 2013 does not handle UTF-8 strings in char literal strings, error C4566
// The Microsoft compiler can be tricked into using UTF-8 strings as follows:
// 1. The file has a UTF-8 BOM
// 2. The string literal is a wide character string literal (ie, prefixed with L)
// at this point.
#define UTF8(x) toUtf8String(L##x)
#else
#define UTF8(x) x
#endif

namespace mongo {
namespace fts {

std::vector<std::string> tokenizeString(const char* str,
                                        const char* language,
                                        FTSTokenizer::Options options) {
    // TODO: in next patch set, make this with text index version 3, and get the tokenizer directly
    // from the FTSLanguage.
    StatusWithFTSLanguage swl = FTSLanguage::make(language, TEXT_INDEX_VERSION_2);
    ASSERT_OK(swl);

    UnicodeFTSTokenizer tokenizer(swl.getValue());

    tokenizer.reset(str, options);

    std::vector<std::string> terms;

    while (tokenizer.moveNext()) {
        terms.push_back(tokenizer.get().toString());
    }

    return terms;
}

// Ensure punctuation is filtered out of the indexed document and the 's is not separated
TEST(FtsUnicodeTokenizer, English) {
    std::vector<std::string> terms =
        tokenizeString("Do you see Mark's dog running?", "english", FTSTokenizer::kNone);

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
    std::vector<std::string> terms = tokenizeString(
        UTF8("Voyez-vous «le chien» de Mark courante? C'est bien!"), "french", FTSTokenizer::kNone);

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

// Ensure punctuation is filtered out of the indexed document and that diacritics are not in the
// resulting tokens.
TEST(FtsUnicodeTokenizer, Turkish) {
    std::vector<std::string> terms = tokenizeString(
        UTF8("KAÇ YAŞINDASIN SEN, VE SEN NEREDEN VARDIR?"), "turkish", FTSTokenizer::kNone);

    ASSERT_EQUALS(7U, terms.size());
    ASSERT_EQUALS("kac", terms[0]);
    ASSERT_EQUALS("ya", terms[1]);
    ASSERT_EQUALS("sen", terms[2]);
    ASSERT_EQUALS("ve", terms[3]);
    ASSERT_EQUALS("sen", terms[4]);
    ASSERT_EQUALS("nere", terms[5]);
    ASSERT_EQUALS("var", terms[6]);
}

// Ensure punctuation is filtered out of the indexed document, that diacritics are not in the
// resulting tokens, and that the generated tokens are not lowercased.
TEST(FtsUnicodeTokenizer, TurkishCaseSensitive) {
    std::vector<std::string> terms =
        tokenizeString(UTF8("KAÇ YAŞINDASIN SEN, VE SEN NEREDEN VARDIR?"),
                       "turkish",
                       FTSTokenizer::kGenerateCaseSensitiveTokens);

    ASSERT_EQUALS(7U, terms.size());
    ASSERT_EQUALS("KAC", terms[0]);
    ASSERT_EQUALS("YASINDASIN", terms[1]);
    ASSERT_EQUALS("SEN", terms[2]);
    ASSERT_EQUALS("VE", terms[3]);
    ASSERT_EQUALS("SEN", terms[4]);
    ASSERT_EQUALS("NEREDEN", terms[5]);
    ASSERT_EQUALS("VARDIR", terms[6]);
}

// Ensure punctuation is filtered out of the indexed document, that diacritics are in the
// resulting tokens, and that the generated tokens are lowercased.
TEST(FtsUnicodeTokenizer, TurkishDiacriticSensitive) {
    std::vector<std::string> terms =
        tokenizeString(UTF8("KAÇ YAŞINDASIN SEN, VE SEN NEREDEN VARDIR?"),
                       "turkish",
                       FTSTokenizer::kGenerateDiacriticSensitiveTokens);

    ASSERT_EQUALS(7U, terms.size());
    ASSERT_EQUALS(UTF8("kaç"), terms[0]);
    ASSERT_EQUALS(UTF8("yaş"), terms[1]);
    ASSERT_EQUALS("sen", terms[2]);
    ASSERT_EQUALS("ve", terms[3]);
    ASSERT_EQUALS("sen", terms[4]);
    ASSERT_EQUALS("nere", terms[5]);
    ASSERT_EQUALS("var", terms[6]);
}

// Ensure punctuation is filtered out of the indexed document, that diacritics are in the
// resulting tokens, and that the generated tokens are not lowercased.
TEST(FtsUnicodeTokenizer, TurkishDiacriticAndCaseSensitive) {
    std::vector<std::string> terms =
        tokenizeString(UTF8("KAÇ YAŞINDASIN SEN, VE SEN NEREDEN VARDIR?"),
                       "turkish",
                       FTSTokenizer::kGenerateDiacriticSensitiveTokens |
                           FTSTokenizer::kGenerateCaseSensitiveTokens);

    ASSERT_EQUALS(7U, terms.size());
    ASSERT_EQUALS(UTF8("KAÇ"), terms[0]);
    ASSERT_EQUALS(UTF8("YAŞINDASIN"), terms[1]);
    ASSERT_EQUALS("SEN", terms[2]);
    ASSERT_EQUALS("VE", terms[3]);
    ASSERT_EQUALS("SEN", terms[4]);
    ASSERT_EQUALS("NEREDEN", terms[5]);
    ASSERT_EQUALS("VARDIR", terms[6]);
}

// Ensure punctuation is filtered out of the indexed document, that diacritics are in the
// resulting tokens, and that the generated tokens are not lowercased.
TEST(FtsUnicodeTokenizer, TurkishDiacriticAndCaseSensitiveAndStopWords) {
    std::vector<std::string> terms = tokenizeString(
        UTF8("KAÇ YAŞINDASIN SEN, VE SEN NEREDEN VARDIR?"),
        "turkish",
        FTSTokenizer::kGenerateDiacriticSensitiveTokens |
            FTSTokenizer::kGenerateCaseSensitiveTokens | FTSTokenizer::kFilterStopWords);

    ASSERT_EQUALS(4U, terms.size());
    ASSERT_EQUALS(UTF8("KAÇ"), terms[0]);
    ASSERT_EQUALS(UTF8("YAŞINDASIN"), terms[1]);
    ASSERT_EQUALS("NEREDEN", terms[2]);
    ASSERT_EQUALS("VARDIR", terms[3]);
}


// Ensure that stop words are only removed if they contain the correct diacritics.
TEST(FtsUnicodeTokenizer, FrenchStopWords) {
    std::vector<std::string> terms =
        tokenizeString(UTF8("Je ne vais pas etre énervé. Je vais être excité."),
                       "french",
                       FTSTokenizer::kFilterStopWords);

    ASSERT_EQUALS(5U, terms.size());
    ASSERT_EQUALS("vais", terms[0]);
    ASSERT_EQUALS("etre", terms[1]);
    ASSERT_EQUALS("enerv", terms[2]);
    ASSERT_EQUALS("vais", terms[3]);
    ASSERT_EQUALS("excit", terms[4]);
}

// Ensure that stop words are only removed if they contain the correct diacritics.
TEST(FtsUnicodeTokenizer, FrenchStopWordsAndDiacriticSensitive) {
    std::vector<std::string> terms = tokenizeString(
        UTF8("Je ne vais pas etre énervé. Je vais être excité."),
        "french",
        FTSTokenizer::kFilterStopWords | FTSTokenizer::kGenerateDiacriticSensitiveTokens);

    ASSERT_EQUALS(5U, terms.size());
    ASSERT_EQUALS("vais", terms[0]);
    ASSERT_EQUALS("etre", terms[1]);
    ASSERT_EQUALS(UTF8("énerv"), terms[2]);
    ASSERT_EQUALS("vais", terms[3]);
    ASSERT_EQUALS("excit", terms[4]);
}

}  // namespace fts
}  // namespace mongo
