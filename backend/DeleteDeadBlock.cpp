/**
 * @file DeleteDeadBlock.cpp
 * @brief 删除无用的基本块实现文件
 * 
 * 本文件实现了 `DeleteDeadBlock` 类中的 `run` 方法，用于在编译过程中删除无用的基本块。
 * 主要功能包括：
 * - 遍历函数中的所有基本块
 * - 查找并合并只包含一条无条件跳转指令的基本块
 * - 更新所有指令中的基本块引用，确保引用指向合并后的基本块
 * - 删除所有未作为代表的基本块，即所有被合并的基本块
 */

/**
 * @brief 删除无用的基本块
 * 
 * 该函数通过合并冗余的基本块来简化控制流图。它主要通过查找和合并那些只包含一条无条件跳转指令的基本块来实现。
 * 
 * @param func RISC-V函数，包含一系列基本块
 * @return bool 总是返回false，表示未修改控制流图的结构
 */
#include "../include/backend/DeleteDeadBlock.hpp"
bool DeleteDeadBlock::run(RISCVFunction *func){
    std::unordered_map<RISCVBasicBlock*,RISCVBasicBlock*> alias;
    std::function<RISCVBasicBlock*(RISCVBasicBlock*)> findfa=[&](RISCVBasicBlock* bb){
        if(alias.find(bb)==alias.end())return bb;
        return alias[bb]=findfa(alias[bb]);
    };
    
    for(auto bb:*func){
        if(bb->Size()==1){
            auto minst=bb->back();
            if(minst->GetOpcode()==RISCVMIR::_j){
                auto target=minst->GetOperand(0)->as<RISCVBasicBlock>();
                assert(target!=nullptr);
                auto fv=findfa(target);
                if(fv!=bb)alias[bb]=fv;
            }
        }
    }
    for(auto bb:*func){
        if(findfa(bb)!=bb)continue;
        for(auto inst:*bb){
            for(auto i=0,limi=inst->GetOperandSize();i<limi;i++){
                auto op=inst->GetOperand(i)->as<RISCVBasicBlock>();
                if(op!=nullptr){
                    auto fv=findfa(op);
                    if(fv!=op)inst->SetOperand(i,fv);
                }
            }
        }
    }

    for(auto it=func->begin();it!=func->end();){
        auto bb=*it;++it;
        if(findfa(bb)!=bb)delete bb;
    }

    return false;
}