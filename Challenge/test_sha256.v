/////////////////////////////////////////////////////////////////////
////                                                             ////
////  SHA-256                                                    ////
////  Secure Hash Algorithm (SHA-256)   testbench                ////
////                                                             ////
////  Author: marsgod                                            ////
////          marsgod@opencores.org                              ////
////                                                             ////
////                                                             ////
////  Downloaded from: http://www.opencores.org/cores/sha_core/  ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2002-2004 marsgod                             ////
////                         marsgod@opencores.org               ////
////                                                             ////
////                                                             ////
//// This source file may be used and distributed without        ////
//// restriction provided that this copyright statement is not   ////
//// removed from the file and that any derivative work contains ////
//// the original copyright notice and the associated disclaimer.////
////                                                             ////
////     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY     ////
//// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   ////
//// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS   ////
//// FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR      ////
//// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         ////
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    ////
//// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE   ////
//// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        ////
//// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  ////
//// LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT  ////
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  ////
//// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         ////
//// POSSIBILITY OF SUCH DAMAGE.                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////


`timescale 1ns/10ps

`define SHA256_TEST		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
`define SHA256_TEST_PADDING	{1'b1,63'b0,448'b0,64'd448}	// 448 bit
`define SHA256_TEST_RESULT	256'h7d8b94e0_2dc6b35a_b0ee53cf_9a280093_e2bd135d_d598e90e_ee3b1cc9_e7248825

`define counter_value 32'h00000002
`define input_message " Keep your FPGA spinning!"
`define pad {152'h0, 64'h8000000000000000, 64'h0000000000000180}

module test_sha;

reg clk,rst,cmd_w_i;
reg [31:0] text_i;

reg [2:0] cmd_i;

wire [255:0] text_o;
wire [3:0] cmd_o;

initial
begin
//	$sdf_annotate("syn/data/sha256.sdf",sha_core);

	clk = 1'b0;
	rst = 1'b0;
	cmd_w_i = 1'b0;
	cmd_i = 3'b0;
	
	#21;
	rst = 1'b1;
	#17;
	rst = 1'b0;
	
	test_SHA256;
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	
	$finish;
end


always #5 clk = ~clk;

sha256 sha_core(
	.clk_i(clk),
	.rst_i(rst),
	.text_i(text_i),
	.text_o(text_o),
	.cmd_i(cmd_i),
	.cmd_w_i(cmd_w_i),
	.cmd_o(cmd_o)
	);


task test_SHA256;
integer i;
reg [511:0] tmp_i;
reg [255:0] tmp_o;
reg [31:0] tmp;
reg [511:0] message_in;
reg [255:0] output_message;


begin

	message_in = {`counter_value,`input_message,`pad };
	
	tmp_i = message_in;
	tmp_o = `SHA256_TEST_RESULT;
	
	
	
	#100;
	
	
	@(posedge clk);
	cmd_i = 3'b010;
	cmd_w_i = 1'b1;
	
	for (i=0;i<16;i=i+1)
	begin
		@(posedge clk);
		cmd_w_i = 1'b0;
		text_i = tmp_i[16*32-1:15*32];
		tmp_i = tmp_i << 32;
	end

	
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);

	while (cmd_o[3])
		@(posedge clk);
	
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	
	#100;
	
	
	 @(posedge clk);

	cmd_i = 3'b001;
	cmd_w_i = 1'b1;
	
	@(posedge clk);
	cmd_w_i = 1'b0;
	
	@(posedge clk);
		
	output_message = text_o;
	tmp = tmp_o;
		
	
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	@(posedge clk);
	#100;
end
endtask

endmodule