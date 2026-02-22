<?xml version="1.0" encoding="UTF-8"?>

<!-- Insert missing inline namespace names in <compoundname>s.              -->

<!-- Required for doxygen >= 1.8.16.                                        -->

<xsl:transform
	version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:noNamespaceSchemaLocation="index.xsd">

<xsl:output method="xml" standalone="no" indent="yes" />
<xsl:strip-space elements="*"/>


<!-- Default: Just copy everything -->

<xsl:template match="node()|@*">
	<xsl:copy>
		<xsl:apply-templates select="node()|@*"/>
	</xsl:copy>
</xsl:template>


<!-- Remove a specified namespace name.                        -->
<!-- This will fail if 'full_name' does not contain 'ns_name'. -->

<xsl:template name="remove_vns">
	<xsl:param name="full_name"/>
	<xsl:param name="ns_name"/>

	<xsl:value-of select="concat(
			substring-before($full_name, $ns_name),
			substring-after ($full_name, $ns_name))"/>
</xsl:template>


<!-- Match all compoundnames containing the version namespace and remove it. -->

<xsl:template match="compoundname[contains(text(),'::v_1_0_0')]">

	<!-- Copy entire element with attributes but modify text-node -->

	<xsl:copy>
		<xsl:apply-templates select="@*"/>

		<xsl:call-template name="remove_vns">
			<xsl:with-param name="full_name" select="text()"/>
			<xsl:with-param name="ns_name"   select="'::v_1_0_0'"/>
		</xsl:call-template>
	</xsl:copy>

</xsl:template>

</xsl:transform>

