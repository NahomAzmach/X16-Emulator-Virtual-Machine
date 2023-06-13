CC=gcc
CPP=g++
CFLAGS=-I. -g
CPPFLAGS=-I. -g -std=c++11
DEPS = x16.h bits.h control.h instruction.h trap.h io.h
OBJ = x16.o bits.o control.o instruction.o trap.o io.o
MAIN = main.o
ASOBJ = xas.o instruction.o bits.o
AS = xas
ODOBJ = xod.o bits.o instruction.o
OD = xod
TARGET = x16
TESTTARGET = test_x16
TESTOBJ = test/test_main.o test/test_bits.o test/test_instruction.o \
	test/test_control_add.o test/test_control_and.o test/test_control_br.o \
	test/test_control_not.o test/test_control_jmp.o \
	test/test_control_jsr.o test/test_control_ld.o \
	test/test_control_ldi.o test/test_control_ldr.o \
	test/test_control_lea.o test/test_control_st.o \
	test/test_control_sti.o test/test_control_str.o \
	test/test_control_trap.o  \
	test/test_xas.cpp

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS)

x16: $(OBJ) $(MAIN)
	$(CC) -o $(TARGET) $^ $(CFLAGS)

clean:
	rm -rf *.o test/*.o $(TARGET) $(TESTTARGET) $(AS) test_x16.dSYM xod

run: x16
	./$(TARGET)

$(AS): $(ASOBJ)
	$(CC) -o $(AS) $^ $(CFLAGS)

$(OD): $(ODOBJ)
	$(CC) -o $(OD) $^ $(CFLAGS)


$(TESTTARGET): $(TESTOBJ) $(OBJ)
	$(CPP) -o $(TESTTARGET) $(TESTOBJ) $(OBJ) $(CPPFLAGS)

test-build: $(TESTTARGET) $(AS) $(TARGET)

test: $(TESTTARGET) xas x16
	./$(TESTTARGET) $(ARGS)

test-bits: $(TESTTARGET)
	./$(TESTTARGET) "[bits]"

test-instruction: $(TESTTARGET)
	./$(TESTTARGET) "[instruction]"

test-registerfile: $(TESTTARGET)
	./$(TESTTARGET) "[registerfile]"

test-alu: $(TESTTARGET)
	./$(TESTTARGET) "[alu]"

test-control-add: $(TESTTARGET)
	./$(TESTTARGET) "[control.add]"

test-control-and: $(TESTTARGET)
	./$(TESTTARGET) "[control.and]"

test-control-br: $(TESTTARGET)
	./$(TESTTARGET) "[control.br]"

test-control-jmp: $(TESTTARGET)
	./$(TESTTARGET) "[control.jmp]"

test-control-jsr: $(TESTTARGET)
	./$(TESTTARGET) "[control.jsr]"

test-control-ld: $(TESTTARGET)
	./$(TESTTARGET) "[control.ld]"

test-control-ldi: $(TESTTARGET)
	./$(TESTTARGET) "[control.ldi]"

test-control-ldr: $(TESTTARGET)
	./$(TESTTARGET) "[control.ldr]"

test-control-lea: $(TESTTARGET)
	./$(TESTTARGET) "[control.lea]"

test-control-not: $(TESTTARGET)
	./$(TESTTARGET) "[control.not]"

test-control-st: $(TESTTARGET)
	./$(TESTTARGET) "[control.st]"

test-control-sti: $(TESTTARGET)
	./$(TESTTARGET) "[control.sti]"

test-control-str: $(TESTTARGET)
	./$(TESTTARGET) "[control.str]"

test-control-trap: $(TESTTARGET)
	./$(TESTTARGET) "[control.trap]"

test-xas: $(TESTTARGET) xas x16
	./$(TESTTARGET) "[xas]"

test-memory-x16: $(TESTTARGET) x16
	./$(TESTTARGET) "Memory.x16"

test-memory-xas: $(TESTTARGET) x16 xas
	./$(TESTTARGET) "Memory.xas"

test-style:
	cpplint *.c
