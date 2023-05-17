# compiles and runs the mesa emulator
all: mesaemu
    ./mesaemu ./rom/test.mesa

mesaemu:
    @echo "Compiling mesaemu..."
    gcc mesaemu.c mmem.c mvm.c -o mesaemu

mesac:
    @echo "Compiling mesac..."
    gcc mesac.c rom.c -o mesac

clean:
    rm mesaemu
