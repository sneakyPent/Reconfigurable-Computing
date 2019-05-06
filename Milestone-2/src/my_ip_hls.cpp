#include "my_ip_hls.hpp"
#include <iostream>
#include <stdint.h>
#include <hls_stream.h>
#include <ap_int.h>
#include <stdio.h>

void my_ip_hls(stream<axiWord> &slaveIn,stream<axiWord> &masterOut,uint32 rule0, uint32 rule1, uint32 rule2, uint32 &rule0cnt, uint32 &rule1cnt, uint32 &rule2cnt) {

// AXI 4 Stream interface
#pragma HLS DATAFLOW interval=1
#pragma HLS INTERFACE axis register both port=slaveIn
#pragma HLS INTERFACE axis register both port=masterOut
#pragma HLS INTERFACE ap_ctrl_none port=return

// ----------------------------------------------------
// PS <-> My IP data transfer using AXI 4 Lite protocol
// ----------------------------------------------------
// Rules
#pragma HLS INTERFACE s_axilite port=rule0 bundle=psAxiLite
#pragma HLS INTERFACE s_axilite port=rule1 bundle=psAxiLite
#pragma HLS INTERFACE s_axilite port=rule2 bundle=psAxiLite
// Rules' Counters
#pragma HLS INTERFACE s_axilite port=rule0cnt bundle=psAxiLite
#pragma HLS INTERFACE s_axilite port=rule1cnt bundle=psAxiLite
#pragma HLS INTERFACE s_axilite port=rule2cnt bundle=psAxiLite

	//internal fifos for input and output data streams
	static stream<axiWord> ps2ipFifo("ps2ipFifo");
#pragma HLS STREAM variable=ps2ipFifo depth=64 dim=1
	static stream<axiWord> ip2psFifo("ip2psFifo");
#pragma HLS STREAM variable=ip2psFifo depth=64 dim=1


	static uint32 rulesCnts [3] = { 0, 0, 0 };						// Rules' Counters registers
	static uint32 rules [3] = { -1 , -1 , -1 };						// Rules registers

	give_rules(rule0, rule1, rule2, rules, rulesCnts);				// Save Rules and their counters in registers
	ps2ip_fifo(slaveIn,ps2ipFifo);									// FIFO that keeps input data
	core(ps2ipFifo,ip2psFifo,rules,rulesCnts);						// Core of the IP with core logic
	read_rules_counters(rulesCnts, rule0cnt, rule1cnt, rule2cnt);	// Give back rules' counters
	ip2ps_fifo(ip2psFifo,masterOut);								// FIFO that keeps output data

	return;

}