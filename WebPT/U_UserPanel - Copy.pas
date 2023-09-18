unit U_UserPanel;

{$WARN SYMBOL_PLATFORM OFF}

interface

uses
  ComObj, ActiveX, AspTlb, WebPT_TLB, StdVcl, U_SQL, Variants, Windows, SysUtils;

type
  TUserPanel = class(TASPObject, IUserPanel)
  protected
    procedure OnEndPage; safecall;
    procedure OnStartPage(const AScriptingContext: IUnknown); safecall;
      procedure OnLoginPage; safecall;
    procedure Index; safecall;
    procedure CheckUpdate; safecall;
  end;

implementation

uses ComServ;

procedure TUserPanel.OnEndPage;
begin
  inherited OnEndPage;
end;

procedure TUserPanel.OnStartPage(const AScriptingContext: IUnknown);
begin
  inherited OnStartPage(AScriptingContext);
end;

procedure TUserPanel.OnLoginPage;
begin
  Response.Write('<!DOCTYPE html>');
  Response.Write('<html lang="en">');
  Response.Write('  <head>');
  Response.Write('    <meta charset="utf-8">');
  Response.Write('    <meta http-equiv="X-UA-Compatible" content="IE=edge">');
  Response.Write('    <meta name="viewport" content="width=device-width, initial-scale=1">');
  Response.Write('');
  Response.Write('    <link href="index.css" rel="stylesheet">');
  Response.Write('  </head>');
  Response.Write('  <body>');
  Response.Write('    <form name=loginpt method=post action="">');
  Response.Write('    <table align="center" border="0" cellpadding="0" cellspacing="0" style="margin-top: 50px">');
  Response.Write('      <tr>');
  Response.Write('        <td class="bold" width="80px">ID:</td>');
  Response.Write('        <td><input type="text" name="login"></td>');
  Response.Write('      </tr>');
  Response.Write('      <tr>');
  Response.Write('        <td class="bold">Password:</td>');
  Response.Write('        <td><input type="password" name="password"></td>');
  Response.Write('      </tr>');
  Response.Write('    </table>');
  Response.Write('    <br>');
  Response.Write('    <center><button>Login</button></center>');
  Response.Write('    </form>');
  Response.Write('  </body>');
  Response.Write('</html>');
end;

procedure TUserPanel.Index;
begin
  if Session.Value['login'] = '' then
  begin
    OnLoginPage;
    Exit;
  end;
end;

procedure TUserPanel.CheckUpdate;
var PTSQL: TSQLPT;
    version: string;
begin
  version:= Request.QueryString.Item['ver'];
  PTSQL:= TSQLPT.Create;
  PTSQL.AntiSqlI(version);
  PTSQL.CreateConnection('ServerDB');
  PTSQL.ExecuteQuery('SELECT TOP 1 Url FROM Patches WHERE VersionID > '+version);
  if Strtoint(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('1');
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  version:= PTSQL.GetColumn('Url');
  Response.Write(version);

  PTSQL.CloseConnection;
  PTSQL.Free;
end;

initialization
  TAutoObjectFactory.Create(ComServer, TUserPanel, Class_UserPanel,
    ciMultiInstance, tmApartment);
end.
