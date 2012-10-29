//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include "util/wgt2allg.h"
#include "ac/global_file.h"
#include "ac/common.h"
#include "ac/file.h"
#include "ac/runtime_defines.h"
#include "ac/string.h"
#include "util/filestream.h"

using AGS::Common::DataStream;

extern int num_open_script_files;
extern DataStream *valid_handles[MAX_OPEN_SCRIPT_FILES+1];


DataStream *FileOpenCMode(const char*fnmm, const char* cmode)
{
  Common::FileOpenMode open_mode;
  Common::FileWorkMode work_mode;
  // NOTE: here we ignore the text-mode flag. AGS 2.62 did not let
  // game devs to open files in text mode. The file reading and
  // writing logic in AGS makes extra control characters added for
  // security reasons, and FileWriteRawLine adds CR/LF to the end
  // of string on its own.
  if (!Common::File::GetFileModesFromCMode(cmode, open_mode, work_mode))
  {
      return NULL;
  }
  return FileOpen(fnmm, open_mode, work_mode);
}

DataStream *FileOpen(const char*fnmm, Common::FileOpenMode open_mode, Common::FileWorkMode work_mode)
{
  int useindx = 0;
  char fileToOpen[MAX_PATH];

  if (!validate_user_file_path(fnmm, fileToOpen,
      (open_mode != Common::kFile_Open || work_mode != Common::kFile_Read)))
    return NULL;

  // find a free file handle to use
  for (useindx = 0; useindx < num_open_script_files; useindx++) 
  {
    if (valid_handles[useindx] == NULL)
      break;
  }

  valid_handles[useindx] = Common::File::OpenFile(fileToOpen, open_mode, work_mode);

  if (valid_handles[useindx] == NULL)
    return NULL;

  if (useindx >= num_open_script_files) 
  {
    if (num_open_script_files >= MAX_OPEN_SCRIPT_FILES)
      quit("!FileOpen: tried to open more than 10 files simultaneously - close some first");
    num_open_script_files++;
  }
  return valid_handles[useindx];
}

void FileClose(DataStream *hha) {
  valid_handles[check_valid_file_handle(hha,"FileClose")] = NULL;
  delete hha;
  }
void FileWrite(DataStream *haa, const char *towrite) {
  check_valid_file_handle(haa,"FileWrite");
  haa->WriteInt32(strlen(towrite)+1);
  haa->Write(towrite,strlen(towrite)+1);
  }
void FileWriteRawLine(DataStream *haa, const char*towrite) {
  check_valid_file_handle(haa,"FileWriteRawLine");
  haa->Write(towrite,strlen(towrite));
  haa->WriteInt8 (13);
  haa->WriteInt8 (10);
  }
void FileRead(DataStream *haa,char*toread) {
  VALIDATE_STRING(toread);
  check_valid_file_handle(haa,"FileRead");
  if (haa->EOS()) {
    toread[0] = 0;
    return;
  }
  int lle=haa->ReadInt32();
  if ((lle>=200) | (lle<1)) quit("!FileRead: file was not written by FileWrite");
  haa->Read(toread,lle);
  }
int FileIsEOF (DataStream *haa) {
  check_valid_file_handle(haa,"FileIsEOF");
  if (haa->EOS())
    return 1;

  // TODO: stream errors
  if (ferror (((Common::FileStream*)haa)->GetHandle()))
    return 1;

  if (haa->GetPosition () >= haa->GetLength())
    return 1;
  return 0;
}
int FileIsError(DataStream *haa) {
  check_valid_file_handle(haa,"FileIsError");

  // TODO: stream errors
  if (ferror(((Common::FileStream*)haa)->GetHandle()))
    return 1;

  return 0;
}
void FileWriteInt(DataStream *haa,int into) {
  check_valid_file_handle(haa,"FileWriteInt");
  haa->WriteInt8('I');
  haa->WriteInt32(into);
  }
int FileReadInt(DataStream *haa) {
  check_valid_file_handle(haa,"FileReadInt");
  if (haa->EOS())
    return -1;
  if (haa->ReadInt8()!='I')
    quit("!FileReadInt: File read back in wrong order");
  return haa->ReadInt32();
  }
char FileReadRawChar(DataStream *haa) {
  check_valid_file_handle(haa,"FileReadRawChar");
  if (haa->EOS())
    return -1;
  return haa->ReadInt8();
  }
int FileReadRawInt(DataStream *haa) {
  check_valid_file_handle(haa,"FileReadRawInt");
  if (haa->EOS())
    return -1;
  return haa->ReadInt32();
}
void FileWriteRawChar(DataStream *haa, int chartoWrite) {
  check_valid_file_handle(haa,"FileWriteRawChar");
  if ((chartoWrite < 0) || (chartoWrite > 255))
    quit("!FileWriteRawChar: can only write values 0-255");

  haa->WriteInt8(chartoWrite);
}
