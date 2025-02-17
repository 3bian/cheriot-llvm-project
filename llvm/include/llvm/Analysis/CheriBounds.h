#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

namespace llvm {

class CheriNeedBoundsChecker {
public:
  CheriNeedBoundsChecker(AllocaInst *AI, const DataLayout &DL)
      : RootInst(AI), DL(DL) {
    auto AllocaSize = AI->getAllocationSizeInBits(DL);
    if (AllocaSize)
      MinSizeInBytes = *AllocaSize / 8;
    PointerAS = AI->getType()->getAddressSpace();
  }
  CheriNeedBoundsChecker(Instruction *I, std::optional<uint64_t> MinSize,
                         const DataLayout &DL)
      : RootInst(I), DL(DL), MinSizeInBytes(MinSize) {
    assert(I->getType()->isPointerTy());
    PointerAS = I->getType()->getPointerAddressSpace();
  }
  bool check(const Use &U, bool Simple = false) const;
  void findUsesThatNeedBounds(SmallVectorImpl<Use *> *UsesThatNeedBounds,
                              bool BoundAllUses, bool Simple = false) const;
  bool anyUseNeedsBounds() const;

private:
  bool useNeedsBounds(const Use &U, const APInt &CurrentGEPOffset,
                      unsigned Depth, unsigned MaxDepth) const;
  bool anyUserNeedsBounds(const Instruction *I, const APInt &CurrentGEPOffset,
                          unsigned Depth, unsigned MaxDepth) const;
  bool canLoadStoreBeOutOfBounds(const Instruction *I, const Use &U,
                                 const APInt &CurrentGEPOffset,
                                 unsigned Depth) const;

  Instruction *RootInst;
  const DataLayout &DL;
  std::optional<uint64_t> MinSizeInBytes;
  unsigned PointerAS = 0;
};

} // namespace llvm
