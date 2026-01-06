module SegDecoder(iD,iS,oD);
input [3:0] iD; //0~9の4ビット入力
input iS;
output [7:0] oD;
assign oD = ~DecF(iD);
function [7:0] DecF;
  input [3:0] iD;
  begin
    case(iD)
      4'b0000: DecF = {iS,7'b1000000}; // 0
      4'b0001: DecF = {iS,7'b1111001}; // 1
      4'b0010: DecF = {iS,7'b0100100}; // 2
      4'b0011: DecF = {iS,7'b0110000}; // 3
      4'b0100: DecF = {iS,7'b0011001}; // 4
      4'b0101: DecF = {iS,7'b0010010}; // 5
      4'b0110: DecF = {iS,7'b0000010}; // 6
      4'b0111: DecF = {iS,7'b1111000}; // 7
      4'b1000: DecF = {iS,7'b0000000}; // 8
      4'b1001: DecF = {iS,7'b0010000}; // 9
        default: DecF = 8'b11111111; // 消灯
    endcase
  end
endfunction

endmodule

module Decoder(iA, oF);
  input  [1:0] iA;
  output [3:0] oF;

  function [3:0] Dec;
      input [1:0] iA;
      begin
          case(iA)
          2'b00: Dec = 4'b0001;
          2'b01: Dec = 4'b0010;
          2'b10: Dec = 4'b0100;
          2'b11: Dec = 4'b1000;
          default:Dec = 4'bXXXX;
          endcase
      end
  endfunction
  assign oF = Dec(iA);
endmodule
