/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014 Damien P. George
 *               2018 Nick Moore for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/socketpool/Socket.h"

#include <stdio.h>
#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "esp_log.h"
static const char* TAG = "socket binding";

//| class Socket:
//|     """TCP, UDP and RAW socket. Cannot be created directly. Instead, call
//|        `SocketPool.socket()`.
//|
//|        Provides a subset of CPython's `socket.socket` API. It only implements the versions of
//|        recv that do not allocate bytes objects."""
//|

//|     def __enter__(self) -> Socket:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically closes the Socket when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t socketpool_socket___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_socketpool_socket_close(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socketpool_socket___exit___obj, 4, 4, socketpool_socket___exit__);

// //|     def bind(self, address: tuple) -> None:
// //|         """Bind a socket to an address
// //|
// //|         :param ~tuple address: tuple of (remote_address, remote_port)"""
// //|         ...
// //|

// STATIC mp_obj_t socketpool_socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
//     // mod_network_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

//     // // get address
//     // uint8_t ip[MOD_NETWORK_IPADDR_BUF_SIZE];
//     // mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

//     // // check if we need to select a NIC
//     // socket_select_nic(self, ip);

//     // // call the NIC to bind the socket
//     // int _errno;
//     // if (self->nic_type->bind(self, ip, port, &_errno) != 0) {
//     //     mp_raise_OSError(_errno);
//     // }

//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socket_bind_obj, socketpool_socket_bind);

// //|     def listen(self, backlog: int) -> None:
// //|         """Set socket to listen for incoming connections
// //|
// //|         :param ~int backlog: length of backlog queue for waiting connetions"""
// //|         ...
// //|

// STATIC mp_obj_t socketpool_socket_listen(mp_obj_t self_in, mp_obj_t backlog) {
//     // mod_network_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

//     // if (self->nic == MP_OBJ_NULL) {
//     //     // not connected
//     //     // TODO I think we can listen even if not bound...
//     //     mp_raise_OSError(MP_ENOTCONN);
//     // }

//     // int _errno;
//     // if (self->nic_type->listen(self, mp_obj_get_int(backlog), &_errno) != 0) {
//     //     mp_raise_OSError(_errno);
//     // }

//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socket_listen_obj, socketpool_socket_listen);

// //|     def accept(self) -> tuple:
// //|         """Accept a connection on a listening socket of type SOCK_STREAM,
// //|         creating a new socket of type SOCK_STREAM.
// //|         Returns a tuple of (new_socket, remote_address)"""
// //|

// STATIC mp_obj_t socketpool_socket_accept(mp_obj_t self_in) {
//     // mod_network_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

//     // // create new socket object
//     // // starts with empty NIC so that finaliser doesn't run close() method if accept() fails
//     // mod_network_socket_obj_t *socket2 = m_new_obj_with_finaliser(mod_network_socket_obj_t);
//     // socket2->base.type = &socket_type;
//     // socket2->nic = MP_OBJ_NULL;
//     // socket2->nic_type = NULL;

//     // // accept incoming connection
//     // uint8_t ip[MOD_NETWORK_IPADDR_BUF_SIZE];
//     // mp_uint_t port;
//     // int _errno;
//     // if (self->nic_type->accept(self, socket2, ip, &port, &_errno) != 0) {
//     //     mp_raise_OSError(_errno);
//     // }

//     // // new socket has valid state, so set the NIC to the same as parent
//     // socket2->nic = self->nic;
//     // socket2->nic_type = self->nic_type;

//     // // make the return value
//     // mp_obj_tuple_t *client = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
//     // client->items[0] = MP_OBJ_FROM_PTR(socket2);
//     // client->items[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);

//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_1(socketpool_socket_accept_obj, socketpool_socket_accept);

//|     def close(self) -> None:
//|         """Closes this Socket and makes its resources available to its SocketPool."""
//|
STATIC mp_obj_t socketpool_socket_close(mp_obj_t self_in) {
    socketpool_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_socketpool_socket_close(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(socketpool_socket_close_obj, socketpool_socket_close);

//|     def connect(self, address: tuple) -> None:
//|         """Connect a socket to a remote address
//|
//|         :param ~tuple address: tuple of (remote_address, remote_port)"""
//|         ...
//|

STATIC mp_obj_t socketpool_socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    socketpool_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);

    size_t hostlen;
    const char* host = mp_obj_str_get_data(addr_items[0], &hostlen);
    mp_int_t port = mp_obj_get_int(addr_items[1]);

    bool ok = common_hal_socketpool_socket_connect(self, host, hostlen, port);
    if (!ok) {
        ESP_EARLY_LOGW(TAG, "socket connect failed");
        mp_raise_OSError(0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socket_connect_obj, socketpool_socket_connect);

//|     def send(self, bytes: ReadableBuffer) -> int:
//|         """Send some bytes to the connected remote address.
//|         Suits sockets of type SOCK_STREAM
//|
//|         :param ~bytes bytes: some bytes to send"""
//|         ...
//|

STATIC mp_obj_t socketpool_socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    socketpool_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_socketpool_socket_get_closed(self)) {
        // Bad file number.
        mp_raise_OSError(MP_EBADF);
    }
    if (!common_hal_socketpool_socket_get_connected(self)) {
        mp_raise_BrokenPipeError();
    }
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    mp_int_t ret = common_hal_socketpool_socket_send(self, bufinfo.buf, bufinfo.len);
    if (ret == -1) {
        mp_raise_BrokenPipeError();
    }
    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socket_send_obj, socketpool_socket_send);


// helper function for socket_recv and socket_recv_into to handle common operations of both
// STATIC mp_int_t _socket_recv_into(mod_network_socket_obj_t *sock, byte *buf, mp_int_t len) {
//     mp_int_t ret = 0;
//     // int _errno;
//     // mp_int_t ret = sock->nic_type->recv(sock, buf, len, &_errno);
//     // if (ret == -1) {
//     //     mp_raise_OSError(_errno);
//     // }
//     return ret;
// }


//|     def recv_into(self, buffer: WriteableBuffer, bufsize: int) -> int:
//|         """Reads some bytes from the connected remote address, writing
//|         into the provided buffer. If bufsize <= len(buffer) is given,
//|         a maximum of bufsize bytes will be read into the buffer. If no
//|         valid value is given for bufsize, the default is the length of
//|         the given buffer.
//|
//|         Suits sockets of type SOCK_STREAM
//|         Returns an int of number of bytes read.
//|
//|         :param bytearray buffer: buffer to receive into
//|         :param int bufsize: optionally, a maximum number of bytes to read."""
//|         ...
//|

STATIC mp_obj_t socketpool_socket_recv_into(size_t n_args, const mp_obj_t *args) {
    socketpool_socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (common_hal_socketpool_socket_get_closed(self)) {
        // Bad file number.
        mp_raise_OSError(MP_EBADF);
    }
    if (!common_hal_socketpool_socket_get_connected(self)) {
        // not connected
        mp_raise_OSError(MP_ENOTCONN);
    }
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    mp_int_t len = bufinfo.len;
    if (n_args == 3) {
        mp_int_t given_len = mp_obj_get_int(args[2]);
        if (given_len < len) {
            len = given_len;
        }
    }

    if (len == 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    mp_int_t ret = common_hal_socketpool_socket_recv_into(self, (byte*)bufinfo.buf, len);
    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socketpool_socket_recv_into_obj, 2, 3, socketpool_socket_recv_into);

// //|     def sendto(self, bytes: ReadableBuffer, address: tuple) -> int:
// //|         """Send some bytes to a specific address.
// //|         Suits sockets of type SOCK_DGRAM
// //|
// //|         :param ~bytes bytes: some bytes to send
// //|         :param ~tuple address: tuple of (remote_address, remote_port)"""
// //|         ...
// //|

// STATIC mp_obj_t socketpool_socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
//     // mod_network_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);

//     // // get the data
//     // mp_buffer_info_t bufinfo;
//     // mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

//     // // get address
//     // uint8_t ip[MOD_NETWORK_IPADDR_BUF_SIZE];
//     // mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

//     // // check if we need to select a NIC
//     // socket_select_nic(self, ip);

//     // // call the NIC to sendto
//     // int _errno;
//     // mp_int_t ret = self->nic_type->sendto(self, bufinfo.buf, bufinfo.len, ip, port, &_errno);
//     // if (ret == -1) {
//     //     mp_raise_OSError(_errno);
//     // }
//     mp_int_t ret = 0;

//     return mp_obj_new_int(ret);
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_3(socketpool_socket_sendto_obj, socketpool_socket_sendto);

// //|     def recvfrom(self, bufsize: int) -> Tuple[bytes, tuple]:
// //|         """Reads some bytes from the connected remote address.
// //|         Suits sockets of type SOCK_STREAM
// //|
// //|         Returns a tuple containing
// //|         * a bytes() of length <= bufsize
// //|         * a remote_address, which is a tuple of ip address and port number
// //|
// //|         :param ~int bufsize: maximum number of bytes to receive"""
// //|         ...
// //|

// STATIC mp_obj_t socketpool_socket_recvfrom_into(mp_obj_t self_in, mp_obj_t len_in) {
//     // mod_network_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
//     // if (self->nic == MP_OBJ_NULL) {
//     //     // not connected
//     //     mp_raise_OSError(MP_ENOTCONN);
//     // }
//     // vstr_t vstr;
//     // vstr_init_len(&vstr, mp_obj_get_int(len_in));
//     // byte ip[4];
//     // mp_uint_t port;
//     // int _errno;
//     // mp_int_t ret = self->nic_type->recvfrom(self, (byte*)vstr.buf, vstr.len, ip, &port, &_errno);
//     // if (ret == -1) {
//     //     mp_raise_OSError(_errno);
//     // }
//     mp_obj_t tuple[2];
//     // if (ret == 0) {
//     //     tuple[0] = mp_const_empty_bytes;
//     // } else {
//     //     vstr.len = ret;
//     //     tuple[0] = mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
//     // }
//     // tuple[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);
//     return mp_obj_new_tuple(2, tuple);
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socket_recvfrom_into_obj, socketpool_socket_recvfrom_into);

// //|     def setsockopt(self, level: int, optname: int, value: int) -> None:
// //|         """Sets socket options"""
// //|         ...
// //|

// STATIC mp_obj_t socketpool_socket_setsockopt(size_t n_args, const mp_obj_t *args) {
//     // mod_network_socket_obj_t *self = MP_OBJ_TO_PTR(args[0]);

//     // mp_int_t level = mp_obj_get_int(args[1]);
//     // mp_int_t opt = mp_obj_get_int(args[2]);

//     // const void *optval;
//     // mp_uint_t optlen;
//     // mp_int_t val;
//     // if (mp_obj_is_integer(args[3])) {
//     //     val = mp_obj_get_int_truncated(args[3]);
//     //     optval = &val;
//     //     optlen = sizeof(val);
//     // } else {
//     //     mp_buffer_info_t bufinfo;
//     //     mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
//     //     optval = bufinfo.buf;
//     //     optlen = bufinfo.len;
//     // }

//     // int _errno;
//     // if (self->nic_type->setsockopt(self, level, opt, optval, optlen, &_errno) != 0) {
//     //     mp_raise_OSError(_errno);
//     // }

//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(socketpool_socket_setsockopt_obj, 4, 4, socketpool_socket_setsockopt);

//|     def settimeout(self, value: int) -> None:
//|         """Set the timeout value for this socket.
//|
//|         :param ~int value: timeout in seconds.  0 means non-blocking.  None means block indefinitely."""
//|         ...
//|

STATIC mp_obj_t socketpool_socket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    socketpool_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t timeout_ms;
    if (timeout_in == mp_const_none) {
        timeout_ms = -1;
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        timeout_ms = 1000 * mp_obj_get_float(timeout_in);
        #else
        timeout_ms = 1000 * mp_obj_get_int(timeout_in);
        #endif
    }
    common_hal_socketpool_socket_settimeout(self, timeout_ms);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socket_settimeout_obj, socketpool_socket_settimeout);

// //|     def setblocking(self, flag: bool) -> Optional[int]:
// //|         """Set the blocking behaviour of this socket.
// //|
// //|         :param ~bool flag: False means non-blocking, True means block indefinitely."""
// //|         ...
// //|

// // method socket.setblocking(flag)
// STATIC mp_obj_t socketpool_socket_setblocking(mp_obj_t self_in, mp_obj_t blocking) {
//     // if (mp_obj_is_true(blocking)) {
//     //     return socket_settimeout(self_in, mp_const_none);
//     // } else {
//     //     return socket_settimeout(self_in, MP_OBJ_NEW_SMALL_INT(0));
//     // }
//     return mp_const_none;
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpool_socket_setblocking_obj, socketpool_socket_setblocking);

//|     def __hash__(self) -> int:
//|         """Returns a hash for the Socket."""
//|         ...
//|
STATIC mp_obj_t socketpool_socket_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    switch (op) {
        case MP_UNARY_OP_HASH: {
            return MP_OBJ_NEW_SMALL_INT(common_hal_socketpool_socket_get_hash(MP_OBJ_TO_PTR(self_in)));
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

STATIC const mp_rom_map_elem_t socketpool_socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&socketpool_socket___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&socketpool_socket_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&socketpool_socket_close_obj) },

    // { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&socketpool_socket_bind_obj) },
    // { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&socketpool_socket_listen_obj) },
    // { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&socketpool_socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&socketpool_socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socketpool_socket_send_obj) },
    // { MP_ROM_QSTR(MP_QSTR_sendto), MP_ROM_PTR(&socketpool_socket_sendto_obj) },
    // { MP_ROM_QSTR(MP_QSTR_recvfrom_into), MP_ROM_PTR(&socketpool_socket_recvfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv_into), MP_ROM_PTR(&socketpool_socket_recv_into_obj) },
    // { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&socketpool_socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&socketpool_socket_settimeout_obj) },
    // { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socketpool_socket_setblocking_obj) },
};

STATIC MP_DEFINE_CONST_DICT(socketpool_socket_locals_dict, socketpool_socket_locals_dict_table);

const mp_obj_type_t socketpool_socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_Socket,
    .locals_dict = (mp_obj_dict_t*)&socketpool_socket_locals_dict,
    .unary_op = socketpool_socket_unary_op,
};
