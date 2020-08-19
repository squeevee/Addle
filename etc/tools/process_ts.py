#!/usr/bin/env python3
# Addle configuration code
# @file
# @copyright Copyright 2020 Eleanor Hawk
# @copyright Modification and distribution permitted under the terms of the
# MIT License. See "LICENSE" for full details.

import xml.etree.ElementTree as ET
import os.path
import sys 

if len(sys.argv) < 2:
    exit('1 argument required')

filename = os.path.join( os.path.dirname(os.path.realpath(__file__)), '../l10n', sys.argv[1] + '.ts')

lang = sys.argv[1]
if lang == 'fallback':
    lang = 'en_US'

tree = ET.parse(filename)

tree.getroot().set('language', lang)

for contextNode in tree.findall('.//context'):

    for messageNode in contextNode.findall('.//message'):

        sourceNode = messageNode.find('source')
        translationNode = messageNode.find('translation')

        if sys.argv[1] == 'en_US' and sourceNode.text:
            contextNode.remove(messageNode)
            continue

        if (sys.argv[1] == 'fallback'):
            if not sourceNode.text:
                contextNode.remove(messageNode)
                continue
            
            translationNode.text = sourceNode.text
            translationNode.set('type', 'finished')

tree.write(filename, 'UTF-8', True)