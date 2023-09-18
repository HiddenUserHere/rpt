unit U_SQL;

interface

uses ComObj, ActiveX, AspTlb, WebPT_TLB;

type
  TSQLPT = class
  private
    ObjConn: OleVariant;
    RS:      OleVariant;
    bRSClosed: Boolean;
  public
    procedure AntiSqlI(var s: string); safecall;
    procedure AntiSqlIClan(var s: string); safecall;
    procedure CreateConnection(DB: OleVariant); safecall;
    procedure ExecuteQuery(query: OleVariant; exec: Boolean = False ); safecall;
    procedure CloseConnection; safecall;
    procedure CloseRS; safecall;
    function  GetColumn(column: OleVariant): OleVariant; safecall;
    function  RecordCount: OleVariant; safecall;
    procedure NextRow;
    function GetRS: OleVariant; safecall;
  end;

implementation

uses SysUtils, Variants;



{ TSQLPT }

const
  PCNAME = 'WIN-689E2NV54L9'; //NS382043    // DESKTOP-U8VJKKJ

procedure TSQLPT.AntiSqlI(var s: string);
begin
  s:= StringReplace(s,'/','',[rfReplaceAll,rfIgnoreCase]);
  s:= StringReplace(s,'-','',[rfReplaceAll,rfIgnoreCase]);
  s:= StringReplace(s,chr($27),'',[rfReplaceAll,rfIgnoreCase]);
end;

procedure TSQLPT.AntiSqlIClan(var s: string);
begin
// s:= StringReplace(s,'.','',[rfReplaceAll,rfIgnoreCase]);
  s:= StringReplace(s,'/','',[rfReplaceAll,rfIgnoreCase]);
  s:= StringReplace(s,chr($27),'',[rfReplaceAll,rfIgnoreCase]);
  s:= StringReplace(s,'%','',[rfReplaceAll,rfIgnoreCase]);
end;

procedure TSQLPT.CloseConnection;
begin
  CloseRS;
  ObjConn.Close;
end;

procedure TSQLPT.CloseRS;
begin
  if bRSClosed = False then
  begin
    RS.Close;
    bRSClosed:= True;
  end;
end;

procedure TSQLPT.CreateConnection(DB: OleVariant);
begin
  ObjConn:= CreateOleObject('ADODB.Connection');
  RS:= CreateOleObject('ADODB.RecordSet');
//ObjConn.CursorLocation := 3; // User client
  ObjConn.ConnectionString := 'Provider=MSOLEDBSQL; Data Source=WIN-689E2NV54L9; Initial Catalog='+DB+'; User ID=sa; Password=N0v1nh4';
  ObjConn.Open;
  bRSClosed:= True;
end;

procedure TSQLPT.ExecuteQuery(query: OleVariant; exec: Boolean = False );
begin
  if exec then
    ObjConn.Execute(query)
  else
  begin
    RS.Open(query,ObjConn, 3,1, 1);
    bRSClosed:= False;
  end;
end;

function TSQLPT.GetColumn(column: OleVariant): OleVariant;
begin
  Result:= RS.Fields[column].Value;
end;

function TSQLPT.GetRS: OleVariant;
begin
  Result:= RS;
end;

procedure TSQLPT.NextRow;
begin
  RS.Move(1);
end;

function TSQLPT.RecordCount: OleVariant;
begin
  Result:= RS.RecordCount;
end;

end.
 