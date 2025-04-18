valgrind --tool=massif ./project/OffsetMain 0
valgrind --tool=massif ./project/OffsetMain 1
valgrind --tool=massif ./project/OffsetMain 2
valgrind --tool=massif ./project/OffsetMain 3

0
TIMER 1 :       
1
terminate called after throwing an instance of 'std::logic_error'
  what():  Error in file: /home/dmal_gradstudent_myt/Mesh/manifold/src/sort.cpp (277): 'Is2Manifold()' is false: mesh is not 2-manifold!
Aborted (core dumped)
2
terminate called after throwing an instance of 'std::logic_error'
  what():  Error in file: /home/dmal_gradstudent_myt/Mesh/manifold/src/sort.cpp (277): 'Is2Manifold()' is false: mesh is not 2-manifold!
Aborted (core dumped)
3
TIMER 1 :       9820.89 ms

no-manifold

3k6

Seg 200 no  manifold
Seg 100 no  manifold
Seg 50 no  manifold
Seg 40 240820 ms
Seg 30 172787 ms
Seg 20 94327.2 ms

0.3k

Seg 20 9820.89 ms