/**
 * @file BackendDCE.cpp
 * @brief 死代码消除 (Dead Code Elimination, DCE) 实现文件
 * 
 * 本文件实现了 `BackendDCE` 类中的方法，用于在 RISC-V 后端编译器中进行死代码消除。
 * 主要功能包括：
 * - 计算基本块的后继
 * - 遍历函数中的所有基本块并尝试删除无用指令
 * - 更新每个基本块的活跃变量集合
 * - 判断是否可以处理某个指令
 */
//? 对寄存器分配前的MIR优化

#include "../include/backend/BackendDCE.hpp"
using DCE = BackendDCE;

bool DCE::RunImpl() {
  for (auto b : *func)
    CalCulateSucc(b);
  return run(func);
}

bool DCE::run(RISCVFunction *func) {
  bool modified = false;
  wait_del.clear();
  RunOnFunction();
  for (RISCVBasicBlock *block : *func)
    modified |= RunOnBlock(block);
  while (!wait_del.empty()) {
    RISCVMIR *inst = wait_del.back();
    wait_del.pop_back();
    delete inst;
  }
  return modified;
}

bool DCE::RunOnBlock(RISCVBasicBlock *block) {
  std::unordered_set<MOperand> live = BlockLiveout[block];
  bool modified = false;
  for (auto it = block->rbegin(); it != block->rend();) {
    RISCVMIR *inst = *it;
    --it;
    if (CanHandle(inst) && TryDeleteInst(inst, live)) {
      modified = true;
      wait_del.push_back(inst);
      continue;
    }
    UpdateLive(inst, live);
  }
  return modified;
}

void DCE::UpdateLive(RISCVMIR *inst, std::unordered_set<MOperand> &live) {
  if (inst->GetOpcode() == RISCVMIR::RISCVISA::ret) {
    if (inst->GetOperandSize() > 0)
      live.insert(static_cast<Register *>(inst->GetOperand(0)));
    else {
      PhyRegister *Phy = PhyRegister::GetPhyReg(PhyRegister::PhyReg::a0);
      live.insert(Phy);
    }
    return;
  }
  if (auto def = inst->GetDef())
  {
    if(auto val = def->ignoreLA())
      live.erase(val);
  }
  for (int i = 0; i < inst->GetOperandSize(); i++) {
    if (auto op = inst->GetOperand(i)->ignoreLA())
      live.insert(op);
  }
}

bool DCE::TryDeleteInst(RISCVMIR *inst, std::unordered_set<MOperand> &live) {
  if(auto def = inst->GetDef())
  {
    if(auto val = def->ignoreLA())
    {
      if(!live.count(val))
        return true;
    }
    return false;
  }
  return false;
}

bool DCE::CanHandle(RISCVMIR *inst) {
  RISCVMIR::RISCVISA op = inst->GetOpcode();
  switch (op) {
  case RISCVMIR::RISCVISA::_j:
  case RISCVMIR::RISCVISA::_beq:
  case RISCVMIR::RISCVISA::_bne:
  case RISCVMIR::RISCVISA::_blt:
  case RISCVMIR::RISCVISA::_bge:
  case RISCVMIR::RISCVISA::_bltu:
  case RISCVMIR::RISCVISA::_bgeu:
  case RISCVMIR::RISCVISA::_bgt:
  case RISCVMIR::RISCVISA::_ble:
  case RISCVMIR::RISCVISA::_jal:
  case RISCVMIR::RISCVISA::_jalr:
  case RISCVMIR::RISCVISA::ret:
  case RISCVMIR::RISCVISA::call:
    return false;
  default:
    return true;
  }
}