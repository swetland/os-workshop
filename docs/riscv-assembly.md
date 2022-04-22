# RISCV Assembly Language

The core 32bit integer instruction set for RISCV is pretty small:

| instruction | description | function |
| -- | -- | -- |
| addi rd, rs1, s12 | add immediate | rd = rs1 + s12 |
| slti rd, rs1, s12 | set less than immediate | rd = (s32)rs1 < (s32)s12 ? 1 : 0 |
| sltui rd, rs1, s12 | set less than unsigned imm | rd = (u32)rs1 < (u32)s12 ? 1 : 0 |
| andi rd, rs1, s12 | logical and immediate | rd = rs1 & s12 |
| ori rd, rs1, s12 | logical or immediate | rd = rs1 | s12 |
| xori rd, rs1, s12 | logical xor immediate | rd = rs1 ^ s12 |
| slli rd, rs1, u5 | logical left shift imm | rd = rs1 << u5 |
| srli rd, rs1, u5 | logical right shift imm | rd = (u32)rs >> u5 |
| srai rd, rs1, u5 | arithmetic right shift imm | rd = (s32)rs >> u5 |
| lui rd, u20 | load upper immediate | rd = u20 << 12 |
| auipc rd, u20 | add upper imm to pc | rd = pc + (u20 << 12) |
| add rd, rs1, rs2 | addition | rd = rs1 + rs2 |
| slt rd, rs1, rs2 | set if less than (signed) | rd = (s32)rs1 < (s32)rs2 ? 1 : 0 ) |
| sltu rd, rs1, rs2 | set if less than (unsigned) | rd = (u32)rs1 < (u32)rs2 ? 1 : 0 |
| and rd, rs1, rs2 | logical and | rd = rs1 & rs2 |
| or rd, rs1, rs2 | logical or | rd = rs1 | rs2 |
| xor rd, rs1, rs2 | logical xor | rd = rs1 ^ rs2 |
| sll rd, rs1, rs2 | logical shift left | rd = rs1 << (rs2 & 0x1F) |
| srl rd, rs1, rs2 | logical shift right | rd = rs1 >> (rs2 & 0x1F) |
| sub rd, rs1, rs2 | subtraction | rd = rs1 + rs2 |
| sra rd, rs1, rs2 | arithmetic shift right | rd = (s32)rs1 >> (rs2 & 0x1F) |
| jal rd, addr | jump and link | rd = pc + 4, pc = addr [1] | 
| jalr rd, rs1, s12 | jump and link register | rd = pc + 4, pc = (rs1 + s12) & ~1 |
| beq rs1, rs2, addr | branch if equal | if rs1 == rs2 then pc = addr [2] |
| bne rs1, rs2, addr | branch if not equal | if rs1 != rs2 then pc = addr [2] |
| blt rs1, rs2, addr | branch if less than| if rs1 < rs2 then pc = addr [2] |
| blt rs1, rs2, addr | branch if less than (unsigned) | if rs1 < rs2 then pc = addr [2] |
| bge rs1, rs2, addr | branch if >= | if rs1 >= rs2 then pc = addr [2] |
| bgeu rs1, rs2, addr | branch if >= (unsigned) | if rs1 >= rs2 then pc = addr [2] |
| lw rd, s12(rs1) | load word | rd = *((u32*) (rs1 + s12)) |
| lh rd, s12(rs1) | load halfword | rd = *((u16*) (rs1 + s12)) |
| lb rd, s12(rs1) | load byte | rd = *((u8*) (rs1 + s12)) |
| sw rs2, s12(rs1) | store word | *((u32*) (rs1 + s12)) = rs2 |
| sh rs2, s12(rs1) | store halfword | *((u16*) (rs1 + s12)) = rs2 |
| sb rs2, s12(rs1) | store byte | *((u8*) (rs1 + s12)) = rs2 |
| fence | memory ordering | |
| ecall | service call | trap to higher processor mode |
| ebreak | hardware breakpoint | return control to debugger |


The assembler and diassembler recognize a number of useful pseudo instructions which put friendly names (and/or shorter forms) on many common use cases.  For example there is a BLT (branch if less than) instruction, but not a BGT instruction, but the assembler treats BGT as an alias for BLT with the arguments swapped.

| pseudo instruction | alias for | description | function |
| ------------------ | --------- | ----------- | -------- |
| nop | addi x0, x0, 0 | nop | nop | - |
| j addr | jal x0, addr | jump | pc = addr [1] |
| jal addr | jal x1, offset | jump and link | x1 = pc + 4, pc = addr [1] |
| jr rs | jalr x0, 0(rs) | jump register | pc = rs |
| jalr rs | jalr x1, 0(rs) | jump and link register | x1 = pc + 4, pc = rs |
| ret | jalr x0, 0(x1) | return | pc = x1 |
| call addr | auipc x1, offhi ; jalr x1, offlo(x1) | far call | pc = addr |
| li rd, imm | various forms | load immediate | rd = imm |
| mv rd, rs | addi rd, rs, 0 | move | rd = rs |
| not rd, rs | xori rd, rs, -1 | logical not | rd = ~rs |
| neg rd, rs | sub rd, x0, rs | negate | rd = -rs |
| seqz rd, rs | sltiu rd, rs, 1 | set if equal zero | rd = rs == 0 ? 1 : 0 |
| snez rd, rs | sltu rd, x0, rs | set if not equal zero | rd = rs != 0 ? 1 : 0 |
| sltz rd, rs | slt rd, rs, x0 | set if < zero | rd = rs < 0 ? 1 : 0 |
| sgtz rd, rs | slt rd, x0, rs | set if > zero | rd = rs > 0 ? 1 : 0 |
| beqz rs, addr | beq rs, x0, addr | branch if equal zero | if rs == 0 then pc = addr [2] |
| bnez rs, addr | bne rs, x0, addr | branch if not zero | if rs != 0 then pc = addr [2] |
| blez rs, addr | bge x0, rs, addr | branch if <= zero | if rs <= 0 then pc = addr [2] |
| bgez rs, addr | bge rs, x0, addr | branch if >= zero | if rs >= 0 then pc = addr [2] |
| bltz rs, addr | blt rs, x0, addr | branch if < zero | if rs < 0 then pc = addr [2] |
| bgtz rs, addr | blt x0, rs, addr | branch if > zero | if rs > 0 then pc = addr [2] |
| bgt rs, rt, addr | blt rt, rs, addr | branch if greater | if rs > rt then pc = addr [2] |
| blt rs, rt, addr | bge rt, rs, addr | branch if <= | if rs <= rt then pc = addr [2] |
| bgtu rs, rt, addr | bltu rt, rs, addr | branch if > (unsigned) | if rs > rt then pc = addr [2] |
| bleu rs, rt, addr | bgeu rt, rs, addr | branch if <= (unsigned) | if rs <= rt then pc = addr [2] |

[1] addr is encoded as a signed 21bit value, allowing a range of +/- 1MB from the pc

[2] addr is encoded as a signed 12bit value, allowing a range of +/- 4KB from the pc


RISCV has 32 integer registers (x0 - x31), but the assembler and disassembler recognize and use aliases for these names which indicate what role they serve in the standard ABI.  Register x0 always reads as zero and writes to it are discarded. There is also a program counter (pc).

| register | alias | description | saver |
| -------- | ----- | ----------- | ----- |
| x0 | zero | hard-wired zero | - |
| x1 | ra | return address | caller |
| x2 | sp | stack pointer | callee |
| x3 | gp | global pointer | - |
| x4 | tp | thread pointer | - |
| x5 | t0 | temporary | caller |
| x6 - x7 | t1 - t2 | temporaries | caller |
| x8 | s0 / fp | saved register / frame pointer | callee |
| x9 | s1 | saved register | callee |
| x10 - x11 | a0 - a1 | arguments / return values | caller |
| x12 - x17 | a2 - a7 | arguments | caller |
| x18 - x27 | s2 - s11 | saved registers | callee |
| x28 - x31 | t3 - t6 | temporaries | caller |
| pc | - | program counter | - |


| symbol | meaning | range |
| ------ | ------- | ----- |
| rd | destination register (written to) | x0 .. x31  |
| rs1 | first source register | x0 .. x31 |
| rs2 | second source register | x0 .. x31 |
| s12 | 12bit signed immediate | -2048 .. 2047 |
| u5 | unsigned 5bit immediate | 0 .. 31 |

