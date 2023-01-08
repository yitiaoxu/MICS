#ifndef __HOSTCOMMUNICATIONUTILS_H
#define __HOSTCOMMUNICATIONUTILS_H

#include "HostCommunication.h"

const char *hc_getHandshakeStatusString(int status);
const char *hc_getFunctionStatusString(int status);
const char *hc_getErrorCodeString(int errcode);
const char *hc_getCommandString(int cmd);

char *HostCommunicationStatus2str(char *const buffer, const HostCommunicationStatus_t *const p_hc_status);

#endif // __HOSTCOMMUNICATIONUTILS_H
