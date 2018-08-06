/**
 * @file	src/dataProxies/LinearConverter.cpp
 * @date	Aug. 2018
 * @author	PhRG - opticalp.fr
 */

/*
 Copyright (c) 2018 Ph. Renaud-Goud / Opticalp

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

#include "LinearConverter.h"

#include "Poco/NumberFormatter.h"

size_t LinearConverter::refCount = 0;

LinearConverter::LinearConverter():
		DataProxy("LinearConverter")
{
	setName(refCount);
    refCount++;

	addParameter(paramScale, "scale", "scale factor (applied before offset)", ParamItem::typeFloat, "1.0");
	addParameter(paramOffset, "offset", "offset applied after scaling. ", ParamItem::typeFloat, "0.0");
	setFloatParameterValue(paramScale, getFloatParameterDefaultValue(paramScale));
	setFloatParameterValue(paramOffset, getFloatParameterDefaultValue(paramOffset));
}

std::set<int> LinearConverter::supportedInputDataType()
{
	std::set<int> ret;

	bool isVect = isVector(dataType());
	bool undef = (dataType() == DataItem::typeUndefined);

	if (isVect || undef)
	{
		ret.insert(typeInt32  | contVector);
		ret.insert(typeUInt32 | contVector);
		ret.insert(typeInt64  | contVector);
		ret.insert(typeUInt64 | contVector);
		ret.insert(typeFloat  | contVector);
		ret.insert(typeDblFloat | contVector);
	}

	if (!isVect || undef)
	{
		ret.insert(typeInt32  | contScalar);
		ret.insert(typeUInt32 | contScalar);
		ret.insert(typeInt64  | contScalar);
		ret.insert(typeUInt64 | contScalar);
		ret.insert(typeFloat  | contScalar);
		ret.insert(typeDblFloat | contScalar);
	}

    return ret;
}

std::set<int> LinearConverter::supportedOutputDataType()
{
	std::set<int> ret;

	int inDataType = DataItem::typeUndefined;
	bool undef = true;

	try
	{
		inDataType = getDataSource()->dataType();
		undef = false;
	}
	catch (Poco::NullPointerException& )
	{
		// do nothing
	}

	bool isVect = isVector(inDataType);

	if (isVect || undef)
	{
		ret.insert(typeInt32  | contVector);
		ret.insert(typeUInt32 | contVector);
		ret.insert(typeInt64  | contVector);
		ret.insert(typeUInt64 | contVector);
		ret.insert(typeFloat  | contVector);
		ret.insert(typeDblFloat | contVector);
	}

	if (!isVect || undef)
	{
		ret.insert(typeInt32  | contScalar);
		ret.insert(typeUInt32 | contScalar);
		ret.insert(typeInt64  | contScalar);
		ret.insert(typeUInt64 | contScalar);
		ret.insert(typeFloat  | contScalar);
		ret.insert(typeDblFloat | contScalar);
	}

    return ret;
}

void LinearConverter::convert()
{
	if (isVector(dataType()))
	{
		switch (noContainerDataType(dataType()))
		{
	    case (typeInt32):
			switch (noContainerDataType(getDataSource()->dataType())) // input ?
			{
			case (typeInt32 ):
				assignVectorInt32<Poco::Int32>(getData< std::vector<Poco::Int32> >());
				break;
			case (typeUInt32):
				assignVectorInt32<Poco::UInt32>(getData< std::vector<Poco::Int32> >());
				break;
			case (typeInt64 ):
				assignVectorInt32<Poco::Int64>(getData< std::vector<Poco::Int32> >());
				break;
			case (typeUInt64):
				assignVectorInt32<Poco::UInt64>(getData< std::vector<Poco::Int32> >());
				break;
			case (typeFloat ):
				assignVectorInt32<float>(getData< std::vector<Poco::Int32> >());
				break;
			case (typeDblFloat):
				assignVectorInt32<double>(getData< std::vector<Poco::Int32> >());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
			break;
	    case (typeUInt32):
			switch (noContainerDataType(getDataSource()->dataType())) // input ?
			{
			case (typeInt32 ):
				assignVectorUInt32<Poco::Int32>(getData< std::vector<Poco::UInt32> >());
				break;
			case (typeUInt32):
				assignVectorUInt32<Poco::UInt32>(getData< std::vector<Poco::UInt32> >());
				break;
			case (typeInt64 ):
				assignVectorUInt32<Poco::Int64>(getData< std::vector<Poco::UInt32> >());
				break;
			case (typeUInt64):
				assignVectorUInt32<Poco::UInt64>(getData< std::vector<Poco::UInt32> >());
				break;
			case (typeFloat ):
				assignVectorUInt32<float>(getData< std::vector<Poco::UInt32> >());
				break;
			case (typeDblFloat):
				assignVectorUInt32<double>(getData< std::vector<Poco::UInt32> >());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
			break;
	    case (typeInt64):
			switch (noContainerDataType(getDataSource()->dataType())) // input ?
			{
			case (typeInt32 ):
				assignVectorInt64<Poco::Int32>(getData< std::vector<Poco::Int64> >());
				break;
			case (typeUInt32):
				assignVectorInt64<Poco::UInt32>(getData< std::vector<Poco::Int64> >());
				break;
			case (typeInt64 ):
				assignVectorInt64<Poco::Int64>(getData< std::vector<Poco::Int64> >());
				break;
			case (typeUInt64):
				assignVectorInt64<Poco::UInt64>(getData< std::vector<Poco::Int64> >());
				break;
			case (typeFloat ):
				assignVectorInt64<float>(getData< std::vector<Poco::Int64> >());
				break;
			case (typeDblFloat):
				assignVectorInt64<double>(getData< std::vector<Poco::Int64> >());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
			break;
	    case (typeUInt64):
			switch (noContainerDataType(getDataSource()->dataType())) // input ?
			{
			case (typeInt32 ):
				assignVectorUInt64<Poco::Int32>(getData< std::vector<Poco::UInt64> >());
				break;
			case (typeUInt32):
				assignVectorUInt64<Poco::UInt32>(getData< std::vector<Poco::UInt64> >());
				break;
			case (typeInt64 ):
				assignVectorUInt64<Poco::Int64>(getData< std::vector<Poco::UInt64> >());
				break;
			case (typeUInt64):
				assignVectorUInt64<Poco::UInt64>(getData< std::vector<Poco::UInt64> >());
				break;
			case (typeFloat ):
				assignVectorUInt64<float>(getData< std::vector<Poco::UInt64> >());
				break;
			case (typeDblFloat):
				assignVectorUInt64<double>(getData< std::vector<Poco::UInt64> >());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
			break;
	    case (typeFloat):
			switch (noContainerDataType(getDataSource()->dataType())) // input ?
			{
			case (typeInt32 ):
				assignVectorFloat<Poco::Int32>(getData< std::vector<float> >());
				break;
			case (typeUInt32):
				assignVectorFloat<Poco::UInt32>(getData< std::vector<float> >());
				break;
			case (typeInt64 ):
				assignVectorFloat<Poco::Int64>(getData< std::vector<float> >());
				break;
			case (typeUInt64):
				assignVectorFloat<Poco::UInt64>(getData< std::vector<float> >());
				break;
			case (typeFloat ):
				assignVectorFloat<float>(getData< std::vector<float> >());
				break;
			case (typeDblFloat):
				assignVectorFloat<double>(getData< std::vector<float> >());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
			break;
	    case (typeDblFloat):
			switch (noContainerDataType(getDataSource()->dataType())) // input ?
			{
			case (typeInt32 ):
				assignVectorDblFloat<Poco::Int32>(getData< std::vector<double> >());
				break;
			case (typeUInt32):
				assignVectorDblFloat<Poco::UInt32>(getData< std::vector<double> >());
				break;
			case (typeInt64 ):
				assignVectorDblFloat<Poco::Int64>(getData< std::vector<double> >());
				break;
			case (typeUInt64):
				assignVectorDblFloat<Poco::UInt64>(getData< std::vector<double> >());
				break;
			case (typeFloat ):
				assignVectorDblFloat<float>(getData< std::vector<double> >());
				break;
			case (typeDblFloat):
				assignVectorDblFloat<double>(getData< std::vector<double> >());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
			break;
		default:
			poco_bugcheck_msg("incorrect output data type");
			throw Poco::BugcheckException();
		}
	}
	else
	{
		switch (dataType())
		{
	    case (typeInt32): // output int32
	    	switch (getDataSource()->dataType()) // input ?
	    	{
			case (typeInt32 ):
				*getData<Poco::Int32>() = getInt32<Poco::Int32>(*getDataSource()->getData<Poco::Int32>());
				break;
			case (typeUInt32):
				*getData<Poco::Int32>() = getInt32<Poco::UInt32>(*getDataSource()->getData<Poco::UInt32>());
				break;
			case (typeInt64 ):
				*getData<Poco::Int32>() = getInt32<Poco::Int64>(*getDataSource()->getData<Poco::Int64>());
				break;
			case (typeUInt64):
				*getData<Poco::Int32>() = getInt32<Poco::UInt64>(*getDataSource()->getData<Poco::UInt64>());
				break;
			case (typeFloat ):
				*getData<Poco::Int32>() = getInt32<float>(*getDataSource()->getData<float>());
				break;
			case (typeDblFloat):
				*getData<Poco::Int32>() = getInt32<double>(*getDataSource()->getData<double>());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
	    	}
	    	break;
	    case (typeUInt32):
			switch (getDataSource()->dataType()) // input ?
			{
			case (typeInt32 ):
				*getData<Poco::UInt32>() = getUInt32<Poco::Int32>(*getDataSource()->getData<Poco::Int32>());
				break;
			case (typeUInt32):
				*getData<Poco::UInt32>() = getUInt32<Poco::UInt32>(*getDataSource()->getData<Poco::UInt32>());
				break;
			case (typeInt64 ):
				*getData<Poco::UInt32>() = getUInt32<Poco::Int64>(*getDataSource()->getData<Poco::Int64>());
				break;
			case (typeUInt64):
				*getData<Poco::UInt32>() = getUInt32<Poco::UInt64>(*getDataSource()->getData<Poco::UInt64>());
				break;
			case (typeFloat ):
				*getData<Poco::UInt32>() = getUInt32<float>(*getDataSource()->getData<float>());
				break;
			case (typeDblFloat):
				*getData<Poco::UInt32>() = getUInt32<double>(*getDataSource()->getData<double>());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
	    	break;
	    case (typeInt64):
			switch (getDataSource()->dataType()) // input ?
			{
			case (typeInt32 ):
				*getData<Poco::Int64>() = getInt64<Poco::Int32>(*getDataSource()->getData<Poco::Int32>());
				break;
			case (typeUInt32):
				*getData<Poco::Int64>() = getInt64<Poco::UInt32>(*getDataSource()->getData<Poco::UInt32>());
				break;
			case (typeInt64 ):
				*getData<Poco::Int64>() = getInt64<Poco::Int64>(*getDataSource()->getData<Poco::Int64>());
				break;
			case (typeUInt64):
				*getData<Poco::Int64>() = getInt64<Poco::UInt64>(*getDataSource()->getData<Poco::UInt64>());
				break;
			case (typeFloat ):
				*getData<Poco::Int64>() = getInt64<float>(*getDataSource()->getData<float>());
				break;
			case (typeDblFloat):
				*getData<Poco::Int64>() = getInt64<double>(*getDataSource()->getData<double>());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
	    	break;
	    case (typeUInt64):
			switch (getDataSource()->dataType()) // input ?
			{
			case (typeInt32 ):
				*getData<Poco::UInt64>() = getUInt64<Poco::Int32>(*getDataSource()->getData<Poco::Int32>());
				break;
			case (typeUInt32):
				*getData<Poco::UInt64>() = getUInt64<Poco::UInt32>(*getDataSource()->getData<Poco::UInt32>());
				break;
			case (typeInt64 ):
				*getData<Poco::UInt64>() = getUInt64<Poco::Int64>(*getDataSource()->getData<Poco::Int64>());
				break;
			case (typeUInt64):
				*getData<Poco::UInt64>() = getUInt64<Poco::UInt64>(*getDataSource()->getData<Poco::UInt64>());
				break;
			case (typeFloat ):
				*getData<Poco::UInt64>() = getUInt64<float>(*getDataSource()->getData<float>());
				break;
			case (typeDblFloat):
				*getData<Poco::UInt64>() = getUInt64<double>(*getDataSource()->getData<double>());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
	    	break;
	    case (typeFloat):
			switch (getDataSource()->dataType()) // input ?
			{
			case (typeInt32 ):
				*getData<float>() = getFloat<Poco::Int32>(*getDataSource()->getData<Poco::Int32>());
				break;
			case (typeUInt32):
				*getData<float>() = getFloat<Poco::UInt32>(*getDataSource()->getData<Poco::UInt32>());
				break;
			case (typeInt64 ):
				*getData<float>() = getFloat<Poco::Int64>(*getDataSource()->getData<Poco::Int64>());
				break;
			case (typeUInt64):
				*getData<float>() = getFloat<Poco::UInt64>(*getDataSource()->getData<Poco::UInt64>());
				break;
			case (typeFloat ):
				*getData<float>() = getFloat<float>(*getDataSource()->getData<float>());
				break;
			case (typeDblFloat):
				*getData<float>() = getFloat<double>(*getDataSource()->getData<double>());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
	    	break;
	    case (typeDblFloat):
			switch (getDataSource()->dataType()) // input ?
			{
			case (typeInt32 ):
				*getData<double>() = getDblFloat<Poco::Int32>(*getDataSource()->getData<Poco::Int32>());
				break;
			case (typeUInt32):
				*getData<double>() = getDblFloat<Poco::UInt32>(*getDataSource()->getData<Poco::UInt32>());
				break;
			case (typeInt64 ):
				*getData<double>() = getDblFloat<Poco::Int64>(*getDataSource()->getData<Poco::Int64>());
				break;
			case (typeUInt64):
				*getData<double>() = getDblFloat<Poco::UInt64>(*getDataSource()->getData<Poco::UInt64>());
				break;
			case (typeFloat ):
				*getData<double>() = getDblFloat<float>(*getDataSource()->getData<float>());
				break;
			case (typeDblFloat):
				*getData<double>() = getDblFloat<double>(*getDataSource()->getData<double>());
				break;
			default:
				poco_bugcheck_msg("incorrect input data type");
				throw Poco::BugcheckException();
			}
	    	break;
		default:
			poco_bugcheck_msg("incorrect output data type");
			throw Poco::BugcheckException();
		}
	}
}

double LinearConverter::getFloatParameterValue(size_t paramIndex)
{
	switch (paramIndex)
	{
	case paramScale:
		return scale;
	case paramOffset:
		return offset;
	default:
		poco_bugcheck_msg("incorrect parameter index");
		throw Poco::BugcheckException();
	}
}

void LinearConverter::setFloatParameterValue(size_t paramIndex, double value)
{
	switch (paramIndex)
	{
	case paramScale:
		scale = value;
		break;
	case paramOffset:
		offset = value;
		break;
	default:
		poco_bugcheck_msg("incorrect parameter index");
		throw Poco::BugcheckException();
	}
}
