//
// Created by avpdiver on 09.09.16.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "network.h"
#include "../core/logger/logger.h"
#include "../core/memory/allocator.h"
#include "../core/memory/alloc_policies/lockfree_pool.h"
#include "../core/memory/default_policies.h"

namespace gladius {
namespace network {

const char* LOG_TYPE = "NETWORK";

struct s_connection {
	sockaddr_in local_addr;
	sockaddr_in remote_addr;
	int socket;
};

typedef typename std::aligned_storage<sizeof(s_connection), alignof(s_connection)>::type s_connection_t;
s_connection_t storage[32];

core::memory::c_allocator<
	core::memory::c_lockfree_pool<s_connection_t>,
	core::memory::c_no_thread_policy,
	core::memory::c_no_bounds_policy,
	core::memory::c_no_tracking_policy,
	core::memory::c_no_tagging_policy> g_connection_pool(32, storage);

bool connect(const char* remote_ip, uint16_t remote_port, handle_t& handle) {
	s_connection* connection = (s_connection*)g_connection_pool.alloc(sizeof(connection));
	if (connection == nullptr) {
		return false;
	}

	int s;
	VERIFY_LOG((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0, LOG_TYPE, "Failed create socket", "");

	connection->socket = s;

	connection->local_addr.sin_family = AF_INET;
	connection->local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	connection->local_addr.sin_port = htons(0);

	connection->remote_addr.sin_family = AF_INET;
	connection->remote_addr.sin_addr.s_addr = htonl(inet_addr(remote_ip));
	connection->remote_addr.sin_port = htons(remote_port);

	VERIFY_LOG(bind(connection->socket, (sockaddr*)&(connection->local_addr), sizeof(connection->local_addr)) < 0,
			   LOG_TYPE, "Failed bind socket", "");

	handle = (size_t)connection - (size_t)storage;
	return true;
}

bool send(handle_t handle, const void* buffer, size_t len) {
	s_connection* c = (s_connection*)((size_t)storage + handle);
	VERIFY_LOG(sendto(c->socket, buffer, len, 0, (sockaddr*)&(c->remote_addr), sizeof(c->remote_addr)) == len, LOG_TYPE,
			   "Failed send", "");
	return true;
}

void receive(handle_t handle, void* buffer, size_t len) {
	s_connection* c = static_cast<s_connection*>((size_t)storage + handle);

	socklen_t size = sizeof(c->remote_addr);
	ssize_t r = recvfrom(c->socket, buffer, len, 0, (sockaddr*)&(c->remote_addr), &size);
}

}
}