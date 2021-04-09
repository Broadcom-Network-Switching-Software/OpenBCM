<?xml version="1.0" encoding="UTF-8"?>

<xsl:transform  version="2.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 
<xsl:template match="/">
<html>
    <head>
        <link rel="stylesheet" href="Manual_CSS.css" type="text/css" />
        <title>DNX Shell Commands</title>
    </head>
    <body>
        <p align="center">
            Shell Commands
        </p>
        <p>This document provides a list of DNX SDK shell command. Each commands has a section that includes an overview, description, command options and command examples.</p>
        <a name="tocLink">
            <h1>
                <font color="CC092F">Table of Contents</font>
            </h1>
        </a>
        <xsl:apply-templates mode="TOC"/>
        <xsl:apply-templates mode="H_ONE"/>
    </body>
</html>
</xsl:template>

<xsl:template match="top" mode="TOC">
    <table style="width:30%" border="1" cellpadding="5">
        <tr bgcolor="#D0D0CE">              
            <th>#</th>
            <th>Commands</th>           
        </tr>
        <xsl:for-each select="*">
            <tr>
                <td>
                    <xsl:number format="1." level="multiple" count="top/*"/>
                </td>
                <td>
                    <a href="#{name(.)}">
                        <xsl:value-of select="name()"/>
                    </a>
                </td>
            </tr>
        </xsl:for-each>
    </table>
</xsl:template>
 
<xsl:template match="top/*" mode="H_ONE">
    <h1>
        <font color="CC092F">
            <xsl:number format="1. " level="multiple" count="top/*"/>
            <xsl:text> </xsl:text>
            <a name="{name(.)}">
                <xsl:value-of select="name(.)">
                </xsl:value-of>
            </a>
        </font>
    </h1>
    <xsl:for-each select="@Brief">
        <p>
            <strong>
                <xsl:text>Overview
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:for-each select="@Introduction">
        <p>
            <strong>
                <xsl:text>Overview
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>    
    </xsl:for-each>
       <xsl:if test="Synopsis/*">
        <p>
            <strong>
                <xsl:text>Synopsis
                </xsl:text>
            </strong>
        </p>
        <xsl:for-each select="Synopsis/item/@command">
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
        </xsl:for-each> 
    </xsl:if>
    <xsl:choose>
    <xsl:when test="Arguments/* or Examples/*">
        <xsl:if test="Arguments/*">
            <p>
                <strong>
                    <xsl:text>Arguments
                    </xsl:text>
                </strong>
            </p>
            <table style="width:70%" border="1" cellpadding="5">
                <tr bgcolor="#D0D0CE">
                    <th>#</th>
                    <th>Name</th>
                    <th >Type</th> 
                    <th>Default</th>
                    <th>Description</th>
                </tr>
                <xsl:for-each select="Arguments/argument">
                    <tr>
                        <td>
                            <xsl:number format="1."/>
                        </td>
                        <td>
                            <xsl:for-each select="@name">
                                <xsl:value-of select="." />
                            </xsl:for-each>
                        </td>
                        <td>
                            <xsl:for-each select="@type">
                                <xsl:value-of select="." />
                            </xsl:for-each>
                        </td>
                        <td>
                            <xsl:for-each select="@default">
                                <xsl:value-of select="." />
                            </xsl:for-each>
                        </td>
                        <td>
                            <xsl:for-each select="@description">
                                <xsl:value-of select="." />
                            </xsl:for-each>
                            <xsl:for-each select="@name">
                                <xsl:if test="../EnumValues">
                                    <strong>
                                        <br/>
                                        <xsl:text>&#xa;</xsl:text>
                                        <br/>
                                        <xsl:text>Enum Values
                                        </xsl:text>
                                    </strong>
                                    <br/>
                                    <xsl:for-each select="../EnumValues/value">
                                        <xsl:for-each select="@name">
                                            <xsl:value-of select="." />
                                        </xsl:for-each>
                                        <xsl:text> - 
                                        </xsl:text>
                                        <xsl:for-each select="@description">
                                            <xsl:value-of select="." />
                                        </xsl:for-each>
                                        <br/>
                                    </xsl:for-each>
                                </xsl:if>
                            </xsl:for-each>
                        </td>
                    </tr>
                </xsl:for-each> 
            </table>
        </xsl:if>
        <xsl:if test="Examples/*">
                <p>
                    <strong>
                        <xsl:text>Examples
                        </xsl:text>
                    </strong>
                </p>
                <xsl:for-each select="Examples/item/@command">
                    <xsl:variable name="command" select="replace(., '^\s+|\s+$', '')"/>
                    <xsl:variable name="l0" select="../../name(..)"/>
                    <xsl:variable name="examplefile" select="concat('results/88800_', lower-case($l0), '.html')"/>
                    <p>
                        <a href="{$examplefile}#{$command}">
                            <xsl:value-of select="$command"/>
                        </a>
                    </p>
                </xsl:for-each> 
            </xsl:if>       
    </xsl:when>
    <xsl:otherwise>
        <xsl:apply-templates mode="H_TWO"/>
    </xsl:otherwise>
    </xsl:choose>
    
        <p><a href="#tocLink">Table of Contents</a></p>
</xsl:template>
 
<xsl:template match="*" mode="H_TWO">
    <h2>
        <xsl:number format="1." level="multiple" count="top/*"/>
        <xsl:number format="1. " level="multiple" count="top/*/*"/>
        <xsl:value-of select="name(..)">
        </xsl:value-of>
        <xsl:text> > </xsl:text>
        <xsl:value-of select="name()">
        </xsl:value-of>
    </h2>
    <xsl:for-each select="@Support">
        <p>
            <strong>
                <xsl:text>Support
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:for-each select="@Brief">
        <p>
            <strong>
                <xsl:text>Description
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:for-each select="@description">
        <p>
            <strong>
                <xsl:text>Description
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:for-each select="@XML">
        <p>
            <strong>
                <xsl:text>XML Output Support
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:if test="Synopsis/*">
        <p>
            <strong>
                <xsl:text>Synopsis
                </xsl:text>
            </strong>
        </p>
        <xsl:for-each select="Synopsis/item/@command">
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
        </xsl:for-each> 
    </xsl:if>
    <xsl:if test="Arguments/*">
    <p>
        <strong>
            <xsl:text>Arguments
            </xsl:text>
        </strong>
    </p>
    <table style="width:70%" border="1" cellpadding="5">
        <tr bgcolor="#D0D0CE">
            <th>#</th>
            <th>Name</th>
            <th >Type</th> 
            <th>Default</th>
            <th>Description</th>
        </tr>
        <xsl:for-each select="Arguments/argument">
            <tr>
                <td>
                    <xsl:number format="1."/>
                </td>
                <td>
                    <xsl:for-each select="@name">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@type">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@default">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@description">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                    <xsl:for-each select="@name">
                        <xsl:if test="../EnumValues">
                            <strong>
                                <br/>
                                <xsl:text>&#xa;</xsl:text>
                                <br/>
                                <xsl:text>Enum Values
                                </xsl:text>
                            </strong>
                            <br/>
                            <xsl:for-each select="../EnumValues/value">
                                <xsl:for-each select="@name">
                                    <xsl:value-of select="." />
                                </xsl:for-each>
                                <xsl:text> - 
                                </xsl:text>
                                <xsl:for-each select="@description">
                                    <xsl:value-of select="." />
                                </xsl:for-each>
                                <br/>
                            </xsl:for-each>
                        </xsl:if>
                    </xsl:for-each>
                </td>
            </tr>
        </xsl:for-each> 
    </table>
    </xsl:if>
    <xsl:if test="Examples/*">
        <p>
            <strong>
                <xsl:text>Examples
                </xsl:text>
            </strong>
        </p>
        <xsl:for-each select="Examples/item/@command">
            <xsl:variable name="command" select="replace(., '^\s+|\s+$', '')"/>
            <xsl:variable name="l0" select="../../../name(..)"/>
            <xsl:variable name="examplefile" select="concat('results/88800_', lower-case($l0), '.html')"/>
            <p>
                <a href="{$examplefile}#{$command}">
                    <xsl:value-of select="$command"/>
                </a>
            </p>
        </xsl:for-each> 
    </xsl:if>   
    <xsl:apply-templates mode="H_THREE"/>
</xsl:template>
 
<xsl:template match="*" mode="H_THREE">
<xsl:if test="@Brief">
    <h3>
        <xsl:number format="1." level="multiple" count="top/*"/>
        <xsl:number format="1." level="multiple" count="top/*/*"/>
        <xsl:number format="1. " level="multiple" count="top/*/*/*"/>
        <xsl:value-of select="name(../..)"/>
        <xsl:text> > </xsl:text>
        <xsl:value-of select="name(..)"/>
        <xsl:text> > </xsl:text>
        <xsl:value-of select="name()"/>
    </h3>
    <xsl:for-each select="@Brief">
        <p>
            <strong>
                <xsl:text>Description
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:for-each select="@description">
        <p>
            <strong>
                <xsl:text>Description
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:if test="Synopsis/*">
        <p>
            <strong>
                <xsl:text>Synopsis
                </xsl:text>
            </strong>
        </p>
        <xsl:for-each select="Synopsis/item/@command">
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
        </xsl:for-each> 
    </xsl:if>
    <xsl:if test="Arguments/*">
    <p>
        <strong>
            <xsl:text>Arguments
            </xsl:text>
        </strong>
    </p>
    <table style="width:70%" border="1" cellpadding="5">
        <tr bgcolor="#D0D0CE">
            <th>#</th>
            <th>Name</th>
            <th>Type</th> 
            <th>Default</th>
            <th>Description</th>
        </tr>
        <xsl:for-each select="Arguments/argument">
            <tr>
                <td>
                    <xsl:number format="1."/>
                </td>
                <td>
                    <xsl:for-each select="@name">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@type">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@default">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@description">
                        <xsl:value-of select="." />
                    </xsl:for-each>
                    <xsl:for-each select="@name">
                        <xsl:if test="../EnumValues">
                            <strong>
                                <br/>
                                <xsl:text>&#xa;</xsl:text>
                                <br/>
                                <xsl:text>Enum Values
                                </xsl:text>
                            </strong>
                            <br/>
                            <xsl:for-each select="../EnumValues/value">
                                <xsl:for-each select="@name">
                                    <xsl:value-of select="." />
                                </xsl:for-each>
                                <xsl:text> - 
                                </xsl:text>
                                <xsl:for-each select="@description">
                                    <xsl:value-of select="." />
                                </xsl:for-each>
                                <br/>
                            </xsl:for-each>
                        </xsl:if>
                    </xsl:for-each>
                </td>
            </tr>
        </xsl:for-each> 
    </table>
    </xsl:if>
    <xsl:if test="Examples/*">
        <p>
            <strong>
                <xsl:text>Examples
                </xsl:text>
            </strong>
        </p>
        <xsl:for-each select="Examples/item/@command">
            <xsl:variable name="command" select="replace(., '^\s+|\s+$', '')"/>
            <xsl:variable name="l0" select="../../../name(../..)"/>
            <xsl:variable name="examplefile" select="concat('results/88800_', lower-case($l0), '.html')"/>
            <p>
                <a href="{$examplefile}#{$command}">
                    <xsl:value-of select="$command"/>
                </a>
            </p>
        </xsl:for-each> 
    </xsl:if>   
    <xsl:apply-templates mode="H_FOUR"/>
</xsl:if>
</xsl:template>

<xsl:template match="*" mode="H_FOUR">
    <xsl:if test="@Brief">
        <h4>
            <xsl:number format="1." level="multiple" count="top/*"/>
            <xsl:number format="1." level="multiple" count="top/*/*"/>
            <xsl:number format="1." level="multiple" count="top/*/*/*"/>
            <xsl:number format="1. " level="multiple" count="top/*/*/*/*"/>
            <xsl:value-of select="name(../../..)"/>
            <xsl:text> > </xsl:text>
            <xsl:value-of select="name(../..)"/>
            <xsl:text> > </xsl:text>
            <xsl:value-of select="name(..)"/>
            <xsl:text> > </xsl:text>
            <xsl:value-of select="name()"/>
        </h4>       
    </xsl:if>
    <xsl:for-each select="@Description">
        <p>
            <strong>
                <xsl:text>Description
                </xsl:text>
            </strong>
        </p>
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
    </xsl:for-each>
    <xsl:if test="Synopsis/*">
        <p>
            <strong>
                <xsl:text>Synopsis
                </xsl:text>
            </strong>
        </p>
        <xsl:for-each select="Synopsis/item/@command">
        <p>
            <xsl:value-of select=".">
            </xsl:value-of>
        </p>
        </xsl:for-each> 
    </xsl:if>
    <xsl:if test="Arguments/*">
    <p>
        <strong>
            <xsl:text>Arguments
            </xsl:text>
        </strong>
    </p>
    <table style="width:70%" border="1" cellpadding="5">
        <tr bgcolor="#D0D0CE">
            <th>#</th>
            <th>Name</th>
            <th >Type</th> 
            <th>Default</th>
            <th>Description</th>
        </tr>
        <xsl:for-each select="Arguments/argument">
            <tr>
                <td>
                    <xsl:number format="1."/>
                </td>
                <td>
                    <xsl:for-each select="@name">
                    <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@type">
                    <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@default">
                    <xsl:value-of select="." />
                    </xsl:for-each>
                </td>
                <td>
                    <xsl:for-each select="@description">
                    <xsl:value-of select="." />
                    </xsl:for-each>
                    <xsl:for-each select="@name">
                        <xsl:if test="../EnumValues">
                            <strong>
                                <br/>
                                <xsl:text>&#xa;</xsl:text>
                                <br/>
                                <xsl:text>Enum Values
                                </xsl:text>
                            </strong>
                            <br/>
                            <xsl:for-each select="../EnumValues/value">
                                <xsl:for-each select="@name">
                                    <xsl:value-of select="." />
                                </xsl:for-each>
                                <xsl:text> - 
                                </xsl:text>
                                <xsl:for-each select="@description">
                                    <xsl:value-of select="." />
                                </xsl:for-each>
                                <br/>
                            </xsl:for-each>
                        </xsl:if>
                    </xsl:for-each>
                </td>
            </tr>
        </xsl:for-each> 
    </table>
    </xsl:if>
    <xsl:if test="Examples/*">
        <p>
            <strong>
                <xsl:text>Examples
                </xsl:text>
            </strong>
        </p>
        <xsl:for-each select="Examples/item/@command">
            <xsl:variable name="command" select="replace(., '^\s+|\s+$', '')"/>
            <xsl:variable name="l0" select="../../../name(../../..)"/>
            <xsl:variable name="examplefile" select="concat('results/88800_', lower-case($l0), '.html')"/>
            <p>
                <a href="{$examplefile}#{$command}">
                    <xsl:value-of select="$command"/>
                </a>
            </p>
        </xsl:for-each> 
    </xsl:if>
</xsl:template>
 
</xsl:transform>
