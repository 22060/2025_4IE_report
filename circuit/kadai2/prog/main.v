module MAIN(iClk,iPoint,iM,iStop,iRst,oD,oD1,oD2,oD3,oD4);
    input iClk; // 27MHz クロック入力
    input iPoint;
    input iM;
    input iRst;
    input iStop;
    output [7:0] oD;
    output oD1;
    output oD2;
    output oD3;
    output oD4;

    
    // wire w1Hz;
    wire w1kHz;
    // wire wd1kHz;`
    wire [3:0] wcnt1;
    wire [3:0] wcnt2;
    wire [3:0] wcnt3;
    wire [3:0] wcnt4;
    wire [3:0] wcnt11;
    wire [3:0] wcnt21;
    wire [3:0] wcnt31;
    wire [3:0] wcnt41;
    wire [3:0] wcnt12;
    wire [3:0] wcnt22;
    wire [3:0] wcnt32;
    wire [3:0] wcnt42;

    wire [3:0] wcnt;
    wire [1:0] wsw;
    wire [1:0] wsp;
    wire ws;
    assign ws = (wsw == wsp )? 1'b0 : 1'b1;

    // d4c u1(.iClk(iClk), .oClk(w1Hz));
    d4dl u2(.iClk(iClk), .oClk(w1kHz));
    Count4sw usw(.iClk(w1kHz), .oSw(wsw));
    Count4sw usw2(.iClk(iPoint), .oSw(wsp));
    Counter u3(.iClk(w1kHz), .iRst(iRst),.iStop(~iStop), .iP(wsp),.oCnt1(wcnt11), .oCnt2(wcnt21), .oCnt3(wcnt31), .oCnt4(wcnt41));
    DeCounter u3d(.iClk(w1kHz), .iRst(iRst), .iStop(~iStop),.iP(wsp),.oCnt1(wcnt12), .oCnt2(wcnt22), .oCnt3(wcnt32), .oCnt4(wcnt42));
    assign wcnt1 = iM == 1'b0 ? wcnt11 : wcnt12;
    assign wcnt2 = iM == 1'b0 ? wcnt21 : wcnt22;
    assign wcnt3 = iM == 1'b0 ? wcnt31 : wcnt32;
    assign wcnt4 = iM == 1'b0 ? wcnt41 : wcnt42;
    Sel u4(.iA(wcnt1), .iB(wcnt2),.iC(wcnt3),.iD(wcnt4), .iSw(wsw), .oOut(wcnt));
    SegDecoder u5(.iD(wcnt),.iS(ws), .oD(oD));
    Decoder u6(.iA(wsw), .oF({oD4,oD3,oD2,oD1}));
endmodule