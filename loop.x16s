start:
        add %r1, %r0, $10

start1:
        ld  %r0, star
        putc
        add %r1, %r1, $-1
        brz stop
        jsr start1

stop:
        halt

star:
        val $42
newline:
        val $10
