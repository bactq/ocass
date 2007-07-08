<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" encoding="UTF-8"/>
<xsl:template match="/">
	<html>
	<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
	</head>
	<body>
	<table border='1'>
    <tr align='left'>
        <th>#</th>
        <th>From</th>
        <th>To</th>
		<th>Message</th>
    </tr>     

	<xsl:for-each select="/MsgLog/MsgItem">
	<tr align='left'>
        <td><p><xsl:value-of select="@TimeStamp"/></p></td>
		<td><p><xsl:value-of select="MsgFrom"/></p></td>
		<td><p><xsl:value-of select="MsgTo"/></p></td>
		<td><p><xsl:value-of select="MsgData"/></p></td>
	</tr>
	</xsl:for-each>

	</table>
	</body>
	</html>
</xsl:template>
</xsl:stylesheet>
