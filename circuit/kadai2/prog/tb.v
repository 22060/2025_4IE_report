
`timescale 10ns / 1ps
module MAINTEST;

reg iClk = 0;
reg iRst = 0;
reg iPoint = 0;
reg iM = 0;
reg iStop = 0;

wire [7:0] oD;
wire oD1;
wire oD2;
wire oD3;
wire oD4;

MAIN main_inst(
    iClk,
    iPoint,
    iM,
    iStop,
    iRst,
    oD,
    oD1,
    oD2,
    oD3,
    oD4
);

// クロック生成（500MHz）
always #1 iClk = ~iClk;

initial begin
    // 初期化
    iClk   = 0;
    iRst   = 1;
    iPoint = 0;
    iM     = 0;
    iStop  = 0;

    // 波形ダンプ
    $dumpfile("main.vcd");
    $dumpvars(0, MAINTEST);
    #1  iRst = 1;
//     #1 iM=1;
    #1 iStop = 0;
    #1 iPoint = 1;
    #1 iPoint = 0;
    #1 iPoint = 1;
    #1 iPoint = 0;
    #1 iPoint = 1;
    #1 iPoint = 0;
    #1 iStop = 1;
    #1000;

    $finish;
end

endmodule
