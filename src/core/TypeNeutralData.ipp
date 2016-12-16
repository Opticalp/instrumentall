/**
 * @file	src/core/TypeNeutralData.ipp
 * @date	june 2016
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


#include "TypeNeutralData.h"

#include "Poco/Types.h"

#include <vector>

//
// scalar containers
//

template <> inline
void TypeNeutralData::checkType<Poco::Int32>()
{
    checkContScalar();
    checkDataType(typeInt32);
}

template <> inline
void TypeNeutralData::checkType<Poco::UInt32>()
{
    checkContScalar();
    checkDataType(typeUInt32);
}

template <> inline
void TypeNeutralData::checkType<Poco::Int64>()
{
    checkContScalar();
    checkDataType(typeInt64);
}

template <> inline
void TypeNeutralData::checkType<Poco::UInt64>()
{
    checkContScalar();
    checkDataType(typeUInt64);
}

template <> inline
void TypeNeutralData::checkType<float>()
{
    checkContScalar();
    checkDataType(typeFloat);
}

template <> inline
void TypeNeutralData::checkType<double>()
{
    checkContScalar();
    checkDataType(typeDblFloat);
}

template <> inline
void TypeNeutralData::checkType<std::string>()
{
    checkContScalar();
    checkDataType(typeString);
}

//
// vector containers
//

template <> inline
void TypeNeutralData::checkType< std::vector<Poco::Int32> >()
{
    checkContVector();
    checkDataType(typeInt32);
}

template <> inline
void TypeNeutralData::checkType< std::vector<Poco::UInt32> >()
{
    checkContVector();
    checkDataType(typeUInt32);
}

template <> inline
void TypeNeutralData::checkType< std::vector<Poco::Int64> >()
{
    checkContVector();
    checkDataType(typeInt64);
}

template <> inline
void TypeNeutralData::checkType< std::vector<Poco::UInt64> >()
{
    checkContVector();
    checkDataType(typeUInt64);
}

template <> inline
void TypeNeutralData::checkType< std::vector<float> >()
{
    checkContVector();
    checkDataType(typeFloat);
}

template <> inline
void TypeNeutralData::checkType< std::vector<double> >()
{
    checkContVector();
    checkDataType(typeDblFloat);
}

template <> inline
void TypeNeutralData::checkType< std::vector<std::string> >()
{
    checkContVector();
    checkDataType(typeString);
}

//
// default template - should not be called.
//

template <typename T> inline
void TypeNeutralData::checkType()
{
    // report a bug to the developer
    poco_bugcheck_msg("TypeNeutralData::checkType<T>(): Wrong type T. ");
    throw Poco::BugcheckException();
}


