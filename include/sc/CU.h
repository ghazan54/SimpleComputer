#pragma once

extern int operations;
extern int accumulator;
extern int instructionCounter;

int CU (void);

// Ввод с терминала в указанную ячейку памяти с контролем переполнения
int cu_read (int operand);

// Вывод на терминал значение указанной ячейки памяти
int cu_write (int operand);

// Загрузка в аккумулятор значения из указанного адреса памяти
int cu_load (int operand);

// Выгружает значение из аккумулятора по указанному адресу памяти
int cu_store (int operand);

// Переход к указанному адресу памяти
int cu_jump (int operand);
// Переход к указанному адресу памяти, если в аккумуляторе находится
// отрицательное число
int cu_jneg (int operand);

// Переход к указанному адресу памяти, если в аккумуляторе находится ноль
int cu_jz (int operand);

// Останов, выполняется при завершении работы программы
int cu_halt ();
