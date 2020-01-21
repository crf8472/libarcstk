# -*- coding: utf-8 -*-

import lxml.etree as ET

def xsl_transform(inDoc, xslFile, outDoc):
    doc = ET.parse(inDoc)
    xslt = ET.parse(xslFile)
    transform = ET.XSLT(xslt)
    transformed_doc = transform(doc)
    with open(outDoc, 'w') as f:
        print(transformed_doc, file=f)
    return

def doxy_deduplicate_index():
    xsl_transform(
            "@DOXYGEN_OUTPUT_DIRECTORY@/xml/index.xml",
            "@PROJECT_DOC_DIR@/thirdparty/doxygen/dedup_index.xsl",
            "@DOXYGEN_OUTPUT_DIRECTORY@/xml/index.xml")
    print('Removed duplicated <compound> elements in index.xml')

doxy_deduplicate_index()

