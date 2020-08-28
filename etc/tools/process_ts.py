#!/usr/bin/env python3
# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.
#
# Run this script to generate/update l10n/en_US.ts
#
# The bulk of Addle text exposed to the user is kept fully out of source. Some
# (en_US) text is provided in source, however, for e.g. debug messages. For
# these, the version in source should be used in the en_US l10n.

import os.path
import sys
import subprocess
import xml.etree.ElementTree

thisdir = os.path.dirname(os.path.realpath(__file__))
filename = os.path.join(thisdir, '../l10n/en_US.ts')

subprocess.run(['lupdate', '../../src',
    '-source-language', 'en_US',
    '-target-language', 'en_US',
    '-no-obsolete',
    '-ts', filename], cwd=thisdir, check=True )

tree = xml.etree.ElementTree.parse(filename)

for messageNode in tree.findall('.//message'):

    sourceNode = messageNode.find('source')
    translationNode = messageNode.find('translation')

    if sourceNode.text:
        translationNode.text = sourceNode.text
        if 'type' in translationNode.attrib:
            del translationNode.attrib['type']

tree.write(filename, 'UTF-8', True)