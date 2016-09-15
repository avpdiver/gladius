//
// Created by avpdiver on 09.09.16.
//

#ifndef GLADIUS_NETWORK_H
#define GLADIUS_NETWORK_H

#include <cstdint>
#include "../core/types.h"

namespace gladius {
namespace network {

bool init();
void shutdown();

bool connect(const char* remote_ip, uint16_t remote_port, handle_t& handle);
bool send(handle_t handle, const char* buffer, size_t len);

template <typename TYPE>
bool send(handle_t handle, const TYPE& value) {
	return send(handle, &value, sizeof(TYPE));
}

}
}
#endif //GLADIUS_NETWORK_H
