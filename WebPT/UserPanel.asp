<HTML>
<BODY>
<TITLE> Testing Delphi ASP </TITLE>
<CENTER>
<H3> You should see the results of your Delphi Active Server method below </H3>
</CENTER>
<HR>
<% Set DelphiASPObj = Server.CreateObject("WebPT.UserPanel") 
   DelphiASPObj.{Insert Method name here}
%>
<HR>
</BODY>
</HTML>