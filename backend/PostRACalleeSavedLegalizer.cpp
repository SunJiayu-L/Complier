/**
 * @file PostRACalleeSavedLegalizer.cpp
 * @brief 实现了调用者保存寄存器的合法性检查和处理。
 * 
 * 本文件包含了 `PostRACalleeSavedLegalizer` 类的实现，该类负责在寄存器分配后处理调用者保存的寄存器。
 * 主要功能包括：
 * - 筛选出需要保存和恢复的调用者保存寄存器
 * - 在函数入口处保存这些寄存器
 * - 在函数出口处恢复这些寄存器
 */
#include "../include/backend/PostRACalleeSavedLegalizer.hpp"
#include "../include/backend/RISCVTrival.hpp"

bool PostRACalleeSavedLegalizer::run(RISCVFunction* func){
    uint64_t grp=func->GetUsedPhyRegMask();
    PhyRegMask::visit(grp,[&grp](uint64_t item){
        if(!PhyRegMask::isCalleeSaved(item))grp&=~item;
        if(PhyRegMask::GetPhyReg(item)==PhyRegister::GetPhyReg(PhyRegister::sp))grp&=~item;
    });
    auto entry=func->GetEntry();
    auto exit_bb=func->GetExit();
    auto& frame=func->GetFrame();
    PhyRegMask::visit(grp,[entry,exit_bb,&frame](uint64_t item){
        auto phyreg=PhyRegMask::GetPhyReg(item);
        // generate a sw
        auto sw=frame->spill(phyreg);
        entry->push_front(sw);
        
        auto stackReg=sw->GetOperand(1);
        // insert before ret
        auto reload=frame->load_to_preg(stackReg->as<StackRegister>(),phyreg);
        exit_bb->push_back(reload);
    });
    return true;
}
