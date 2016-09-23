/**
 * @file	src/DataGen.h
 * @date	Mar 2016
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2016 Ph. Renaud-Goud / Opticalp

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

#ifndef SRC_DATAGEN_H_
#define SRC_DATAGEN_H_

#include "Module.h"
#include "DataItem.h"

#include "Poco/RWLock.h"

#include <queue>

/**
 * DataGen
 *
 * Data generator module to be used for scalar data types
 */
class DataGen: public Module
{
public:
	DataGen(ModuleFactory* parent, std::string customName, int dataType);
	virtual ~DataGen() { }

    std::string description();

private:
    static size_t refCount; ///< reference counter to generate a unique internal name

    void process(int startCond);

    /**
     * Cleaning action in case of failure
     */
    void reset();

    void triggedRun();

    void freeRun();

    DataAttributeOut attr;

    int mDataType;

    /// Indexes of the input ports
    enum inPorts
    {
        trigPort,
        inPortCnt
    };

    /// Indexes of the output ports
    enum outPorts
    {
        outPortData,
        outPortCnt
    };

    enum params
    {
        paramValue,
        paramSeqStart,
        paramSeqEnd,
        paramCnt
    };

    Poco::Int64 seqStart;
    Poco::Int64 seqEnd;

    Poco::Int64 iPar;
    double fPar;
    std::string sPar;

    std::queue<Poco::Int64> iQueue;
    std::queue<double> fQueue;
    std::queue<std::string> sQueue;
    std::queue<DataAttributeOut> attrQueue;

    Poco::RWLock dataLock; ///< general lock for any data of this module

    Poco::Int64 getIntParameterValue(size_t paramIndex);
    double getFloatParameterValue(size_t paramIndex);
    std::string getStrParameterValue(size_t paramIndex);

    void setIntParameterValue(size_t paramIndex, Poco::Int64 value);
    void setFloatParameterValue(size_t paramIndex, double value);
    void setStrParameterValue(size_t paramIndex, std::string value);

    /**
     * Set the data to the output port
     *
     * acquire the datalock and release it
     */
    void sendData();

    Poco::Int32* pInt32;
    Poco::UInt32* pUInt32;
    Poco::Int64* pInt64;
    Poco::UInt64* pUInt64;
    float* pFloat;
    double* pDblFloat;
    std::string* pString;

    std::vector<Poco::Int32>* pVectInt32;
    std::vector<Poco::UInt32>* pVectUInt32;
    std::vector<Poco::Int64>* pVectInt64;
    std::vector<Poco::UInt64>* pVectUInt64;
    std::vector<float>* pVectFloat;
    std::vector<double>* pVectDblFloat;
    std::vector<std::string>* pVectString;

    /// Convenience function to fill out the output vector
    template <typename T>
    std::vector<T> fillOutIntVect(int dataType);

    template <typename T>
    std::vector<T> fillOutFloatVect(int dataType);

    std::vector<std::string> fillOutStrVect();
};

#include "DataGen.ipp"

#endif /* SRC_DATAGEN_H_ */
