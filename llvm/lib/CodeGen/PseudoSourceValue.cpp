//===-- llvm/CodeGen/PseudoSourceValue.cpp ----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the PseudoSourceValue class.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

static const char *const PSVNames[] = {
    "Stack", "GOT", "CapTable", "JumpTable", "ConstantPool", "FixedStack",
    "GlobalValueCallEntry", "ExternalSymbolCallEntry"};
static_assert(std::size(PSVNames) == PseudoSourceValue::TargetCustom, "Missing entry?");

PseudoSourceValue::PseudoSourceValue(unsigned Kind, const TargetMachine &TM)
    : Kind(Kind) {
  AddressSpace = TM.getAddressSpaceForPseudoSourceKind(Kind);
}

PseudoSourceValue::~PseudoSourceValue() = default;

void PseudoSourceValue::printCustom(raw_ostream &O) const {
  if (Kind < TargetCustom)
    O << PSVNames[Kind];
  else
    O << "TargetCustom" << (Kind - 1);
  // XXXAR: Kind -1 to offset the added CapTable entry (easier than merge
  // conflicts in multiple test files)
  // FIXME: this should probably be (Kind - TargetCustom)
}

bool PseudoSourceValue::isConstant(const MachineFrameInfo *) const {
  if (isStack())
    return false;
  if (isGOT() || isCapTable() || isConstantPool() || isJumpTable())
    return true;
  llvm_unreachable("Unknown PseudoSourceValue!");
}

bool PseudoSourceValue::isAliased(const MachineFrameInfo *) const {
  if (isStack() || isGOT() || isCapTable() || isConstantPool() || isJumpTable())
    return false;
  llvm_unreachable("Unknown PseudoSourceValue!");
}

bool PseudoSourceValue::mayAlias(const MachineFrameInfo *) const {
  return !(isGOT() || isCapTable() || isConstantPool() || isJumpTable());
}

bool FixedStackPseudoSourceValue::isConstant(
    const MachineFrameInfo *MFI) const {
  return MFI && MFI->isImmutableObjectIndex(FI);
}

bool FixedStackPseudoSourceValue::isAliased(const MachineFrameInfo *MFI) const {
  if (!MFI)
    return true;
  return MFI->isAliasedObjectIndex(FI);
}

bool FixedStackPseudoSourceValue::mayAlias(const MachineFrameInfo *MFI) const {
  if (!MFI)
    return true;
  // Spill slots will not alias any LLVM IR value.
  return !MFI->isSpillSlotObjectIndex(FI);
}

void FixedStackPseudoSourceValue::printCustom(raw_ostream &OS) const {
  OS << "FixedStack" << FI;
}

CallEntryPseudoSourceValue::CallEntryPseudoSourceValue(unsigned Kind,
                                                       const TargetMachine &TM)
    : PseudoSourceValue(Kind, TM) {}

bool CallEntryPseudoSourceValue::isConstant(const MachineFrameInfo *) const {
  return false;
}

bool CallEntryPseudoSourceValue::isAliased(const MachineFrameInfo *) const {
  return false;
}

bool CallEntryPseudoSourceValue::mayAlias(const MachineFrameInfo *) const {
  return false;
}

GlobalValuePseudoSourceValue::GlobalValuePseudoSourceValue(
    const GlobalValue *GV, const TargetMachine &TM)
    : CallEntryPseudoSourceValue(GlobalValueCallEntry, TM), GV(GV) {}
ExternalSymbolPseudoSourceValue::ExternalSymbolPseudoSourceValue(
    const char *ES, const TargetMachine &TM)
    : CallEntryPseudoSourceValue(ExternalSymbolCallEntry, TM), ES(ES) {}

PseudoSourceValueManager::PseudoSourceValueManager(const TargetMachine &TMInfo)
    : TM(TMInfo), StackPSV(PseudoSourceValue::Stack, TM),
      GOTPSV(PseudoSourceValue::GOT, TM),
      CapTablePSV(PseudoSourceValue::CapTable, TM),
      JumpTablePSV(PseudoSourceValue::JumpTable, TM),
      ConstantPoolPSV(PseudoSourceValue::ConstantPool, TM) {}

const PseudoSourceValue *PseudoSourceValueManager::getStack() {
  return &StackPSV;
}

const PseudoSourceValue *PseudoSourceValueManager::getGOT() { return &GOTPSV; }

const PseudoSourceValue *PseudoSourceValueManager::getCapTable() {
  return &CapTablePSV;
}

const PseudoSourceValue *PseudoSourceValueManager::getConstantPool() {
  return &ConstantPoolPSV;
}

const PseudoSourceValue *PseudoSourceValueManager::getJumpTable() {
  return &JumpTablePSV;
}

const PseudoSourceValue *
PseudoSourceValueManager::getFixedStack(int FI) {
  std::unique_ptr<FixedStackPseudoSourceValue> &V = FSValues[FI];
  if (!V)
    V = std::make_unique<FixedStackPseudoSourceValue>(FI, TM);
  return V.get();
}

const PseudoSourceValue *
PseudoSourceValueManager::getGlobalValueCallEntry(const GlobalValue *GV) {
  std::unique_ptr<const GlobalValuePseudoSourceValue> &E =
      GlobalCallEntries[GV];
  if (!E)
    E = std::make_unique<GlobalValuePseudoSourceValue>(GV, TM);
  return E.get();
}

const PseudoSourceValue *
PseudoSourceValueManager::getExternalSymbolCallEntry(const char *ES) {
  std::unique_ptr<const ExternalSymbolPseudoSourceValue> &E =
      ExternalCallEntries[ES];
  if (!E)
    E = std::make_unique<ExternalSymbolPseudoSourceValue>(ES, TM);
  return E.get();
}
