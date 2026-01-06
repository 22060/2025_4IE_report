// 27MHz を 1Hz に分周するクロック分周器
// module d4c(iClk, oClk);
// 	input iClk; /* 27[MHz] */
// 	output oClk; /* 1[Hz] */
// 	reg [23:0] dd = 24'd1349; // 24ビットカウンタ
// 	reg oClk = 1'b0;
	
// 	always @(posedge iClk) begin
// 		if(dd == 24'd1349) begin // 13,500,000 - 1
// 			oClk <= ~oClk; // 出力クロックを反転
// 			dd <= 24'd0;   // カウンタリセット
// 		end
// 		else begin
// 			dd <= dd + 24'd1; // カウンタインクリメント
// 		end
// 	end
// endmodule

module d4dl(iClk, oClk);
	input iClk; /* 27[MHz] */
	output oClk; /* 1[kHz] */
	reg [14:0] dd = 15'd13499;  //14ビットカウンタ
	reg oClk = 1'b0;
	
	always @(posedge iClk) begin
		if(dd == 15'd13499) begin //13,500 - 1
			oClk <= ~oClk;  //出力クロックを反転
			dd <= 15'd0;    //カウンタリセット
		end
		else begin
			dd <= dd + 15'd1;  //カウンタインクリメント
		end
	end
endmodule

