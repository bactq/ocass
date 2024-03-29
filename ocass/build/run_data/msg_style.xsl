<?xml version="1.0"?>
<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" encoding="UTF-8"/>
<xsl:template match="/">
	<html>
	<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
	<Title>Conversation history</Title>
	</head>
	<body style="background-color:#f0f0f0">
	<br/>
	<center>
	<table align="center" width="90%" cellSpacing="0" cellPadding="1" border="1" BORDERCOLOR="black" 
		style="BORDER: #000000 1px solid; font-family:Verdana; font-size:67%; text-align:left; vertical-align:top; table-layout:fixed">
    <tr align='left'  width="100%" style="background-color:#e0edff">
        <th>#</th>
        <th>From</th>
        <th>To</th>
	<th width="50%">Message</th>
    </tr>     

	<xsl:for-each select="/MsgLog/MsgItem">
	<tr align='left' width="100%" style="background-color:#e0e0e0">
		<td><p>&nbsp;<xsl:value-of select="@TimeStamp"/></p></td>
		<td><p>&nbsp;<xsl:value-of select="MsgFrom"/></p></td>
		<td><p>&nbsp;<xsl:value-of select="MsgTo"/></p></td>
		<td width="50%"><p>&nbsp;<xsl:value-of select="MsgData"/></p></td>
	</tr>
	</xsl:for-each>
	</table>
	</center>
	<br/><center><hr color="black" width="95%"/></center>
	<table align="center" width="90%" cellSpacing="0" cellPadding="0" 
	style="font-family:Verdana; font-size:67%; text-align:left; vertical-align:top; table-layout:fixed">
		<tr>
			<td>
			<p>ocass version 0.0.0.1 alpha - <a href="http://code.google.com/p/ocass/">Home Page</a> - 
			<a href="http://code.google.com/p/ocass/downloads/list">Downloads Page</a> -
			<a href="http://code.google.com/p/ocass/issues/list">New issue</a> - 
			<a href="http://code.google.com/p/ocass/source">Get source</a>
			</p>
			</td>

			<td>
			<p>Author: <b>Le Xiongjia</b></p>
			</td>
		</tr>
		<tr width="25%">
			<td></td>
			<td>
			<p>Mail: <a href="mailto:webmaster@lexx.51.net" title="webmaster@lexx.51.net">webmaster@lexx.51.net</a>
			&nbsp;&nbsp;<a href="mailto:xiongjia_le@sonic.com" title="xiongjia_le@sonic.com">xiongjia_le@sonic.com</a>
			<br/>Blog: <a href="http://lexxwebmaster.spaces.live.com">http://lexxwebmaster.spaces.live.com</a>
			<br/>Home: <a href="http://wiki-tower.org">http://wiki-tower.org</a>
			</p>
			</td>
		</tr>
	</table>

	</body>
	</html>
</xsl:template>
</xsl:stylesheet>
