#pragma once
#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__
class WebSokcetParser;
enum WS_FrameType
{
	WS_EMPTY_FRAME = 0xF0,
	WS_ERROR_FRAME = 0xF1,
	WS_WAIT_FRAME = 0xF2,
	WS_TEXT_FRAME = 0x01,
	WS_BINARY_FRAME = 0x02,
	WS_PING_FRAME = 0x09,
	WS_PONG_FRAME = 0x0A,
	WS_OPENING_FRAME = 0xF3,
	WS_CLOSING_FRAME = 0x08
};
enum WS_ParseResult
{
	WS_PARSE_RESULT_OK=0,
	WS_PARSE_RESULT_WAIT_NETX_FRAME,
	WS_PARSE_RESULT_WAIT_NEXT_DATA,
	WS_PARSE_RESULT_SKIP,
	WS_PARSE_RESULT_ERROR
};
class WebSokcetParser
{
public:
	WebSokcetParser();
	int DecodeFrame(char * frameData, int frameSize,int &outHead,int &outSize);
	int EncodeFrame(char * frameData, int frameSize, int emptySize, int &outSize);
	
};

#endif // !__WEBSOCKET_H__
