/*
 * Copyright (c) 2011 Agnieszka Tabaka <nufcia@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __X10_H
#define __X10_H

#define CMDLEN                          2
#define TIMELEN				7

#define BUILDCMD(buf, a, b)             do {        \
                                        buf[0] = a; \
                                        buf[1] = b; \
                                        } while(0)

#define BUILDHEADER(dim, fa, es)        ((es & 0x1) | (fa & 0x1) << 1 | 0x1 << 2 | (dim & 0x1f) << 3)
#define BUILDADDR(hcode, dcode)         ((dcode & 0xf) | (hcode & 0xf) << 4)
#define BUILDFUNC(hcode, function)      ((function & 0xf) | (hcode & 0xf) << 4)

#define DEBUG(...)     			fprintf(stderr, "debug: " __VA_ARGS__);

#define X10_COMMAND_ALL_OFF		0x0
#define X10_COMMAND_ALL_ON		0x1
#define X10_COMMAND_ON			0x2
#define X10_COMMAND_OFF			0x3
#define X10_COMMAND_DIM			0x4
#define X10_COMMAND_BRIGHT		0x5

#include <stdint.h>
#include <exception>
#include <stdexcept>

namespace X10
{
	class X10Housecode;
	class X10Devcode;
	class X10Device;
	class X10Context;

	class X10Housecode
	{
	public:
		X10Housecode();
		X10Housecode(char housecode);
		~X10Housecode();
	
		char as_char() const;
		uint8_t as_code() const;
		void from_char(char housecode);
	
	private:
		char m_housecode;

	};

	class X10Devcode
	{
	public:
		X10Devcode();
		X10Devcode(int devcode);
		~X10Devcode();

		int as_int() const;
		uint8_t as_code() const;
		void from_int(int devcode);

	public:
		uint8_t m_devcode;

	};

	class X10IOException : public std::runtime_error
	{
	public:
		X10IOException() : std::runtime_error("read/write timed out.") {}
	};

	class X10DeviceException : public std::runtime_error
	{
	public:
		X10DeviceException() : std::runtime_error("x10 device failure") { }
		X10DeviceException(const std::string &what) : std::runtime_error(what) { }
	};

	class X10ValueException : public std::runtime_error
	{
	public:
		X10ValueException() : std::runtime_error("x10 failure: bad value") { }
		X10ValueException(const std::string &what) : std::runtime_error(what) { }
	};

	class X10ContextException : public std::runtime_error
	{
	public:
		X10ContextException() : std::runtime_error("context not defined or invalid!") { }
	};

	class X10Device
	{
	public:
		X10Device(const std::string &name);
		X10Device(const X10Devcode &devcode, const std::string &name);
		X10Device(X10Context &ctx, const X10Devcode &devcode, const std::string &name);

		~X10Device();

		void execute();
		X10Context &get_context() const;
		void set_context(X10Context &ctx);
		const std::string &get_name() const;
		const X10Devcode &get_devcode() const;
		void set_devcode(const X10Devcode &devcode);
		int get_brightness() const;
		bool is_on() const;
		void set_brightness(int brightness);
		void set_on() ;
		void set_off();
		void set_dim();
		void set_light(); 

	private:
		friend class X10Context;

		X10Context *m_context;
		std::string m_name;
		X10Devcode m_devcode;
		bool m_synchronized;
		int m_desired_brightness;
		int m_actual_brightness;
		bool m_desired_on;
		bool m_actual_on;
	};

	class X10Context
	{
	public:
		X10Context();
		X10Context(const std::string &serial_device, const X10Housecode &housecode);
		~X10Context();

		const X10Housecode &get_housecode() const;
		void set_housecode(const X10Housecode &housecode);
		int get_maxretry() const;
		void set_maxretry(int maxretry);

		void open(const std::string &serial_device);
		void close();

	private:
		friend class X10Device;

		int m_fd;
		int m_maxretry;
		bool m_open;
		std::string m_serial_device;
		X10Housecode m_housecode;

		void update_device_state(X10Device &device);
		void update_device_brightness(X10Device &device);

		void x10_send(uint8_t cmd[], size_t length);
		void x10_synchronize_time();
		void x10_build_time(uint8_t time_buffer[TIMELEN]);
		uint8_t x10_checksum(uint8_t buffer[], int len);
		ssize_t x10_xwrite(int d, const uint8_t *buf, size_t nbytes);
		ssize_t x10_xread(int d, uint8_t *buf, size_t nbytes);
	};
};

#endif
