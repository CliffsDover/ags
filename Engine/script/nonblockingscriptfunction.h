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
//
//
//
//=============================================================================
#ifndef __AGS_EE_SCRIPT__NONBLOCKINGSCRIPTFUNCTION_H
#define __AGS_EE_SCRIPT__NONBLOCKINGSCRIPTFUNCTION_H

#include "ac/runtime_defines.h"

struct NonBlockingScriptFunction
{
    const char* functionName;
    int numParameters;
    void* param1;
    void* param2;
    bool roomHasFunction;
    bool globalScriptHasFunction;
    bool moduleHasFunction[MAX_SCRIPT_MODULES];
    bool atLeastOneImplementationExists;

    NonBlockingScriptFunction(const char*funcName, int numParams)
    {
        this->functionName = funcName;
        this->numParameters = numParams;
        atLeastOneImplementationExists = false;
        roomHasFunction = true;
        globalScriptHasFunction = true;

        for (int i = 0; i < MAX_SCRIPT_MODULES; i++)
        {
            moduleHasFunction[i] = true;
        }
    }
};

#endif // __AGS_EE_SCRIPT__NONBLOCKINGSCRIPTFUNCTION_H
