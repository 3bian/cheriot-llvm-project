//===-- Mips.h - Top-level interface for Mips representation ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM Mips back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MIPS_MIPS_H
#define LLVM_LIB_TARGET_MIPS_MIPS_H

#include "MCTargetDesc/MipsMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class FunctionPass;
class InstructionSelector;
class MachineFunctionPass;
class MipsRegisterBankInfo;
class MipsSubtarget;
class MipsTargetMachine;
class MipsTargetMachine;
class ModulePass;
class PassRegistry;

ModulePass *createMipsOs16Pass();
ModulePass *createMips16HardFloatPass();

FunctionPass *createMipsModuleISelDagPass();
FunctionPass *createMipsOptimizePICCallPass();
FunctionPass *createMipsDelaySlotFillerPass();
FunctionPass *createMipsBranchExpansion();
FunctionPass *createMipsConstantIslandPass();
FunctionPass *createMicroMipsSizeReducePass();
FunctionPass *createMipsExpandPseudoPass();
FunctionPass *createMipsPreLegalizeCombiner();
FunctionPass *createMipsPostLegalizeCombiner(bool IsOptNone);
FunctionPass *createMipsMulMulBugPass();

FunctionPass *createCheriInvalidatePass();
FunctionPass *createCheriRangeChecker();
FunctionPass *createCheriLoopPointerDecanonicalize();

MachineFunctionPass *createCheriAddressingModeFolder();
MachineFunctionPass *createCheri128FailHardPass();
InstructionSelector *createMipsInstructionSelector(const MipsTargetMachine &,
                                                   MipsSubtarget &,
                                                   MipsRegisterBankInfo &);

void initializeMicroMipsSizeReducePass(PassRegistry &);
void initializeMipsBranchExpansionPass(PassRegistry &);
void initializeMipsDAGToDAGISelPass(PassRegistry &);
void initializeMipsDelaySlotFillerPass(PassRegistry &);
void initializeMipsMulMulBugFixPass(PassRegistry &);
void initializeMipsOptimizePICCallPass(PassRegistry &);
void initializeCheriAddressingModeFolderPass(PassRegistry &);
void initializeCheriRangeCheckerPass(PassRegistry &);
void initializeMipsPostLegalizerCombinerPass(PassRegistry &);
void initializeMipsPreLegalizerCombinerPass(PassRegistry &);
} // namespace llvm

#endif
