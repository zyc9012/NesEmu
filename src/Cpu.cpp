#include "Cpu.h"
#include "Memory.h"
#include "Console.h"
#include "StateFile.h"

stepInfo fakeStepInfo;

Cpu::Cpu(Console* console)
{
  memory = new CpuMemory(console);
  createTable();
  Reset();
}


Cpu::~Cpu()
{
}

// instructionModes indicates the addressing mode for each instruction
uint8_t instructionModes[256]{
  6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
};

// instructionSizes indicates the size of each instruction in uint8_ts
uint8_t instructionSizes[256]{
  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
  2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
};

// instructionCycles indicates the number of cycles used by each instruction,
// not including conditional cycles
uint8_t instructionCycles[256]{
  7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
  2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
  2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
  2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
  2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

// instructionPageCycles indicates the number of cycles used by each
// instruction when a page is crossed
uint8_t instructionPageCycles[256]{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

// instructionNames indicates the name of each instruction
const char* instructionNames[256]{
  "BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
  "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
  "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
  "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
  "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
  "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
  "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
  "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
  "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
  "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
  "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
  "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
  "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
  "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
  "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
  "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
  "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
  "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
  "BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
  "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
  "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
  "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
  "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
  "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
  "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
  "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
  "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
  "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
  "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC",
  "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
  "BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC",
  "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC",
};

// createTable builds a function table for each instruction
void Cpu::createTable() {
  table = new InstrExecFunc[256]{
    &Cpu::brk, &Cpu::ora, &Cpu::kil, &Cpu::slo, &Cpu::nop, &Cpu::ora, &Cpu::asl, &Cpu::slo,
    &Cpu::php, &Cpu::ora, &Cpu::asl, &Cpu::anc, &Cpu::nop, &Cpu::ora, &Cpu::asl, &Cpu::slo,
    &Cpu::bpl, &Cpu::ora, &Cpu::kil, &Cpu::slo, &Cpu::nop, &Cpu::ora, &Cpu::asl, &Cpu::slo,
    &Cpu::clc, &Cpu::ora, &Cpu::nop, &Cpu::slo, &Cpu::nop, &Cpu::ora, &Cpu::asl, &Cpu::slo,
    &Cpu::jsr, &Cpu::AND, &Cpu::kil, &Cpu::rla, &Cpu::bit, &Cpu::AND, &Cpu::rol, &Cpu::rla,
    &Cpu::plp, &Cpu::AND, &Cpu::rol, &Cpu::anc, &Cpu::bit, &Cpu::AND, &Cpu::rol, &Cpu::rla,
    &Cpu::bmi, &Cpu::AND, &Cpu::kil, &Cpu::rla, &Cpu::nop, &Cpu::AND, &Cpu::rol, &Cpu::rla,
    &Cpu::sec, &Cpu::AND, &Cpu::nop, &Cpu::rla, &Cpu::nop, &Cpu::AND, &Cpu::rol, &Cpu::rla,
    &Cpu::rti, &Cpu::eor, &Cpu::kil, &Cpu::sre, &Cpu::nop, &Cpu::eor, &Cpu::lsr, &Cpu::sre,
    &Cpu::pha, &Cpu::eor, &Cpu::lsr, &Cpu::alr, &Cpu::jmp, &Cpu::eor, &Cpu::lsr, &Cpu::sre,
    &Cpu::bvc, &Cpu::eor, &Cpu::kil, &Cpu::sre, &Cpu::nop, &Cpu::eor, &Cpu::lsr, &Cpu::sre,
    &Cpu::cli, &Cpu::eor, &Cpu::nop, &Cpu::sre, &Cpu::nop, &Cpu::eor, &Cpu::lsr, &Cpu::sre,
    &Cpu::rts, &Cpu::adc, &Cpu::kil, &Cpu::rra, &Cpu::nop, &Cpu::adc, &Cpu::ror, &Cpu::rra,
    &Cpu::pla, &Cpu::adc, &Cpu::ror, &Cpu::arr, &Cpu::jmp, &Cpu::adc, &Cpu::ror, &Cpu::rra,
    &Cpu::bvs, &Cpu::adc, &Cpu::kil, &Cpu::rra, &Cpu::nop, &Cpu::adc, &Cpu::ror, &Cpu::rra,
    &Cpu::sei, &Cpu::adc, &Cpu::nop, &Cpu::rra, &Cpu::nop, &Cpu::adc, &Cpu::ror, &Cpu::rra,
    &Cpu::nop, &Cpu::sta, &Cpu::nop, &Cpu::sax, &Cpu::sty, &Cpu::sta, &Cpu::stx, &Cpu::sax,
    &Cpu::dey, &Cpu::nop, &Cpu::txa, &Cpu::xaa, &Cpu::sty, &Cpu::sta, &Cpu::stx, &Cpu::sax,
    &Cpu::bcc, &Cpu::sta, &Cpu::kil, &Cpu::ahx, &Cpu::sty, &Cpu::sta, &Cpu::stx, &Cpu::sax,
    &Cpu::tya, &Cpu::sta, &Cpu::txs, &Cpu::tas, &Cpu::shy, &Cpu::sta, &Cpu::shx, &Cpu::ahx,
    &Cpu::ldy, &Cpu::lda, &Cpu::ldx, &Cpu::lax, &Cpu::ldy, &Cpu::lda, &Cpu::ldx, &Cpu::lax,
    &Cpu::tay, &Cpu::lda, &Cpu::tax, &Cpu::lax, &Cpu::ldy, &Cpu::lda, &Cpu::ldx, &Cpu::lax,
    &Cpu::bcs, &Cpu::lda, &Cpu::kil, &Cpu::lax, &Cpu::ldy, &Cpu::lda, &Cpu::ldx, &Cpu::lax,
    &Cpu::clv, &Cpu::lda, &Cpu::tsx, &Cpu::las, &Cpu::ldy, &Cpu::lda, &Cpu::ldx, &Cpu::lax,
    &Cpu::cpy, &Cpu::cmp, &Cpu::nop, &Cpu::dcp, &Cpu::cpy, &Cpu::cmp, &Cpu::dec, &Cpu::dcp,
    &Cpu::iny, &Cpu::cmp, &Cpu::dex, &Cpu::axs, &Cpu::cpy, &Cpu::cmp, &Cpu::dec, &Cpu::dcp,
    &Cpu::bne, &Cpu::cmp, &Cpu::kil, &Cpu::dcp, &Cpu::nop, &Cpu::cmp, &Cpu::dec, &Cpu::dcp,
    &Cpu::cld, &Cpu::cmp, &Cpu::nop, &Cpu::dcp, &Cpu::nop, &Cpu::cmp, &Cpu::dec, &Cpu::dcp,
    &Cpu::cpx, &Cpu::sbc, &Cpu::nop, &Cpu::isc, &Cpu::cpx, &Cpu::sbc, &Cpu::inc, &Cpu::isc,
    &Cpu::inx, &Cpu::sbc, &Cpu::nop, &Cpu::sbc, &Cpu::cpx, &Cpu::sbc, &Cpu::inc, &Cpu::isc,
    &Cpu::beq, &Cpu::sbc, &Cpu::kil, &Cpu::isc, &Cpu::nop, &Cpu::sbc, &Cpu::inc, &Cpu::isc,
    &Cpu::sed, &Cpu::sbc, &Cpu::nop, &Cpu::isc, &Cpu::nop, &Cpu::sbc, &Cpu::inc, &Cpu::isc,
  };
}

bool Cpu::Save(StateFile* f) {
  f->Put(&Cycles);
  f->Put(&PC);
  f->Put(&SP);
  f->Put(&A);
  f->Put(&X);
  f->Put(&Y);
  f->Put(&C);
  f->Put(&Z);
  f->Put(&I);
  f->Put(&D);
  f->Put(&B);
  f->Put(&U);
  f->Put(&V);
  f->Put(&N);
  f->Put(&interrupt);
  f->Put(&stall);
  return true;
}

bool Cpu::Load(StateFile* f) {
  f->Get(&Cycles);
  f->Get(&PC);
  f->Get(&SP);
  f->Get(&A);
  f->Get(&X);
  f->Get(&Y);
  f->Get(&C);
  f->Get(&Z);
  f->Get(&I);
  f->Get(&D);
  f->Get(&B);
  f->Get(&U);
  f->Get(&V);
  f->Get(&N);
  f->Get(&interrupt);
  f->Get(&stall);
  return true;
}

// Reset resets the CPU to its initial powerup state
void Cpu::Reset() {
  PC = Read16(0xFFFC);
  SP = 0xFD;
  SetFlags(0x24);
}

// pagesDiffer returns true if the two addresses reference different pages
bool Cpu::pagesDiffer(uint16_t a, uint16_t b) {
  return (a & 0xFF00) != (b & 0xFF00);
}

// addBranchCycles adds a cycle for taking a branch and adds another cycle
// if the branch jumps to a new page
void Cpu::addBranchCycles(stepInfo& info) {
  Cycles++;
  if (pagesDiffer(info.pc, info.address)) {
    Cycles++;
  }
}

void Cpu::compare(uint8_t a, uint8_t b) {
  setZN(a - b);
  if (a >= b) {
    C = 1;
  }
  else {
    C = 0;
  }
}

uint8_t Cpu::Read(uint16_t address) {
  return memory->Read(address);
}

void Cpu::Write(uint16_t address, uint8_t value) {
  memory->Write(address, value);
}

// Read16 reads two uint8_ts using Read to return a double-word value
uint16_t Cpu::Read16(uint16_t address) {
  auto lo = uint16_t(Read(address));
  auto hi = uint16_t(Read(address + 1));
  return (hi << 8) | lo;
}

// read16bug emulates a 6502 bug that caused the low uint8_t to wrap without
// incrementing the high uint8_t
uint16_t Cpu::read16bug(uint16_t address) {
  auto a = address;
  auto b = (a & 0xFF00) | ((a + 1) & 0x00FF);
  auto lo = Read(a);
  auto hi = Read(b);
  return (uint16_t(hi) << 8) | uint16_t(lo);
}

// push pushes a uint8_t onto the stack
void Cpu::push(uint8_t value) {
  Write(0x100 | uint16_t(SP), value);
  SP--;
}

// pull pops a uint8_t from the stack
uint8_t Cpu::pull() {
  SP++;
  return Read(0x100 | uint16_t(SP));
}

// push16 pushes two uint8_ts onto the stack
void Cpu::push16(uint16_t value) {
  auto hi = uint8_t(value >> 8);
  auto lo = uint8_t(value & 0xFF);
  push(hi);
  push(lo);
}

// pull16 pops two uint8_ts from the stack
uint16_t Cpu::pull16() {
  auto lo = uint16_t(pull());
  auto hi = uint16_t(pull());
  return hi << 8 | lo;
}

// Flags returns the processor status flags
uint8_t Cpu::Flags() {
  uint8_t flags = 0;
  flags |= C << 0;
  flags |= Z << 1;
  flags |= I << 2;
  flags |= D << 3;
  flags |= B << 4;
  flags |= U << 5;
  flags |= V << 6;
  flags |= N << 7;
  return flags;
}

// SetFlags sets the processor status flags
void Cpu::SetFlags(uint8_t flags) {
  C = (flags >> 0) & 1;
  Z = (flags >> 1) & 1;
  I = (flags >> 2) & 1;
  D = (flags >> 3) & 1;
  B = (flags >> 4) & 1;
  U = (flags >> 5) & 1;
  V = (flags >> 6) & 1;
  N = (flags >> 7) & 1;
}

// setZ sets the zero flag if the argument is zero
void Cpu::setZ(uint8_t value) {
  if (value == 0) {
    Z = 1;
  }
  else {
    Z = 0;
  }
}

// setN sets the negative flag if the argument is negative (high bit is set)
void Cpu::setN(uint8_t value) {
  if ((value & 0x80) != 0) {
    N = 1;
  }
  else {
    N = 0;
  }
}

// setZN sets the zero flag and the negative flag
void Cpu::setZN(uint8_t value) {
  setZ(value);
  setN(value);
}

// triggerNMI causes a non-maskable interrupt to occur on the next cycle
void Cpu::triggerNMI() {
  interrupt = interruptNMI;
}

// triggerIRQ causes an IRQ interrupt to occur on the next cycle
void Cpu::triggerIRQ() {
  if (I == 0) {
    interrupt = interruptIRQ;
  }
}

// Step executes a single CPU instruction
int Cpu::Step() {
  if (stall > 0) {
    stall--;
    return 1;
  }

  auto cycles = Cycles;

  switch (interrupt) {
  case interruptNMI:
    nmi(); break;
  case interruptIRQ:
    irq(); break;
  }
  interrupt = interruptNone;

  auto opcode = Read(PC);
  auto mode = instructionModes[opcode];

  uint16_t address, offset;
  bool pageCrossed = false;
  switch (mode) {
  case modeAbsolute:
    address = Read16(PC + 1);
    break;
  case modeAbsoluteX:
    address = Read16(PC + 1) + uint16_t(X);
    pageCrossed = pagesDiffer(address - uint16_t(X), address);
    break;
  case modeAbsoluteY:
    address = Read16(PC + 1) + uint16_t(Y);
    pageCrossed = pagesDiffer(address - uint16_t(Y), address);
    break;
  case modeAccumulator:
    address = 0;
    break;
  case modeImmediate:
    address = PC + 1;
    break;
  case modeImplied:
    address = 0;
    break;
  case modeIndexedIndirect:
    address = read16bug(uint8_t(Read(PC + 1) + X));
    break;
  case modeIndirect:
    address = read16bug(Read16(PC + 1));
    break;
  case modeIndirectIndexed:
    address = read16bug(uint16_t(Read(PC + 1))) + uint16_t(Y);
    pageCrossed = pagesDiffer(address - uint16_t(Y), address);
    break;
  case modeRelative:
    offset = uint16_t(Read(PC + 1));
    if (offset < 0x80) {
      address = PC + 2 + offset;
    }
    else {
      address = PC + 2 + offset - 0x100;
    }
    break;
  case modeZeroPage:
    address = uint8_t(Read(PC + 1));
    break;
  case modeZeroPageX:
    address = uint8_t(Read(PC + 1) + X) & 0xFF;
    break;
  case modeZeroPageY:
    address = uint8_t(Read(PC + 1) + Y) & 0xFF;
    break;
  }

  PC += uint16_t(instructionSizes[opcode]);
  Cycles += uint64_t(instructionCycles[opcode]);
  if (pageCrossed) {
    Cycles += uint64_t(instructionPageCycles[opcode]);
  }
  stepInfo info;
  info.address = address;
  info.pc = PC;
  info.mode = mode;

  InstrExecFunc e = table[opcode];
  (this->*e)(info);

  return int(Cycles - cycles);
}

// NMI - Non-Maskable Interrupt
void Cpu::nmi() {
  push16(PC);
  php(fakeStepInfo);
  PC = Read16(0xFFFA);
  I = 1;
  Cycles += 7;
}

// IRQ - IRQ Interrupt
void Cpu::irq() {
  push16(PC);
  php(fakeStepInfo);
  PC = Read16(0xFFFE);
  I = 1;
  Cycles += 7;
}

// ADC - Add with Carry
void Cpu::adc(stepInfo& info) {
  auto a = A;
  auto b = Read(info.address);
  auto c = C;
  A = a + b + c;
  setZN(A);
  if (int(a) + int(b) + int(c) > 0xFF) {
    C = 1;
  }
  else {
    C = 0;
  }
  if (((a^b) & 0x80) == 0 && ((a^A) & 0x80) != 0) {
    V = 1;
  }
  else {
    V = 0;
  }
}

// AND - Logical AND
void Cpu::AND(stepInfo& info) {
  A = A & Read(info.address);
  setZN(A);
}

// ASL - Arithmetic Shift Left
void Cpu::asl(stepInfo& info) {
  if (info.mode == modeAccumulator) {
    C = (A >> 7) & 1;
    A <<= 1;
    setZN(A);
  }
  else {
    auto value = Read(info.address);
    C = (value >> 7) & 1;
    value <<= 1;
    Write(info.address, value);
    setZN(value);
  }
}

// BCC - Branch if Carry Clear
void Cpu::bcc(stepInfo& info) {
  if (C == 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// BCS - Branch if Carry Set
void Cpu::bcs(stepInfo& info) {
  if (C != 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// BEQ - Branch if Equal
void Cpu::beq(stepInfo& info) {
  if (Z != 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// BIT - Bit Test
void Cpu::bit(stepInfo& info) {
  auto value = Read(info.address);
  V = (value >> 6) & 1;
  setZ(value & A);
  setN(value);
}

// BMI - Branch if Minus
void Cpu::bmi(stepInfo& info) {
  if (N != 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// BNE - Branch if Not Equal
void Cpu::bne(stepInfo& info) {
  if (Z == 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// BPL - Branch if Positive
void Cpu::bpl(stepInfo& info) {
  if (N == 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// BRK - Force Interrupt
void Cpu::brk(stepInfo& info) {
  push16(PC);
  php(info);
  sei(info);
  PC = Read16(0xFFFE);
}

// BVC - Branch if Overflow Clear
void Cpu::bvc(stepInfo& info) {
  if (V == 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// BVS - Branch if Overflow Set
void Cpu::bvs(stepInfo& info) {
  if (V != 0) {
    PC = info.address;
    addBranchCycles(info);
  }
}

// CLC - Clear Carry Flag
void Cpu::clc(stepInfo& info) {
  C = 0;
}

// CLD - Clear Decimal Mode
void Cpu::cld(stepInfo& info) {
  D = 0;
}

// CLI - Clear Interrupt Disable
void Cpu::cli(stepInfo& info) {
  I = 0;
}

// CLV - Clear Overflow Flag
void Cpu::clv(stepInfo& info) {
  V = 0;
}

// CMP - Compare
void Cpu::cmp(stepInfo& info) {
  auto value = Read(info.address);
  compare(A, value);
}

// CPX - Compare X Register
void Cpu::cpx(stepInfo& info) {
  auto value = Read(info.address);
  compare(X, value);
}

// CPY - Compare Y Register
void Cpu::cpy(stepInfo& info) {
  auto value = Read(info.address);
  compare(Y, value);
}

// DEC - Decrement Memory
void Cpu::dec(stepInfo& info) {
  auto value = Read(info.address) - 1;
  Write(info.address, value);
  setZN(value);
}

// DEX - Decrement X Register
void Cpu::dex(stepInfo& info) {
  X--;
  setZN(X);
}

// DEY - Decrement Y Register
void Cpu::dey(stepInfo& info) {
  Y--;
  setZN(Y);
}

// EOR - Exclusive OR
void Cpu::eor(stepInfo& info) {
  A = A ^ Read(info.address);
  setZN(A);
}

// INC - Increment Memory
void Cpu::inc(stepInfo& info) {
  auto value = Read(info.address) + 1;
  Write(info.address, value);
  setZN(value);
}

// INX - Increment X Register
void Cpu::inx(stepInfo& info) {
  X++;
  setZN(X);
}

// INY - Increment Y Register
void Cpu::iny(stepInfo& info) {
  Y++;
  setZN(Y);
}

// JMP - Jump
void Cpu::jmp(stepInfo& info) {
  PC = info.address;
}

// JSR - Jump to Subroutine
void Cpu::jsr(stepInfo& info) {
  push16(PC - 1);
  PC = info.address;
}

// LDA - Load Accumulator
void Cpu::lda(stepInfo& info) {
  A = Read(info.address);
  setZN(A);
}

// LDX - Load X Register
void Cpu::ldx(stepInfo& info) {
  X = Read(info.address);
  setZN(X);
}

// LDY - Load Y Register
void Cpu::ldy(stepInfo& info) {
  Y = Read(info.address);
  setZN(Y);
}

// LSR - Logical Shift Right
void Cpu::lsr(stepInfo& info) {
  if (info.mode == modeAccumulator) {
    C = A & 1;
    A >>= 1;
    setZN(A);
  }
  else {
    auto value = Read(info.address);
    C = value & 1;
    value >>= 1;
    Write(info.address, value);
    setZN(value);
  }
}

// NOP - No Operation
void Cpu::nop(stepInfo& info) {
}

// ORA - Logical Inclusive OR
void Cpu::ora(stepInfo& info) {
  A = A | Read(info.address);
  setZN(A);
}

// PHA - Push Accumulator
void Cpu::pha(stepInfo& info) {
  push(A);
}

// PHP - Push Processor Status
void Cpu::php(stepInfo& info) {
  push(Flags() | 0x10);
}

// PLA - Pull Accumulator
void Cpu::pla(stepInfo& info) {
  A = pull();
  setZN(A);
}

// PLP - Pull Processor Status
void Cpu::plp(stepInfo& info) {
  SetFlags((pull() & 0xEF) | 0x20);
}

// ROL - Rotate Left
void Cpu::rol(stepInfo& info) {
  if (info.mode == modeAccumulator) {
    auto c = C;
    C = (A >> 7) & 1;
    A = (A << 1) | c;
    setZN(A);
  }
  else {
    auto c = C;
    auto value = Read(info.address);
    C = (value >> 7) & 1;
    value = (value << 1) | c;
    Write(info.address, value);
    setZN(value);
  }
}

// ROR - Rotate Right
void Cpu::ror(stepInfo& info) {
  if (info.mode == modeAccumulator) {
    auto c = C;
    C = A & 1;
    A = (A >> 1) | (c << 7);
    setZN(A);
  }
  else {
    auto c = C;
    auto value = Read(info.address);
    C = value & 1;
    value = (value >> 1) | (c << 7);
    Write(info.address, value);
    setZN(value);
  }
}

// RTI - Return from Interrupt
void Cpu::rti(stepInfo& info) {
  SetFlags((pull() & 0xEF) | 0x20);
  PC = pull16();
}

// RTS - Return from Subroutine
void Cpu::rts(stepInfo& info) {
  PC = pull16() + 1;
}

// SBC - Subtract with Carry
void Cpu::sbc(stepInfo& info) {
  auto a = A;
  auto b = Read(info.address);
  auto c = C;
  A = a - b - (1 - c);
  setZN(A);
  if (int(a) - int(b) - int(1 - c) >= 0) {
    C = 1;
  }
  else {
    C = 0;
  }
  if (((a^b) & 0x80) != 0 && ((a^A) & 0x80) != 0) {
    V = 1;
  }
  else {
    V = 0;
  }
}

// SEC - Set Carry Flag
void Cpu::sec(stepInfo& info) {
  C = 1;
}

// SED - Set Decimal Flag
void Cpu::sed(stepInfo& info) {
  D = 1;
}

// SEI - Set Interrupt Disable
void Cpu::sei(stepInfo& info) {
  I = 1;
}

// STA - Store Accumulator
void Cpu::sta(stepInfo& info) {
  Write(info.address, A);
}

// STX - Store X Register
void Cpu::stx(stepInfo& info) {
  Write(info.address, X);
}

// STY - Store Y Register
void Cpu::sty(stepInfo& info) {
  Write(info.address, Y);
}

// TAX - Transfer Accumulator to X
void Cpu::tax(stepInfo& info) {
  X = A;
  setZN(X);
}

// TAY - Transfer Accumulator to Y
void Cpu::tay(stepInfo& info) {
  Y = A;
  setZN(Y);
}

// TSX - Transfer Stack Pointer to X
void Cpu::tsx(stepInfo& info) {
  X = SP;
  setZN(X);
}

// TXA - Transfer X to Accumulator
void Cpu::txa(stepInfo& info) {
  A = X;
  setZN(A);
}

// TXS - Transfer X to Stack Pointer
void Cpu::txs(stepInfo& info) {
  SP = X;
}

// TYA - Transfer Y to Accumulator
void Cpu::tya(stepInfo& info) {
  A = Y;
  setZN(A);
}

// illegal opcodes below

void Cpu::ahx(stepInfo& info) {
}

void Cpu::alr(stepInfo& info) {
}

void Cpu::anc(stepInfo& info) {
}

void Cpu::arr(stepInfo& info) {
}

void Cpu::axs(stepInfo& info) {
}

void Cpu::dcp(stepInfo& info) {
}

void Cpu::isc(stepInfo& info) {
}

void Cpu::kil(stepInfo& info) {
}

void Cpu::las(stepInfo& info) {
}

void Cpu::lax(stepInfo& info) {
}

void Cpu::rla(stepInfo& info) {
}

void Cpu::rra(stepInfo& info) {
}

void Cpu::sax(stepInfo& info) {
}

void Cpu::shx(stepInfo& info) {
}

void Cpu::shy(stepInfo& info) {
}

void Cpu::slo(stepInfo& info) {
}

void Cpu::sre(stepInfo& info) {
}

void Cpu::tas(stepInfo& info) {
}

void Cpu::xaa(stepInfo& info) {
}
