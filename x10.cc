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

#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>
#include <list>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <termios.h>
#include <sys/types.h>
#include "x10.h"

namespace X10
{
	/*
	 * X10Housecode
	 */
	
	X10Housecode::X10Housecode() { from_char('A'); }
	X10Housecode::X10Housecode(char housecode) { from_char(housecode); }
	X10Housecode::~X10Housecode() { }

	char X10Housecode::as_char() const
	{
		return m_housecode;
	}

	uint8_t X10Housecode::as_code() const
	{
		const uint8_t codetable[] = {6, 14, 2, 10, 1, 9, 5, 13, 7, 15, 3, 11, 0, 8, 4};
		return codetable[m_housecode - 'A'];
	}

	void X10Housecode::from_char(char housecode)
	{
		m_housecode = housecode;
	}

	/*
	 * X10Devcode
	 */

	X10Devcode::X10Devcode() { from_int(1); }
	X10Devcode::X10Devcode(int devcode) { from_int(devcode); }
	X10Devcode::~X10Devcode() { }

	int X10Devcode::as_int() const
	{
		return m_devcode;
	}

	uint8_t X10Devcode::as_code() const
	{
		const uint8_t codetable[] = {6, 14, 2, 10, 1, 9, 5, 13, 7, 15, 3, 11, 0, 8, 4};
		return codetable[m_devcode - 1];
	}

	void X10Devcode::from_int(int devcode)
	{
		if (devcode <= 16 && devcode > 0)
			m_devcode = devcode;
		else
			throw X10ValueException("Devcode must be in range [0;16]!");
	}

	/*
	 * X10Device
	 */

	X10Device::X10Device(const std::string &name) :
		m_name(name),
		m_actual_on(false),
		m_desired_on(false),
		m_synchronized(false)
	{
		m_actual_brightness = m_desired_brightness = 0;
		m_context = NULL;
	}

	X10Device::X10Device(const X10Devcode &devcode, const std::string &name) :
		m_name(name),
		m_devcode(devcode),
		m_actual_on(false),
		m_desired_on(false),
		m_synchronized(false)
	{
		m_actual_brightness = m_desired_brightness = 0;
		m_context = NULL;
	}

	X10Device::X10Device(X10Context &context, const X10Devcode &devcode, const std::string &name) :
		m_name(name),
		m_devcode(devcode),
		m_actual_on(false),
		m_desired_on(false),
		m_synchronized(false)
	{
		m_actual_brightness = m_desired_brightness = 0;
		m_context = &context;
	}

	X10Device::~X10Device()
	{ }

	void X10Device::execute()
	{
		if (!m_context)
			throw X10ContextException();

		m_context->update_device_state(*this);
		m_context->update_device_brightness(*this);
	}

	X10Context &X10Device::get_context() const 
	{ return *m_context; }
	
	void X10Device::set_context(X10Context &ctx) 
	{ m_context = &ctx; execute(); }
	
	const std::string &X10Device::get_name() const 
	{ return m_name; }
	
	const X10Devcode &X10Device::get_devcode() const 
	{ return m_devcode; }
	
	void X10Device::set_devcode(const X10Devcode &devcode) 
	{ m_devcode = devcode; }
	
	bool X10Device::is_on() const 
	{ return m_actual_on; }
	
	int X10Device::get_brightness() const 
	{ return m_actual_brightness; }

	void X10Device::set_on()
	{
		m_desired_on = true;

		if (m_context)
			m_context->update_device_state(*this);
	}

	void X10Device::set_off()
	{
		m_desired_on = false;

		if (m_context)
			m_context->update_device_state(*this);
	}

	void X10Device::set_dim()
	{
		m_desired_brightness--;

		if (m_context) {
			if (!m_synchronized) {
				m_desired_brightness = 22;
				m_context->update_device_brightness(*this);
				m_synchronized = true;
			}

			m_context->update_device_brightness(*this);
		}
	}

	void X10Device::set_light()
	{
		m_desired_brightness++;

		if (m_context) {
			if (!m_synchronized) {
				m_desired_brightness = 22;
				m_context->update_device_brightness(*this);
				m_synchronized = true;
			}

			m_context->update_device_brightness(*this);
		}
	}

	void X10Device::set_brightness(int brightness)
	{
		if (brightness <= 22 && brightness >= 0)
			m_desired_brightness = brightness;
		else
			throw X10ValueException("Brightness must be in range [0;22]!");

		if(m_context) {
			if (!m_synchronized) {
				m_desired_brightness = 22;
				m_context->update_device_brightness(*this);
				m_synchronized = true;
			}

			m_context->update_device_brightness(*this);
		}
	}

	/*
	 * X10Context
	 */

	X10Context::X10Context() :
		m_open(false)
	{ }

	X10Context::X10Context(const std::string &serial_device, const X10Housecode &housecode) :
		m_serial_device(serial_device),
		m_housecode(housecode),
		m_open(false)
	{
		open(m_serial_device);
		x10_synchronize_time();
	}

	X10Context::~X10Context() 
	{ 
		close();
	}
	
	const X10Housecode &X10Context::get_housecode() const { return m_housecode; }
	void X10Context::set_housecode(const X10Housecode &housecode) { m_housecode = housecode; }
	int X10Context::get_maxretry() const { return m_maxretry; }
	void X10Context::set_maxretry(int maxretry) { m_maxretry = maxretry; }

	void X10Context::update_device_state(X10Device &device)
	{
		uint8_t buffer[CMDLEN];

		if (!m_open)
			return;

		BUILDCMD(buffer, BUILDHEADER(0, 0, 0), BUILDADDR(m_housecode.as_code(), device.m_devcode.as_code()));
		x10_send(buffer, CMDLEN);

		if (device.m_desired_on) {
			BUILDCMD(buffer, BUILDHEADER(0, 1, 0), BUILDADDR(m_housecode.as_code(), X10_COMMAND_ON));
			x10_send(buffer, CMDLEN);
			device.m_actual_on = true;
			device.m_actual_brightness = 22;
		} else {
			BUILDCMD(buffer, BUILDHEADER(0, 1, 0), BUILDADDR(m_housecode.as_code(), X10_COMMAND_OFF));
			x10_send(buffer, CMDLEN);
			device.m_actual_on = false;
			device.m_actual_brightness = 0;
		}
	}
	
	void X10Context::update_device_brightness(X10Device &device)
	{
		uint8_t buffer[CMDLEN];
		uint8_t command = 0;
		uint8_t value = abs(device.m_desired_brightness - device.m_actual_brightness);

		if (!m_open)
			return;

		if (device.m_desired_brightness == device.m_actual_brightness)
			return;

		if (device.m_desired_brightness - device.m_actual_brightness > 0) {
			command = X10_COMMAND_BRIGHT;
		} else {
			command = X10_COMMAND_DIM;
		}

		DEBUG("desired=%d acutal=%d\n", device.m_desired_brightness, device.m_actual_brightness);
		DEBUG("value=%d\n", value);

		BUILDCMD(buffer, BUILDHEADER(0, 0, 0), BUILDADDR(m_housecode.as_code(), device.get_devcode().as_code()));
		x10_send(buffer, CMDLEN);

		BUILDCMD(buffer, BUILDHEADER(value + 1, 1, 0), BUILDADDR(m_housecode.as_code(), command));
		x10_send(buffer, CMDLEN);

		device.m_actual_brightness = device.m_desired_brightness;
	}

	void X10Context::open(const std::string &serial_device)
	{
		struct termios tio;
		m_serial_device = serial_device;
        
		if ((m_fd = ::open(m_serial_device.c_str(), O_RDWR | O_NOCTTY)) < 0)
			throw X10DeviceException(std::string("Cannot open serial port: ") + strerror(errno));

		tcgetattr(m_fd, &tio);
        
	        if (cfsetspeed(&tio, B4800) < 0)
			throw X10DeviceException(std::string("Cannot set serial port speed: ") + strerror(errno));

		tio.c_cflag = CS8 | CLOCAL | CREAD | B4800;
		tio.c_iflag = IGNPAR | IGNBRK;
		tio.c_oflag = 0;
		tio.c_lflag = 0;
		tio.c_cc[VTIME] = 0;
		tio.c_cc[VINTR] = 0;
		tio.c_cc[VQUIT] = 0;
		tio.c_cc[VMIN] = 1;

		if (tcflush(m_fd, TCIFLUSH) < 0) 
			throw X10DeviceException(std::string("Cannot set serial port attributes: ") + strerror(errno));
        
		if (tcsetattr(m_fd, TCSANOW, &tio) < 0)
			throw X10DeviceException(std::string("Cannot set serial port attributes: ") + strerror(errno));

		x10_synchronize_time();

		m_open = true;
	}

	void X10Context::close()
	{
		::close(m_fd);
	}

	void X10Context::x10_send(uint8_t cmd[], size_t length)
	{
	        int tries = 0;
		bool success = false;
	        uint8_t buffer[length];
	        uint8_t checksum = 0;

        	do {
        	        memcpy(buffer, cmd, length);
	                checksum = x10_checksum(buffer, length);

			try {
			 	x10_xwrite(m_fd, buffer, length);
			 	x10_xread(m_fd, buffer, 1);
			} catch (X10IOException &e) {
				tries++;
				continue;
			}
                
                	if (buffer[0] == checksum) {
				success = true;
				break;
			} else {
	                	tries++;
				DEBUG("Wrong checksum (0x%.2x instead of 0x%.2x)\n", buffer[0], checksum);
			}
	        } while (tries < m_maxretry);

	        if (!success) {
	                DEBUG("Write failed\n");
	        	throw X10DeviceException(std::string("Device timed out (maximum retry limit reached"));
		} else {
	                memset(buffer, 0, length);

			try {
		                x10_xwrite(m_fd, buffer, 1);
		                x10_xread(m_fd, buffer, 1);
			} catch (X10IOException &e) {
				throw X10DeviceException(std::string("Device timed out (read/write timed out)"));
			}
		}

       	        if (buffer[0] != 0x55) 
        		throw X10DeviceException(std::string("X10 controller returned invalid value"));
	}

	void X10Context::x10_synchronize_time()
	{
		int tries = 0;
		bool success = false;
		uint8_t buffer[CMDLEN];
		uint8_t time[TIMELEN];

		BUILDCMD(buffer, 0x9b, 0x00);

		try {
			DEBUG("Trying to read time request...\n");
			x10_xread(m_fd, buffer, 1);
		} catch (X10IOException &e) { 
			DEBUG("Time request timed out.\n");
			return;
		}

		if (buffer[0] == 0xa5) {
			x10_build_time(time);
			do {
				x10_xwrite(m_fd, time, TIMELEN);
				x10_xread(m_fd, buffer, 1);
		
				if (buffer[0] == x10_checksum((uint8_t*)(time + 1), TIMELEN - 1)) {
					success = true;
					break;
				} else if (buffer[0] == 0xa5) {
					;
				} else {
					tries++;
				}
			} while (tries < m_maxretry);
		}

		if (!success) {
			DEBUG("Write failed\n");
			throw X10DeviceException(std::string("Device timed out (maximum retry limit reached"));
		} else {
			DEBUG("Writing 0x00\n");
	                memset(buffer, 0, CMDLEN);
	                x10_xwrite(m_fd, buffer, 1);
	                x10_xread(m_fd, buffer, 1);       
       		        DEBUG("Read 0x%.2x\n", buffer[0]);
		}

       	        if (buffer[0] != 0x55) 
        		throw X10DeviceException(std::string("X10 controller returned invalid value"));
	}

	void X10Context::x10_build_time(uint8_t buffer[TIMELEN])
	{
		/* DUMMY */
		buffer[0] = 0x9b;
		buffer[1] = 0x1f;
		buffer[2] = 0x34;
		buffer[3] = 0x00;
		buffer[4] = 0x55;
		buffer[5] = 0x04;
		buffer[6] = 0x60;
	}

	uint8_t X10Context::x10_checksum(uint8_t buffer[], int len)
	{
	        uint8_t csum = 0;

        	while(len--)
	                csum += buffer[len];

        	csum &= 0xff;
	        return csum;
	}

	ssize_t X10Context::x10_xwrite(int d, const uint8_t *buf, size_t nbytes)
	{ 
		fd_set dset;
		struct timeval tv;
		size_t done = 0;

		tv.tv_sec = 3;
		tv.tv_usec = 0;

		FD_ZERO(&dset);
		FD_SET(d, &dset);

		while (done < nbytes) {
			if (select(d + 1, NULL, &dset, NULL, &tv) == 0)
				throw X10IOException();

			done += write(d, reinterpret_cast<const uint8_t *>(buf + done), nbytes - done);
		}

		return done;
	}

	ssize_t X10Context::x10_xread(int d, uint8_t *buf, size_t nbytes)
	{
		fd_set dset;
		struct timeval tv;
		size_t done = 0;

		tv.tv_sec = 6;
		tv.tv_usec = 0;

		FD_ZERO(&dset);
		FD_SET(d, &dset);

		while (done < nbytes) {
			if (select(d + 1, &dset, NULL, NULL, &tv) == 0)
				throw X10IOException();

			done += read(d, reinterpret_cast<uint8_t *>(buf + done), nbytes - done);
		}

		return done;
	}

}

