# -*- coding: utf-8 -*-

## Applies an XSLT script on an input XML file and specifies the output.

import sys
import lxml.etree as ET

def xsl_transform(inDoc, xslFile, outDoc):
    doc = ET.parse(inDoc)
    xslt = ET.parse(xslFile)
    transform = ET.XSLT(xslt)
    transformed_doc = transform(doc)
    with open(outDoc, 'w') as f:
        print(transformed_doc, file=f)
    return

xsl_transform(sys.argv[1], sys.argv[2], sys.argv[3])

