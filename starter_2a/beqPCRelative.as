        lw 0 1 five
loop    beq 0 1 done
        lw 0 2 neg1
        add 1 2 1
        beq 0 0 loop
done    halt
        noop
five    .fill 5
neg1    .fill -1
