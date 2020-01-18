<?xml version="1.0" encoding="UTF-8"?>

<!-- Removes duplicate <compound>-entries in the index.xml file.           -->
<!-- This fixes the duplicate classnames in doxygen's 'Classes' index when -->
<!-- using inline namespaces in C++.                                       -->

<!-- Required for doxygen >= 1.8.16.                                       -->

<xsl:transform
	version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:noNamespaceSchemaLocation="index.xsd">

<xsl:output method="xml" standalone="yes" indent="yes" />
<xsl:strip-space elements="*"/>

<xsl:key name="keyCompoundById" match="compound" use="@refid"/>

<xsl:template match="node()|@*">
	<xsl:copy>
		<xsl:apply-templates select="node()|@*"/>
	</xsl:copy>
</xsl:template>

<xsl:template match="compound[
	not(generate-id() = generate-id(key('keyCompoundById', @refid)[1]))
]"/>

</xsl:transform>

