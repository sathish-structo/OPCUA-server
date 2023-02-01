/*****************************************************************************
*                                                                            *
*                     SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                          *
*                                D-85540 Haar                                *
*                        Phone: ++49-89-4 56 56-0                            *
*                          Fax: ++49-89-4 56 56-3 99                         *
*                                                                            *
*                            SOFTING CONFIDENTIAL                            *
*                                                                            *
*                     Copyright (C) SOFTING IA GmbH 2022                     *
*                             All Rights Reserved                            *
*                                                                            *
* NOTICE: All information contained herein is, and remains the property of   *
*   SOFTING Industrial Automation GmbH and its suppliers, if any. The intel- *
*   lectual and technical concepts contained herein are proprietary to       *
*   SOFTING Industrial Automation GmbH and its suppliers and may be covered  *
*   by German and Foreign Patents, patents in process, and are protected by  *
*   trade secret or copyright law. Dissemination of this information or      *
*   reproduction of this material is strictly forbidden unless prior         *
*   written permission is obtained from SOFTING Industrial Automation GmbH.  *
******************************************************************************
******************************************************************************
*                                                                            *
* PROJECT_NAME             OPC UA C++ Server & Client SDK                    *
*                                                                            *
* VERSION                  6.00.0                                            *
*                                                                            *
* DATE                     13.6.2022                                            *
*                                                                            *
*****************************************************************************/

#ifndef STRUCTHELPERS_H
#define STRUCTHELPERS_H

#include <errno.h>
#include <limits>

class StructHelpers
{
public:
	static void ReleaseString(OTChar*& string);
	static EnumStatusCode CopyString(OTChar*& target, const OTChar* source);
	static EnumStatusCode CopyString(OTChar*& target, const tstring& source);
	static EnumStatusCode ReplaceString(OTChar*& target, const OTChar* source);
	static bool CompareString(const OTChar* string1, const OTChar* string2);

	static void ReleaseByteString(ByteStringStruct*& cp);
	static EnumStatusCode CopyByteStringStruct(ByteStringStruct*& target, const ByteStringStruct* source);
	static EnumStatusCode CopyByteStringStruct(ByteStringStruct& target, const ByteStringStruct& source);
	static EnumStatusCode ReplaceByteStringStruct(ByteStringStruct*& target, const ByteStringStruct* source);

	static EnumStatusCode MakeHexString(OTChar*& target, unsigned char* buf, OTUInt32 bufSize);
	static EnumStatusCode MakeBufFromHexString(OpcUa_ByteString &taget, const OTChar* source);

	static EnumStatusCode AllocByteBuffer(unsigned char* &target, const void* src, OTUInt32 length);

	/*! Convenience function to convert an array length to an unsigned value with the correct sign handling */
	static OTUInt32 toUnsignedLength(OTInt32 arrayLength);
};

/*! Helper class to to implement the toString() methods of structures in a consistent way.
*
* It creates a sequence of "<Name1>: <Value1>, <Name2>: <Value2>, ... <NameN>: <ValueN>".
* Arrays are written in the format "<Name>[<array size>]: {<Value1>, <Value2>, ... <ValueN>}, ..." */
class ToStringHelper
{
public:
	ToStringHelper(size_t maxLen = std::numeric_limits<size_t>::max());

	void clear();
	size_t size();
	tstring getString() const;

	// Adds a scalar field whose value can be put directly into the stringstream (char* int, double ...)
	template<typename T>
	inline bool addSimpleField(const tstring& name, const T& simpleValue);

	// explicit setters for certain types
	// we didn't manage to get this template specialization through all compilers and validations
	bool addBoolField(const tstring& name, const bool boolValue);
	bool addStringField(const tstring& name, const tstring& stringValue);
	bool addStringField(const tstring& name, const OTChar* stringValue);

	// Adds a scalar field, whose value is an OTStructure (with a toString() method)
	template<typename T>
	inline bool addStructureField(const tstring& name, const T& value);

	// Adds an array field whose value can be put directly into the stringstream (char* int, double ...)
	template<typename T>
	inline bool addSimpleArrayField(const tstring& name, OTUInt32 arraySize, const T* pSimpleArray);

	// Adds an array field, whose value is an OTStructure (with a toString() method)
	template<typename T>
	inline bool addStructureArrayField(const tstring& name, OTUInt32 arraySize, const T* pStructureArray);
private:

	// adds the string content of an entire field or adds "..." when the size is exceeded
	bool addFieldContent(const tstring& name, const tstring& value, bool encloseField);
	bool addArrayFieldHeader(const tstring& name, size_t arraySize);
	bool addArrayFieldValue(const tstring& value, bool encloseField, bool firstField);
	void addArrayFieldFooter();

	static const OTChar* c_nameValueSeparator;
	static const OTChar* c_fieldSeparator;
	static const OTChar* c_arraySizeBeginToken;
	static const OTChar* c_arraySizeEndToken;
	static const OTChar* c_arrayGroupBeginToken;
	static const OTChar* c_arrayGroupEndToken;
	static const OTChar* c_fieldBeginToken;
	static const OTChar* c_fieldEndToken;
	static const OTChar* c_maxLenContinuationToken;

	bool m_hasFields;
	size_t m_maxLen;
	bool m_lengthExceededInFields;
	bool m_lengthExceededInArray;
	tstringstream m_stream;
};

template<typename T>
inline bool ToStringHelper::addSimpleField(const tstring& name, const T& simpleValue)
{
	tstringstream valueStream;
	valueStream << simpleValue;
	// simple types don't need to be enclosed in additional brackets
	return addFieldContent(name, valueStream.str(), false);
}

template<typename T>
inline bool ToStringHelper::addStructureField(const tstring& name, const T& value)
{
	// skip the call to the toString() when nothing shall be added
	if(m_lengthExceededInFields)
	{
		return false;
	}
	// enclose structures in extra brackets to see the start and end
	return addFieldContent(name, value.toString(), true);
}

template<typename T>
inline bool ToStringHelper::addSimpleArrayField(const tstring& name, OTUInt32 arraySize, const T* pSimpleArray)
{
	bool res = addArrayFieldHeader(name, arraySize);
	if(res)
	{
		for(OTUInt32 i = 0; i < arraySize; i++)
		{
			tstringstream valueStream;
			valueStream << pSimpleArray[i];
			// simple types don't need to be enclosed in additional brackets
			res = addArrayFieldValue(valueStream.str(), false, (i == 0));
			if(!res)
			{
				// the size exceeded, skip all further array elements
				break;
			}
		}
		// add the footer even when an array element exceeded the length
		addArrayFieldFooter();
	}
	return res;
}

template<typename T>
inline bool ToStringHelper::addStructureArrayField(const tstring& name, OTUInt32 arraySize, const T* pStructureArray)
{
	bool res = addArrayFieldHeader(name, arraySize);
	if(res)
	{
		for(OTUInt32 i = 0; i < arraySize; i++)
		{
			// enclose structures in extra brackets to see the start and end
			res = addArrayFieldValue(pStructureArray[i].toString(), true, (i == 0));
			if(!res)
			{
				// the size exceeded, skip all further array elements
				break;
			}
		}
		// add the footer even when an array element exceeded the length
		addArrayFieldFooter();
	}
	return res;
}

/*! @brief function for allocating typed pointer arrays in the stack.
*	@param pTarget [out] Pointer which will be initialized by OpcUa_Alloc() */
template<class pointerType> pointerType* typedUAlloc(
	pointerType*& pPointer,
	size_t elemCount)
{
	if(elemCount)
	{
		pPointer = (pointerType*)OpcUa_Alloc((OpcUa_UInt32)(sizeof(pointerType) * elemCount));
	}
	else
	{
		pPointer = 0;
	}
	return pPointer;
}


/*! @brief Copy an array of primitives from source to target.
* @attention Use it only for primitive types. */
template<class targetType, class sourceType> EnumStatusCode copyTypedUArray(
	targetType*& pTarget,
	sourceType* pSource,
	size_t length)
{
	if(pSource == NULL && length == 0)
	{
		pTarget = NULL;
		return EnumStatusCode_Good;
	}
	else if(pSource == NULL && length > 0)
	{
		pTarget = NULL;
		return EnumStatusCode_BadInvalidArgument;
	}

	typedUAlloc(
		pTarget,
		length);

	if(pTarget == NULL)
	{
		return EnumStatusCode_BadOutOfMemory;
	}

	// avoid warning...
	OTUInt32 size = sizeof(targetType);

	if(size == sizeof(sourceType))
	{ // then we can make a memcpy!
		memcpy(
			pTarget,
			pSource,
			sizeof(targetType) * length);
	}
	else
	{
		targetType* pTargetLocal = pTarget;
		sourceType* pSourceLocal = pSource;
		while(length--)
		{ // for all array members...
			*pTargetLocal++ = *pSourceLocal++;
		}
	}
	return EnumStatusCode_Good;
}


/*! @brief Calls the clear() method on value structures.
@param The structure pointer. */
template<class pointerType> void clearStructure(pointerType* pPointer)
{
	if(pPointer != NULL)
	{
		pPointer->clear();
		OTFreeMemory(pPointer);
	}
}

#endif	// STRUCTHELPERS_H
