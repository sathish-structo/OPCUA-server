////////////////////////////////////////////////////////////////////////////////////
/// @file PrintApi.h
///
/// Contain a function defination for print API
/// 
/// 
/// @par Copyright (c) 2022 Structo Pte Ltd, Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef __PRINT_API_FOR_EXTERNAL
#define __PRINT_API_FOR_EXTERNAL

#include <iostream>
#include <string.h>
extern "C"
{

namespace PRINT_API_EXT
{  
   // Job API
   void PrintApi_SendJobApi(char* msg);
   void PrintApi_ReceiveJobApi(char* buf);
   std::string PrintApi_AddPrintJob(const char* strData,const char* strMotorProfilePath,const char* strUvProfilePath);
   std::string PrintApi_StopPrintJob();
   std::string PrintApi_StartNextJob();
   std::string PrintApi_GetPrintStatus();

   // Resource API
   void PrintApi_SendResourceApi(char* msg);
   void PrintApi_ReceiveResourceApi(char* buf);
   std::string PrintApi_HomeTiltMotor();
   std::string PrintApi_HomeVertMotor();
   std::string PrintApi_MoveToTiltMotor(double deg, int a1, int v1, int d1,
                                 int amax, int vmax, int dmax);
   std::string PrintApi_MoveToVertMotor(double pos, double acc, double vmax,
                                 double vslow, double head, double tail);
   std::string PrintApi_PulseMask(std::string filepath, double intensity, int dur);

} // namespace name
}
#endif
