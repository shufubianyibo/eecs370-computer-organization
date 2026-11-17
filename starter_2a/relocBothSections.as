Main    lw 0 1 GlobalData
        sw 1 2 GlobalData
        lw 0 3 localData
Text2   add 1 2 3
        halt
localData .fill 42
GlobalData .fill 100
Ptr1    .fill Main
Ptr2    .fill Text2
Ptr3    .fill GlobalData
