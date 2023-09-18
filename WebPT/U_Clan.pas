unit U_Clan;

{$WARN SYMBOL_PLATFORM OFF}

interface

uses
  ComObj, ActiveX, AspTlb, WebPT_TLB, StdVcl, U_SQL, Variants, Windows;

type
  TClan = class(TASPObject, IClan)
  protected
    procedure OnEndPage; safecall;
    procedure OnStartPage(const AScriptingContext: IUnknown); safecall;
    function IsUser: Boolean; safecall;
    function IsInGame(ticket: integer): Boolean;
    function GetSubLeader(clname: string): OleVariant; safecall;
    procedure ClanInsert; safecall;
    procedure ClanMember; safecall;
    procedure GetClanMembers; safecall;
    procedure CheckClanLeader; safecall;
    procedure CheckClanName; safecall;
    procedure CheckClanMember; safecall;
    procedure ClanDelete; safecall;
    procedure InvitePlayer; safecall;
    procedure CheckInvitedPlayer; safecall;
    procedure SetClanSubLeader; safecall;
    procedure ReleaseClanSubLeader; safecall;
    procedure SetClanLeader; safecall;
    procedure GetClanData; safecall;
    procedure SetReleasePlayerSelf; safecall;
    procedure SetReleasePlayer; safecall;
    procedure SodScore; safecall;
  end;

implementation

uses ComServ, SysUtils;

var
  bAccess : Boolean = True;

const
  DaysDeleteClan = 5;
  Maxmembers = 30;      // Max members in the clan
  Top3Sod = True;       // True = 3 crowns
  ClanNote = 'New Clan! ^_^';
  BellatraTax = 10;


procedure TClan.OnEndPage;
begin
  inherited OnEndPage;
end;

procedure TClan.OnStartPage(const AScriptingContext: IUnknown);
begin
  inherited OnStartPage(AScriptingContext);
end;

function UNIXTimeToDateTime(UnixTime: LongWord): TDateTime;
begin
Result := (UnixTime / 86400) + 25569;
end;

function GetUNIXTime: LongWord;
begin
Result := Round((now - 25569) * 86400);
end;

procedure TClan.ClanInsert;
var chname, clname, userid, query, clanid: string;
    iconid: Integer;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
//  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;


  PTSQL:= TSQLPT.Create;

  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);
  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);
  clname:= Request.QueryString.Item['clname'];
  PTSQL.AntiSqlIClan(clname);

  clanid:= '';

  // No data?
  if (chname = '') or (clname = '') or (userid = '') then
  begin
    // Error
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Make Connection
  PTSQL.CreateConnection('ClanDB');

  // Player have clan?
  query:= 'SELECT ClanName FROM ClanList WHERE ClanLeader='+QuotedStr(chname);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    Response.Write('Code=2'+#13+'CMoney=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  PTSQL.CloseRS;

  // Clan name is alive?
  query:= 'SELECT ClanName FROM ClanList WHERE ClanName='+QuotedStr(clname);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    Response.Write('Code=3'+#13+'CMoney=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  PTSQL.CloseRS;

  // Get new icon
  query:= 'SELECT TOP 1 IconID FROM ClanList ORDER BY IconID DESC';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
    iconid:= 1000000001
  else
    iconid:= strtoint(vartostr(PTSQL.GetColumn('IconID'))) + 1;
    
  PTSQL.CloseRS;

  // Insert New Clan
  if (Length(clname) <= 32) and (Length(chname) <= 32) and (Length(userid) <= 20) then
  begin
    query:= 'INSERT INTO ClanList([ClanName],[ClanLeader],[Note],[AccountName],[MembersCount],[IconID],[RegisDate],[LimitDate],[DeleteActive],[Flag],[SiegeWarPoints],[BellatraPoints],[SiegeWarGold],[BellatraGold],[BellatraDate],[LoginMessage]) ';
    query:= query + 'VALUES('+quotedstr(clname)+','+quotedstr(chname)+','+quotedstr(ClanNote)+','+quotedstr(userid)+',1,'+IntToStr(iconid)+','+IntToStr(GetUNIXTime)+','+IntToStr(GetUNIXTime+(60*60*(24*DaysDeleteClan)))+', 1, 0,0,0,0,0,0,'+quotedstr('We love rPT! ^_^')+')';
    PTSQL.ExecuteQuery(query, True);
    PTSQL.CloseRS;

    query:= 'SELECT ID FROM ClanList WHERE AccountName='+quotedstr(userid);
    PTSQL.ExecuteQuery(query);
    clanid:= PTSQL.GetColumn('ID');
    PTSQL.CloseConnection;

    PTSQL.CreateConnection('UserDB');
    query:= 'UPDATE CharacterInfo SET ClanID='+clanid+' WHERE Name='+quotedstr(chname);
    PTSQL.ExecuteQuery(query, True);
    PTSQL.CloseConnection;
    Response.Write('Code=1'+#13);

    clanid:= 'C:\inetpub\wwwroot\ClanImage\'+IntToStr(iconid)+'.bmp';

    CopyFileA('C:\inetpub\wwwroot\ClanImage\1000000000.bmp', PAnsiChar(clanid), FALSE);

  end;

  PTSQL.Free;
end;

procedure TClan.ClanMember;
var chname, clname, userid, query, subleader, leader, note, ClanIMG, ClanMembers, CDate, EDate: string;
    clnametop, clpoint, nflag, clgold: string;
    clanid, i: Integer;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;

  PTSQL:= TSQLPT.Create;

  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);
  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);

  if (userid = '') or (chname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Make Connection
  PTSQL.CreateConnection('UserDB');

  // Get clan
  query:= 'SELECT ClanID,ClanPermission FROM CharacterInfo WHERE (Name='+Quotedstr(chname)+') AND (ClanID != 0)';
  PTSQL.ExecuteQuery(query);
  // No clan?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13+'CMoney=500000'+#13+'CNFlag=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clanid:= strtoint(vartostr(PTSQL.GetColumn('ClanID')));
  PTSQL.CloseRS;

  // Get SubLeader
  query:= 'SELECT Name FROM CharacterInfo WHERE (ClanID='+IntToStr(clanid)+') AND (ClanPermission=2)';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
    subleader:= ''
  else
    subleader:= PTSQL.GetColumn('Name');

  PTSQL.CloseConnection;

  // Set to clandb
  PTSQL.CreateConnection('ClanDB');

  // Clan Name
  query:= 'SELECT * FROM ClanList WHERE ID='+IntToStr(clanid);
  PTSQL.ExecuteQuery(Query);
  // No have? o.o
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13+'CMoney=500000'+#13+'CNFlag=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  clname:= PTSQL.GetColumn('ClanName');
  leader:= PTSQL.GetColumn('ClanLeader');
  note:= PTSQL.GetColumn('Note');
  ClanIMG:= PTSQL.GetColumn('IconID');
  ClanMembers:= PTSQL.GetColumn('MembersCount');
  CDate:= FormatDateTime('dd/mm/yy hh:mm:ss',UNIXTimeToDateTime(StrToInt(VarToStr(PTSQL.GetColumn('RegisDate')))));
  EDate:= FormatDateTime('dd/mm/yy hh:mm:ss',UNIXTimeToDateTime(StrToInt(VarToStr(PTSQL.GetColumn('LimitDate')))));
  PTSQL.CloseRS;

  // Sod Top 3?
  if top3sod = True then
    query:= 'SELECT TOP 3 ClanName, BellatraPoints, BellatraGold FROM ClanList ORDER BY BellatraPoints DESC'
  else
    query:= 'SELECT TOP 1 ClanName, BellatraPoints, BellatraGold FROM ClanList ORDER BY BellatraPoints DESC';

  PTSQL.ExecuteQuery(query);

  // Get Crown
  nflag:= '0';
  i:= 1;
  repeat
    clnametop:= PTSQL.GetColumn('ClanName');
    clpoint:= PTSQL.GetColumn('BellatraPoints');
    clgold:= PTSQL.GetColumn('BellatraGold');
    if (StrToInt(clpoint) > 0) and (clname = clnametop) then
    begin
      nflag:= IntToStr(i);
      Break;
    end;
    clpoint:= '0';
    clgold:= '0';
    i:= i + 1;
    PTSQL.NextRow;
  until PTSQL.GetRS.EOF;

  PTSQL.CloseConnection;
  PTSQL.Free;

  // Return clan
  if leader = chname then
  begin
    if subleader = '' then
    begin
      Response.Write('Code=2' + #13 + 'CName=' + clname + #13 + 'CNote=' + Note + #13 + 'CZang=' + Leader + #13 + 'CStats=1' + #13 + 'CMCnt=' + ClanMembers + #13 + 'CIMG=' + ClanIMG + #13 + 'CSec=60' + #13 + 'CRegiD=' + CDate + #13 + 'CLimitD=' + EDate + #13 + 'CDelActive=0' + #13 + 'CPFlag=0'+ #13 + 'CKFlag=0' + #13 + 'CMoney=' + clgold + #13 + 'CNFlag=' + NFlag + #13);
    end else begin
      Response.Write('Code=2' + #13 + 'CName=' + clname + #13 + 'CNote=' + note + #13 + 'CZang=' + Leader + #13 + 'CSubChip=' + subleader + #13 + 'CStats=1' + #13 + 'CMCnt=' + ClanMembers + #13 + 'CIMG=' + ClanIMG + #13 + 'CSec=60' + #13 + 'CRegiD=' + CDate + #13 + 'CLimitD=' + EDate + #13 + 'CDelActive=0' + #13 + 'CPFlag=0' + #13 + 'CKFlag=0' + #13 + 'CMoney=' + clgold + #13 + 'CNFlag=' + nflag + #13);
    end;
  end
  else
  begin

    if subleader = chname then
    begin
      if leader <> chname then
      begin
        Response.Write('Code=5' + #13 + 'CName=' + clname + #13 + 'CNote=' + Note + #13 + 'CZang=' + Leader + #13 + 'CSubChip=' + subleader + #13 + 'CStats=1' + #13 + 'CMCnt=' + ClanMembers + #13 + 'CIMG=' + ClanIMG + #13 + 'CSec=60' + #13 + 'CRegiD=' + CDate + #13 + 'CLimitD=' + EDate + #13 + 'CDelActive=0' + #13 + 'CPFlag=0' + #13 + 'CKFlag=0' + #13 + 'CMoney=' + clgold + #13 + 'CNFlag=' + NFlag + #13);
      end;
    end else
    if subleader = '' then
    begin
      Response.Write('Code=1' + #13 + 'CName=' + clname + #13 + 'CNote=' + Note + #13 + 'CZang=' + Leader + #13 + 'CStats=1' + #13 + 'CMCnt=' + ClanMembers + #13 + 'CIMG=' + ClanIMG + #13 + 'CSec=60' + #13 + 'CRegiD=' + CDate + #13 + 'CLimitD=' + EDate + #13 + 'CDelActive=0' + #13 + 'CPFlag=0' + #13 + 'CKFlag=0' + #13 + 'CMoney=' + clgold + #13 + 'CNFlag=' + NFlag + #13);
    end else begin
      Response.Write('Code=1' + #13 + 'CName=' + clname + #13 + 'CNote=' + Note + #13 + 'CZang=' + Leader + #13 + 'CSubChip=' + subleader + #13 + 'CStats=1' + #13 + 'CMCnt=' + ClanMembers + #13 + 'CIMG=' + ClanIMG + #13 + 'CSec=60' + #13 + 'CRegiD=' + CDate + #13 + 'CLimitD=' + EDate + #13 + 'CDelActive=0' + #13 + 'CPFlag=0' + #13 + 'CKFlag=0' + #13 + 'CMoney=' + clgold + #13 + 'CNFlag=' + NFlag + #13);
    end;
  end;


end;

procedure TClan.GetClanMembers;
var chname, clmember, userid, query, leader, clanid, clname: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;

  PTSQL:= TSQLPT.Create;

  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);
  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);

  if (userid = '') or (chname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Get Clan ID
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT ClanID FROM CharacterInfo WHERE Name='+quotedstr(chname);
  PTSQL.ExecuteQuery(query);
   // Not exists character? o.o
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clanid:= PTSQL.GetColumn('ClanID');
  PTSQL.CloseRS;

  // No have clan?
  if StrToInt(clanid) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  PTSQL.CloseConnection;

  // Get Leader
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ClanName, ClanLeader FROM ClanList WHERE ID='+clanid;
  PTSQL.ExecuteQuery(query);

  // No have clan?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clname:= PTSQL.GetColumn('ClanName');
  leader:= PTSQL.GetColumn('ClanLeader');
  PTSQL.CloseConnection;
  Response.Write('Code=1'+#13+'CClanName='+clname+#13+'CClanZang='+leader+#13);

  // Get Members
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT Name FROM CharacterInfo WHERE ClanID='+clanid;
  PTSQL.ExecuteQuery(query);

  // No members?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  // Set members
  repeat
    clmember:= PTSQL.GetColumn('Name');
    Response.Write('CMem='+clmember+#13);
    PTSQL.NextRow;
  until PTSQL.GetRS.EOF;

  PTSQL.CloseConnection;
  PTSQL.Free;
end;

procedure TClan.CheckClanLeader;
var userid, query: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;


  PTSQL:= TSQLPT.Create;

  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);

  if (userid = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Check if is leader by AccountName
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ClanName FROM ClanList WHERE AccountName='+quotedstr(userid);
  PTSQL.ExecuteQuery(query);

  // No have clan or no is leader?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  // Is Leader
  Response.Write('Code=1'+#13);
  PTSQL.CloseConnection;
  PTSQL.Free;

end;

procedure TClan.CheckClanName;
var query, clname: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;

  PTSQL:= TSQLPT.Create;

  clname:= Request.QueryString.Item['clname'];
  PTSQL.AntiSqlIClan(clname);

  if (clname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Check if have clan
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ClanLeader FROM ClanList WHERE ClanName='+quotedstr(clname);
  PTSQL.ExecuteQuery(query);

  // No have clan?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  // Have clan
  Response.Write('Code=1'+#13);
  PTSQL.CloseConnection;
  PTSQL.Free;

end;

procedure TClan.CheckClanMember;
var query, chname, clanid: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;

  PTSQL:= TSQLPT.Create;

  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);

  if (chname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Check if char is in clan
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT ClanID FROM CharacterInfo WHERE Name='+quotedstr(chname);
  PTSQL.ExecuteQuery(query);

  // No have clan?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  clanid:= PTSQL.GetColumn('ClanID');
  // user not in clan?
  if StrToInt(clanid) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  PTSQL.CloseConnection;

   // Check if clan exists
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ID FROM ClanList WHERE ID='+clanid;
  PTSQL.ExecuteQuery(query);

  // No have clan?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  // Have clan
  Response.Write('Code=1'+#13);
  PTSQL.CloseConnection;
  PTSQL.Free;

end;

function TClan.IsUser: Boolean;
var s: string;
begin
  Result:= True;
  s:= Request.ServerVariables.Item['HTTP_USER_AGENT'];
  if Pos('fpthttp',s)>0 then
    Result:= False;
end;

procedure TClan.ClanDelete;
var query, chname, clname, userid, clanid: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;


  PTSQL:= TSQLPT.Create;

  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);
  clname:= Request.QueryString.Item['clname'];
  PTSQL.AntiSqlIClan(clname);
  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);

  if (chname = '') or (clname = '') or (userid = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  PTSQL.CreateConnection('ClanDB');

  query:= 'SELECT ID,ClanName FROM ClanList WHERE (AccountName='+quotedstr(userid)+') AND (ClanLeader='+quotedstr(chname)+') AND (ClanName='+quotedstr(clname)+')';
  PTSQL.ExecuteQuery(query);

  // No mismatch clan?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clanid:= PTSQL.GetColumn('ID');

  PTSQL.CloseRS;

  // Delete clan
  query:= 'DELETE FROM ClanList WHERE ClanName='+quotedstr(clname);
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;

  // Set clan to 0
  PTSQL.CreateConnection('UserDB');
  query:= 'UPDATE CharacterInfo SET ClanID=0 WHERE ClanID='+clanid;
  PTSQL.ExecuteQuery(query,true);
  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13);

end;

procedure TClan.InvitePlayer;
var query, chname, clname, userid, clanid, clwon, clwonUserid, chname2, memcnt: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;

  PTSQL:= TSQLPT.Create;

  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);
  clname:= Request.QueryString.Item['clname'];
  PTSQL.AntiSqlIClan(clname);
  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);
  clwon:= Request.QueryString.Item['clwon'];
  PTSQL.AntiSqlIClan(clwon);
  clwonUserid:= Request.QueryString.Item['clwonUserid'];
  PTSQL.AntiSqlIClan(clwonUserid);

  if (chname = '') or (clname = '') or (userid = '') or (clwon = '') or (clwonUserid = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  chname2:= GetSubLeader(clname);

  PTSQL.CreateConnection('ClanDB');

  // Get Clan data
  query:= 'SELECT ID,MembersCount FROM ClanList WHERE (AccountName='+quotedstr(userid)+') AND (ClanLeader='+quotedstr(chname)+')';
  PTSQL.ExecuteQuery(query);
  // No mismatch clan?
  if (StrToInt(VarToStr(PTSQL.RecordCount)) = 0) then
  begin
    // Is Subleader?
    if (chname = chname2) then
    begin
      PTSQL.CloseRS;
      query:= 'SELECT ID,MembersCount FROM ClanList WHERE ClanName='+quotedstr(clname);
      PTSQL.ExecuteQuery(query);
      if (StrToInt(VarToStr(PTSQL.RecordCount)) = 0) then
      begin
        Response.Write('Code=0'+#13);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end;
    end else
    begin
      Response.Write('Code=0'+#13);
      PTSQL.CloseConnection;
      PTSQL.Free;
      Exit;
    end;
  end;
  clanid:= PTSQL.GetColumn('ID');
  memcnt:= PTSQL.GetColumn('MembersCount');
  PTSQL.CloseRS;

  // Clan is full members?
  if (StrToInt(memcnt) + 1) > Maxmembers then
  begin
    Response.Write('Code=2'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  // Insert new member
  memcnt:= IntToStr(StrToInt(memcnt) + 1);
  query:= 'UPDATE ClanList SET MembersCount='+memcnt+' WHERE ID='+clanid;
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseRS;

  query:= 'DELETE FROM BellatraTeamScore WHERE (CharacterName='+quotedstr(clwon)+')';
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;

  // Set Member user
  PTSQL.CreateConnection('UserDB');
  query:= 'UPDATE CharacterInfo SET ClanID='+clanid+' WHERE Name='+quotedstr(clwon);
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13);

end;

procedure TClan.CheckInvitedPlayer;
var clwon: string;
begin
  if IsUser then Exit;

  clwon:= Request.QueryString.Item['clwon'];

  if (clwon = '') then
  begin
    Response.Write('Code=100'+#13);
    Exit;
  end;

  Response.Write('Code=1'+#13);

end;

procedure TClan.SetClanSubLeader;
var query, chname, clanid: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;

  PTSQL:= TSQLPT.Create;

  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);

  if (chname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT ClanID FROM CharacterInfo WHERE Name='+quotedstr(chname);
  PTSQL.ExecuteQuery(query);
  // No clan member?
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clanid:= PTSQL.GetColumn('ClanID');
  PTSQL.CloseRS;

  query:= 'UPDATE CharacterInfo SET ClanPermission=0 WHERE (ClanPermission=2) AND (ClanID='+clanid+')';
  PTSQL.ExecuteQuery(query,true);
  PTSQL.CloseRS;

  // Set new subleader
  query:= 'UPDATE CharacterInfo SET ClanPermission=2 WHERE (Name='+quotedstr(chname)+')';
  PTSQL.ExecuteQuery(query,true);
  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13);

end;

procedure TClan.ReleaseClanSubLeader;
var query, chname: string;
    PTSQL: TSQLPT;
begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;

  PTSQL:= TSQLPT.Create;

  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);

  if (chname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Release subleader
  PTSQL.CreateConnection('UserDB');
  query:= 'UPDATE CharacterInfo SET ClanPermission=0 WHERE (Name='+quotedstr(chname)+')';
  PTSQL.ExecuteQuery(query,true);
  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13);

end;

procedure TClan.SetClanLeader;
var query, chname, clname, userid, leader: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;


  PTSQL:= TSQLPT.Create;

  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);
  clname:= Request.QueryString.Item['clname'];
  PTSQL.AntiSqlIClan(clname);

  if (chname = '') or (clname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Get Clan Leader name
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ClanLeader FROM ClanList WHERE ClanName='+quotedstr(clname);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  leader:= PTSQL.GetColumn('ClanLeader');
  PTSQL.CloseConnection;

  PTSQL.CreateConnection('UserDB');

  // Get AccountName of new leader
  query:= 'SELECT AccountName FROM CharacterInfo WHERE Name='+quotedstr(chname);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  userid:= PTSQL.GetColumn('AccountName');
  PTSQL.CloseRS;
  // Set ClanPermission to 0
  query:= 'UPDATE CharacterInfo SET ClanPermission=0 WHERE Name='+quotedstr(leader);
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;

  // Set New leader
  PTSQL.CreateConnection('ClanDB');
  query:= 'UPDATE ClanList SET AccountName='+quotedstr(userid)+', ClanLeader='+quotedstr(chname)+' WHERE ClanName='+quotedstr(clname);
  PTSQL.ExecuteQuery(query,true);
  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13);

end;

function TClan.GetSubLeader(clname: string): OleVariant;
var query, clanid: string;
    PTSQL: TSQLPT;
begin
  result:= '';
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ID FROM ClanList WHERE ClanName='+quotedstr(clname);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clanid:= PTSQL.GetColumn('ID');
  PTSQL.CloseConnection;

  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT Name FROM CharacterInfo WHERE (ClanID='+clanid+') AND (ClanPermission=2)';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  Result:= PTSQL.GetColumn('Name');
  PTSQL.CloseConnection;
  PTSQL.Free;
end;

procedure TClan.GetClanData;
var query, clname, note, num: string;
    PTSQL: TSQLPT;
Begin

  if IsUser then Exit;

  PTSQL:= TSQLPT.Create;

  num:= Request.QueryString.Item['num'];
  PTSQL.AntiSqlIClan(num);

  if num = '' then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.Free;
  end;

  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ClanName,Note FROM ClanList WHERE IconID='+num;
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=0'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clname:= PTSQL.GetColumn('ClanName');
  note:= PTSQL.GetColumn('Note');

  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13+'CName='+clname+#13+'CNote='+note+#13);

end;

procedure TClan.SetReleasePlayerSelf;
var query, clname, userid, chname, clanid: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;


  PTSQL:= TSQLPT.Create;

  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);
  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);
  clname:= Request.QueryString.Item['clname'];
  PTSQL.AntiSqlIClan(clname);

  if (userid = '') or (chname = '') or (clname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;


  // Is leader? o.o
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ClanLeader FROM ClanList WHERE ClanLeader='+quotedstr(chname);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    Response.Write('Code=4'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  PTSQL.CloseConnection;

  // Disable clan from player
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT ClanID FROM CharacterInfo WHERE (Name='+quotedstr(chname)+') AND (AccountName='+quotedstr(userid)+')';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clanid:= PTSQL.GetColumn('ClanID');
  PTSQL.CloseRS;

  if clanid <> '0' then
  begin
  query:= 'UPDATE CharacterInfo SET ClanID=0 WHERE (Name='+quotedstr(chname)+') AND (AccountName='+quotedstr(userid)+')';
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;
  end else
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  // UPDATE CLAN
  PTSQL.CreateConnection('ClanDB');
  query:= 'UPDATE ClanList SET MembersCount = MembersCount-1 WHERE ClanName='+quotedstr(clname);
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13);

end;

procedure TClan.SetReleasePlayer;
var query, clname, userid, chname, leader, clanid: string;
    PTSQL: TSQLPT;
Begin

  // Is Browser or is not ingame? Exit
  if IsUser then Exit;
  if IsInGame(StrToInt(Request.QueryString.Item['ticket'])) = False then Exit;


  PTSQL:= TSQLPT.Create;

  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);
  chname:= Request.QueryString.Item['clwon1'];
  PTSQL.AntiSqlIClan(chname);
  leader:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(leader);
  clname:= Request.QueryString.Item['clname'];
  PTSQL.AntiSqlIClan(clname);

  if (userid = '') or (chname = '') or (clname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;


  // Is leader? o.o
  PTSQL.CreateConnection('ClanDB');
  query:= 'SELECT ClanLeader FROM ClanList WHERE ClanLeader='+quotedstr(chname);
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    Response.Write('Code=4'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  PTSQL.CloseRS;

  // no leader from kick?
  query:= 'SELECT ClanLeader FROM ClanList WHERE (ClanLeader='+quotedstr(leader)+') AND (AccountName='+quotedstr(userid)+')';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  // Disable clan from player
  PTSQL.CreateConnection('UserDB');
  query:= 'SELECT ClanID FROM CharacterInfo WHERE (Name='+quotedstr(chname)+')';
  PTSQL.ExecuteQuery(query);
  if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;
  clanid:= PTSQL.GetColumn('ClanID');
  PTSQL.CloseRS;

  if clanid <> '0' then
  begin
  query:= 'UPDATE CharacterInfo SET ClanID=0 WHERE (Name='+quotedstr(chname)+')';
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;
  end else
  begin
    Response.Write('Code=100'+#13);
    PTSQL.CloseConnection;
    PTSQL.Free;
    Exit;
  end;

  // UPDATE CLAN
  PTSQL.CreateConnection('ClanDB');
  query:= 'UPDATE ClanList SET MembersCount = MembersCount-1 WHERE ClanName='+quotedstr(clname);
  PTSQL.ExecuteQuery(query, true);
  PTSQL.CloseConnection;
  PTSQL.Free;

  Response.Write('Code=1'+#13);

end;
procedure TClan.SodScore;
var query, clname, note, clgold, clpoint, userid, chname, leader, leader2, clanid, clanid2, clperm, codeimg, strret, index: string;
    PTSQL: TSQLPT;
Begin

  if IsUser then Exit;

  PTSQL:= TSQLPT.Create;

  userid:= Request.QueryString.Item['userid'];
  PTSQL.AntiSqlIClan(userid);
  chname:= Request.QueryString.Item['chname'];
  PTSQL.AntiSqlIClan(chname);
  index:= Request.QueryString.Item['index'];
  PTSQL.AntiSqlIClan(index);

  if (userid = '') or (chname = '') then
  begin
    Response.Write('Code=100'+#13);
    PTSQL.Free;
    Exit;
  end;

  // Index null?
  if (index = '') then
  begin
    Response.Write('Code=104'+#13);
    PTSQL.Free;
    Exit;
  end;

  PTSQL.CreateConnection('ClanDB');

  // Karina Main Page?
  if index = '1' then
  begin
    query:= 'SELECT TOP 1 * FROM ClanList ORDER BY BellatraPoints DESC';
    PTSQL.ExecuteQuery(query);

    // No clan? :/
    if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
    begin
      Response.Write('Code=0'+#13);
      PTSQL.CloseConnection;
      PTSQL.Free;
      Exit;
    end;
    clpoint:= PTSQL.GetColumn('BellatraPoints');
    note:= PTSQL.GetColumn('Note');
    clname:= PTSQL.GetColumn('ClanName');
    clanid:= PTSQL.GetColumn('ID');
    codeimg:= PTSQL.GetColumn('IconID');
    leader:= PTSQL.GetColumn('ClanLeader');
    clgold:= PTSQL.GetColumn('BellatraGold');

    PTSQL.CloseConnection;

    // Get player data clan
    PTSQL.CreateConnection('UserDB');
    query:= 'SELECT ClanID, ClanPermission FROM CharacterInfo WHERE Name='+quotedstr(chname);
    PTSQL.ExecuteQuery(query);
    // No data from player?
    if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
    begin
      Response.Write('Code=0' + #124 + 'CClanMoney=0' + #124 + 'CTax=' + IntToStr(BellatraTax) + #124 + 'CName=' + ClName + #124 + 'CNote=' + Note + #124 + 'CZang=' + Leader + #124 + 'CIMG=' + codeimg + #124);
      PTSQL.CloseConnection;
      PTSQL.Free;
      Exit;
    end;
    clanid2:= PTSQL.GetColumn('ClanID');
    clperm:= PTSQL.GetColumn('ClanPermission');
    PTSQL.CloseRS;

    // Clan mismatch?
    if clanid = clanid2 then
    begin
      if leader = chname then
      begin
        Response.Write('Code=1' + #124 + 'CClanMoney=' + clgold + #124 + 'CTax=' + IntToStr(BellatraTax) + #124 + 'CName=' + clname + #124 + 'CNote=' + Note + #124 + 'CZang=' + Leader + #124 + 'CIMG=' + codeIMG + #124 + 'TotalEMoney=' + clgold + #124 + 'TotalMoney=' + clgold + #124);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end else
      if clperm = '2' then
      begin
        Response.Write('Code=2' + #124 + 'CClanMoney=0' + #124 + 'CTax=' + IntToStr(BellatraTax) + #124 + 'CName=' + clname + #124 + 'CNote=' + Note + #124 + 'CZang=' + Leader + #124 + 'CIMG=' + codeIMG + #124 + 'TotalEMoney=' + clgold + #124 + 'TotalMoney=' + clgold + #124);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end else
      begin
        Response.Write('Code=3' + #124 + 'CClanMoney=0' + #124 + 'CTax=' + IntToStr(BellatraTax) + #124 + 'CName=' + clname + #124 + 'CNote=' + Note + #124 + 'CZang=' + Leader + #124 + 'CIMG=' + codeIMG + #124 + 'TotalEMoney=' + clgold + #124 + 'TotalMoney=' + clgold + #124);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end;
    end else
    begin
      PTSQL.CreateConnection('ClanDB');
      query:= 'SELECT ClanLeader FROM ClanList WHERE ID='+clanid2;
      PTSQL.ExecuteQuery(query);

      // No clan?
      if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
      begin
        Response.Write('Code=0'+#13);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end;
      leader2:= PTSQL.GetColumn('ClanLeader');
      PTSQL.CloseRS;

      if leader2 = chname then
      begin
        Response.Write('Code=4' + #124 + 'CClanMoney=0' + #124 + 'CTax=' + IntToStr(BellatraTax) + #124 + 'CName=' + ClName + #124 + 'CNote=' + Note + #124 + 'CZang=' + Leader + #124 + 'CIMG=' + CodeIMG + #124);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end else
      if clperm = '2' then
      begin
        Response.Write('Code=5' + #124 + 'CClanMoney=0' + #124 + 'CTax=' + IntToStr(BellatraTax) + #124 + 'CName=' + ClName + #124 + 'CNote=' + Note + #124 + 'CZang=' + Leader + #124 + 'CIMG=' + CodeIMG + #124);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end else
      begin
        Response.Write('Code=6' + #124 + 'CClanMoney=0' + #124 + 'CTax=' + IntToStr(BellatraTax) + #124 + 'CName=' + ClName + #124 + 'CNote=' + Note + #124 + 'CZang=' + Leader + #124 + 'CIMG=' + CodeIMG + #124);
        PTSQL.CloseConnection;
        PTSQL.Free;
        Exit;
      end;
    end;
  end else
  // Sod Rank
  if index = '3' then
  begin
    query:= 'SELECT TOP 10 * FROM ClanList ORDER BY BellatraPoints DESC';
    PTSQL.ExecuteQuery(query);

    // No clan? :/
    if StrToInt(VarToStr(PTSQL.RecordCount)) = 0 then
    begin
      Response.Write('Code=0');
      PTSQL.CloseConnection;
      PTSQL.Free;
      Exit;
    end;

    strret:= 'Code=1'+#13;
    repeat
      codeimg:= PTSQL.GetColumn('IconID');
      Clname:= PTSQL.GetColumn('ClanName');
      clpoint:= PTSQL.GetColumn('BellatraPoints');
      index:= FormatDateTime('mm/dd/yy',UNIXTimeToDateTime(StrToInt(VarToStr(PTSQL.GetColumn('BellatraDate')))));
      if clpoint <> '0' then
        strret:= strret + 'CIMG=' + codeimg + #13 + 'CName=' + ClName + #13 + 'CPoint=' + clpoint + #13 + 'CRegistDay=' + index + #13;
      PTSQL.NextRow;
    until PTSQL.GetRS.EOF;
    PTSQL.CloseConnection;
    PTSQL.Free;
    Response.Write(strret);
    Exit;
  end;
  Response.Write('Code=104');
end;

function TClan.IsInGame(ticket: integer): Boolean;
var    PTSQL: TSQLPT;
Begin

  PTSQL:= TSQLPT.Create;

  PTSQL.CreateConnection('ServerDB');

  PTSQL.ExecuteQuery('SELECT AccountName FROM UsersOnline WHERE Ticket='+QuotedStr(IntToStr(ticket)));

  Result:= False;
  if StrToInt(VarToStr(PTSQL.RecordCount)) > 0 then
    Result:= True;

  PTSQL.CloseConnection;
  PTSQL.Free;

end;

initialization
  TAutoObjectFactory.Create(ComServer, TClan, Class_Clan,
    ciMultiInstance, tmApartment);
end.
