unit U_Ranking;

{$WARN SYMBOL_PLATFORM OFF}

interface

uses
  ComObj, ActiveX, AspTlb, WebPT_TLB, StdVcl, U_SQL, sysutils, variants;

type
  TRanking = class(TASPObject, IRanking)
  protected
    procedure OnEndPage; safecall;
    procedure OnStartPage(const AScriptingContext: IUnknown); safecall;
    procedure Index; safecall;
    procedure Top10; safecall;
    procedure RankingPVP; safecall;
    function GetJobCode(name: string): integer; safecall;
    function GetLevel(name: string): integer; safecall;
    function GetClan(name: string): string; safecall;
    function GetClanIconID(clname: string): integer; safecall;
  end;

implementation

uses ComServ;

const
  TOPCOUNT = 10;
  TOPCOUNTPVP = 10;

procedure TRanking.OnEndPage;
begin
  inherited OnEndPage;
end;

procedure TRanking.OnStartPage(const AScriptingContext: IUnknown);
begin
  inherited OnStartPage(AScriptingContext);
end;

procedure TRanking.Index;
begin

end;

procedure TRanking.Top10;
var PTSQL, PTSQL2: TSQLPT;
    id: integer;
    level, name, jobcode, userid: string;
begin
  id:= 1;
  Response.Write('<table style="font-family: Verdana; font-size: 13px; color: #5f5f5f;">');
  Response.Write('<tr>');
  Response.Write('<th style="width: 5%;">#</th>');
  Response.Write('<th></th>');
  Response.Write('<th>Character Name</th>');
  Response.Write('<th>Level</th>');

  Response.Write('</tr>');
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  PTSQL2:= TSQLPT.Create;
  PTSQL2.CreateConnection('UserDB');
  PTSQL.ExecuteQuery('SELECT [Level], Name, Jobcode, UserID FROM CharacterInfo ORDER BY [Level] DESC');
  if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    repeat
      userid:= PTSQL.GetColumn('UserID');
      PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(userid)+') AND (Banned=0) AND (GameMasterType=0)');
      if Strtoint(VarToStr(PTSQL2.RecordCount)) > 0 then
      begin
        name:= PTSQL.GetColumn('Name');
        jobcode:= PTSQL.GetColumn('JobCode');
        level:= PTSQL.GetColumn('Level');
        Response.Write('<tr align=center>');
        Response.Write('<td>'+IntToStr(id)+'</td>');
        Response.Write('<td><img src="/images/'+jobcode+'.gif"></img></td>');
        Response.Write('<td>'+name+'</td>');
        Response.Write('<td>'+level+'</td>');
        Response.Write('</tr>');
        id:= id+1;
      end;
      PTSQL2.CloseRS;
      PTSQL.NextRow;
      IF id = TOPCOUNT+1 then break;
    until PTSQL.GetRS.EOF;
  end;

  PTSQL2.CloseConnection;
  PTSQL2.Free;
  PTSQL.CloseConnection;
  PTSQL.Free;
  Response.Write('</table>');
end;

procedure TRanking.RankingPVP;
var PTSQL: TSQLPT;
    id: integer;
    name, jobcode, kills, deaths, killstreak, lvl, clname, iconcl: string;
begin
  id:= 1;
  Response.Write('<center><table style="font-family: Verdana; font-size: 13px; color: #5f5f5f;">');
  Response.Write('<tr>');
  Response.Write('<th style="width: 5%;">#</th>');
  Response.Write('<th>Class</th>');
  Response.Write('<th>Clan</th>');
  Response.Write('<th>Character Name</th>');
  Response.Write('<th>Level</th>');
  Response.Write('<th>Kills</th>');
  Response.Write('<th>Deaths</th>');
  Response.Write('<th>KillStreak</th>');

  Response.Write('</tr>');
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  PTSQL.ExecuteQuery('SELECT CharName, Kills, Deaths, KillStreak FROM CharacterPvP ORDER BY Kills DESC');
  if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
  begin
    repeat
        name:= PTSQL.GetColumn('CharName');
        kills:= PTSQL.GetColumn('Kills');
        deaths:= PTSQL.GetColumn('Deaths');
        killstreak:= PTSQL.GetColumn('KillStreak');
        jobcode:= IntToStr(GetJobCode(name));
        lvl:= IntToStr(GetLevel(name));
        clname:= GetClan(name);
        iconcl:= '0';
        if clname <> '' then
          iconcl:= IntToStr(GetClanIconID(clname));

        Response.Write('<tr align=center>');
        Response.Write('<td>'+IntToStr(id)+'</td>');
        if jobcode='0' then
          Response.Write('<td></td>')
        else
          Response.Write('<td><img src="/images/'+jobcode+'.gif"></img></td>');

        if iconcl <> '0' then
          Response.Write('<td><img src="/ClanImage/'+iconcl+'.bmp" alt="'+clname+'" height="20" width="20"></img></td>')
        else
          Response.Write('<td></td>');
          
        Response.Write('<td>'+name+'</td>');
        Response.Write('<td>'+lvl+'</td>');
        Response.Write('<td>'+kills+'</td>');
        Response.Write('<td>'+deaths+'</td>');
        Response.Write('<td>'+killstreak+'</td>');
        Response.Write('</tr>');
        id:= id+1;
        PTSQL.NextRow;
        if id = TOPCOUNTPVP+1 then Break;
    until PTSQL.GetRS.EOF;
  end;

  PTSQL.CloseConnection;
  PTSQL.Free;
  Response.Write('</table></center>');
end;

function TRanking.GetJobCode(name: string): integer;
var PTSQL: TSQLPT;
    i: Integer;
    spec: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  i:= 0;
  spec:= '0';
  PTSQL.ExecuteQuery('SELECT JobCode FROM CharacterInfo WHERE Name='+quotedstr(name));

  if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
    spec:= PTSQL.GetColumn('JobCode');

  i:= StrToInt(spec);
  PTSQL.CloseConnection;
  PTSQL.Free;
  Result:= i;
end;

function TRanking.GetLevel(name: string): integer;
var PTSQL: TSQLPT;
    i: Integer;
    lvl: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  i:= 0;
  lvl:= '0';
  PTSQL.ExecuteQuery('SELECT [Level] FROM CharacterInfo WHERE Name='+quotedstr(name));

  if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
    lvl:= PTSQL.GetColumn('Level');

  i:= StrToInt(lvl);
  PTSQL.CloseConnection;
  PTSQL.Free;
  Result:= i;
end;

function TRanking.GetClan(name: string): string;
var PTSQL: TSQLPT;
    i: Integer;
    clanid, clname: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  i:= 0;
  clanid:= '0';
  clname:= '';
  PTSQL.ExecuteQuery('SELECT ClanID FROM CharacterInfo WHERE Name='+quotedstr(name));

  if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
    clanid:= PTSQL.GetColumn('ClanID');

  PTSQL.CloseConnection;

  if clanid<>'0' then begin
    PTSQL.CreateConnection('ClanDB');

    PTSQL.ExecuteQuery('SELECT ClanName FROM ClanList WHERE ID='+clanid);
    if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
      clname:= PTSQL.GetColumn('ClanName');

    PTSQL.CloseConnection;
  end;
  PTSQL.Free;
  Result:= clname;
end;

function TRanking.GetClanIconID(clname: string): integer;
var PTSQL: TSQLPT;
    i: Integer;
    clanid: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ClanDB');
  i:= 0;
  clanid:= '0';
  PTSQL.ExecuteQuery('SELECT IconID FROM ClanList WHERE ClanName='+quotedstr(clname));

  if Strtoint(VarToStr(PTSQL.RecordCount)) > 0 then
    clanid:= PTSQL.GetColumn('IconID');

  PTSQL.CloseConnection;
  PTSQL.Free;
  result:= StrToInt(clanid);
end;

initialization
  TAutoObjectFactory.Create(ComServer, TRanking, Class_Ranking,
    ciMultiInstance, tmBoth);
end.
