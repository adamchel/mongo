 #!/usr/bin/python
 # -*- coding: utf-8 -*-
import os
import sys

from gen_helper import getCopyrightNotice, openNamespaces, closeNamespaces, include

def generate(unicode_casefold_file, source):
    """Generates a C++ source file that contains a Unicode case folding function. 

    The case folding function contains a switch statement with cases for every Unicode
    codepoint that has a case folding mapping. 
    """
    print("source : %s" % source)
    out = open(source, "w")

    out.write(getCopyrightNotice())
    out.write(include("mongo/db/fts/unicode/codepoints.h"))
    out.write("\n")
    out.write(openNamespaces())

    case_mappings = {}

    cf_file = open(unicode_casefold_file, 'r')

    for line in cf_file:
        data = line[:line.find('#')]
        values = data.split("; ")
        if len(values) < 3:
            continue

        status = values[1]
        if status == 'C' or status == 'S':
            # We only include the "Common" and "Simple" mappings. "Full" case folding mappings
            # expand certain letters to multiple codepoints, which we currently do not support.
            original_codepoint = int(values[0], 16)
            codepoint_mapping  = int(values[2], 16)
            case_mappings[original_codepoint] = codepoint_mapping

    out.write("""char32_t codepointToLower(char32_t codepoint, CaseFoldMode mode) {
    if (mode == CaseFoldMode::kTurkish) {
        if (codepoint == 0x049) {  // I -> ı
            return 0x131;
        } else if (codepoint == 0x130) {  // İ -> i
            return 0x069;
        }
    }

    switch (codepoint) {\n""")

    for mapping in case_mappings:
        out.write("\
    case " + str(hex(mapping)) + ": return " + str(hex(case_mappings[mapping])) +";\n")

    out.write("\
    default: return codepoint;\n    }\n}")

    out.write(closeNamespaces())

if __name__ == "__main__":
    generate(sys.argv[1], sys.argv[2])
