<?xml version="1.0" encoding="UTF-8"?>


<!-- Required for doxygen >= 1.8.16 (Aug-08, 2019). -->

<xsl:transform
	version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:noNamespaceSchemaLocation="compound.xsd">

<xsl:output method="xml" standalone="no" indent="yes" />
<xsl:strip-space elements="*"/>


<!-- Name of the namespace that is to be removed from compound names. -->

<xsl:variable name="ns_to_remove" select="'v_1_0_0'"/>


<!-- Copy everything by default but use specified templates. -->

<xsl:template match="node()|@*">
	<xsl:copy>
		<xsl:apply-templates select="node()|@*"/>
	</xsl:copy>
</xsl:template>


<!-- Remove namespace from compoundname. -->

<xsl:variable name="ns_path" select="concat('arcstk::', $ns_to_remove, '::')"/>

<xsl:template match="compoundname[starts-with(text(), '$ns_path')]">
	<xsl:element name="compoundname">
		<xsl:text>arcstk::</xsl:text>
		<xsl:value-of select="substring-after(., $ns_path)"/>
	</xsl:element>
</xsl:template>

</xsl:transform>

