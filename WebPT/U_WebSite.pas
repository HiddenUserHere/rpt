unit U_WebSite;

{$WARN SYMBOL_PLATFORM OFF}

interface

uses
  ComObj, ActiveX, AspTlb, WebPT_TLB, StdVcl, U_SQL, Variants;

type
  TWebSite = class(TASPObject, IWebSite)
  protected
    procedure OnEndPage; safecall;
    procedure OnStartPage(const AScriptingContext: IUnknown); safecall;
    procedure OnListNewsPage; safecall;
    procedure EndPage; safecall;
    procedure Index; safecall;
    procedure StartPage; safecall;
    procedure OnViewNewsPage; safecall;
    procedure OnNextMaintenancePage; safecall;
    procedure OnAddNoticePage; safecall;
    procedure OnAddNoticePageHandle; safecall;
    function IsNumeric(S : String) : Boolean; safecall;
    procedure OnListAllNewsPage; safecall;
    procedure OnServerTime; safecall;
    procedure OnServerTimeSite; safecall;
    procedure OnLeaderSodPage; safecall;
    procedure OnRankingsPage; safecall;
    procedure OnLeaderBCPage; safecall;
    procedure OnLeaderPvPPage; safecall;
  end;

implementation

uses ComServ, SysUtils;

procedure TWebSite.OnEndPage;
begin
  inherited OnEndPage;
end;

procedure TWebSite.OnStartPage(const AScriptingContext: IUnknown);
begin
  inherited OnStartPage(AScriptingContext);
end;

procedure TWebSite.OnListNewsPage;
var PTSQL : TSQLPT;
    imgtype : string;
begin

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('WebDB');

  PTSQL.ExecuteQuery('SELECT TOP 8 * FROM LatestNews ORDER BY ID DESC' );

  Response.Write('<!-- Notices -->');
  Response.Write('<article class="notices">');
  Response.Write('<div class="title">news<span style="float: right; font-weight: 400; font-size: 11px; padding-top: 3px;"><a href="?page=listallnews">+ SEE ALL NEWS</a></span></div>');
  Response.Write('  <table id="news" width="100%">');

  repeat
    if PTSQL.GetColumn('Type') = 0 then
      imgtype:= 'notice'
    else if PTSQL.GetColumn('Type') = 1 then
      imgtype:= 'event'
    else if PTSQL.GetColumn('Type') = 3 then
      imgtype:= 'maint'
    else
      imgtype:= 'patch';

    Response.Write('  	<tr>');
    Response.Write('  		<td width="10%" style="text-align: center"><img src="../images/'+imgtype+'.png"></td>');
    Response.Write('  		<td width="70%"><a href="?page=viewnew&id='+VarToStr(PTSQL.GetColumn('ID'))+'">'+VarToStr(PTSQL.GetColumn('Title'))+'</a></td>');
    Response.Write('  		<td style="color: #a5a5a5; text-align: right; padding-right: 10px"><span class="glyphicon glyphicon-time"></span> '+VarToStr(PTSQL.GetColumn('Date'))+'</td>');
    Response.Write('  	</tr>');

    PTSQL.NextRow();
  until PTSQL.GetRS.EOF;

  Response.Write('  </table>');
  Response.Write('</article>');

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TWebSite.EndPage;
begin
Response.Write('</body>');
Response.Write('');
Response.Write('</html>');
end;

procedure TWebSite.Index;
var pag : string;
begin
  StartPage;

  pag:= Request.QueryString.Item['page'];
  if pag = 'listnews' then
  begin
    OnListNewsPage;
  end
  else if pag = 'nextmaint' then
  begin
    OnNextMaintenancePage;
  end
  else if pag = 'listallnews' then
  begin
    OnListAllNewsPage;
  end
  else if pag = 'servertime' then
  begin
    OnServerTime;
  end
  else if pag = 'servertimenew' then
  begin
    OnServerTimeSite;
  end
  else if pag = 'leadersod' then
  begin
    OnLeaderSodPage;
  end
  else if pag = 'leaderbc' then
  begin
    OnLeaderBCPage;
  end
  else if pag = 'rankings' then
  begin
    OnRankingsPage;
  end
  else if pag = 'viewnew' then
  begin
    OnViewNewsPage;
  end
  else if pag = 'leaderpvp' then
  begin
    OnLeaderPvPPage;
  end;

  EndPage;
end;

procedure TWebSite.StartPage;
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
Response.Write('');
Response.Write('</head>');
Response.Write('');
Response.Write('<body>');
Response.Write('');
end;

procedure TWebSite.OnViewNewsPage;
var PTSQL : TSQLPT;
    newid, imgtype : string;
begin

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('WebDB');

  newid:= Request.QueryString.Item['id'];

  if IsNumeric(newid) = True then
  begin
    PTSQL.ExecuteQuery('SELECT * FROM LatestNews WHERE ID='+newid+'');
    if PTSQL.RecordCount > 0 then begin

    if PTSQL.GetColumn('Type') = 0 then
      imgtype:= 'notice'
    else if PTSQL.GetColumn('Type') = 1 then
      imgtype:= 'event'
    else if PTSQL.GetColumn('Type') = 3 then
      imgtype:= 'maint'
    else
      imgtype:= 'patch';

    Response.Write('<!-- Notices -->');
    Response.Write('<article class="notices">');
    Response.Write('  <div class="title">news</div>');

    Response.Write('<div style="padding: 20px">');
    Response.Write('  	<h4 style="margin: 0"><img style="margin-top: -5px; margin-right: 5px;" src="http://www.fortresspt.net/images/'+imgtype+'.png">'+VarToStr(PTSQL.GetColumn('title'))+'</h4>');
    Response.Write('  	<span style="color: rgb(170, 170, 170);">Posted in '+VarToStr(PTSQL.GetColumn('date'))+'</span>');
    Response.Write('  	<br><br>');
    Response.Write('  	<div>');
    Response.Write(PTSQL.GetColumn('message'));
    Response.Write('  	</div>');

    Response.Write('</div></article>');
    end;
  end;
  PTSQL.CloseConnection;
  PTSQL.Free;
end;

procedure TWebSite.OnNextMaintenancePage;
var PTSQL : TSQLPT;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('WebDB');

  PTSQL.ExecuteQuery('SELECT TOP 1 * FROM NextMaintenance ORDER BY ID DESC');
  Response.Write(PTSQL.GetColumn('Message'));

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TWebSite.OnAddNoticePage;
begin

end;

procedure TWebSite.OnAddNoticePageHandle;
begin

end;

function TWebSite.IsNumeric(S: String): Boolean;
var
  i: integer;
begin
Result := TryStrToInt(s, i);
end;

procedure TWebSite.OnListAllNewsPage;
var PTSQL : TSQLPT;
    imgtype : string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('WebDB');

  PTSQL.ExecuteQuery('SELECT * FROM LatestNews ORDER BY ID DESC' );

  Response.Write('<!-- Notices -->');
  Response.Write('<article class="notices">');
  Response.Write('<div class="title">list news</div>');
  Response.Write('  <table id="news" width="100%">');

  repeat
    if PTSQL.GetColumn('Type') = 0 then
      imgtype:= 'notice'
    else if PTSQL.GetColumn('Type') = 1 then
      imgtype:= 'event'
    else if PTSQL.GetColumn('Type') = 3 then
      imgtype:= 'maint'
    else
      imgtype:= 'patch';

    Response.Write('  	<tr>');
    Response.Write('  		<td width="10%" style="text-align: center"><img src="../images/'+imgtype+'.png"></td>');
    Response.Write('  		<td width="70%"><a href="?page=viewnew&id='+VarToStr(PTSQL.GetColumn('ID'))+'">'+VarToStr(PTSQL.GetColumn('Title'))+'</a></td>');
    Response.Write('  		<td style="color: #a5a5a5; text-align: right; padding-right: 10px"><span class="glyphicon glyphicon-time"></span> '+VarToStr(PTSQL.GetColumn('Date'))+'</td>');
    Response.Write('  	</tr>');

    PTSQL.NextRow();
  until PTSQL.GetRS.EOF;

  Response.Write('  </table>');
  Response.Write('</article>');

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TWebSite.OnServerTime;
var PTSQL : TSQLPT;
    myDate : TDateTime;
begin

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ServerDB');

  PTSQL.ExecuteQuery('SELECT * FROM Maintenance WHERE Mode=3');

  if PTSQL.RecordCount > 0 then
  begin
    myDate:= Now();
    Response.Write('<div style="width: 100%; text-align: right;"><span style="text-align: right; font-family: Verdana; font-size: 13px; color: #c8c8c8;">');

    if PTSQL.GetColumn('IP') = '0' then begin
      Response.Write('<span style="color: #40ab40; padding-right: 5px">Server Online</span>')
    end
    else begin
      Response.Write('<span style="color: #a93030; padding-right: 5px">Maintenance!</span>');
    end;

    Response.Write(''+FormatDateTime('t', myDate)+'</span></div>');
  end;

  PTSQL.CloseConnection();
  PTSQL.Free();

end;

procedure TWebSite.OnLeaderSodPage;
var PTSQL : TSQLPT;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ClanDB');

  PTSQL.ExecuteQuery('SELECT TOP 1 * FROM ClanList ORDER BY BellatraPoints DESC');
  Response.Write('<img style="margin-top: -2px; margin-right: 5px;" src="http://192.99.160.211/ClanImage/'+VarToStr(PTSQL.GetColumn('IconID'))+'.bmp" width="16">');
  Response.Write(PTSQL.GetColumn('ClanName'));

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

procedure TWebSite.OnRankingsPage;
var page, classch, account, clanid, namech : string;
    id : Integer;
    PTSQL, PTSQL2, PTSQLC : TSQLPT;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');

  PTSQL2:= TSQLPT.Create;
  PTSQL2.CreateConnection('UserDB');

  PTSQLC:= TSQLPT.Create;
  PTSQLC.CreateConnection('ClanDB');

  classch:= Request.QueryString.Item['class'];
  page:= Request.QueryString.Item['type'];
  
  if (IsNumeric(page) = True) AND (IsNumeric(classch) = True) then begin
    Response.Write('<center>');

    Response.Write('<style type="text/css">');
    Response.Write('                                body{ ');
    Response.Write('                                    font-family: "HelveticaNeueW02-67MdCn 694060","HelveticaNeueW10-67MdCn 777270","Helvetica Neue",Helvetica,Arial,sans-serif; ');
    Response.Write('                                    font-size: 12px;');
    Response.Write('                                    color: #525252; ');
    Response.Write('                                    }');
    Response.Write('                                    /* Normal, standard links. */');
    Response.Write('                                    a:link, a:visited');
    Response.Write('                                    {');
    Response.Write('                                        color: #7f7567;');
    Response.Write('                                        text-decoration: none;');
    Response.Write('                                    }');
    Response.Write('                                    a:hover');
    Response.Write('                                    {');
    Response.Write('                                        color: #38342f;');
    Response.Write('                                        cursor: pointer;');
    Response.Write('                                    }');
    Response.Write('                            </style>');

    //Player Level Ranking
    if StrToInt(page) = 0 then begin

      Response.Write('<a href="?page=rankings&type=0&class=0">All</a> - <a href="?page=rankings&type=0&class=1">Fighter</a> - <a href="?page=rankings&type=0&class=2">Mechanician</a> - ');
      Response.Write('<a href="?page=rankings&type=0&class=3">Archer</a> - <a href="?page=rankings&type=0&class=4">Pikeman</a> - <a href="?page=rankings&type=0&class=5">Atalanta</a> - ');
      Response.Write('<a href="?page=rankings&type=0&class=6">Knight</a> - <a href="?page=rankings&type=0&class=7">Magician</a> - <a href="?page=rankings&type=0&class=8">Priestess</a>');
      Response.Write(' - <a href="?page=rankings&type=0&class=9">Assassin</a> - <a href="?page=rankings&type=0&class=10">Shaman</a>');

      Response.Write('<br><br>');

      Response.Write('<table>');
      Response.Write('                                <tr>');
      Response.Write('                                    <td width="64px">#</td>');
      Response.Write('                                    <td width="214px">Character Name</td>');
      Response.Write('                                    <td width="43px"></td>');
      Response.Write('                                    <td width="43px"></td>');
      Response.Write('                                    <td width="64px">Level</td>');
      Response.Write('                                </tr>');

      if classch = '0' then begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 20+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '1' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=1 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '2' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=2 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '3' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=3 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '4' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=4 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '5' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=5 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '6' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=6 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '7' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=7 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '8' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=8 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '9' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=9 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end
      else if classch = '10' then
      begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE JobCode=10 ORDER BY Level DESC');
        id:= 1;
        repeat
           account:= PTSQL.GetColumn('UserID');
           clanid:= PTSQL.GetColumn('ClanID');

           PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
           PTSQL2.ExecuteQuery('SELECT UserID FROM UserInfo WHERE (UserID='+QUOTEDSTR(account)+') AND (Banned=0) AND (GameMasterType=0)');
           if PTSQL2.RecordCount > 0 then begin
              Response.Write('<tr>');
              Response.Write('<td>'+IntToStr(id)+'</td>');
              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Name'))+'</td>');
              Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');

              if PTSQLC.RecordCount > 0 then begin
                Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
              end
              else begin
                Response.Write('<td></td>');
              end;

              Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Level'))+'</td>');

              id:= id + 1;
           end;

          PTSQLC.CloseRS;
          PTSQL2.CloseRS;
          PTSQL.NextRow;

          IF id = 10+1 then break;

        until PTSQL.GetRS.EOF;
      end;

      Response.Write('                            </table>');
    end
    //Bellatra Player Ranking
    else if StrToInt(page) = 1 then begin
      Response.Write('<br><br>');

      Response.Write('<table>');
      Response.Write('                                <tr>');
      Response.Write('                                    <td width="54px">#</td>');
      Response.Write('                                    <td width="214px">Character Name</td>');
      Response.Write('                                    <td width="43px"></td>');
      Response.Write('                                    <td width="70px">Kills</td>');
      Response.Write('                                    <td width="90px">Score</td>');
      Response.Write('                                    <td width="150px">Date</td>');
      Response.Write('                                </tr>');

      if classch = '0' then begin
        PTSQLC.ExecuteQuery('SELECT * FROM BellatraTeamScore ORDER BY Score DESC');
        id:= 1;

        if PTSQLC.RecordCount > 0 then begin
          repeat

            Response.Write('<tr><td>'+IntToStr(id)+'</td>');
            Response.Write('<td>'+VarToStr(PTSQLC.GetColumn('CharName'))+'</td>');

            namech:= PTSQLC.GetColumn('CharName');

            PTSQL.ExecuteQuery('SELECT * FROM CharacterInfo WHERE Name='+QuotedStr(namech)+'');
            if PTSQL.RecordCount > 0 then begin
               Response.Write('<td><img src="images/'+VarToStr(PTSQL.GetColumn('JobCode'))+'.gif"></td>');
            end;
            PTSQL.CloseRS;

            Response.Write('<td>'+VarToStr(PTSQLC.GetColumn('Kills'))+'</td>');
            Response.Write('<td>'+VarToStr(PTSQLC.GetColumn('Score'))+'</td>');
            Response.Write('<td>'+VarToStr(PTSQLC.GetColumn('Date'))+'</td>');
            Response.Write('</tr>');

            id:= id + 1;
            PTSQLC.NextRow;

            IF id = 20+1 then break;

          until PTSQLC.GetRS.EOF;
        end;
      end;
      Response.Write('                            </table>');
    end
    //Bellatra Clan Ranking
    else if StrToInt(page) = 2 then begin

    end
    //PVP Player Ranking
    else if StrToInt(page) = 3 then begin
      Response.Write('<a href="?page=rankings&type=3&class=0">All</a> - <a href="?page=rankings&type=3&class=1">Fighter</a> - <a href="?page=rankings&type=3&class=2">Mechanician</a> - ');
      Response.Write('<a href="?page=rankings&type=3&class=3">Archer</a> - <a href="?page=rankings&type=3&class=4">Pikeman</a> - <a href="?page=rankings&type=3&class=5">Atalanta</a> - ');
      Response.Write('<a href="?page=rankings&type=3&class=6">Knight</a> - <a href="?page=rankings&type=3&class=7">Magician</a> - <a href="?page=rankings&type=3&class=8">Priestess</a>');

      Response.Write('<br><br>');

      Response.Write('<table>');
      Response.Write('                                <tr>');
      Response.Write('                                    <td width="64px">#</td>');
      Response.Write('                                    <td width="214px">Character Name</td>');
      Response.Write('                                    <td width="43px"></td>');
      Response.Write('                                    <td width="43px"></td>');
      Response.Write('                                    <td width="64px">Level</td>');
      Response.Write('                                    <td width="64px">Kills</td>');
      Response.Write('                                    <td width="64px">Deaths</td>');
      Response.Write('                                    <td width="64px">Kills Streak</td>');
      Response.Write('                                </tr>');

      if classch = '0' then begin
        PTSQL.ExecuteQuery('SELECT * FROM CharacterPvP ORDER BY Kills DESC');
        id:= 1;
        repeat

          Response.Write('<tr><td>'+IntToStr(id)+'</td>');
          Response.Write('<td>'+VarToStr(PTSQL.GetColumn('CharName'))+'</td>');

          namech:= PTSQL.GetColumn('CharName');
          PTSQL2.ExecuteQuery('SELECT * FROM CharacterInfo WHERE Name='+QuotedStr(namech)+'');

          if PTSQL2.RecordCount > 0 then begin
            clanid:= PTSQL2.GetColumn('ClanID');

            PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE ID='+clanid+'');
            
            if PTSQLC.RecordCount > 0 then begin
              Response.Write('<td><img alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" width="16px" src="ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp"></td>');
            end
            else begin
              Response.Write('<td></td>');
            end;

            PTSQLC.CloseRS;
          end;

          Response.Write('<td><img src="images/'+VarToStr(PTSQL2.GetColumn('JobCode'))+'.gif"></td>');
          Response.Write('<td>'+VarToStr(PTSQL2.GetColumn('Level'))+'</td>');
          PTSQL2.CloseRS;

          Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Kills'))+'');
          Response.Write('<td>'+VarToStr(PTSQL.GetColumn('Deaths'))+'');
          Response.Write('<td>'+VarToStr(PTSQL.GetColumn('KillStreak'))+'');

          PTSQL.NextRow;
          id:= id + 1;

          IF id = 20+1 then break;

        until PTSQL.GetRS.EOF;
      end;
      Response.Write('                            </table>');
    end;
  end;

  PTSQL.CloseConnection();
  PTSQL.Free();
  PTSQLC.CloseConnection;
  PTSQLC.Free;
  PTSQL2.CloseConnection;
  PTSQL2.Free;
end;

procedure TWebSite.OnLeaderBCPage;
var PTSQL, PTSQLC : TSQLPT;
    clanid : string;
begin
  PTSQLC:= TSQLPT.Create;
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ServerDB');
  PTSQL.ExecuteQuery('SELECT TOP 1 * FROM BlessCastleSettings');

  if PTSQL.RecordCount > 0 then begin
    clanid:= PTSQL.GetColumn('ClanID');

    PTSQLC.CreateConnection('ClanDB');
    PTSQLC.ExecuteQuery('SELECT * FROM ClanList WHERE IconID='+QuotedStr(clanid)+'');

    if PTSQLC.RecordCount > 0 then begin
      Response.Write('<img style="margin-top: -2px; margin-right: 5px;" src="http://192.99.160.211/ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp" width="16">');
      Response.Write(PTSQLC.GetColumn('ClanName'));
    end;
  end;

  PTSQL.CloseConnection();
  PTSQL.Free();

  PTSQLC.CloseConnection();
  PTSQLC.Free();

end;

procedure TWebSite.OnLeaderPvPPage;
var PTSQL, PTSQLC : TSQLPT;
    clanid, chname, clanimg, kills: string;
begin
  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('UserDB');
  PTSQLC:= TSQLPT.Create;
  PTSQLC.CreateConnection('ClanDB');

  clanid:= '';

  PTSQL.ExecuteQuery('SELECT TOP 1 CharacterName, Kills FROM CharacterPvP WHERE (Kills > 0) ORDER BY Kills DESC');

  if PTSQL.RecordCount > 0 then begin

   chname:= PTSQL.GetColumn('CharacterName');

   kills:= PTSQL.GetColumn('Kills');

   PTSQL.CloseRS;

   PTSQL.ExecuteQuery('SELECT ClanID FROM CharacterInfo WHERE Name='+quotedstr(chname));

   if PTSQL.RecordCount > 0 then begin
    clanid:= PTSQL.GetColumn('ClanID');

    PTSQLC.ExecuteQuery('SELECT ClanName, IconID FROM ClanList WHERE ID='+QuotedStr(clanid));

    if PTSQLC.RecordCount > 0 then begin

      Response.Write('<img style="margin-top: -2px; margin-right: 5px;" alt="'+VarToStr(PTSQLC.GetColumn('ClanName'))+'" src="http://192.99.160.211/ClanImage/'+VarToStr(PTSQLC.GetColumn('IconID'))+'.bmp" width="16">');
      Response.Write(chname+' ('+kills+' Kills)');
    end else
    begin
      Response.Write(chname+' ('+kills+' Kills)');
    end;
   end;
  end;

  PTSQL.CloseConnection();
  PTSQL.Free();

  PTSQLC.CloseConnection();
  PTSQLC.Free();
end;

procedure TWebSite.OnServerTimeSite;
var PTSQL : TSQLPT;
    myDate : TDateTime;
begin

  PTSQL:= TSQLPT.Create;
  PTSQL.CreateConnection('ServerDB');

  PTSQL.ExecuteQuery('SELECT * FROM Maintenance WHERE Mode=3');

  Response.Write('<div class="status">');

  if PTSQL.RecordCount > 0 then
  begin
    myDate:= Now();

    if PTSQL.GetColumn('IP') = '0' then begin
      Response.Write('<p><img src="img/status_on.png" alt="Fortress Priston Tale is Online">Server Online</p>');
    end
    else begin
      Response.Write('<p><img src="img/status_off.png" alt="Fortress Priston Tale is in Maintenance">Server Maintenance</p>');
    end;
    Response.Write('<p class="server-time">Server Time: <time datetime="'+FormatDateTime('t', myDate)+'">'+FormatDateTime('t', myDate)+'</time></p>');
  end;

  Response.Write('</div>');

  PTSQL.CloseConnection();
  PTSQL.Free();
end;

initialization
  TAutoObjectFactory.Create(ComServer, TWebSite, Class_WebSite,
    ciMultiInstance, tmApartment);
end.
