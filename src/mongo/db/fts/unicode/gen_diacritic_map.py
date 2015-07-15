 #!/usr/bin/python
 # -*- coding: utf-8 -*-
import sys
from unicodedata import normalize, category, unidata_version

from gen_helper import getCopyrightNotice, openNamespaces, closeNamespaces, include

def is_diacritic(codepoint):
    """Returns whether the given codepoint is a combining diacritical mark.

       The ranges in this function come from the combining diacritical marks code blocks, which are
       defined in ftp://unicode.org/Public/8.0.0/ucd/Blocks.txt, also see
       https://en.wikipedia.org/wiki/Combining_character
    """
    # Combining Diacritical Marks
    if (codepoint >= 0x0300 and codepoint <= 0x036F):
        return True

    # Combining Diacritical Marks Extended
    if (codepoint >= 0x1AB0 and codepoint <= 0x1AFF):
        return True

    # Combining Diacritical Marks Supplement
    if (codepoint >= 0x1DC0 and codepoint <= 0x1DFF):
        return True

    # Combining Diacritical Marks for Symbols
    if (codepoint >= 0x20D0 and codepoint <= 0x20FF):
        return True

    return False


diacritic_mappings = {}

def add_diacritic_mapping(codepoint):
    # c : original unicode character
    # d : decomposed unicode character
    # f : decomposed unicode character with diacritics removed
    # k : recomposed unicode character with diacritics removed
    c = chr(codepoint)
    d = normalize('NFD', c)
    f = u''

    for i in range(len(d)):
        if is_diacritic(ord(d[i])) == 0:
            f += d[i]

    k = normalize('NFC', f)

    # Only use mappings where the final recomposed form is a single codepoint
    if (c != k and len(k) == 1):
        diacritic_mappings[codepoint] = ord(k[0])

def add_diacritic_range(start, end):
    for x in range(start, end + 1):
        add_diacritic_mapping(x)

def generate(source):
    """Generates a C++ source file that contains a diacritic removal mapping function. 

    The delimiter checking function contains a switch statement with cases for every character in
    Unicode that has a removable combining diacritical mark.
    """
    print("source : %s" % source)
    out = open(source, "w")

    out.write(getCopyrightNotice())
    out.write(include("mongo/db/fts/unicode/codepoints.h"))
    out.write("\n")
    out.write(openNamespaces())

    # Map diacritics from 0 to the maximum Unicode codepoint
    add_diacritic_range(0x0000, 0x10FFFF)

    out.write("""char32_t codepointRemoveDiacritics(char32_t codepoint) {
    switch (codepoint) {\n""")

    mappings_list = []

    for mapping in diacritic_mappings:
        mappings_list.append((mapping, diacritic_mappings[mapping]))

    sorted_mappings = sorted(mappings_list, key=lambda mapping: mapping[0])

    for mapping in sorted_mappings:
        out.write("    case " + str(hex(mapping[0])) + ": return " + str(hex(mapping[1])) +";\n")

    out.write("    default: return codepoint;\n    }\n}")

    out.write(closeNamespaces())

if __name__ == "__main__":
    if(unidata_version != '8.0.0'):
        print("""ERROR: This script must be run with a version of Python that contains the \
Unicode 8.0.0 Character Database.""")
        sys.exit(1)
    generate(sys.argv[1])
