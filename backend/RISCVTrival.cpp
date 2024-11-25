/**
 * @file RISCVTrival.cpp
 * @brief RISC-V 简单指令生成类的实现文件
 * @details 本文件实现了 RISC-V 架构中简单指令的生成方法，包括从源操作数复制到目标虚拟寄存器的指令生成。
 */
#include "../include/backend/RISCVTrival.hpp"

RISCVMIR* RISCVTrival::CopyFrom(VirRegister* dst,RISCVMOperand* src){
    RISCVMIR* copyinst = nullptr;
    if(auto imm=src->as<Imm>()){
        assert(imm->GetType()==RISCVType::riscv_i32);
        copyinst=new RISCVMIR(RISCVMIR::li);
    }
    else if(dst->GetType()==RISCVType::riscv_float32 && src->GetType()==RISCVType::riscv_float32) {
        copyinst=new RISCVMIR(RISCVMIR::_fmv_s);
    }
    else{
        copyinst=new RISCVMIR(RISCVMIR::mv);
    }
    // else assert(0&&"Error Type!");
        copyinst->SetDef(dst);
        copyinst->AddOperand(src);
    return copyinst;
}