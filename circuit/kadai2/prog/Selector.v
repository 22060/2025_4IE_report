module Sel(iA,iB,iC,iD,iSw,oOut);
  input [3:0]iA;
  input [3:0]iB;
  input [3:0]iC;
  input [3:0]iD;
  input [1:0]iSw;
  output [3:0]oOut;
  assign oOut = SelF(iA,iB,iC,iD,iSw);
  function [3:0] SelF;
    input [3:0] iA;
    input [3:0] iB;
    input [3:0] iC;
    input [3:0] iD;
    input [1:0] iSw;
  begin
    case(iSw)
        2'b00: SelF = iA;
        2'b01: SelF = iB;
        2'b10: SelF = iC;
        2'b11: SelF = iD;
      default: SelF=4'bXXXX;
    endcase
  end
  endfunction
endmodule
