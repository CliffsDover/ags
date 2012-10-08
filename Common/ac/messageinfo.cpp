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

#include <stdio.h>
#include "ac/messageinfo.h"
#include "util/datastream.h"

using AGS::Common::DataStream;

void MessageInfo::ReadFromFile(DataStream *in)
{
//#ifdef ALLEGRO_BIG_ENDIAN
    displayas = in->ReadInt8();
    flags = in->ReadInt8();
//#else
//    throw "DialogTopic::ReadFromFile() is not implemented for little-endian platforms and should not be called.";
//#endif
}