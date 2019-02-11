"""
generate.py

Author: Peter Lenkefi
Date: 2019-02-11
Description: This python script generates the data mappings from the official
unicode character databases.
"""

# Process UnicodeData.txt
def process_unicode_data():
    # Indicies
    CODEPOINT             = 0
    NAME                  = 1
    GENERAL_CATEGORY      = 2
    COMBINING_CLASS       = 3
    BIDIR_CATEGORY        = 4
    DECOMPOSITION_MAPPING = 5
    DECIMAL_DIGIT_VALUE   = 6
    DIGIT_VALUE           = 7
    NUMERIC_VALUE         = 8
    MIRRORED              = 9
    UNICODE_1_NAME        = 10
    COMMENT_FIELD         = 11
    UPPERCASE_MAPPING     = 12
    LOWERCASE_MAPPING     = 13
    TITLECASE_MAPPING     = 14

    file = open('UnicodeData.txt', 'r')

    print("Longest mapping is: ", end = '', flush = True)
    longest_decomp = 0
    longest_line = None

    for line in file:
        sections = line.split(';')

        decomp = sections[DECOMPOSITION_MAPPING]

        dc = decomp.split(' ')
        if longest_decomp < len(dc):
            longest_decomp = len(dc)
            longest_line = decomp

    print(longest_decomp)
    print(longest_line)

process_unicode_data()
