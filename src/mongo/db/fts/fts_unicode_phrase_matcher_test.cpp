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

#include "mongo/db/fts/fts_language.h"
#include "mongo/db/fts/fts_unicode_phrase_matcher.h"
#include "mongo/unittest/unittest.h"

namespace mongo {
namespace fts {

// Case insensitive & diacritic insensitive match.
TEST(FtsUnicodePhraseMatcher, CaseAndDiacriticInsensitive) {
    std::string str =
        "El pingüino Wenceslao hizo kilómetros bajo exhaustiva lluvia y frío, añoraba";
    std::string find1 = "pinguino wenceslao";
    std::string find2 = "frio, anoraba";

    std::string nofind1 = "bajo lluvia";
    std::string nofind2 = "El Wenceslao";

    // TODO: in next code review, make this with text index version 3, and get the phrase matcher
    // directly from the FTSLanguage.
    StatusWithFTSLanguage swl = FTSLanguage::make("spanish", TEXT_INDEX_VERSION_2);
    ASSERT_OK(swl);

    UnicodeFTSPhraseMatcher phraseMatcher(*swl.getValue());
    FTSPhraseMatcher::Options options = FTSPhraseMatcher::kNone;

    ASSERT(phraseMatcher.phraseMatches(find1, str, options));
    ASSERT(phraseMatcher.phraseMatches(find2, str, options));

    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind1, str, options));
    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind2, str, options));
}

// Case sensitive & diacritic insensitive match.
TEST(FtsUnicodePhraseMatcher, CaseSensitiveAndDiacriticInsensitive) {
    std::string str =
        "El pingüino Wenceslao hizo kilómetros bajo exhaustiva lluvia y frío, añoraba";
    std::string find1 = "pinguino Wenceslao";
    std::string find2 = "El pinguino";

    std::string nofind1 = "pinguino wenceslao";
    std::string nofind2 = "el pinguino";

    // TODO: in next code review, make this with text index version 3, and get the phrase matcher
    // directly from the FTSLanguage.
    StatusWithFTSLanguage swl = FTSLanguage::make("spanish", TEXT_INDEX_VERSION_2);
    ASSERT_OK(swl);

    UnicodeFTSPhraseMatcher phraseMatcher(*swl.getValue());
    FTSPhraseMatcher::Options options = FTSPhraseMatcher::kCaseSensitive;

    ASSERT(phraseMatcher.phraseMatches(find1, str, options));
    ASSERT(phraseMatcher.phraseMatches(find2, str, options));

    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind1, str, options));
    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind2, str, options));
}

// Case insensitive & diacritic sensitive match.
TEST(FtsUnicodePhraseMatcher, CaseInsensitiveAndDiacriticSensitive) {
    std::string str =
        "El pingüino Wenceslao hizo kilómetros bajo exhaustiva lluvia y frío, añoraba";
    std::string find1 = "HIZO KILÓMETROS";
    std::string find2 = "el pingüino";

    std::string nofind1 = "hizo kilometros";
    std::string nofind2 = "pinguino";

    // TODO: in next code review, make this with text index version 3, and get the phrase matcher
    // directly from the FTSLanguage.
    StatusWithFTSLanguage swl = FTSLanguage::make("spanish", TEXT_INDEX_VERSION_2);
    ASSERT_OK(swl);

    UnicodeFTSPhraseMatcher phraseMatcher(*swl.getValue());
    FTSPhraseMatcher::Options options = FTSPhraseMatcher::kDiacriticSensitive;

    ASSERT(phraseMatcher.phraseMatches(find1, str, options));
    ASSERT(phraseMatcher.phraseMatches(find2, str, options));

    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind1, str, options));
    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind2, str, options));
}

// Case sensitive & diacritic sensitive match.
TEST(FtsUnicodePhraseMatcher, CaseAndDiacriticSensitive) {
    std::string str =
        "El pingüino Wenceslao hizo kilómetros bajo exhaustiva lluvia y frío, añoraba";
    std::string find1 = "pingüino Wenceslao";
    std::string find2 = "kilómetros bajo";

    std::string nofind1 = "pinguino Wenceslao";
    std::string nofind2 = "kilómetros BaJo";

    // TODO: in next code review, make this with text index version 3, and get the phrase matcher
    // directly from the FTSLanguage.
    StatusWithFTSLanguage swl = FTSLanguage::make("spanish", TEXT_INDEX_VERSION_2);
    ASSERT_OK(swl);

    UnicodeFTSPhraseMatcher phraseMatcher(*swl.getValue());
    FTSPhraseMatcher::Options options =
        FTSPhraseMatcher::kCaseSensitive | FTSPhraseMatcher::kDiacriticSensitive;

    ASSERT(phraseMatcher.phraseMatches(find1, str, options));
    ASSERT(phraseMatcher.phraseMatches(find2, str, options));

    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind1, str, options));
    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind2, str, options));
}

// Case insensitive & diacritic insensitive match.
TEST(FtsUnicodePhraseMatcher, CaseAndDiacriticInsensitiveTurkish) {
    std::string str = "Pijamalı hasta yağız şoföre çabucak güvendi.";
    std::string find1 = "PİJAMALI hasta";
    std::string find2 = "YAGIZ sofore";

    std::string nofind1 = "çabucak GÜVENDI";
    std::string nofind2 = "yagiz sofore";

    // TODO: in next code review, make this with text index version 3, and get the phrase matcher
    // directly from the FTSLanguage.
    StatusWithFTSLanguage swl = FTSLanguage::make("turkish", TEXT_INDEX_VERSION_2);
    ASSERT_OK(swl);

    UnicodeFTSPhraseMatcher phraseMatcher(*swl.getValue());
    FTSPhraseMatcher::Options options = FTSPhraseMatcher::kNone;

    ASSERT(phraseMatcher.phraseMatches(find1, str, options));
    ASSERT(phraseMatcher.phraseMatches(find2, str, options));

    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind1, str, options));
    ASSERT_FALSE(phraseMatcher.phraseMatches(nofind2, str, options));
}


}  // namespace fts
}  // namespace mongo
