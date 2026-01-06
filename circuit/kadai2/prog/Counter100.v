module Counter(iClk,iRst,iStop,iP,oCnt1,oCnt2,oCnt3,oCnt4);
    input iClk;
    input iRst;
    input iStop;
    input [1:0] iP;
    output [3:0] oCnt1;
    output [3:0] oCnt2;
    output [3:0] oCnt3;
    output [3:0] oCnt4;
    reg [3:0] cnt1 = 4'd0;
    reg [3:0] cnt2 = 4'd0;
    reg [3:0] cnt3 = 4'd0;
    reg [3:0] cnt4 = 4'd0;
    reg [3:0] cnt5 = 4'd0;
    reg [3:0] cnt6 = 4'd0;
    reg [3:0] cnt7 = 4'd0;
    reg [3:0] cnt8 = 4'd0;

    assign oCnt1 = iP == 2'b00 ? cnt4 :
                   iP == 2'b01 ? cnt3 :
                   iP == 2'b10 ? cnt2 : cnt1;
    assign oCnt2 = iP == 2'b00 ? cnt5 :
                   iP == 2'b01 ? cnt4 :
                   iP == 2'b10 ? cnt3 : cnt2;
    assign oCnt3 = iP == 2'b00 ? cnt6 :
                   iP == 2'b01 ? cnt5 :
                   iP == 2'b10 ? cnt4 : cnt3;
    assign oCnt4 = iP == 2'b00 ? cnt7 :
                   iP == 2'b01 ? cnt6 :
                   iP == 2'b10 ? cnt5 : cnt4;

    always @(posedge iClk) begin
        if (~iRst) begin
            cnt1 <= 4'd0;
            cnt2 <= 4'd0;
            cnt3 <= 4'd0;
            cnt4 <= 4'd0;
            cnt5 <= 4'd0;
            cnt6 <= 4'd0;
            cnt7 <= 4'd0;
            cnt8 <= 4'd0;
        end
        else if (iStop) begin
            cnt1 <= cnt1;
            cnt2 <= cnt2;
            cnt3 <= cnt3;
            cnt4 <= cnt4;
            cnt5 <= cnt5;
            cnt6 <= cnt6;
            cnt7 <= cnt7;
            cnt8 <= cnt8;
        end else if (cnt1 == 4'd9) begin
            cnt1 <= 4'd0;
            if (cnt2 == 4'd9) begin
                cnt2 <= 4'd0;
                if (cnt3 == 4'd9) begin
                    cnt3 <= 4'd0;
                    if (cnt4 == 4'd9) begin
                        cnt4 <= 4'd0;
                        if (cnt5 == 4'd9) begin
                            cnt5 <= 4'd0;
                            if (cnt6 == 4'd9) begin
                                cnt6 <= 4'd0;
                                if (cnt7 == 4'd9) begin
                                    cnt7 <= 4'd0;
                                    if (cnt8 == 4'd9) begin
                                        cnt8 <= 4'd0;
                                    end
                                    else begin
                                        cnt8 <=cnt8 + 4'd1;
                                    end
                                end
                                else begin
                                    cnt7 <=cnt7 + 4'd1;
                                end
                            end
                            else begin
                                cnt6 <=cnt6 + 4'd1;
                            end
                        end
                        else begin
                            cnt5 <=cnt5 + 4'd1;
                        end
                    end
                    else begin
                        cnt4 <=cnt4 + 4'd1;
                    end
                end
                else begin
                    cnt3 <=cnt3 + 4'd1;
                end
            end
            else begin
                cnt2 <=cnt2 + 4'd1;
            end
        end
        else begin
           cnt1 <=cnt1 + 4'd1;
        end
    end
    
endmodule

module DeCounter(iClk,iRst,iStop,iP,oCnt1,oCnt2,oCnt3,oCnt4);
    input iClk;
    input iRst;
    input iStop;
    input [1:0] iP;
    output [3:0] oCnt1;
    output [3:0] oCnt2;
    output [3:0] oCnt3;
    output [3:0] oCnt4;
    reg [3:0] cnt1 = 4'd0;
    reg [3:0] cnt2 = 4'd0;
    reg [3:0] cnt3 = 4'd0;
    reg [3:0] cnt4 = 4'd0;
    reg [3:0] cnt5 = 4'd0;
    reg [3:0] cnt6 = 4'd0;
    reg [3:0] cnt7 = 4'd0;
    reg [3:0] cnt8 = 4'd0;

    assign oCnt1 = iP == 2'b00 ? cnt4 :
                   iP == 2'b01 ? cnt3 :
                   iP == 2'b10 ? cnt2 : cnt1;
    assign oCnt2 = iP == 2'b00 ? cnt5 :
                   iP == 2'b01 ? cnt4 :
                   iP == 2'b10 ? cnt3 : cnt2;
    assign oCnt3 = iP == 2'b00 ? cnt6 :
                   iP == 2'b01 ? cnt5 :
                   iP == 2'b10 ? cnt4 : cnt3;
    assign oCnt4 = iP == 2'b00 ? cnt7 :
                   iP == 2'b01 ? cnt6 :
                   iP == 2'b10 ? cnt5 : cnt4;

    always @(posedge iClk) begin
        if (~iRst) begin
            cnt1 <= 4'd0;
            cnt2 <= 4'd0;
            cnt3 <= 4'd0;
            cnt4 <= 4'd0;
            cnt5 <= 4'd0;
            cnt6 <= 4'd0;
            cnt7 <= 4'd0;
            cnt8 <= 4'd0;
        end
        else if (iStop) begin
            cnt1 <= cnt1;
            cnt2 <= cnt2;
            cnt3 <= cnt3;
            cnt4 <= cnt4;
            cnt5 <= cnt5;
            cnt6 <= cnt6;
            cnt7 <= cnt7;
            cnt8 <= cnt8;
        end else  if (cnt1 == 4'd0) begin
            cnt1 <= 4'd9;
            if (cnt2 == 4'd0) begin
                cnt2 <= 4'd9;
                if (cnt3 == 4'd0) begin
                    cnt3 <= 4'd9;
                    if (cnt4 == 4'd0) begin
                        cnt4 <= 4'd9;
                        if (cnt5 == 4'd0) begin
                            cnt5 <= 4'd9;
                            if (cnt6 == 4'd0) begin
                                cnt6 <= 4'd9;
                                if (cnt7 == 4'd0) begin
                                    cnt7 <= 4'd9;
                                    if (cnt8 == 4'd0) begin
                                        cnt8 <= 4'd9;
                                    end
                                    else begin
                                        cnt8 <=cnt8 - 4'd1;
                                    end
                                end
                                else begin
                                    cnt7 <=cnt7 - 4'd1;
                                end
                            end
                            else begin
                                cnt6 <=cnt6 - 4'd1;
                            end
                        end
                        else begin
                            cnt5 <=cnt5 - 4'd1;
                        end
                    end
                    else begin
                        cnt4 <=cnt4 - 4'd1;
                    end
                end
                else begin
                    cnt3 <=cnt3 - 4'd1;
                end
            end
            else begin
                cnt2 <=cnt2 - 4'd1;
            end
        end
        else begin
           cnt1 <=cnt1 - 4'd1;
        end
    end
    
endmodule

module Count4sw(iClk, oSw);
    input iClk;
    output reg [1:0] oSw = 2'b00;

    always @(posedge iClk) begin
        if (oSw == 2'b11) begin
            oSw <= 2'b00;
        end
        else begin
            oSw <= oSw + 2'b01;
        end
    end
endmodule