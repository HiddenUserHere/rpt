unit U_UploadFile;

interface

uses ComObj, ActiveX, AspTlb, WebPT_TLB, Variants;

type
  TUploadFile = class
  private
    ObjUpload: OleVariant;
    pByteRequest: OleVariant;
    pRequest: IRequest;
  public
    procedure Init(req: IRequest); safecall;
    procedure ReadRequest(var TotalBytes: OleVariant); safecall;
    function GetBytesCount : Integer; safecall;
  end;

implementation

uses SysUtils, StrUtils;




{ TUploadFile }



function TUploadFile.GetBytesCount : Integer;
begin
  Result:= pRequest.TotalBytes;
end;

procedure TUploadFile.Init(req: IRequest);
begin
  ObjUpload:= CreateOleObject('Scripting.Dictionary');
  pRequest:= req;
end;

procedure TUploadFile.ReadRequest(var TotalBytes: OleVariant);
var
  PosBeg, PosEnd, boundaryPos: Integer;
  boundary: string;
begin
  pByteRequest:= pRequest.BinaryRead(TotalBytes);
  PosBeg:= 1;
  PosEnd:= PosEx(Chr($13),pByteRequest,PosBeg);
  boundary:= Copy(VartoStr(pByteRequest),PosBeg,PosEnd - PosBeg);
  boundaryPos:= PosEx(boundary,pByteRequest,1);
  boundaryPos:= PosEx(boundary+'-',pByteRequest,1);
  while boundaryPos <> 0 do
  begin
    boundaryPos:= PosEx(boundary+'-',pByteRequest,1);
  end;
end;

end.
