/**
 * @file BuildInFunctionTransform.cpp
 * @brief 内置函数转换实现文件
 * 
 * 本文件实现了 `BuildInFunctionTransform` 类中的 `run` 方法，用于在编译过程中对内置函数调用进行转换。
 * 主要功能包括：
 * - 遍历函数中的所有基本块
 * - 检查每个基本块中的指令，如果是 `CallInst` 类型的指令，则调用 `Trival::BuildInTransform` 进行转换
 * - 更新迭代器以确保遍历的正确性
 */
#include "../include/backend/BuildInFunctionTransform.hpp"
#include "../include/lib/Trival.hpp"

bool BuildInFunctionTransform::run(Function* func){
    for(auto bb:*func)
        for(auto it=bb->begin();it!=bb->end();++it){
            auto inst=*it;
            if(auto call=dynamic_cast<CallInst*>(inst))
                inst=Trival::BuildInTransform(call);
            it=mylist<BasicBlock,User>::iterator(inst);
        }
    return true;
}