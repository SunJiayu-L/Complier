/**
 * @file RegAllocImpl.cpp
 * @brief 实现了寄存器分配的图着色算法。
 * 
 * 本文件包含了 `RegAllocImpl` 类的实现，该类负责执行寄存器分配的图着色算法。
 * 主要功能包括：
 * - 初始化图着色对象
 * - 运行图着色算法
 * - 释放图着色对象
 */
#include "../include/backend/RegAlloc.hpp"
#include "../include/backend/LegalizePass.hpp"
void spill_reg(Operand vreg);
void get_frameObj(Operand vreg);

void RegAllocImpl::RunGCpass() {
  gc = new GraphColor(m_func, ctx);
  gc->RunOnFunc();
  delete gc;
}