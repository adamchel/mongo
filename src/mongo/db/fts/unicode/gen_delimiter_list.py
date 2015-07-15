 #!/usr/bin/python
 # -*- coding: utf-8 -*-
import sys
import unicodedata

from gen_helper import getCopyrightNotice, openNamespaces, closeNamespaces, include

def generate(unicode_proplist_file, source):
    """Generates a C++ source file that contains a delimiter checking function. 

    The delimiter checking function contains a switch statement with cases for every delimiter
    in the Unicode Character Database with the properties specified in delim_properties.
    """
    print("source : %s" % source)
    out = open(source, "w")

    out.write(getCopyrightNotice())
    out.write(include("mongo/db/fts/unicode/codepoints.h"))
    out.write("\n")
    out.write(openNamespaces())

    delim_codepoints = []

    cf_file = open(unicode_proplist_file, 'r')

    delim_properties = ["White_Space", 
                        "Dash", 
                        "Hyphen", 
                        "Quotation_Mark", 
                        "Terminal_Punctuation", 
                        "Pattern_Syntax", 
                        "STerm"]

    for line in cf_file:
        data = line[:line.find('#')]
        values = data.split("; ")
        if len(values) < 2:
            continue

        uproperty = values[1].strip()
        if uproperty in delim_properties:
            if len(values[0].split('..')) == 2:
                codepoint_range = values[0].split('..')
                for i in range(int(codepoint_range[0], 16), int(codepoint_range[1], 16) + 1):
                    if i not in delim_codepoints: 
                        delim_codepoints.append(i)
            else:
                if int(values[0], 16) not in delim_codepoints:
                    delim_codepoints.append(int(values[0], 16))

    # As of Unicode 8.0.0, all of the delimiters we used for text index version 2 are also in the list.

    out.write("""bool codepointIsDelimiter(char32_t codepoint, DelimiterListLanguage lang) {
    if (lang == DelimiterListLanguage::kEnglish && codepoint == '\\'') {
        return false;
    }

    switch (codepoint) {\n""")

    for delim in delim_codepoints:
        out.write("\
    case " + str(hex(delim)) + ": return true;\n")

    out.write("\
    default: return false;\n    }\n}")

    out.write(closeNamespaces())

if __name__ == "__main__":
    generate(sys.argv[1], sys.argv[2])
