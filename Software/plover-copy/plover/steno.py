# Copyright (c) 2010-2011 Joshua Harlan Lifton.
# See LICENSE.txt for details.

# TODO: unit test this file

"""Generic stenography data models.

This module contains the following class:

Stroke -- A data model class that encapsulates a sequence of steno keys.

"""

import re

STROKE_DELIMITER = '/'
IMPLICIT_HYPHENS = set('')

def normalize_steno(strokes_string):
    """Convert steno strings to one common form."""

    strokes = strokes_string.split(STROKE_DELIMITER)
    normalized_strokes = []
    for stroke in strokes:
        if '#' in stroke:
            stroke = stroke.replace('#', '')
            if not re.search('[0-9]', stroke):
                stroke = '#' + stroke
        has_implicit_dash = True #bool(set(stroke) & IMPLICIT_HYPHENS)
        if has_implicit_dash:
            stroke = stroke.replace('-', '')
        if stroke.endswith('-'):
            stroke = stroke[:-1]
        normalized_strokes.append(stroke)
        print 'normalize_steno : ' + strokes_string + ' = ' + ''.join(tuple(normalized_strokes))
    return tuple(normalized_strokes)

STENO_KEY_NUMBERS = {'S-': '1-',
                     'P-': '2-',
                     'T-': '3-',
                     'V-': '4-',
                     'I-': '5-',
                     'A-': '0-',
                     '-C': '-6',
                     '-T': '-7',
                     '-P': '-8',
                     '-I': '-9'}

STENO_KEY_ORDER = {"#": 0,
                   "S-": 1,
                   "P-": 2,
                   "C-": 3,
                   "T-": 4,
                   "H-": 5,
                   "V-": 6,
                   "R-": 7,
                   "I-": 8,
                   "A-": 9,
                   "E-": 10,
                   "O-": 11,
                   "*": 12,
                   "-c": 13,
                   "-s": 14,
                   "-t": 15,
                   "-h": 16,
                   "-p": 17,
                   "-r": 18,
                   "-i": 19,
                   "-e": 20,
                   "-a": 21,
                   "-o": 22}


class Stroke:
    """A standardized data model for stenotype machine strokes.

    This class standardizes the representation of a stenotype chord. A stenotype
    chord can be any sequence of stenotype keys that can be simultaneously
    pressed. Nearly all stenotype machines offer the same set of keys that can
    be combined into a chord, though some variation exists due to duplicate
    keys. This class accounts for such duplication, imposes the standard
    stenographic ordering on the keys, and combines the keys into a single
    string (called RTFCRE for historical reasons).

    """

    IMPLICIT_HYPHEN = set(()) #set(('I-', 'A-', '5-', '0-', 'E-', 'O-', '*'))

    def __init__(self, steno_keys) :
        """Create a steno stroke by formatting steno keys.

        Arguments:

        steno_keys -- A sequence of pressed keys.

        """
        # Remove duplicate keys and save local versions of the input 
        # parameters.
        steno_keys_set = set(steno_keys)
        steno_keys = list(steno_keys_set)

        # Order the steno keys so comparisons can be made.
        steno_keys.sort(key=lambda x: STENO_KEY_ORDER.get(x, -1))
         
        # Convert strokes involving the number bar to numbers.
        if '#' in steno_keys:
            numeral = False
            for i, e in enumerate(steno_keys):
                if e in STENO_KEY_NUMBERS:
                    steno_keys[i] = STENO_KEY_NUMBERS[e]
                    numeral = True
            if numeral:
                steno_keys.remove('#')
        
        if True:
            self.rtfcre = ''.join(key.strip('-') for key in steno_keys)
        else:
            pre = ''.join(k.strip('-') for k in steno_keys if k[-1] == '-' or 
                          k == '#')
            post = ''.join(k.strip('-') for k in steno_keys if k[0] == '-')
            self.rtfcre = '-'.join([pre, post]) if post else pre

        self.steno_keys = steno_keys

        # Determine if this stroke is a correction stroke.
        self.is_correction = (self.rtfcre == '*')

    def __str__(self):
        if self.is_correction:
            prefix = '*'
        else:
            prefix = ''
        return '%sStroke(%s : %s)' % (prefix, self.rtfcre, self.steno_keys)

    def __eq__(self, other):
        return (isinstance(other, Stroke)
                and self.steno_keys == other.steno_keys)

    def __ne__(self, other):
        return not self.__eq__(other)

    def __repr__(self):
        return str(self)

