#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include "shell_io.h"
#include "lb_layer_data.h"
#include "lb_layer2.h"

#define	PORT			5000

static int sock_conn;			/* request socked */
void tcp_server_thread(void const *p_arg)
{
	struct sockaddr_in server_addr;
	struct sockaddr_in conn_addr;
	int sock_fd;				/* server socked */

	socklen_t addr_len;
	int err;
	int length;
	int count = 0;
	StartEthPhy();

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		TraceUser("failed to create sock_fd!\n");
		assert(0);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr =htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	err = bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err < 0) {
		assert(0);
	}

	err = listen(sock_fd, 1);
	if (err < 0) {
		assert(0);
	}

	addr_len = sizeof(struct sockaddr_in);

	TraceUser("before accept!\n");
	sock_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len);
	TraceUser("after accept!\n");

	while (1) {
		MsgFrame* ptrFrame = LB_Layer2_GetNewBuff();
		ptrFrame->usedSrc = FROM_ETH;
		ptrFrame->typeMsg = FROM_ETH;
		length = recv(sock_conn, (void *)ptrFrame->frame.data, sizeof(ptrFrame->frame.data), 0);
		if(length <= 0)
		{
			closesocket(sock_conn);
			TraceUser("before accept!\n");
			sock_conn = accept(sock_fd, (struct sockaddr *)&conn_addr, &addr_len);
			TraceUser("after accept!\n");
		}
		else
			LB_Layer2_Rx(ptrFrame);

		//TraceUser("length received %d\n", length);
		//TraceUser("received string: %s\n", data_buffer);
		//TraceUser("received count: %d\n", count);

	//	send(sock_conn, (void *)ptrFrame->frame.data, length, 0);
	}
}

uint16_t LB_Layer2_Eth_Tx(MsgFrame* ptrFrame)
{
	static uint32_t cnt = 0;
	int ret = send(sock_conn, (void *)ptrFrame->frame.data, (int)GetFrameLen(&ptrFrame->frame), 0);
	cnt++;
	if(cnt%50 == 1)
		TraceUser("send back, %d!\n",cnt);
	return (uint16_t)ret;
}

void tcp_server_init(void)
{

	//tcp_server_thread(NULL);
	sys_thread_new("tcp_server_thread",  tcp_server_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO - 1);
}
