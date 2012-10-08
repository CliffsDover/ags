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
// 'C'-style script compiler
//
//=============================================================================

#ifndef __CC_INSTANCE_H
#define __CC_INSTANCE_H

#include "cc_script.h"  // ccScript

#define SREG_SP           1     // stack pointer
#define SREG_MAR          2     // memory address register
#define SREG_AX           3     // general purpose
#define SREG_BX           4
#define SREG_CX           5
#define SREG_OP           6    // object pointer for member func calls
#define SREG_DX           7
#define CC_NUM_REGISTERS  8
#define INSTF_SHAREDATA   1
#define INSTF_ABORTED     2
#define INSTF_FREE        4
#define INSTF_RUNNING     8   // set by main code to confirm script isn't stuck
#define CC_STACK_SIZE     (1000 * sizeof(long))
#define MAX_CALL_STACK    100

// Running instance of the script
struct ccInstance
{
    long flags;
    char *globaldata;
    long globaldatasize;
    unsigned long *code;
    ccInstance *runningInst;  // might point to another instance if in far call
    long codesize;
    char *strings;
    long stringssize;
    char **exportaddr;  // real pointer to export
    char *stack;
    long stacksize;
    long registers[CC_NUM_REGISTERS];
    long pc;                        // program counter
    long line_number;               // source code line number
    ccScript *instanceof;
    long callStackLineNumber[MAX_CALL_STACK];
    long callStackAddr[MAX_CALL_STACK];
    ccInstance *callStackCodeInst[MAX_CALL_STACK];
    int  callStackSize;
    int  loadedInstanceId;
    int  returnValue;

#if defined(AGS_64BIT)
    // 64 bit: Variables to keep track of the size of the variables on the stack.
    // This is necessary because the compiled code accesses values on the stack with
    // absolute offsets that don't take into account the 8 byte long pointers on
    // 64 bit systems. These variables help with rewriting the offsets for the
    // modified stack.
    int stackSizes[CC_STACK_SIZE];
    int stackSizeIndex;
#endif
};

// [IKM] 2012-06-12
// This function was implemented in csrun.cpp and probably (not completely sure yet)
// belongs to runtime only, but it is referenced in cc_error() function which, by-turn,
// is referenced by AGS.Native.
// Hence I am moving it here until more elegant solution is found.

// returns the currently executing instance, or NULL if none
extern ccInstance *ccGetCurrentInstance(void);
extern ccInstance *current_instance; // should not be exposed this way, temporary solution

#endif
