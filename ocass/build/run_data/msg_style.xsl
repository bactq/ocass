<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" encoding="UTF-8"/>
<xsl:template match="/">
	<html>
	<head>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
	<Title>Microsoft Office Communicator 2005</Title>
	</head>
	<body style="background-color:#f0f0f0">
	<br/>
	<center>
	<table align="center" width="90%" cellSpacing="0" cellPadding="0" border="1" 
		style="BORDER: #000000 1px solid; font-family:Verdana; font-size:67%; text-align:left; vertical-align:top; table-layout:fixed">
    <tr align='left' style="background-color:#e0edff">
        <th>#</th>
        <th>From</th>
        <th>To</th>
		<th>Message</th>
    </tr>     

	<xsl:for-each select="/MsgLog/MsgItem">
	<tr align='left' style="background-color:#e0e0e0">
        <td><p><xsl:value-of select="@TimeStamp"/></p></td>
		<td><p><xsl:value-of select="MsgFrom"/></p></td>
		<td><p><xsl:value-of select="MsgTo"/></p></td>
		<td><p><xsl:value-of select="MsgData"/></p></td>
	</tr>
	</xsl:for-each>
	</table>
	</center>
	<br/><center><hr color="black" width="95%"/></center>
	<table align="center" width="90%" cellSpacing="0" cellPadding="0" 
	style="font-family:Verdana; font-size:67%; text-align:left; vertical-align:top; table-layout:fixed">
		<tr>
			<td>
			<p>ocass version 0.1 - <a href="http://code.google.com/p/ocass/">Home Page</a> - 
			<a href="http://code.google.com/p/ocass/downloads/list">Downloads Page</a> -
			<a href="http://code.google.com/p/ocass/issues/list">New issue</a>
			</p>
			</td>
		</tr>
		<tr><td><br/></td></tr>
		<tr>
			<td>
			<p>Author: <b>Le Xiongjia</b> 
			<br/>Mail: <a href="mailto:webmaster@lexx.51.net" title="webmaster@lexx.51.net">webmaster@lexx.51.net</a>
			<br/>Blog: <a href="http://lexxwebmaster.spaces.live.com">http://lexxwebmaster.spaces.live.com</a>
			</p>
			</td>
		</tr>
	</table>

	</body>
	</html>
</xsl:template>
</xsl:stylesheet>
