/*

print_minidump / print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef _MINIDUMP_STREAM_TYPE_H
#define _MINIDUMP_STREAM_TYPE_H

static const char *stream_types[] =
{
  "Unused",
  "Reserved0",
  "Reserved1",
  "ThreadList",
  "ModuleList",
  "MemoryList",
  "Exception",
  "SystemInfo",
  "ThreadExList",
  "Memory64List",
  "CommentA",
  "CommentW",
  "HandleData",
  "FunctionTable",
  "UnloadedModuleList",
  "MiscInfo",
  "MemoryInfoList",
  "ThreadInfoList",
  "HandleOperationList",
  "Token",
  "JavaScriptData",
  "SystemMemoryInfo",
  "ProcessVmCounters",
  "IptTrace",
  "ThreadNames",
  "ceNull",
  "ceSystemInfo",
  "ceException",
  "ceModuleList",
  "ceProcessList",
  "ceThreadList",
  "ceThreadContextList",
  "ceThreadCallStackList",
  "ceMemoryVirtualList",
  "ceMemoryPhysicalList",
  "ceBucketParameters",
  "ceProcessModuleMap",
  "ceDiagnosisList",
  "LastReserved",
};

const char *get_minidump_stream_type(int type)
{
  if (type >= sizeof(stream_types) / sizeof(const char *))
  {
    return "???";
  }

  return stream_types[type];
}

#endif

