#ifndef _CPU_H
#define _CPU_H

#include <cstdint>
#include <array>
#include <memory>

class Memory;
class Console;
class StateFile;

enum class InterruptType : uint8_t {
  None = 1,
  NMI,
  IRQ,
};

enum class AddressingMode : uint8_t {
  Absolute = 1,
  AbsoluteX,
  AbsoluteY,
  Accumulator,
  Immediate,
  Implied,
  IndexedIndirect,
  Indirect,
  IndirectIndexed,
  Relative,
  ZeroPage,
  ZeroPageX,
  ZeroPageY,
};

struct StepInfo {
  uint16_t address;
  uint16_t pc;
  uint8_t mode;
};

class Cpu;
using InstrExecFunc = void(Cpu::*)(StepInfo&);

class Cpu
{
public:
  explicit Cpu(Console* console);
  ~Cpu();
  
  Cpu(const Cpu&) = delete;
  Cpu& operator=(const Cpu&) = delete;
  Cpu(Cpu&&) = delete;
  Cpu& operator=(Cpu&&) = delete;

  bool Save(StateFile*);
  bool Load(StateFile*);

  std::array<uint8_t, 0x10000> mem{};
  void Reset();
  int Step();
  void triggerNMI();
  void triggerIRQ();
  uint8_t Read(uint16_t address);
  void Write(uint16_t address, uint8_t value);
  uint64_t Cycles{0}; // number of cycles
  InterruptType interrupt{InterruptType::None};   // interrupt type to perform
  int stall{0};    // number of cycles to stall

private:
  std::unique_ptr<Memory> memory;  // memory interface
  uint16_t PC{0}; // program counter
  uint8_t SP{0};   // stack pointer
  uint8_t A{0};   // accumulator
  uint8_t X{0};   // x register
  uint8_t Y{0};   // y register
  uint8_t C{0};   // carry flag
  uint8_t Z{0};   // zero flag
  uint8_t I{0};   // interrupt disable flag
  uint8_t D{0};   // decimal mode flag
  uint8_t B{0};   // break command flag
  uint8_t U{0};   // unused flag
  uint8_t V{0};   // overflow flag
  uint8_t N{0};   // negative flag


  std::unique_ptr<InstrExecFunc[]> table;

  void createTable();
  bool pagesDiffer(uint16_t a, uint16_t b) const;
  void addBranchCycles(StepInfo& info);
  void compare(uint8_t a, uint8_t b);
  uint16_t Read16(uint16_t address);
  uint16_t read16bug(uint16_t address);
  void push(uint8_t value);
  uint8_t pull();
  void push16(uint16_t value);
  uint16_t pull16();
  uint8_t Flags() const;
  void SetFlags(uint8_t flags);
  void setZ(uint8_t value);
  void setN(uint8_t value);
  void setZN(uint8_t value);

  void nmi();
  void irq();
  void adc(StepInfo& info);
  void AND(StepInfo& info);
  void asl(StepInfo& info);
  void bcc(StepInfo& info);
  void bcs(StepInfo& info);
  void beq(StepInfo& info);
  void bit(StepInfo& info);
  void bmi(StepInfo& info);
  void bne(StepInfo& info);
  void bpl(StepInfo& info);
  void brk(StepInfo& info);
  void bvc(StepInfo& info);
  void bvs(StepInfo& info);
  void clc(StepInfo& info);
  void cld(StepInfo& info);
  void cli(StepInfo& info);
  void clv(StepInfo& info);
  void cmp(StepInfo& info);
  void cpx(StepInfo& info);
  void cpy(StepInfo& info);
  void dec(StepInfo& info);
  void dex(StepInfo& info);
  void dey(StepInfo& info);
  void eor(StepInfo& info);
  void inc(StepInfo& info);
  void inx(StepInfo& info);
  void iny(StepInfo& info);
  void jmp(StepInfo& info);
  void jsr(StepInfo& info);
  void lda(StepInfo& info);
  void ldx(StepInfo& info);
  void ldy(StepInfo& info);
  void lsr(StepInfo& info);
  void nop(StepInfo& info);
  void ora(StepInfo& info);
  void pha(StepInfo& info);
  void php(StepInfo& info);
  void pla(StepInfo& info);
  void plp(StepInfo& info);
  void rol(StepInfo& info);
  void ror(StepInfo& info);
  void rti(StepInfo& info);
  void rts(StepInfo& info);
  void sbc(StepInfo& info);
  void sec(StepInfo& info);
  void sed(StepInfo& info);
  void sei(StepInfo& info);
  void sta(StepInfo& info);
  void stx(StepInfo& info);
  void sty(StepInfo& info);
  void tax(StepInfo& info);
  void tay(StepInfo& info);
  void tsx(StepInfo& info);
  void txa(StepInfo& info);
  void txs(StepInfo& info);
  void tya(StepInfo& info);
  void ahx(StepInfo& info);
  void alr(StepInfo& info);
  void anc(StepInfo& info);
  void arr(StepInfo& info);
  void axs(StepInfo& info);
  void dcp(StepInfo& info);
  void isc(StepInfo& info);
  void kil(StepInfo& info);
  void las(StepInfo& info);
  void lax(StepInfo& info);
  void rla(StepInfo& info);
  void rra(StepInfo& info);
  void sax(StepInfo& info);
  void shx(StepInfo& info);
  void shy(StepInfo& info);
  void slo(StepInfo& info);
  void sre(StepInfo& info);
  void tas(StepInfo& info);
  void xaa(StepInfo& info);

};

#endif