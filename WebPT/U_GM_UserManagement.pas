unit U_GM_UserManagement;

interface

type
  TUserManagementGM = class
  private
    procedure GetUsersInfo; safecall;
    procedure DeleteUserInfo; safecall;
  public
    procedure Index; safecall;
    procedure GetParams; safecall;
  end;

implementation

{ TUserManagementGM }

uses U_GMPanel, U_GMPanelEnum, U_SQL, SysUtils;

const
// Actions ID
  EAUM_SearchUser = '1';
  EAUM_EditUser = '2';
  EAUM_DeleteUser = '3';

  PAGE_USERMANAGEMENT         = '?page='+EPageGM_UserManagement;
  EPageGM_SearchUser          = PAGE_USERMANAGEMENT + EAction + EAUM_SearchUser;
  EPageGM_Edit                = PAGE_USERMANAGEMENT + EAction + EAUM_EditUser + '&p1=';
  EPageGM_Delete              = PAGE_USERMANAGEMENT + EAction + EAUM_DeleteUser + '&p1=';

procedure TUserManagementGM.GetUsersInfo;
var account, pass, email, ip, query, id: string;
    PT: TSQLPT;
    bOR: Boolean;
begin
  bOR:= False;
  account:= getrequest.Form.Item['account'];
  pass:= getrequest.Form.Item['pass'];
  email:= getrequest.Form.Item['email'];
  ip:= getrequest.Form.Item['ip'];
  PT:= TSQLPT.Create;
  PT.CreateConnection('UserDB');
  query:= 'SELECT * FROM UserInfo WHERE ';
  if account <> '' then
  begin
    query:= query + '(userid LIKE '+QuotedStr('%'+account+'%')+') ';
    bOR:= True;
  end;
  if pass <> '' then
  begin
    if bOR then
      query:= query + ' OR ';
    query:= query + '(Password LIKE '+QuotedStr('%'+pass+'%')+') ';
    bOR:= True;
  end;
  if email <> '' then
  begin
    if bOR then
      query:= query + ' OR ';
    query:= query + '(Email LIKE '+QuotedStr('%'+email+'%')+') ';
    bOR:= True;
  end;
  if ip <> '' then
  begin
    if bOR then
      query:= query + ' OR ';
    query:= query + '(IP LIKE '+QuotedStr('%'+ip+'%')+') ';
    bOR:= True;
  end;
  PT.ExecuteQuery(query);

  GetResponse.Write('     <table class="list" width="60%" align="center" border="0" cellpadding="0" cellspacing="0" style="margin-top: 50px">');
  GetResponse.Write('      <tr width="100%">');
  GetResponse.Write('        <td class="bold" width="3%">#</td>');
  GetResponse.Write('        <td class="bold" width="15%">Account</td>');
  GetResponse.Write('        <td class="bold" width="15%">Password</td>');
  GetResponse.Write('        <td class="bold" width="15%">IP</td>');
  GetResponse.Write('        <td class="bold" width="20%">Email</td>');
  GetResponse.Write('        <td width="15%"></td>');
  GetResponse.Write('      </tr>');

  repeat
    id:= PT.GetColumn('ID');
    account:= PT.GetColumn('UserID');
    pass:= PT.GetColumn('Password');
    email:= PT.GetColumn('Email');
    ip:= PT.GetColumn('IP');
    GetResponse.Write('      <tr>');
    GetResponse.Write('        <td>'+id+'</td>');
    GetResponse.Write('        <td>'+account+'</td>');
    GetResponse.Write('        <td>'+pass+'</td>');
    GetResponse.Write('        <td>'+ip+'</td>');
    GetResponse.Write('        <td>'+email+'</td>');
    GetResponse.Write('        <td><button onclick="location.href='+QuotedStr(EPageGM_Edit+id)+'">Edit</button>');
    GetResponse.Write('           <a class="bold" href="'+EPageGM_Delete+id+'" onclick="return confirm('+QuotedStr('Delete?')+')">Delete</a></td>');
    GetResponse.Write('      </tr>');

    PT.NextRow;
  until PT.GetRS.EOF;
  GetResponse.Write('     </table>');
  PT.CloseConnection;
  PT.Free;
end;


procedure TUserManagementGM.Index;
begin
GetResponse.Write('  <form name=formpt action="'+EPageGM_SearchUser+'" method=post>');
GetResponse.Write('    <table align="center" border="0" cellpadding="0" cellspacing="0" style="margin-top: 50px">');
GetResponse.Write('      <tr>');
GetResponse.Write('        <td width="120px">Account:</td>');
GetResponse.Write('        <td><input size="48" type="text" name="account"></td>');
GetResponse.Write('      </tr>');
GetResponse.Write('      <tr>');
GetResponse.Write('        <td>Password:</td>');
GetResponse.Write('        <td><input size="48" type="text" name="pass"></td>');
GetResponse.Write('      </tr>');
GetResponse.Write('      <tr>');
GetResponse.Write('        <td>Email:</td>');
GetResponse.Write('        <td><input size="48" type="text" name="email"></td>');
GetResponse.Write('      </tr>');
GetResponse.Write('      <tr>');
GetResponse.Write('        <td>IP:</td>');
GetResponse.Write('        <td><input size="48" type="text" name="ip"></td>');
GetResponse.Write('      </tr>');
GetResponse.Write('    </table>');
GetResponse.Write('');
GetResponse.Write('    <br>');
GetResponse.Write('    <button type=submit>Search</button>');
GetResponse.Write('  </form>');
GetParams;
end;

procedure TUserManagementGM.GetParams;
var
  action: String;
begin
  action:= GetRequest.QueryString.Item['action'];
  if action = EAUM_SearchUser then
    GetUsersInfo;
  if action = EAUM_DeleteUser then
    DeleteUserInfo;
end;

procedure TUserManagementGM.DeleteUserInfo;
var id, query: string;
    PT: TSQLPT;
begin
  id:= '';
  id:= GetRequest.QueryString.Item['p1'];
  if id <> '' then
  begin
    PT:= TSQLPT.Create;
    PT.CreateConnection('UserDB');
    query:= 'SELECT ID FROM UserInfo WHERE ID='+id;
    PT.ExecuteQuery(query);
    id:= PT.GetColumn('ID');
    if id<>'' then
    begin
      PT.CloseRS;
      query:= 'DELETE FROM UserInfo WHERE ID='+id;
      PT.ExecuteQuery(query, True);
    end;
    PT.CloseConnection;
    PT.Free;
  end;
end;

end.
 