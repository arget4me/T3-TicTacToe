#pragma once



namespace t3
{
	int init_server(void);
	void sendData(char* data, int size);
	void register_receive_callback(void(*callback)(char*, int));
};