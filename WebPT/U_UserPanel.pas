unit U_UserPanel;

{$WARN SYMBOL_PLATFORM OFF}

interface

uses
  ComObj, ActiveX, AspTlb, WebPT_TLB, StdVcl, U_SQL, Windows, SysUtils, Variants,   Classes, sHELLaPI;

type
  TUserPanel = class(TASPObject, IUserPanel)
  protected
    procedure OnEndPage; safecall;
    procedure OnStartPage(const AScriptingContext: IUnknown); safecall;
    procedure StartPage; safecall;
    procedure EndPage; safecall;
    procedure Index; safecall;
    procedure LoginPage; safecall;
    procedure RegisterPage; safecall;
    procedure HandleRegister; safecall;
    procedure HandleLogin; safecall;
    procedure UpdateVersion; safecall;
    procedure ActiveAccountPage; safecall;
    procedure ActivateAccountHandle; safecall;
    procedure ChangePassword; safecall;
    procedure ChangePasswordHandle; safecall;
    procedure ClanPage; safecall;
    procedure SetCharacterList(userid: string); safecall;
    function GetCharacterClanName(chname: string) : string; safecall;
    function GetCharacterJobCode(jobcode: integer) : string; safecall;
    function GetCharacterExpPercent(ExpMin, ExpCur, ExpMax: Int64) : string; safecall;
    function CheckActiveAccount(userid: string): Boolean; safecall;
    function GetCoinAccount(userid: string): string; safecall;
    function GenerateKeyRegister: string; safecall;
    procedure TimeServer; safecall;
    procedure OnMainPage; safecall;
    procedure EndMenu; safecall;
    procedure Menu; safecall;
    procedure SetNotifications; safecall;
    procedure OnListNoficationsPage; safecall;
    procedure OnNewTicketPage; safecall;
    procedure OnNewTicketPageHandle; safecall;
    procedure OnListTicketPage; safecall;
    procedure OnReplyTicketPage; safecall;
    procedure OnReplyTicketPageHandle; safecall;
    procedure OnVotePointsPage; safecall;
    procedure OnVotePointsPageHandle; safecall;
    procedure OnClanManagementPage; safecall;
    procedure OnSettingsPage; safecall;
    procedure OnSettingsPageHandle; safecall;
    procedure OnClanManagementPageHandle; safecall;
  end;

implementation

uses ComServ;

var ExpTable: array [0..140] of Int64 =
(
0,
1000,
2500,
5000,
9500,
17100,
29925,
51471,
87500,
140001,
212801,
306434,
416750,
537608,
672010,
833293,
1024950,
1250439,
1515533,
1824702,
2182343,
2592624,
3059296,
3591614,
4195005,
4874596,
5635033,
6480288,
7413449,
8443919,
9575404,
10820207,
12194373,
13706475,
15378665,
17224105,
19273774,
21548079,
24069204,
26861232,
29950274,
33364605,
37134805,
41293904,
45836233,
50878219,
56474823,
62687054,
69582630,
77236719,
85809995,
95334904,
105917079,
117673875,
130735675,
145312702,
161515069,
179523999,
199540925,
221789738,
246630189,
274252770,
304969080,
339125617,
377107687,
419532302,
466729685,
519236775,
577650912,
642636640,
715254581,
796078348,
886035202,
986157179,
1097592941,
1222169740,
1360886005,
1515346567,
1687338402,
1878851311,
2104313468,
2356831084,
2639650815,
2956408912,
3311177982,
3708519340,
4153541661,
4651966660,
5210202659,
6252243191,
7502691829,
9003230195,
10803876234,
12964651481,
15557581778,
18669098134,
22402917760,
26883501313,
32260201575,
38712241890,
46454690268,
55745628322,
66894753987,
80273704784,
96328445741,
115594134890,
138712961868,
166455554241,
199746665090,
239695998108,
287635197729,
345162237275,
414194684730,
497033621676,
596440346012,
715728415214,
858874098257,
1030648917908,
1236778701490,
1484134441788,
1706754608056,
1962767799264,
2257182969153,
2595760414525,
2985124476703,
3432893148208,
3947827120439,
4540001188504,
5221001366779,
5899731544460,
6666696645239,
7533367209120,
8512704946305,
9619356589324,
10869872945936,
12282956428907,
13879740764664,
15684107064070,
17723040982399,
19672575490462,
0
);

function CleanStringTag(const S: String): String;
begin
  Result := S;
  Result:= StringReplace(Result,chr($27),'[aspaspt]',[rfReplaceAll,rfIgnoreCase]);
  Result:= StringReplace(Result,'/','[barpt]',[rfReplaceAll,rfIgnoreCase]);
  Result:= StringReplace(Result,'.','[dotpt]',[rfReplaceAll,rfIgnoreCase]);
  Result:= StringReplace(Result,'-','[minuspt]',[rfReplaceAll,rfIgnoreCase]);
end;

function PutStringTag(const S: String): String;
begin
  Result := S;
  Result:= StringReplace(Result,'[aspaspt]', chr($27),[rfReplaceAll,rfIgnoreCase]);
  Result:= StringReplace(Result,'[barpt]','/',[rfReplaceAll,rfIgnoreCase]);
  Result:= StringReplace(Result,'[dotpt]','.',[rfReplaceAll,rfIgnoreCase]);
  Result:= StringReplace(Result,'[minuspt]','-',[rfReplaceAll,rfIgnoreCase]);
end;


procedure TUserPanel.OnEndPage;
begin
  inherited OnEndPage;
end;

procedure TUserPanel.OnStartPage(const AScriptingContext: IUnknown);
begin
  inherited OnStartPage(AScriptingContext);
end;

procedure TUserPanel.Index;
var loginpt: string;
begin
  StartPage;

  loginpt:= Session.Value['loginpt'];

  if VarToStr(Request.QueryString.Item['pag']) = 'login2' then
  begin
    if loginpt = '' then
      HandleLogin;
  end;

  // Quando está logado
  loginpt:= Session.Value['loginpt'];
  if loginpt <> '' then
  begin
    if VarToStr(Request.QueryString.Item['pag']) = 'logout' then
    begin
      Session.Abandon;
      LoginPage;
      Exit;
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'login2' then
    begin
      Menu();
      OnMainPage();
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'listnotifications' then
    begin
      Menu();
      OnListNoficationsPage();
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'newticket' then
    begin
      Menu();
      OnNewTicketPage();
      OnNewTicketPageHandle();
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'settings' then
    begin
      Menu();
      OnSettingsPage();
      OnSettingsPageHandle();
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'ticketlist' then
    begin
      Menu();
      OnListTicketPage();
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'votepoints' then
    begin
      Menu();
   //  OnVotePointsPage();
   //   OnVotePointsPageHandle();
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'replyticket' then
    begin
      Menu();
      OnReplyTicketPage();
      OnReplyTicketPageHandle();
    end
    else if VarToStr(Request.QueryString.Item['pag']) = 'clanmanagement' then
    begin
      Menu();
      OnClanManagementPage();
      OnClanManagementPageHandle();
    end
    else begin
      Menu();
      OnMainPage();
    end;

  end;

  if VarToStr(Request.QueryString.Item['pag']) = 'register' then
  begin
    RegisterPage;
    Exit;
  end;
  if VarToStr(Request.QueryString.Item['pag']) = 'register2' then
  begin
    HandleRegister;
    Exit;
  end;
  if loginpt = '' then
  begin
    LoginPage;
    Exit;
  end;

  EndPage;

end;

procedure TUserPanel.LoginPage;
begin
  Response.Write('    <div class="container">');
  Response.Write('        <div class="row">');
  Response.Write('            <div class="col-md-4 col-md-offset-4">');
  Response.Write('<br>');
  Response.Write('                <img src="imgs/logo.png" style="margin-top: 50px">');
  Response.Write('                <div class="login-panel panel panel-default" style="margin-top: 50px">');
  Response.Write('                    <div class="panel-heading">');
  Response.Write('                        <h3 class="panel-title">Please Sign In</h3>');
  Response.Write('                    </div>');
  Response.Write('                    <div class="panel-body">');
  Response.Write('                        <form name=loginpt method=post action="?pag=login2" role="form">');
  Response.Write('                            <fieldset>');
  Response.Write('                                <div class="form-group">');
  Response.Write('                                    <input class="form-control" placeholder="Account" name="login" type="text" autofocus>');
  Response.Write('                                </div>');
  Response.Write('                                <div class="form-group">');
  Response.Write('                                    <input class="form-control" placeholder="Password" name="passwd" type="password" value="">');
  Response.Write('                                </div>');
  Response.Write('                                <!-- Change this to a button or input when using this as a form -->');
  Response.Write('                                <button class="btn btn-lg btn-success btn-block">Login</button>');
  Response.Write('<a href="?pag=register" class="btn btn-lg btn-default btn-block">Register Account</a>');
  Response.Write('                            </fieldset>');
  Response.Write('                        </form>');
  Response.Write('                    </div>');
  Response.Write('                </div>');
  Response.Write('            </div>');
  Response.Write('        </div>');
  Response.Write('    </div>');
end;

procedure TUserPanel.UpdateVersion;
var PTSQL: TSQLPT;
    version, ip: string;
begin
  version:= Request.QueryString.Item['ver'];

  // Check Maintenance
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ServerDB');
  if version = '' then
  begin
    Response.Write('0');
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  if StrToIntDef(version, 0) < 1285 then
  begin
    Response.Write('3');
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  PTSQL.AntiSqlI(version);
  PTSQL.ExecuteQuery('SELECT TOP 1 Url FROM Patches WHERE VersionID > '+version);
  if Strtoint(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    PTSQL.CloseRS;
    PTSQL.ExecuteQuery('SELECT IP FROM Maintenance WHERE Mode=3');
    if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
    begin
      ip:= PTSQL.GetColumn('IP');
      if ip = '1' then
      begin
        Response.Write('2');
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end;
    end;
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

procedure TUserPanel.RegisterPage;
var list: TStringList;
begin
list:= TStringList.Create;
list.LoadFromFile('C:\inetpub\wwwroot\panel\termspanel.txt');

Response.Write('<div class="container">');
Response.Write('        <div class="row">');
Response.Write('            <div class="col-md-8 col-md-offset-2">');
Response.Write('                <div class="login-panel panel panel-default" style="margin-top: 100px">');
Response.Write('                    <div class="panel-heading">');
Response.Write('                        <h3 class="panel-title">Register new Account</h3>');
Response.Write('                    </div>');
Response.Write('                    <div class="panel-body">');
Response.Write('                        <form name=registerpt method=post action="?pag=register2" role="form">');
Response.Write('                            <fieldset>');
Response.Write('                                <div class="form-group">');
Response.Write('                                    <label>ID</label>');
Response.Write('                                    <input name="login" class="form-control">');
Response.Write('                                </div>');
Response.Write('                                <div class="form-group">');
Response.Write('                                    <label>Password</label>');
Response.Write('                                    <input name="passwd" type="password" class="form-control">');
Response.Write('                                </div>');
Response.Write('                                <div class="form-group">');
Response.Write('                                    <label>Repeat Password</label>');
Response.Write('                                    <input name="passwd2" type="password" class="form-control">');
Response.Write('                                </div>');
Response.Write('                                <div class="form-group">');
Response.Write('                                    <label>Email</label>');
Response.Write('                                    <input name="email" class="form-control">');
Response.Write('                                </div>');
Response.Write('                                <div class="form-group">');
Response.Write('                                    <label>Terms</label>');
Response.Write('                                    <textarea class="form-control" rows="3"readonly>'+list.Text+'</textarea>');
Response.Write('                                </div>');
Response.Write('                                <!-- Change this to a button or input when using this as a form -->');
Response.Write('                                <button class="btn btn-lg btn-success btn-block">I am 16+ years old and I accept the terms of Registration.</button>');
Response.Write('                            </fieldset>');
Response.Write('                        </form>');
Response.Write('                    </div>');
Response.Write('                </div>');
Response.Write('');
Response.Write('                <center><a href="?">Back to Login</a></center>');
Response.Write('            </div>');
Response.Write('        </div>');
Response.Write('    </div>');
list.Free;
end;

procedure TUserPanel.HandleRegister;
var PTSQL: TSQLPT;
    passwd, passwd2, userid, email, keycode, emailblocked, query: string;
    blatcmd, msg: string;
begin
  PTSQL:= TSQLPT.Create;
  userid:= Request.Form.Item['login'];
  passwd:= Request.Form.Item['passwd'];
  passwd2:= Request.Form.Item['passwd2'];
  email:= Request.Form.Item['email'];
  PTSQL.AntiSqlI(userid);
  PTSQL.AntiSqlI(passwd);
  PTSQL.AntiSqlI(passwd2);

  email:= StringReplace(email,chr($27),'',[rfReplaceAll,rfIgnoreCase]);
                          
  if passwd <> passwd2 then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('Password not match! try again.</div>');
    PTSQL.Free;
    RegisterPage;
    Exit;
  end;
  if (Length(passwd) < 4) or (Length(passwd2) > 15) then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('Password must be min 4 and max 15 characters.</div>');
    PTSQL.Free;
    RegisterPage;
    Exit;
  end;

  // Valid Email?
  if Pos('@',email) <= 0 then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('Email must be valid!</div>');
    PTSQL.Free;
    RegisterPage;
    Exit;
  end;

  // Email Blocked List
  PTSQL.CreateConnection('ServerDB');
  PTSQL.ExecuteQuery('SELECT email FROM BlockedEmailList');

  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    repeat
      emailblocked:= PTSQL.GetColumn('email');
      if Pos(emailblocked,email) > 0 then
      begin
        Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
        Response.Write('This email is not real! Try again.</div>');
        PTSQL.CloseConnection;
        PTSQL.Free;
        RegisterPage;
        Exit;
      end;
      PTSQL.NextRow;
    until PTSQL.GetRS.EOF;
  end;
  PTSQL.CloseRS;

  // Account exists?
  PTSQL.CloseConnection;
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT AccountName FROM UserInfo WHERE AccountName='+quotedstr(userid);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('This account is already registered!, Try another.</div>');
    PTSQL.CloseConnection;
    PTSQL.Free;
    RegisterPage;
    Exit;
  end;
  PTSQL.CloseRS;
  query:= 'SELECT Email FROM UserInfo WHERE Email='+quotedstr(email);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('This email is already registered!, Try another.</div>');
    PTSQL.CloseConnection;
    PTSQL.Free;
    RegisterPage;
    Exit;
  end;
  PTSQL.CloseRS;

  keycode:= GenerateKeyRegister;
  query:= 'INSERT INTO UserInfo VALUES('+quotedstr(userid)+', '+quotedstr(passwd)+',getdate(), 1,'+quotedstr(keycode)+',0,0,0,'+quotedstr(email)+',0,0,0,0,0,0)';
  PTSQL.ExecuteQuery(query, true);

  // Email
  msg:= 'Hi|Welcome to FortressPT! ^_^|ID: '+userid+'|Code: '+keycode+'|'+
  'Activate your account in User Panel with this code.| |FortressPT, From Players, for Players!';
  blatcmd:= 'C:\blat\blat -body "'+msg+'" -to '+email+' -subject "Activate account!"';
 // WinExec(PChar(blatcmd), SW_HIDE);

  Response.Write('<div class="alert alert-success alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
  Response.Write('Account register successful!</div>');

  PTSQL.CloseConnection;
  PTSQL.Free;
  LoginPage;
end;

function GetRandomInt64() : int64; safecall;
begin
   Int64Rec(result).Words[0] := Random(High(Word));
   Int64Rec(result).Words[1] := Random(High(Word));
   Int64Rec(result).Words[2] := Random(High(Word));
   Int64Rec(result).Words[3] := Random(High(Word));
end;

function TUserPanel.GenerateKeyRegister: string;
begin
  Result:= IntToHex(GetRandomInt64, 16);
end;

procedure TUserPanel.HandleLogin;
var PTSQL: TSQLPT;
    passwd, userid, query, active, banned: string;
begin
  PTSQL:= TSQLPT.Create;
  userid:= Request.Form.Item['login'];
  passwd:= Request.Form.Item['passwd'];
  PTSQL.AntiSqlI(userid);
  PTSQL.AntiSqlI(passwd);
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT AccountName, Password, Active, Banned FROM UserInfo WHERE (AccountName='+quotedstr(userid)+') AND (Password='+quotedstr(passwd)+')';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('Account incorrect!</div>');
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  active:= PTSQL.GetColumn('Active');
  banned:= PTSQL.GetColumn('Banned');
  PTSQL.CloseRS;
  //if banned = '1' then
  //begin
  //  Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
  //  Response.Write('Account is banned!</div>');
  //  PTSQL.CloseConnection;
  //  PTSQL.Free;
  //  Exit;
  //end;
  Session.Set_Value('loginpt',userid);
  if active = '0' then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('Account is not activated!</div>');
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
end;

procedure TUserPanel.ActivateAccountHandle;
var PTSQL: TSQLPT;
    code, userid, query: string;
begin
  PTSQL:= TSQLPT.Create;
  userid:= Session.Value['loginpt'];
  code:= Request.Form.Item['code'];
  PTSQL.AntiSqlI(code);
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT ActiveCode FROM UserInfo WHERE (AccountName='+quotedstr(userid)+') AND (ActiveCode='+quotedstr(code)+')';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
    Response.Write('Incorrect activated Code!</div>');
    PTSQL.CloseConnection;
    PTSQL.Free;
    ActiveAccountPage;
    Exit;
  end;
  PTSQL.CloseRS;
  query:= 'UPDATE UserInfo SET Active=1 WHERE AccountName='+quotedstr(userid);
  PTSQL.ExecuteQuery(query,true);
  PTSQL.CloseConnection;
  PTSQL.Free;
  Response.Write('<div class="alert alert-success alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;z-index: 10001;">');
  Response.Write('Account activated successful!</div>');
end;

procedure TUserPanel.ActiveAccountPage;
begin
Response.Write('<div class="panel panel-default">');
Response.Write('                        <div class="panel-heading">');
Response.Write('                            Users List');
Response.Write('                        </div>');
Response.Write('                        <!-- /.panel-heading -->');
Response.Write('<div class="panel-body">');
Response.Write('                                <div class="form-group">');
Response.Write('                                    <form name=loginpt method=post action="?pag=activate">');
Response.Write('                                        <label>Code</label>');
Response.Write('                                        <input name="code" type="text" maxlength=32 class="form-control"><br>');
Response.Write('                                        <button type="submit" class="btn btn-default">Active</button>');
Response.Write('                                    </form>');
Response.Write('                                </div>');
Response.Write('                            </div>');
Response.Write('                        </div>');
Response.Write('                        </div>');
end;

function TUserPanel.CheckActiveAccount(userid: string): Boolean;
var PTSQL: TSQLPT;
  query, active: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT Active FROM UserInfo WHERE AccountName='+quotedstr(userid);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    PTSQL.CloseConnection;
    PTSQL.Free;
    Result:= False;
    Exit;
  end;
  active:= PTSQL.GetColumn('Active');
  Result:= False;
  if active = '1' then
    Result:= True;
  PTSQL.CloseConnection;
  PTSQL.Free;
end;

procedure TUserPanel.TimeServer;
begin

end;

procedure TUserPanel.EndPage;
begin
Response.Write('                    </div>');
Response.Write('                </div>');
Response.Write('            </div>');
Response.Write('         </div>');
Response.Write('    </div>');
Response.Write('    <!-- /#wrapper -->');
Response.Write('');
Response.Write('    <!-- jQuery -->');
Response.Write('    <script src="bower_components/jquery/dist/jquery.min.js"></script>');
Response.Write('');
Response.Write('    <!-- Bootstrap Core JavaScript -->');
Response.Write('    <script src="bower_components/bootstrap/dist/js/bootstrap.min.js"></script>');
Response.Write('');
Response.Write('    <!-- Metis Menu Plugin JavaScript -->');
Response.Write('    <script src="bower_components/metisMenu/dist/metisMenu.min.js"></script>');
Response.Write('');
Response.Write('    <!-- Custom Theme JavaScript -->');
Response.Write('    <script src="dist/js/sb-admin-2.js"></script>');
Response.Write('');
Response.Write('    <!-- DataTables JavaScript -->');
Response.Write('    <script src="bower_components/datatables/media/js/jquery.dataTables.min.js"></script>');
Response.Write('    <script src="bower_components/datatables-plugins/integration/bootstrap/3/dataTables.bootstrap.min.js"></script>');
Response.Write('');
Response.Write('    <!-- Page-Level Demo Scripts - Tables - Use for reference -->');
Response.Write('    <script>');
Response.Write('    $(document).ready(function() {');
Response.Write('        $(''#dataTables-example'').DataTable({');
Response.Write('                responsive: true');
Response.Write('        });');
Response.Write('    });');
Response.Write('    </script>');
Response.Write('<script>');
Response.Write('$(".notifications").click(function() {');
Response.Write('    $(''#newnot'').hide();');
Response.Write('});');
Response.Write('</script>');
Response.Write('</body>');
Response.Write('');
Response.Write('</html>');
end;

procedure TUserPanel.StartPage;
begin
Response.Write('<!DOCTYPE html>');
Response.Write('<html lang="en">');
Response.Write('');
Response.Write('<head>');
Response.Write('');
Response.Write('    <meta charset="utf-8">');
Response.Write('    <meta http-equiv="X-UA-Compatible" content="IE=edge">');
Response.Write('    <meta name="viewport" content="width=device-width, initial-scale=1">');
Response.Write('    <meta name="description" content="">');
Response.Write('    <meta name="author" content="">');
Response.Write('');
Response.Write('    <title>Account Management</title>');
Response.Write('');
Response.Write('    <!-- Bootstrap Core CSS -->');
Response.Write('    <link href="bower_components/bootstrap/dist/css/bootstrap.min.css" rel="stylesheet">');
Response.Write('');
Response.Write('    <!-- MetisMenu CSS -->');
Response.Write('    <link href="bower_components/metisMenu/dist/metisMenu.min.css" rel="stylesheet">');
Response.Write('');
Response.Write('    <!-- Custom CSS -->');
Response.Write('    <link href="dist/css/sb-admin-2.css" rel="stylesheet">');
Response.Write('');
Response.Write('    <!-- DataTables CSS -->');
Response.Write('    <link href="bower_components/datatables-plugins/integration/bootstrap/3/dataTables.bootstrap.css" rel="stylesheet">');
Response.Write('');
Response.Write('    <!-- DataTables Responsive CSS -->');
Response.Write('    <link href="bower_components/datatables-responsive/css/dataTables.responsive.css" rel="stylesheet">');
Response.Write('    <!-- Custom Fonts -->');
Response.Write('    <link href="bower_components/font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">');
Response.Write('</head>');
Response.Write('');
Response.Write('<body>');
Response.Write('');
end;

procedure TUserPanel.ClanPage;
var PTSQL: TSQLPT;
    userid, loginpt, query, iconid, clname, note, messagelogin: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ClanDB');

  loginpt:= Session.Value['loginpt'];

  query:= 'SELECT IconID, ClanName, Note, MessageLogin FROM ClanList WHERE AccountName='+quotedstr(loginpt);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

end;

procedure TUserPanel.ChangePassword;
begin
  Response.Write('    <form name=changepw method=post action="?pag=changepw">');
  Response.Write('    <table align="center" border="0" cellpadding="0" cellspacing="0" style="margin-top: 50px">');
  Response.Write('      <tr>');
  Response.Write('        <td class="bold" width="80px">Current Password:</td>');
  Response.Write('        <td><input type="text" name="oldpasswd"></td>');
  Response.Write('      </tr>');
  Response.Write('      <tr>');
  Response.Write('        <td class="bold">New Password:</td>');
  Response.Write('        <td><input type="password" name="newpasswd"></td>');
  Response.Write('      </tr>');
  Response.Write('      <tr>');
  Response.Write('        <td class="bold">Repeat Password:</td>');
  Response.Write('        <td><input type="password" name="newpasswd"></td>');
  Response.Write('      </tr>');
  Response.Write('    </table>');
  Response.Write('    <br>');
  Response.Write('    <center><button>Login</button></center>');
  Response.Write('    </form>');
end;

procedure TUserPanel.ChangePasswordHandle;
begin

end;

function TUserPanel.GetCoinAccount(userid: string): string;
var PTSQL: TSQLPT;
  query, active: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT Coins FROM UserInfo WHERE AccountName='+quotedstr(userid);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    PTSQL.CloseConnection;
    PTSQL.Free;
    Result:= '0';
    Exit;
  end;
  Result:= PTSQL.GetColumn('Coins');
  PTSQL.CloseConnection;
  PTSQL.Free;
end;

function TUserPanel.GetCharacterClanName(chname: string): string;
begin

end;

procedure TUserPanel.SetCharacterList(userid: string);
var chname, clname, lvl, clanid, jobcode, query, expp, exp : string;
    expnum: Int64;
    num: Integer;
    PTSQL, PTSQLC: TSQLPT;
begin
  // Users
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT * FROM CharacterInfo WHERE AccountName='+quotedstr(userid)+' ORDER BY Experience DESC';
  PTSQL.ExecuteQuery(query);

  PTSQLC:= TSQLPT.Create;
  PTSQLC.CreateConnection('ClanDB');

  Response.Write('<div class="table-responsive">');
  Response.Write('                                <table class="table table-hover">');
  Response.Write('                                    <thead>');
  Response.Write('                                        <tr>');
  Response.Write('                                            <th>#</th>');
  Response.Write('                                            <th>Character Name</th>');
  Response.Write('                                            <th>Clan</th>');
  Response.Write('                                            <th>Class</th>');
  Response.Write('                                            <th>Level</th>');
  Response.Write('                                            <th>Experience</th>');
  Response.Write('                                        </tr>');
  Response.Write('                                    </thead>');
  Response.Write('                                    <tbody>');

  num:= 1;

  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    repeat
      clanid:= PTSQL.GetColumn('ClanID');
      PTSQLC.ExecuteQuery('SELECT COUNT(*) FROM ClanList WHERE ID='+clanid+'');

      chname:= PTSQL.GetColumn('Name');
      lvl:= PTSQL.GetColumn('Level');
      jobcode:= GetCharacterJobCode(StrToInt(VarToStr(PTSQL.GetColumn('JobCode'))));
      Response.Write('			<tr>');
      Response.Write('<td>'+IntToStr(num)+'</td>');
      Response.Write('				<td>'+chname+'</td>');
      Response.Write('<td>');
      if PTSQLC.GetColumn(0) > 0 then begin
        PTSQLC.CloseRS();
        PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');

        Response.Write('<img src="../ClanImage/');
        Response.Write(PTSQLC.GetColumn('IconID'));
        Response.Write('.bmp"> ');
        Response.Write(PTSQLC.GetColumn('ClanName'));
      end
      else
        Response.Write('No Clan');
      Response.Write('</td>');
      Response.Write('				<td>'+jobcode+'</td>');
      Response.Write('				<td>'+lvl+'</td>');
      // Calculate Exp
      exp:= PTSQL.GetColumn('Experience');
      expnum:= StrToInt64(exp);
      expp:= GetCharacterExpPercent(exptable[StrToInt(lvl)-1], expnum, exptable[StrToInt(lvl)]);

      Response.Write('				<td>'+expp+'%</td>');
      Response.Write('			</tr>');

      num:= num + 1;

      PTSQLC.NextRow;
      PTSQLC.CloseRS();

      //Next row
      PTSQL.NextRow;

    until PTSQL.GetRS.EOF;

    PTSQLC.CloseConnection();
    PTSQLC.Free();
  end;

  PTSQL.CloseConnection;
  PTSQL.Free;
  Response.Write('</tbody>');
  Response.Write('                                </table>');
  Response.Write('                            </div>');
end;

function TUserPanel.GetCharacterJobCode(jobcode: integer): string;
begin
  Result:= 'None';
  case jobcode of
    1:
      Result:= 'Fighter';
    2:
      Result:= 'Mechanician';
    3:
      Result:= 'Archer';
    4:
      Result:= 'Pikeman';
    5:
      Result:= 'Atalanta';
    6:
      Result:= 'Knight';
    7:
      Result:= 'Magician';
    8:
      Result:= 'Priestess';
    9:
      Result:= 'Assassin';
    10:
      Result:= 'Shaman';
  end;
end;

function TUserPanel.GetCharacterExpPercent(ExpMin, ExpCur,
  ExpMax: Int64): string;
var ExpSize, ExpSizeNow: Int64;
    ExpPercent: Integer;
    Exp1, Exp2: Single;
begin
  ExpSize:= (ExpMax - ExpMin);
  ExpSizeNow:= ExpCur - ExpMin;
  Exp1:= ExpSize;
  Exp2:= ExpSizeNow;
  Exp1:= (10000.0 / Exp1 * Exp2);
  Result:= FloatToStrF(Exp1 / 100 , ffNumber, 3, 1);
end;

procedure TUserPanel.OnMainPage;
var loginpt: string;
begin
    loginpt:= Session.Value['loginpt'];

    Response.Write('<h1 class="page-header">Main Page</h1>');

    if VarToStr(Request.QueryString.Item['pag']) = 'activate' then
    begin
      ActivateAccountHandle;
      Exit;
    end;

    if CheckActiveAccount(loginpt) = False then
    begin
      ActiveAccountPage;
      Exit;
    end;

    SetCharacterList(loginpt);
end;

procedure TUserPanel.EndMenu;
begin

end;

procedure TUserPanel.Menu;
var PTSQL, PTSQLC : TSQLPT;
    loginpt , IDCharacter, dpoint, coin: string;
begin

    loginpt:= Session.Value['loginpt'];

    PTSQL:= TSQLPT.Create;
    PTSQL.CreateConnection('UserDB');

    PTSQLC:= TSQLPT.Create;
    PTSQLC.CreateConnection('ClanDB');



    Response.Write(' <!-- Navigation -->');
    Response.Write('        <nav class="navbar navbar-default navbar-static-top" role="navigation" style="margin-bottom: 0">');
    Response.Write('            <div class="navbar-header">');
    Response.Write('                <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">');
    Response.Write('                    <span class="sr-only">Toggle navigation</span>');
    Response.Write('                    <span class="icon-bar"></span>');
    Response.Write('                    <span class="icon-bar"></span>');
    Response.Write('                    <span class="icon-bar"></span>');
    Response.Write('                </button>');
    Response.Write('                <a class="navbar-brand" href="?pag=login2">User Management</a>');
    Response.Write('            </div>');
    Response.Write('            <!-- /.navbar-header -->');
    Response.Write('');
    Response.Write('            <ul class="nav navbar-top-links navbar-right">');

    PTSQL.ExecuteQuery('SELECT * FROM UserInfo WHERE AccountName='+QuotedStr(loginpt));
    dpoint:= PTSQL.GetColumn('DivulgePoints');
    coin:= PTSQL.GetColumn('Coins');


    Response.Write('<li><span style="font-style: italic; color: #6B6B6B;"><b>Credits:</b> '+coin+' </span> / <span style="font-style: italic; color: #6B6B6B;"><b>Divulgation Points:</b> '+dpoint+' </span> / <span style="font-style: italic; color: #6B6B6B;">Your warning state is ');
    IDCharacter:= PTSQL.GetColumn('ID');

    Response.Write(PTSQL.GetColumn('WarningLevel'));
    PTSQL.CloseRS();
    Response.Write('% </span></li>');
    Response.Write('                <li class="dropdown">');
    Response.Write('                    <a class="dropdown-toggle notifications" data-toggle="dropdown" href="#">');

    PTSQL.ExecuteQuery('SELECT COUNT(*) FROM NotificationData WHERE (ToAccount='+Chr(39)+''+loginpt+''+Chr(39)+') AND (Checked=0)');
    if PTSQL.GetColumn(0) > 0 then
      Response.Write('<i style="font-size: 7px; color: rgb(255, 156, 0);" class="fa fa-circle" id="newnot"></i>');
    Response.Write('                        <i class="fa fa-envelope fa-fw"></i>  <i class="fa fa-caret-down"></i>');
    Response.Write('                    </a>');
    Response.Write('                    <ul class="dropdown-menu dropdown-messages">');
    SetNotifications();
    Response.Write('                        <li class="divider"></li>');
    Response.Write('                        <li>');
    Response.Write('                            <a class="text-center" href="?pag=listnotifications">');
    Response.Write('                                <strong>Read All Messages</strong>');
    Response.Write('                                <i class="fa fa-angle-right"></i>');
    Response.Write('                            </a>');
    Response.Write('                        </li>');
    Response.Write('                    </ul>');
    Response.Write('                    <!-- /.dropdown-messages -->');
    Response.Write('                </li>');
    Response.Write('                <li class="dropdown">');
    Response.Write('                    <a class="dropdown-toggle" data-toggle="dropdown" href="#">');
    Response.Write('                        <i class="fa fa-user fa-fw"></i>  <i class="fa fa-caret-down"></i>');
    Response.Write('                    </a>');
    Response.Write('                    <ul class="dropdown-menu dropdown-user">');
    Response.Write('                        <li><a href="?pag=settings"><i class="fa fa-gear fa-fw"></i> Settings</a>');
    Response.Write('                        </li>');
    Response.Write('                        <li class="divider"></li>');
    Response.Write('                        <li><a href="?pag=logout"><i class="fa fa-sign-out fa-fw"></i> Logout</a>');
    Response.Write('                        </li>');
    Response.Write('                    </ul>');
    Response.Write('                    <!-- /.dropdown-user -->');
    Response.Write('                </li>');
    Response.Write('                <!-- /.dropdown -->');
    Response.Write('            </ul>');
    Response.Write('            <div class="navbar-default sidebar" role="navigation">');
    Response.Write('                <div class="sidebar-nav navbar-collapse">');
    Response.Write('                    <ul class="nav" id="side-menu">');
    Response.Write('                        <li>');
    Response.Write('                            <a href="?pag=login2"><i class="fa fa-dashboard fa-fw"></i> Main</a>');
    Response.Write('                        </li>');
    PTSQLC.ExecuteQuery('SELECT ID FROM ClanList WHERE AccountName='+QuotedStr(loginpt));
    if PTSQLC.RecordCount > 0 then begin
      Response.Write('                        <li>');
      Response.Write('                            <a href="?pag=clanmanagement"><i class="fa fa-bookmark fa-fw"></i> Clan Management</a>');
      Response.Write('                        </li>');
    end;
    PTSQLC.CloseRS();

    Response.Write('                        <li>');
    Response.Write('                            <a href="#"><i class="fa fa-support fa-fw"></i> Support Tickets<span class="fa arrow"></span></a>');
    Response.Write('                            <ul class="nav nav-second-level">');
    Response.Write('                                <li>');
    Response.Write('                                    <a href="?pag=ticketlist">Ticket List</a>');
    Response.Write('                                </li>');
    Response.Write('                                <li>');
    Response.Write('                                    <a href="?pag=newticket">New Ticket</a>');
    Response.Write('                                </li>');
    Response.Write('                            </ul>');
    Response.Write('                            <!-- /.nav-second-level -->');
    Response.Write('                        </li>');
    Response.Write('                        <li>');
    Response.Write('                            <a href="?pag=listnotifications"><i class="fa fa-bell fa-fw"></i> Notifications</a>');
    Response.Write('                        </li>');
    Response.Write('                        <li>');
    Response.Write('                            <a href="http://www.fortresspt.net/donate.php?code='+IDCharacter+'"><i class="fa fa-certificate fa-fw"></i> Donate</a>');
    Response.Write('                            <!-- /.nav-second-level -->');
    Response.Write('                        </li>');
    Response.Write('                        <li>');
    Response.Write('                            <a href="?pag=votepoints"><i class="fa  fa-star  fa-fw"></i> Vote Points</a>');
    Response.Write('                        </li>');
    Response.Write('                    </ul>');
    Response.Write('                </div>');
    Response.Write('                <!-- /.sidebar-collapse -->');
    Response.Write('            </div>');
    Response.Write('            <!-- /.navbar-static-side -->');
    Response.Write('        </nav>');
    Response.Write('    <div id="wrapper">');
    Response.Write('        <div id="page-wrapper">');
    Response.Write('            <div class="container-fluid">');
    Response.Write('                <div class="row">');
    Response.Write('                    <div class="col-lg-12">');

    PTSQL.CloseConnection();
    PTSQL.Free();

    PTSQLC.CloseConnection();
    PTSQLC.Free();
end;

procedure TUserPanel.SetNotifications;
var PTSQL : TSQLPT;
    loginpt, fromaccount, date, message : string;
    num, count : Integer;
begin
    loginpt:= Session.Value['loginpt'];

    PTSQL:= TSQLPT.Create;
    PTSQL.CreateConnection('UserDB');

    PTSQL.ExecuteQuery('SELECT COUNT(*) FROM NotificationData WHERE ToAccount='+Chr(39)+''+loginpt+''+Chr(39)+'');

    if PTSQL.GetColumn(0) > 0 then
    begin
      PTSQL.CloseRS();
      PTSQL.ExecuteQuery('SELECT TOP 3 * FROM NotificationData WHERE ToAccount='+Chr(39)+''+loginpt+''+Chr(39)+' ORDER BY ID DESC');

      num:= 1;

      repeat
        fromaccount:= PTSQL.GetColumn('FromAccount');
        date:= PTSQL.GetColumn('Date');
        message:= PTSQL.GetColumn('Message');
        count:= PTSQL.RecordCount();

        Response.Write('<li>');
        Response.Write('<a href="?pag=listnotifications">');
        Response.Write('                                <div>');
        Response.Write('                                    <strong>'+fromaccount+'</strong>');
        Response.Write('                                    <span class="pull-right text-muted">');
        Response.Write('                                        <em>'+date+'</em>');
        Response.Write('                                    </span>');
        Response.Write('                                </div>');
        Response.Write('                                <div>'+message+'</div>');
        Response.Write('</a>');
        Response.Write('                        </li>');

        if num <> count then
          Response.Write('<li class="divider"></li>');

        num:= num + 1;

        PTSQL.NextRow;

      until PTSQL.GetRS.EOF;
    end
    else
    begin
      Response.Write('<li>');
      Response.Write('<span style="margin: 10px"><center>No New Notifications</center></span>');
      Response.Write('</li>');
    end;

    PTSQL.CloseConnection();
    PTSQL.Free();
end;

procedure TUserPanel.OnListNoficationsPage;
var PTSQL: TSQLPT;
    loginpt : string;
    num : Integer;
begin

  loginpt:= Session.Value['loginpt'];

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');

  Response.Write('<h1 class="page-header">Notifications</h1>');

  Response.Write('<div class="table-responsive">');
  Response.Write('                                <table class="table">');
  Response.Write('                                    <thead>');
  Response.Write('                                        <tr>');
  Response.Write('                                            <th>#</th>');
  Response.Write('                                            <th>From</th>');
  Response.Write('                                            <th>Message</th>');
  Response.Write('                                            <th>Date</th>');
  Response.Write('                                        </tr>');
  Response.Write('                                    </thead>');
  Response.Write('                                    <tbody>');

  PTSQL.ExecuteQuery('SELECT COUNT(*) FROM NotificationData WHERE ToAccount='+Chr(39)+''+loginpt+''+Chr(39)+'');

  if PTSQL.GetColumn(0) > 0 then begin

    PTSQL.CloseRS();
    PTSQL.ExecuteQuery('SELECT * FROM NotificationData WHERE ToAccount='+Chr(39)+''+loginpt+''+Chr(39)+' ORDER BY ID DESC');

    num:= 1;

    repeat
      Response.Write('                                        <tr>');
      Response.Write('                                            <td>');
      Response.Write(num);
      Response.Write('                                            </td>');
      Response.Write('                                            <td>');
      Response.Write(PTSQL.GetColumn('FromAccount'));
      Response.Write('                                            </td>');
      Response.Write('                                            <td>');
      Response.Write(PTSQL.GetColumn('Message'));
      Response.Write('                                            </td>');
      Response.Write('                                            <td>');
      Response.Write(PTSQL.GetColumn('Date'));
      Response.Write('                                            </td>');
      Response.Write('                                        </tr>');

      num:= num + 1;

      PTSQL.NextRow;

    until PTSQL.GetRS.EOF;

    PTSQL.CloseRS();
    PTSQL.ExecuteQuery('UPDATE NotificationData SET Checked=1 WHERE ToAccount='+Chr(39)+''+loginpt+''+Chr(39)+'',true);

  end;

  Response.Write('                                    </tbody>');
  Response.Write('                                </table>');
  Response.Write('                            </div>');

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TUserPanel.OnNewTicketPage;
begin
    Response.Write('<script type="text/javascript" src="js/nicEdit.js"></script>');
    Response.Write('<script type="text/javascript">');
    Response.Write('	bkLib.onDomLoaded(function() { nicEditors.allTextAreas() });');
    Response.Write('</script>');
    Response.Write('<h1 class="page-header">New Ticket</h1>');
    Response.Write('<div class="panel panel-default">');
    Response.Write('                            <div class="panel-heading">');
    Response.Write('                                New Ticket');
    Response.Write('                            </div>');
    Response.Write('                            <div class="panel-body">');
    Response.Write('                                Ticket System disabled! <a href="http://www.fortresspt.net/forum/index.php#c3">Click Here and post on forum!</a>');
    Response.Write('                            </div>');
    Response.Write('                        </div>  ');
end;

procedure TUserPanel.OnNewTicketPageHandle;
var title, category, description, loginpt, ticketid : string;
    PTSQL, PTSQLC : TSQLPT;
    typeticket : Integer;
    myDateTime : TDateTime;
    st: TStringList;
begin
  IF Request.Form.Count <= 0 then Exit;

  myDateTime := Now();
  loginpt:= Session.Value['loginpt'];

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');

  PTSQLC:= TSQLPT.Create;
  PTSQLC.CreateConnection('UserDB');

  title:= Request.Form.Item['title'];
  category:= Request.Form.Item['category'];
  description:= Request.Form.Item['description'];

  PTSQL.AntiSqlI(title);
  PTSQL.AntiSqlI(category);

  description:= CleanStringTag(description);

  if category = 'Account' then
    typeticket:= 0
  else if category = 'Technical' then
    typeticket:= 1
  else if category = 'Lost & Found' then
    typeticket:= 2
  else if category = 'Report User' then
    typeticket:= 3
  else if category = 'Request Unban' then
    typeticket:= 4;

  PTSQL.ExecuteQuery('SELECT TOP 3 * FROM TicketList WHERE Type='+IntToStr(typeticket)+' AND State=0 AND Author='+QuotedStr(loginpt)+'');

  if PTSQL.RecordCount = 3 then begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
    Response.Write('You have reached the max number of tickes allowed!</div>');
    Exit;
  end;

  PTSQL.CloseRS();

  PTSQL.ExecuteQuery('SELECT * FROM TicketList WHERE Author='+QuotedStr(loginpt)+' AND Title='+QuotedStr(title)+'');
  if PTSQL.RecordCount > 0 then begin
    PTSQL.CloseRS;
    Exit;
  end
  else begin
    PTSQL.CloseRS;

    if (title <> '') AND (category <> '') AND (description <> '') then begin

      ticketid:= '['+VarToStr(Request.ServerVariables.Item['REMOTE_ADDR'])+']'+title;
      PTSQL.ExecuteQuery('INSERT INTO TicketList (Title,Author,Type,State,Date) VALUES ('+QuotedStr(ticketid)+','+QuotedStr(loginpt)+','+IntToStr(typeticket)+',0,'+QuotedStr(FormatDateTime('c', myDateTime))+')',true);

      PTSQL.ExecuteQuery('SELECT ID FROM TicketList WHERE (Title='+QuotedStr(ticketid)+') AND (Author='+QuotedStr(loginpt)+') AND (Type='+IntToStr(typeticket)+')');
      ticketid:= PTSQL.GetColumn(0);
      PTSQL.CloseRS();

      PTSQL.ExecuteQuery('INSERT INTO TicketData (TicketID,Message,Type,Date) VALUES ('+ticketid+','+QuotedStr(description)+',0,'+QuotedStr(FormatDateTime('c', myDateTime))+')',true);

      Response.Write('<div class="alert alert-success alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
      Response.Write('Ticket has been sent!</div>');
    end
    else begin
      Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
      Response.Write('You need to fill the ticket correctly!</div>');
    end;


    PTSQL.CloseConnection();
    PTSQL.Free();
  end;
end;

procedure TUserPanel.OnListTicketPage;
var PTSQL, PTSQLC : TSQLPT;
    loginpt, tickid : string;
    num : Integer;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');

  PTSQLC:= TSQLPT.Create;
  PTSQLC.CreateConnection('UserDB');

  loginpt:= Session.Value['loginpt'];

  PTSQL.ExecuteQuery('SELECT * FROM TicketList WHERE Author='+QuotedStr(loginpt)+' ORDER BY ID DESC');
  
  Response.Write('<h1 class="page-header">Ticket List</h1>');
  Response.Write('                        <div class="panel panel-default">');
  Response.Write('                        <div class="panel-heading">');
  Response.Write('                            Tickets List');
  Response.Write('                        </div>');
  Response.Write('                        <!-- /.panel-heading -->');
  Response.Write('                        <div class="panel-body">');
  Response.Write('                            <div class="dataTable_wrapper">');
  Response.Write('                                Ticket System disabled! <a href="http://www.fortresspt.net/forum/index.php#c3">Click Here and post on forum!</a>');
  Response.Write('                            </div>');
  Response.Write('                            <!-- /.table-responsive -->');
  Response.Write('                        </div>');
  Response.Write('                        <!-- /.panel-body -->');
  Response.Write('                    </div>');

  PTSQLC.CloseConnection();
  PTSQLC.Free();
  PTSQL.CloseConnection();
  PTSQL.Free();

end;

procedure TUserPanel.OnReplyTicketPage;
var PTSQL: TSQLPT;
    ticketid, loginpt, message: string;
    closed : Integer;
begin
  ticketid:= Request.QueryString.Item['ticketid'];
  if ticketid = '' then
    Exit;

  loginpt:= Session.Value['loginpt'];

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');

  PTSQL.ExecuteQuery('SELECT * FROM TicketList WHERE (ID='+ticketid+') AND (Author='+QuotedStr(loginpt)+')');

  closed:= PTSQL.GetColumn('State');

  if PTSQL.RecordCount > 0 then begin
    Response.Write('<h1 class="page-header">Support Tickets</h1>');
    Response.Write('                        <table width="100%">');
    Response.Write('                            <tr>');
    Response.Write('                                <td width="15%"><b>Title:</b></td>');
    Response.Write('                                <td>');
    Response.Write(PTSQL.GetColumn('Title'));
    Response.Write('                                </td>');
    Response.Write('                            </tr>');
    Response.Write('                            <tr>');
    Response.Write('                                <td><b>Author:</b></td>');
    Response.Write('                                <td>');
    Response.Write(PTSQL.GetColumn('Author'));
    Response.Write('                                </td>');
    Response.Write('                            </tr>');
    Response.Write('                            <tr>');
    Response.Write('                                <td><b>Date:</b></td>');
    Response.Write('                                <td>');
    Response.Write(PTSQL.GetColumn('Date'));
    Response.Write('                                </td>');
    Response.Write('                            <tr>');
    Response.Write('                                <td><b>State:</b></td>');
    Response.Write('                                <td>');
    if PTSQL.GetColumn('State') = '0' then
      Response.Write('<span style="color: #1fae1f">Open</span>')
    else
      Response.Write('<span style="color: #ae1f1f">Closed</span>');
    Response.Write('                                </td>');
    Response.Write('                            </tr>');
    Response.Write('                            <tr>');
    Response.Write('                                <td><b>Type:</b></td>');
    Response.Write('                                <td>');
    if PTSQL.GetColumn('Type') = '0' then
      Response.Write('Account')
    else if PTSQL.GetColumn('Type') = '1' then
      Response.Write('Technical')
    else if PTSQL.GetColumn('Type') = '2' then
      Response.Write('Lost & Found')
    else if PTSQL.GetColumn('Type') = '3' then
      Response.Write('Report User')
    else if PTSQL.GetColumn('Type') = '4' then
      Response.Write('Request Unban');
    Response.Write('                                </td>');
    Response.Write('                            </tr>');

    Response.Write('<tr>');
    Response.Write('<div class="form-group"><td valign="top">');
    Response.Write('                                            <label>Description:</label></td>');
    Response.Write('                                            <td><div>');
    PTSQL.CloseRS();
    PTSQL.ExecuteQuery('SELECT * FROM TicketData WHERE TicketID='+ticketid+' AND Type=0');

    message:= PutStringTag(PTSQL.GetColumn('Message'));

    Response.Write(message);
    Response.Write('</div>');
    Response.Write('                                        </div></td>');

    Response.Write('                        </table>');

    Response.Write('<hr>');
    Response.Write('                        <b>Progress of Ticket</b><br><br>');

    Response.Write('<div class="panel-body">');
    Response.Write('                            <ul class="chat">');

    PTSQL.CloseRS();
    PTSQL.ExecuteQuery('SELECT * FROM TicketData WHERE TicketID='+ticketid+' ORDER BY ID ASC');
    if PTSQL.RecordCount > 0 then
    begin
      repeat
        if PTSQL.GetColumn('Type') = '2' then
        begin
          Response.Write('<li class="left clearfix">');
          Response.Write('                                    <span class="chat-img pull-left">');
          Response.Write('                                        <img src="imgs/staff.png" alt="User Avatar" class="img-circle" />');
          Response.Write('                                    </span>');
          Response.Write('                                    <div class="chat-body clearfix">');
          Response.Write('                                        <div class="header">');
          Response.Write('                                            <strong class="primary-font">');
          Response.Write('fPT Staff');
          Response.Write('                                            </strong>');
          Response.Write('                                            <small class="pull-right text-muted">');
          Response.Write('                                                <i class="fa fa-clock-o fa-fw"></i> ');
          Response.Write(PTSQL.GetColumn('Date'));
          Response.Write('                                            </small>');
          Response.Write('                                        </div>');
          Response.Write('                                        <p>');
          Response.Write(PTSQL.GetColumn('Message'));
          Response.Write('                                        </p>');
          Response.Write('                                    </div>');
          Response.Write('                                </li>');
        end
        else if PTSQL.GetColumn('Type') = 1 then
        begin
          Response.Write('<li class="left clearfix">');
          Response.Write('                                    <span class="chat-img pull-left">');
          Response.Write('                                        <img src="imgs/user.png" alt="User Avatar" class="img-circle" />');
          Response.Write('                                    </span>');
          Response.Write('                                    <div class="chat-body clearfix">');
          Response.Write('                                        <div class="header">');
          Response.Write('                                            <strong class="primary-font">');
          Response.Write(loginpt);
          Response.Write('                                            </strong>');
          Response.Write('                                            <small class="pull-right text-muted">');
          Response.Write('                                                <i class="fa fa-clock-o fa-fw"></i> ');
          Response.Write(PTSQL.GetColumn('Date'));
          Response.Write('                                            </small>');
          Response.Write('                                        </div>');
          Response.Write('                                        <p>');
          Response.Write(PTSQL.GetColumn('Message'));
          Response.Write('                                        </p>');
          Response.Write('                                    </div>');
          Response.Write('                                </li>');
        end;

        PTSQL.NextRow;

      until PTSQL.GetRS.EOF;
    end;
    Response.Write('                            </ul>');
    Response.Write('                        </div>');

    if closed <> 1 then begin
      Response.Write('<div class="panel-footer" style="margin-bottom: 100px">');
      Response.Write('                          <form name=sendreply method=post action="?pag=replyticket&ticketid='+ticketid+'">');
      Response.Write('                            <div class="input-group">');
      Response.Write('                                <input name="message" id="btn-input" type="text" class="form-control input-sm" placeholder="Type your message here..." />');
      Response.Write('                                <span class="input-group-btn">');
      Response.Write('                                    <button type="submit" class="btn btn-warning btn-sm" id="btn-chat">');
      Response.Write('                                        Send');
      Response.Write('                                    </button>');
      Response.Write('                                </span>');
      Response.Write('                            </div>');
      Response.Write('</form>');
      Response.Write('                        </div>');
    end;
  end;

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TUserPanel.OnReplyTicketPageHandle;
var loginpt, ticketid, message : string;
    PTSQL : TSQLPT;
    myDateTime : TDateTime;
begin
  ticketid:= Request.QueryString.Item['ticketid'];
  if ticketid = '' then
    Exit;

  IF Request.Form.Count <= 0 then Exit;

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');

  PTSQL.ExecuteQuery('SELECT TOP 1 * FROM TicketData WHERE TicketID='+ticketid+' ORDER BY ID DESC');

  if PTSQL.GetColumn('Type') = '2' then begin
    myDateTime := Now();

    loginpt:= Session.Value['loginpt'];
    message:= Request.Form.Item['message'];
    PTSQL.AntiSqlI(message);

    PTSQL.ExecuteQuery('INSERT INTO TicketData (TicketID,Message,Type,Date) VALUES ('+ticketid+','+QuotedStr(message)+',1,'+QuotedStr(FormatDateTime('c', myDateTime))+')',True);

    Response.Write('<div class="alert alert-success alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
    Response.Write('Reply has been sent!</div>');

    Response.Write('<META HTTP-EQUIV="refresh" CONTENT="1; URL=?pag=replyticket&ticketid='+ticketid+'">');
  end
  else if PTSQL.GetColumn('Type') = '1' then begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
    Response.Write('You cant send simultaneous message!</div>');
  end
  else begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
    Response.Write('You need wait the answer from staff!</div>');
  end;

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TUserPanel.OnVotePointsPage;
var PTSQL : TSQLPT;
    votes,iditem, query, loginpt: string;
begin
  votes:= '0';

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  loginpt:= Session.Value['loginpt'];
  query:= 'SELECT VotePoints FROM UserInfo WHERE AccountName='+Quotedstr(loginpt);
  PTSQL.ExecuteQuery(query);

  if PTSQL.RecordCount > 0 then begin
    votes:= PTSQL.GetColumn('VotePoints');
  end;
  PTSQL.CloseConnection;

  Response.Write('<h1 class="page-header">Vote Points</h1>');
  Response.Write('<center><b>'+votes+'</b>/10 Vote Points available!<br><br></center>');

  Response.Write('<div class="table-responsive">');
  Response.Write('                                <table class="table table-hover">');
  Response.Write('                                    <thead>');
  Response.Write('                                        <tr>');
  Response.Write('                                            <th>Item</th>');
  Response.Write('                                            <th>Points</th>');
  Response.Write('                                            <th>Description</th>');
  Response.Write('                                            <th>Action</th>');
  Response.Write('                                        </tr>');
  Response.Write('                                    </thead>');
  Response.Write('                                    <tbody>');

  PTSQL.CreateConnection('ServerDB');
  query:= 'SELECT ID,ItemName,ItemDescription,Votes FROM VotePointItems';
  PTSQL.ExecuteQuery(query);
  if PTSQL.RecordCount > 0 then begin
    repeat
      Response.Write('<form name=votept action="?pag=votepoints" method=post>');
      Response.Write('<tr>');
      Response.Write('  <td>'+PTSQL.GetColumn('ItemName')+'</td>');
      votes:= PTSQL.GetColumn('Votes');
      Response.Write('  <td>'+votes+' point(s)</td>');
      Response.Write(' 	<td>'+PTSQL.GetColumn('ItemDescription')+'</td>');
      iditem:= PTSQL.GetColumn('ID');
      Response.Write(' 	<input type=hidden name=voteitemid value="'+iditem+'">');
      Response.Write('  <td><button class="btn btn-outline btn-success btn-xs" type=submit>Get It!</button></td>');
      Response.Write('</tr>');
      Response.Write('</form>');
      PTSQL.NextRow;
    until PTSQL.GetRS.EOF;
  end;
  PTSQL.CloseConnection;
  PTSQL.Free;
  Response.Write('</tbody>');
  Response.Write('                                </table>');
  Response.Write('                            </div>');

  Response.Write('<div class="panel panel-default">');
  Response.Write('                        <div class="panel-heading">');
  Response.Write('How to get Vote Points?');
  Response.Write('                        </div>');
  Response.Write('<div class="panel-body">');
  Response.Write('You need to vote on FortressPT at xtremetop100.com, it'+chr($27)+'s easy just click on the Vote Button and you will be redirected to the Vote Page.');
  Response.Write('Once there you will need to solve the CAPTCHA and then will be able to vote.');
  Response.Write('Solving the CAPTCHA is easy just put the animals the way up and click Done!');
  Response.Write('</div>');
  Response.Write('</div>');

  Response.Write('<div class="panel panel-default">');
  Response.Write('                        <div class="panel-heading">');
  Response.Write('How to get my item?');
  Response.Write('                        </div>');
  Response.Write('<div class="panel-body">');
  Response.Write('Just click on <i>Get It</i> button on the Item you want and it will go to your item distributor ingame. Remember you will need to have the required points for the item.');
  Response.Write('</div>');
  Response.Write('</div>');
  Response.Write('<form name=votept action="?pag=votepoints&act=vote" method=post>');
  Response.Write('<center><button type="submit" class="btn btn-default">Vote for FortressPT</button></center>');
  Response.Write('</form>');


end;

procedure TUserPanel.OnVotePointsPageHandle;
var PTSQL : TSQLPT;
    iditem, itemcode, query, loginpt, votecookie, voteact, ip: string;
    votes, voteneed, count, i: Integer;
begin
  votes:= 0;
  voteneed:= 0;
  count:= 0;
  loginpt:= Session.Value['loginpt'];
  PTSQL:= TSQLPT.Create;
  iditem:= Request.Form.Item['voteitemid'];
  votecookie:= Request.Cookies.Item['1132353382'];
  voteact:= Request.QueryString.Item['act'];
  ip:= Request.ServerVariables.Item['REMOTE_ADDR'];
  if iditem <> '' then
  begin
    PTSQL.AntiSqlI(iditem);
    PTSQL.CreateConnection('UserDB');
    query:= 'SELECT VotePoints FROM UserInfo WHERE AccountName='+quotedstr(loginpt);
    PTSQL.ExecuteQuery(query);
    if PTSQL.RecordCount > 0 then begin
      votes:= StrToInt(PTSQL.GetColumn('VotePoints'));
    end;
    PTSQL.CloseConnection;

    PTSQL.CreateConnection('ServerDB');
    query:= 'SELECT Count, Votes, ItemCode FROM VotePointItems WHERE ID='+iditem;
    PTSQL.ExecuteQuery(query);

    if PTSQL.RecordCount > 0 then begin
      voteneed:= StrToInt(PTSQL.GetColumn('Votes'));
      count:= StrToInt(PTSQL.GetColumn('Count'));
      itemcode:= PTSQL.GetColumn('ItemCode');
    end;
    PTSQL.CloseRS;

    if votes < voteneed then
    begin
      PTSQL.CloseConnection;
      PTSQL.Free;
      Exit;
    end;
    votes:= votes - voteneed;
    if (LowerCase(itemcode[1]) = 'p') and ((LowerCase(itemcode[2]) = 'l') or (LowerCase(itemcode[2]) = 'm') or (LowerCase(itemcode[2]) = 's')) then
    begin
      query:= 'INSERT INTO PostBox VALUES('+quotedstr(loginpt)+','+quotedstr(itemcode)+','+IntToStr(count)+','+quotedstr('Vote Point Item! ^_^')+')';
      PTSQL.ExecuteQuery(query, true);
      PTSQL.CloseConnection;
    end
    else
    begin
      for i:= 0 to count-1 do begin
        query:= 'INSERT INTO PostBox VALUES('+quotedstr(loginpt)+','+quotedstr(itemcode)+',1,'+quotedstr('Vote Point Item! ^_^')+')';
        PTSQL.ExecuteQuery(query, true);
      end;
      PTSQL.CloseConnection;
    end;

    PTSQL.CreateConnection('UserDB');
    PTSQL.ExecuteQuery('UPDATE UserInfo SET VotePoints='+inttostr(votes)+' WHERE AccountName='+quotedstr(loginpt), true);
    PTSQL.CloseConnection;
    PTSQL.Free;

    Response.Write('<div class="alert alert-success alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
    Response.Write('Item has been sent to Item Distributor!</div>');
    Response.Write('<META HTTP-EQUIV="refresh" CONTENT="0; URL=?pag=votepoints">');
    Exit;
  end else
  if voteact = 'vote' then
  begin
    if votecookie<>'' then
    begin
      Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
      Response.Write('You can vote every 12 hours!</div>');
      PTSQL.Free;
      Exit;
    end;
    PTSQL.CreateConnection('UserDB');
    query:= 'SELECT VotePoints FROM UserInfo WHERE AccountName='+quotedstr(loginpt);
    PTSQL.ExecuteQuery(query);

    votes:= 0;
    if PTSQL.RecordCount > 0 then
    begin
      votes:= Strtoint(PTSQL.GetColumn('VotePoints'));
    end;
    PTSQL.CloseRS;

    if votes < 10 then
      votes:= votes+1;
    query:= 'UPDATE UserInfo SET VotePoints=VotePoints+1 WHERE (AccountName='+QuotedStr(loginpt)+') AND (VotePoints < 10)';
    PTSQL.ExecuteQuery(query, true);
    PTSQL.CloseConnection;
    PTSQL.CreateConnection('LogDB');
    query:= 'INSERT INTO VoteLog VALUES('+quotedstr(ip)+','+quotedstr(loginpt)+','+IntToStr(votes)+','+Quotedstr(DateTimeToStr(Now))+')';
    PTSQL.ExecuteQuery(query,true);
    PTSQL.CloseConnection;
    PTSQL.Free;
    votecookie:= '1132353382=1; Expires='+DateTimeToStr(now + (12/24));
    Response.AddHeader('Set-Cookie',votecookie);
    Response.Write('<META HTTP-EQUIV="refresh" CONTENT="0; URL=http://www.xtremetop100.com/in.php?site=1132353382">');
    Exit;
  end;
  PTSQL.Free;
end;

procedure TUserPanel.OnClanManagementPage;
var PTSQL : TSQLPT;
    loginpt : string;
begin

  loginpt:= Session.Value['loginpt'];

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ClanDB');

  PTSQL.ExecuteQuery('SELECT * FROM ClanList WHERE AccountName='+QuotedStr(loginpt));

  Response.Write('<h1 class="page-header">Clan Management</h1>');
  Response.Write('                        <table width="100%">');
  Response.Write('<tr><td width="15%"><b>Icon:</b></td>');
  Response.Write('<td>');
  Response.Write('<img width="24px" height="24px" src="../ClanImage/');
  Response.Write(PTSQL.GetColumn('IconID'));
  Response.Write('.bmp"></td></tr>');
  Response.Write('                            <tr>');
  Response.Write('                                <td width="15%"><b>Clan Name:</b></td>');
  Response.Write('                                <td>');
  Response.Write(PTSQL.GetColumn('ClanName'));
  Response.Write('                                </td>');
  Response.Write('                            </tr>');
  Response.Write('                            <tr>');
  Response.Write('                                <td><b>Note:</b></td>');
  Response.Write('                                <td>');
  Response.Write(PTSQL.GetColumn('Note'));
  Response.Write('                                </td>');
  Response.Write('                            </tr>');
  Response.Write('                            <tr>');
  Response.Write('                                <td><b>Leader:</b></td>');
  Response.Write('                                <td>');
  Response.Write(PTSQL.GetColumn('ClanLeader'));
  Response.Write('                                </td>');
  Response.Write('</tr></table><br><br>');

  Response.Write('<div class="panel panel-default">');
  Response.Write('                                <div class="panel-heading">');
  Response.Write('                                    Clan Management');
  Response.Write('                                </div>');
  Response.Write('                                <div class="panel-body">');
  Response.Write('<form name="form1" action="upload_form2.asp" method="post" enctype="multipart/form-data" onSubmit="return form1_onSubmit();">');
  Response.Write('                                        <div class="form-group">');
  Response.Write('                                            <label style="float: left; width: 100%">Change Icon</label>');
  Response.Write('                                            <input style="float: left;" name="foto" type="file">');
  Response.Write('<input type="hidden" name="codigo" id="codigo" value="'+VarToStr(PTSQL.GetColumn('IconID'))+'" />');
  Response.Write('<input style="margin-left: 10px" name="acao" type="submit" class="" id="acao" value="Change">');
  Response.Write('                                        </div>');
  Response.Write('                                    </form>');
  Response.Write('                                    <form name=changeclan method=post action="?pag=clanmanagement">');
  Response.Write('                                        <div class="form-group">');
  Response.Write('                                            <label>Change Note</label>');
  Response.Write('                                            <input name="note" value="'+PTSQL.GetColumn('Note')+'" class="form-control">');
  Response.Write('                                        </div>');
  Response.Write('                                        <div class="form-group">');
  Response.Write('                                            <label>Change Notice</label>');
  Response.Write('                                            <input name="notice" value="'+PTSQL.GetColumn('LoginMessage')+'" class="form-control">');
  Response.Write('                                        </div>');
  Response.Write('');
  Response.Write('');
  Response.Write('                                        <center>');
  Response.Write('                                            <button type="submit" class="btn btn-default">Save</button>');
  Response.Write('                                            <button type="reset" class="btn btn-default">Reset Fields</button>');
  Response.Write('                                        </center>');
  Response.Write('                                    </form>');
  Response.Write('                                </div>');
  Response.Write('                            </div>');

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TUserPanel.OnSettingsPage;
var PTSQL : TSQLPT;
    loginpt : string;
begin

  loginpt:= Session.Value['loginpt'];

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');

  PTSQL.ExecuteQuery('SELECT * FROM UserInfo WHERE AccountName='+QuotedStr(loginpt)+'');

  Response.Write('<h1 class="page-header">Account Settings</h1>');

  Response.Write('<div class="panel panel-default">');
  Response.Write('                                <div class="panel-heading">');
  Response.Write('                                    My Account');
  Response.Write('                                </div>');
  Response.Write('                                <div class="panel-body">');
  Response.Write('                                    <form name=changepw method=post action="?pag=settings">');
  Response.Write('                                        <div class="form-group">');
  Response.Write('											<label>Account</label>');
  Response.Write('											<input class="form-control" value="'+loginpt+'" disabled>');
  Response.Write('										</div>');
  Response.Write('										<div class="form-group">');
  Response.Write('											<label>Email</label>');
  Response.Write('											<input class="form-control" value="'+VarToStr(PTSQL.GetColumn('Email'))+'" disabled>');
  Response.Write('										</div>');
  Response.Write('										<div class="form-group">');
  Response.Write('											<label>Current Password</label>');
  Response.Write('											<input name="curpassword" type="password" class="form-control">');
  Response.Write('										</div>');
  Response.Write('										<div class="form-group">');
  Response.Write('											<label>New Password</label>');
  Response.Write('											<input name="newpassword" type="password" class="form-control">');
  Response.Write('										</div>');
  Response.Write('										<div class="form-group">');
  Response.Write('											<label>Repeat New Password</label>');
  Response.Write('											<input name="reppassword" type="password" class="form-control">');
  Response.Write('										</div>');
  Response.Write('');
  Response.Write('                                        <center>');
  Response.Write('                                            <button type="submit" class="btn btn-default">Save</button>');
  Response.Write('                                            <button type="reset" class="btn btn-default">Reset Fields</button>');
  Response.Write('                                        </center>');
  Response.Write('                                    </form>');
  Response.Write('                                </div>');
  Response.Write('                            </div>');

  PTSQL.CloseConnection;
  PTSQL.Free;
end;

procedure TUserPanel.OnSettingsPageHandle;
var PTSQL : TSQLPT;
    loginpt, curpw, newpw, reppw : string;
begin
  IF Request.Form.Count <= 0 then Exit;

  loginpt:= Session.Value['loginpt'];
  curpw:= Request.Form.Item['curpassword'];
  newpw:= Request.Form.Item['newpassword'];
  reppw:= Request.Form.Item['reppassword'];

  PTSQL.AntiSqlI(curpw);
  PTSQL.AntiSqlI(newpw);
  PTSQL.AntiSqlI(reppw);

  if (curpw = '') OR (newpw = '') OR (reppw = '') then
  begin
    Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
    Response.Write('You need fill all fields!</div>');
  end
  else begin
    PTSQL:= TSQLPT.Create;
    PTSQL.CreateConnection('UserDB');

    PTSQL.ExecuteQuery('SELECT * FROM UserInfo WHERE AccountName='+QuotedStr(loginpt)+'');

    if curpw = PTSQL.GetColumn('Password') then begin
      if newpw = reppw then begin
        PTSQL.ExecuteQuery('UPDATE UserInfo SET Password='+QuotedStr(newpw)+' WHERE AccountName='+QuotedStr(loginpt)+'',true);

        Response.Write('<div class="alert alert-success alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
        Response.Write('Your password has been successfully changed!</div>');

        Response.Write('<META HTTP-EQUIV="refresh" CONTENT="1; URL=?pag=settings">');
      end
      else begin
        Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
        Response.Write('Repeat New Password is wrong. Try again!</div>');
      end;
    end
    else begin
      Response.Write('<div class="alert alert-danger alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
      Response.Write('Current Password is wrong. Try again!</div>');
    end;
    
  end;

  PTSQL.CloseConnection();
  PTSQL.Free();

end;

procedure TUserPanel.OnClanManagementPageHandle;
var PTSQL : TSQLPT;
    loginpt, note, notice : string;
begin
  IF Request.Form.Count <= 0 then Exit;

  loginpt:= Session.Value['loginpt'];
  note:= Request.Form.Item['note'];
  notice:= Request.Form.Item['notice'];

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ClanDB');

  PTSQL.AntiSqlI(note);
  PTSQL.AntiSqlI(notice);

  PTSQL.ExecuteQuery('UPDATE ClanList SET Note='+QuotedStr(note)+',LoginMessage='+QuotedStr(notice)+' WHERE AccountName='+QuotedStr(loginpt)+'',true);

  Response.Write('<div class="alert alert-success alert-dismissable container-fluid" style="top: 0px; left: 0px; right: 0px; margin-top: 20px; position: absolute; width: 400px;"><button type="button" class="close" data-dismiss="alert" aria-hidden="true">&times;</button>');
  Response.Write('Clan Settings has been changed!</div>');

  Response.Write('<META HTTP-EQUIV="refresh" CONTENT="1; URL=?pag=clanmanagement">');

  PTSQL.CloseConnection;
  PTSQL.Free;

end;

initialization
  TAutoObjectFactory.Create(ComServer, TUserPanel, Class_UserPanel,
    ciMultiInstance, tmBoth);
end.
