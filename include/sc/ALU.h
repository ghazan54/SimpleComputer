#pragma once

int ALU (int command, int operand);

// Выполняет сложение слова в аккумуляторе и слова из указанной ячейки памяти
// (результат в аккумуляторе)
int alu_add (int operand);

// Вычитает из слова в аккумуляторе слово из указанной ячейки памяти
// (результат в аккумуляторе)
int alu_sub (int operand);

// Выполняет деление слова в аккумуляторе на слово из указанной ячейки памяти
// (результат в аккумуляторе)
int alu_divide (int operand);

// Вычисляет произведение слова в аккумуляторе на слово из указанной ячейки
// памяти (результат в аккумуляторе)
int alu_mul (int operand);

// Циклический двоичный сдвиг содержимого указанной ячейки памяти влево
// (результат в аккумуляторе)
int alu_rcl (int operand);
