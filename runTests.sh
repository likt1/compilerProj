#!/bin/bash
rm -R results
mkdir results
mkdir results/incorrect

./compiler programs/correct/iterativeFib.src >> results/iterativeFib.txt
./compiler programs/correct/logicals.src >> results/logicals.txt
./compiler programs/correct/math.src >> results/math.txt
./compiler programs/correct/multipleProcs.src >> results/multipleProcs.txt
./compiler programs/correct/recursiveFib.src >> results/recursiveFib.txt
./compiler programs/correct/source.src >> results/source.txt
./compiler programs/correct/test1.src >> results/test1.txt
./compiler programs/correct/test1b.src >> results/test1b.txt
./compiler programs/correct/test2.src >> results/test2.txt
./compiler programs/correct/test_heap.src >> results/test_heap.txt
./compiler programs/correct/test_program_array.src >> results/test_program_array.txt
./compiler programs/correct/test_program_minimal.src >> results/test_program_minimal.txt
./compiler programs/correct/vectorOps.src >> results/vectorOps.txt

./compiler programs/incorrect/lexer/invalidChars.src >> results/incorrect/invalidChars.txt
./compiler programs/incorrect/parser/badSource.src >> results/incorrect/badSource.txt
./compiler programs/incorrect/parser/iterativeFib.src >> results/incorrect/iterativeFib.txt
./compiler programs/incorrect/parser/logical.src >> results/incorrect/logical.txt
./compiler programs/incorrect/parser/recursiveFib.src >> results/incorrect/recursiveFib.txt
./compiler programs/incorrect/parser/vectorOps.src >> results/incorrect/vectorOps.txt
./compiler programs/incorrect/typecheck/assignBoolAndChar.src >> results/incorrect/assignBoolAndChar.txt
./compiler programs/incorrect/typecheck/assignFloatToInt.src >> results/incorrect/assignFloatToInt.txt

./compiler programs/incorrect/test1.src >> results/incorrect/test1.txt
./compiler programs/incorrect/test2.src >> results/incorrect/test2.txt
./compiler programs/incorrect/test3.src >> results/incorrect/test3.txt
