#ifndef _CPU_H
#define _CPU_H

#include <stdint.h>
class Memory;
class Console;
class StateFile;

enum interruptTypes {
  interruptNone = 1,
  interruptNMI,
  interruptIRQ,
};

enum addressingModes {
  modeAbsolute = 1,
  modeAbsoluteX,
  modeAbsoluteY,
  modeAccumulator,
  modeImmediate,
  modeImplied,
  modeIndexedIndirect,
  modeIndirect,
  modeIndirectIndexed,
  modeRelative,
  modeZeroPage,
  modeZeroPageX,
  modeZeroPageY,
};

typedef struct stepInfo {
  uint16_t address;
  uint16_t pc;
  uint8_t	 mode;
} stepInfo;

class Cpu;
typedef void(Cpu::*InstrExecFunc)(stepInfo&);

class Cpu
{
public:
  Cpu(Console* console);
  ~Cpu();

  bool Save(StateFile*);
  bool Load(StateFile*);

  uint8_t mem[0x10000];
  void Reset();
  int Step();
  void triggerNMI();
  void triggerIRQ();
  uint8_t Read(uint16_t address);
  void Write(uint16_t address, uint8_t value);
  uint64_t Cycles; // number of cycles
  uint8_t interrupt;   // interrupt type to perform
  int stall;    // number of cycles to stall

private:
  Memory* memory;  // memory interface
  uint16_t PC; // program counter
  uint8_t SP;   // stack pointer
  uint8_t A;   // accumulator
  uint8_t X;   // x register
  uint8_t Y;   // y register
  uint8_t C;   // carry flag
  uint8_t Z;   // zero flag
  uint8_t I;   // interrupt disable flag
  uint8_t D;   // decimal mode flag
  uint8_t B;   // break command flag
  uint8_t U;   // unused flag
  uint8_t V;   // overflow flag
  uint8_t N;   // negative flag


  InstrExecFunc *table;

  void createTable();
  bool pagesDiffer(uint16_t a, uint16_t b);
  void addBranchCycles(stepInfo& info);
  void compare(uint8_t a, uint8_t b);
  uint16_t Read16(uint16_t address);
  uint16_t read16bug(uint16_t address);
  void push(uint8_t value);
  uint8_t pull();
  void push16(uint16_t value);
  uint16_t pull16();
  uint8_t Flags();
  void SetFlags(uint8_t flags);
  void setZ(uint8_t value);
  void setN(uint8_t value);
  void setZN(uint8_t value);

  void nmi();
  void irq();
  void adc(stepInfo& info);
  void AND(stepInfo& info);
  void asl(stepInfo& info);
  void bcc(stepInfo& info);
  void bcs(stepInfo& info);
  void beq(stepInfo& info);
  void bit(stepInfo& info);
  void bmi(stepInfo& info);
  void bne(stepInfo& info);
  void bpl(stepInfo& info);
  void brk(stepInfo& info);
  void bvc(stepInfo& info);
  void bvs(stepInfo& info);
  void clc(stepInfo& info);
  void cld(stepInfo& info);
  void cli(stepInfo& info);
  void clv(stepInfo& info);
  void cmp(stepInfo& info);
  void cpx(stepInfo& info);
  void cpy(stepInfo& info);
  void dec(stepInfo& info);
  void dex(stepInfo& info);
  void dey(stepInfo& info);
  void eor(stepInfo& info);
  void inc(stepInfo& info);
  void inx(stepInfo& info);
  void iny(stepInfo& info);
  void jmp(stepInfo& info);
  void jsr(stepInfo& info);
  void lda(stepInfo& info);
  void ldx(stepInfo& info);
  void ldy(stepInfo& info);
  void lsr(stepInfo& info);
  void nop(stepInfo& info);
  void ora(stepInfo& info);
  void pha(stepInfo& info);
  void php(stepInfo& info);
  void pla(stepInfo& info);
  void plp(stepInfo& info);
  void rol(stepInfo& info);
  void ror(stepInfo& info);
  void rti(stepInfo& info);
  void rts(stepInfo& info);
  void sbc(stepInfo& info);
  void sec(stepInfo& info);
  void sed(stepInfo& info);
  void sei(stepInfo& info);
  void sta(stepInfo& info);
  void stx(stepInfo& info);
  void sty(stepInfo& info);
  void tax(stepInfo& info);
  void tay(stepInfo& info);
  void tsx(stepInfo& info);
  void txa(stepInfo& info);
  void txs(stepInfo& info);
  void tya(stepInfo& info);
  void ahx(stepInfo& info);
  void alr(stepInfo& info);
  void anc(stepInfo& info);
  void arr(stepInfo& info);
  void axs(stepInfo& info);
  void dcp(stepInfo& info);
  void isc(stepInfo& info);
  void kil(stepInfo& info);
  void las(stepInfo& info);
  void lax(stepInfo& info);
  void rla(stepInfo& info);
  void rra(stepInfo& info);
  void sax(stepInfo& info);
  void shx(stepInfo& info);
  void shy(stepInfo& info);
  void slo(stepInfo& info);
  void sre(stepInfo& info);
  void tas(stepInfo& info);
  void xaa(stepInfo& info);

};

#endif