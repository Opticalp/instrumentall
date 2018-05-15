/**
 * @file	src/modules/devices/genicam/GenDevTransportLayer.h
 * @date	may 2017
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2017 Ph. Renaud-Goud / Opticalp

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#ifndef SRC_MODULES_DEVICES_GENICAM_GENDEVTRANSPORTLAYER_H_
#define SRC_MODULES_DEVICES_GENICAM_GENDEVTRANSPORTLAYER_H_


#ifdef HAVE_GENAPI
#include "GenTL_v1_5.h"
#include "GenICam.h"

#include "GenTLLib.h"

/**
 * Genicam device transport layer class
 * 
 * to connect a genicam device NodeMap
 */
class GenDevTransportLayer : public GenApi::IPort
{
public:
    GenDevTransportLayer(GenTLLib* genTLLib, GenTL::PORT_HANDLE& hPort):
        genTL(genTLLib), port(hPort) { }

    GenApi::EAccessMode GetAccessMode() const
    {
		if (port == GENTL_INVALID_HANDLE)
			return GenApi::NA; // not available
		else
	        return GenApi::RW; // read write
    }

    void Read(void *pBuffer, int64_t Address, int64_t Length)
	{
		size_t len = Length;
		genTL->GCReadPort(port, Address, pBuffer, &len);
	}

    void Write(const void *pBuffer, int64_t Address, int64_t Length)
	{
		size_t len = Length;
		genTL->GCWritePort(port, Address, pBuffer, &len);
	}

private:
    GenTLLib* genTL;
    GenTL::PORT_HANDLE& port;
};

#endif /* HAVE_GENAPI */
#endif /* SRC_MODULES_DEVICES_GENICAM_GENDEVTRANSPORTLAYER_H_ */
