# build software version of testbench (to check the "desired behaviour")
AHIR_INCLUDE=$(AHIR_RELEASE)/include
AHIR_LIB=$(AHIR_RELEASE)/lib
VHDL_LIB=$(AHIR_RELEASE)/vhdl
VHDL_VHPI_LIB=$(AHIR_RELEASE)/CtestBench/vhdl
FUNCTIONLIB=$(AHIR_RELEASE)/functionLibrary/
SRC=./src
all: HW 

TOPMODULES=-T filterDaemon

# five steps from C to vhdl simulator.
HW: aalink aa2vc  vc2vhdl vhdltb ghdlsim
SW: aalink aa2c

# This builds the AA2C simulation model using the testbench.
TO_C: aalink aa2c

# Aa linking..
aalink: $(SRC)/fir_filter.aa 
	AaLinkExtMem $(SRC)/fir_filter.aa | vcFormat > fir_filter.linked.aa
	AaOpt -B fir_filter.linked.aa | vcFormat > fir_filter.linked.opt.aa

# Aa to C..  this produces aa_c_model* files which are compiled
# with the testbench to produce an executable..
aa2c:fir_filter.linked.opt.aa
	Aa2C -o ./ $(TOPMODULES) fir_filter.linked.opt.aa  
	gcc -g -o testbench_sw -DSW -I ./ -I $(AHIR_INCLUDE)  aa_c_model.c src/testbench.c -L $(AHIR_LIB) -lPipeHandler -lpthread -l BitVectors -lSockPipes

# the linked optimized Aa is converted to a vC model.
aa2vc: fir_filter.linked.opt.aa
	Aa2VC -O -C prog.linked.opt.aa | vcFormat > fir_filter.vc


# vC to VHDL
vc2vhdl: fir_filter.vc
	rm -rf vhdl
	mkdir vhdl
	vc2vhdl -U -O -a -C -e ahir_system -w -s ghdl $(TOPMODULES) -f fir_filter.vc 
	vhdlFormat < ahir_system_global_package.unformatted_vhdl > vhdl/ahir_system_global_package.vhdl
	vhdlFormat < ahir_system.unformatted_vhdl > vhdl/ahir_system.vhdl
	vhdlFormat < ahir_system_test_bench.unformatted_vhdl > vhdl/ahir_system_test_bench.vhdl
	rm *.unformatted_vhdl

#
# build testbench and ghdl executables.
#
vhdltb: vhdl/ahir_system.vhdl vhdl/ahir_system_test_bench.vhdl $(SRC)/testbench.c vhdlCStubs.h vhdlCStubs.c
	gcc -c vhdlCStubs.c  -I$(SRC) -I./ -I$(AHIR_INCLUDE)
	gcc -D VERBOSE -c $(SRC)/testbench.c -I$(AHIR_INCLUDE) -I$(SRC) -I./
	gcc -o testbench_hw testbench.o vhdlCStubs.o  -L$(AHIR_LIB) -lSocketLibPipeHandler -lpthread

ghdlsim: vhdl/ahir_system.vhdl vhdl/ahir_system_test_bench.vhdl $(SRC)/testbench.c vhdlCStubs.h vhdlCStubs.c
	ghdl --clean
	ghdl --remove
	ghdl -i --work=GhdlLink  $(VHDL_LIB)/GhdlLink.vhdl
	ghdl -i --work=aHiR_ieee_proposed  $(VHDL_LIB)/aHiR_ieee_proposed.vhdl
	ghdl -i --work=ahir  $(VHDL_LIB)/ahir.vhdl
	ghdl -i --work=work vhdl/ahir_system_global_package.vhdl
	ghdl -i --work=work vhdl/ahir_system.vhdl
	ghdl -i --work=work vhdl/ahir_system_test_bench.vhdl
	ghdl -m --work=work -Wl,-L$(AHIR_LIB) -Wl,-lVhpi ahir_system_test_bench 

clean:
	rm -rf *.o* *.cf vhdl/* *.aa *.vc *.h *.c testbench_* PREFIX vhpi.log ahir_system_test_bench

PHONY: all clean	
