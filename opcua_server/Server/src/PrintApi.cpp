////////////////////////////////////////////////////////////////////////////////////
/// @file PrintApi.cpp
///
/// Implementation detail for displaying image in LCD
/// 
/// @par Copyright (c) 2022 Structo Pte Ltd, Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES

#include "PrintApi.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>


#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE +10)

extern "C"
{
namespace PRINT_API_EXT
{ 
  // Job API
  void PrintApi_SendJobApi(char* msg)
  {
    int recvr_msgq_fd=0;
    if((recvr_msgq_fd=mq_open("/firmware_server_job_req", O_WRONLY | O_CREAT, 0,0))==-1)
    {
      printf("JobAPI | Request mq_open() failed, errno=%d", errno);
      exit(1);
    }

    if(mq_send(recvr_msgq_fd,msg,strlen(msg)+1,0)==-1)
    {
      perror("JobApi | Request not able to send message to client");
      exit(1);
    }
    mq_close(recvr_msgq_fd);
  }

  void PrintApi_ReceiveJobApi(char* buf)
  {
    int replyq_fd=0;
    if((replyq_fd=mq_open("/firmware_server_job_res", O_RDONLY | O_CREAT, 0660,0))==-1)
    {
      printf("JobAPI | Response mq_open() API failed, errno=%d", errno);
      exit(1);
    }
    memset(buf,0, MSG_BUFFER_SIZE);
    if(mq_receive(replyq_fd, buf, MSG_BUFFER_SIZE, NULL) == -1)
    {
      printf("JobAPI | Response mq_receive error, errno=%d\n", errno);
      exit(1);
    }
    std::cout<<"JobAPI | Response: "<<buf<<std::endl;
  }

  std::string PrintApi_AddPrintJob(const char* strData,const char* strMotorProfilePath,const char* strUvProfilePath)
  {
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"ADDJOB,%s,%s,%s",strData,strMotorProfilePath,strUvProfilePath);
    PrintApi_SendJobApi(buffer);
    PrintApi_ReceiveJobApi(buffer);
    return std::string(buffer);
  } 

  std::string PrintApi_StopPrintJob()
  {
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"STOPPRINTJOB");
    PrintApi_SendJobApi(buffer);
    PrintApi_ReceiveJobApi(buffer);
    return std::string(buffer);
  } 

  std::string PrintApi_StartNextJob()
  {
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"STARTNEXTJOB");
    PrintApi_SendJobApi(buffer);
    PrintApi_ReceiveJobApi(buffer);
    return std::string(buffer);
  } 

  std::string PrintApi_GetPrintStatus()
  {
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"GETPRINTSTS");
    PrintApi_SendJobApi(buffer);
    PrintApi_ReceiveJobApi(buffer);
    return std::string(buffer);
  } 

  // Request API
  void PrintApi_SendResourceApi(char* msg)
  {
    int recvr_msgq_fd=0;
    if((recvr_msgq_fd=mq_open("/firmware_server_resource_req", O_WRONLY | O_CREAT, 0,0))==-1)
    {
      printf("ResourceAPI | Request mq_open() failed, errno=%d", errno);
      exit(1);
    }

    if(mq_send(recvr_msgq_fd,msg,strlen(msg)+1,0)==-1)
    {
      perror("ResourceApi | Request not able to send message to client");
      exit(1);
    }
    mq_close(recvr_msgq_fd);
  }

  void PrintApi_ReceiveResourceApi(char* buf)
  {
    int replyq_fd=0;
    if((replyq_fd=mq_open("/firmware_server_resource_res", O_RDONLY | O_CREAT, 0660,0))==-1)
    {
      printf("ResourceAPI | Response mq_open() API failed, errno=%d", errno);
      exit(1);
    }
    memset(buf,0, MSG_BUFFER_SIZE);
    if(mq_receive(replyq_fd, buf, MSG_BUFFER_SIZE, NULL) == -1)
    {
      printf("ResourceAPI | Response mq_receive error, errno=%d\n", errno);
      exit(1);
    }
    std::cout<<"ResourceAPI | Response: "<<buf<<std::endl;
  }

  std::string PrintApi_HomeTiltMotor()
  {
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"HOMETILTMOTOR");
    PrintApi_SendResourceApi(buffer);
    PrintApi_ReceiveResourceApi(buffer);
    return std::string(buffer);
  }

  std::string PrintApi_HomeVertMotor()
  {
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"HOMEVERTMOTOR");
    PrintApi_SendResourceApi(buffer);
    PrintApi_ReceiveResourceApi(buffer);
    return std::string(buffer);
  } 

  std::string PrintApi_MoveToTiltMotor(double deg, int a1, int v1, int d1,
                                int amax, int vmax, int dmax){
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"MOVETOTILTMOTOR,%f,%d,%d,%d,%d,%d,%d",
                    deg,a1,v1,d1,amax,vmax,dmax);
    PrintApi_SendResourceApi(buffer);
    PrintApi_ReceiveResourceApi(buffer);
    return std::string(buffer);
  }

  std::string PrintApi_MoveToVertMotor(double pos, double acc, double vmax,
                                double vslow, double head, double tail){
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"MOVETOVERTMOTOR,%f,%f,%f,%f,%f,%f",
                    pos,acc,vmax,vslow,head,tail);
    PrintApi_SendResourceApi(buffer);
    PrintApi_ReceiveResourceApi(buffer);
    return std::string(buffer);
  }

  std::string PrintApi_PulseMask(std::string filepath, double intensity, int dur)
  {
    char buffer[MSG_BUFFER_SIZE];
    sprintf(buffer,"PULSEMASK,%s,%f,%d",
                    (filepath).c_str(),intensity,dur);
    PrintApi_SendResourceApi(buffer);
    PrintApi_ReceiveResourceApi(buffer);
    return std::string(buffer);
  }

}
}