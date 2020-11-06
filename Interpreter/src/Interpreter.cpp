#include "Interpreter.h"
#include "time.h"

Interpreter::Interpreter()
    : m_stack_ptr(0), m_i(0), m_pc(512), m_dt(0), st(0), m_opcode(0), should_draw(false)
{
    m_memory.fill(0);
    m_stack.fill(0);
    m_v_reg.fill(0);
    buffer.fill(0);
    key_buffer.fill(0);
    srand(time(NULL));

    unsigned char m_font[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    for (size_t i = 0; i < 80; i++)
        m_memory[i] = m_font[i];
}

Interpreter::~Interpreter()
{

}

void Interpreter::cycle()
{
    if (m_pc > 4095)
        return;

    m_opcode = m_memory[m_pc] << 8 | m_memory[m_pc + 1];

    unsigned short nnn = m_opcode & NNN_MASK;
    unsigned char x = (m_opcode & X_MASK) >> 8;
    unsigned char kk = m_opcode & KK_MASK;
    unsigned char y = (m_opcode & Y_MASK) >> 4;
    unsigned char n = m_opcode & N_MASK;

    if (m_opcode == 0x00EE)
    {
        m_stack_ptr--;
        m_pc = m_stack[m_stack_ptr];
        m_pc += 2;
    }
    else if (m_opcode == 0x00E0)
    {
        buffer.fill(0);
        should_draw = true;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0x1000)
    {
        m_pc = nnn;
    }
    else if ((m_opcode & 0xF000) == 0x2000)
    {
        m_stack[m_stack_ptr] = m_pc;
        m_stack_ptr++;
        m_pc = nnn;
    }
    else if ((m_opcode & 0xF000) == 0x3000)
    {
        if (m_v_reg[x] == (kk))
            m_pc += 2;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0x4000)
    {
        if (m_v_reg[x] != (kk))
            m_pc += 2;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0x5000)
    {
        if (m_v_reg[x] == m_v_reg[y])
            m_pc += 2;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0x6000)
    {
        m_v_reg[x] = kk;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0x7000)
    {
        m_v_reg[x] += kk;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8000)
    {
        m_v_reg[x] = m_v_reg[y];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8001)
    {
        m_v_reg[x] |= m_v_reg[y];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8002)
    {
        m_v_reg[x] &= m_v_reg[y];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8003)
    {
        m_v_reg[x] ^= m_v_reg[y];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8004)
    {
        m_v_reg[15] = (m_v_reg[x] + m_v_reg[y]) > 255;
        m_v_reg[x] += m_v_reg[y];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8005)
    {
        m_v_reg[15] = m_v_reg[x] > m_v_reg[y];
        m_v_reg[x] -= m_v_reg[y];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8006)
    {
        m_v_reg[15] = m_v_reg[x] & 1;
        m_v_reg[x] >>= 1;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x8007)
    {
        m_v_reg[15] = m_v_reg[x] < m_v_reg[y];
        m_v_reg[x] = m_v_reg[y] - m_v_reg[x];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x800E)
    {
        m_v_reg[15] = m_v_reg[x] >> 7;
        m_v_reg[x] = m_v_reg[x] << 1;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF00F) == 0x9000)
    {
        if (m_v_reg[x] != m_v_reg[y])
            m_pc += 2;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0xA000)
    {

        m_i = nnn;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0xB000)
    {
        m_pc = nnn + m_v_reg[0];
    }
    else if ((m_opcode & 0xF000) == 0xC000)
    {
        m_v_reg[x] = (rand() % 256) & kk;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF000) == 0xD000)
    {
        char temp_x;
        char temp_y;

        char new_pixel;

        for (int y_c = 0; y_c < n; y_c++)
        {
            for (int x_c = 0; x_c < 8; x_c++)
            {
                new_pixel = m_memory[m_i + y_c] & (128 >> x_c);

                temp_x = (m_v_reg[(m_opcode & X_MASK) >> 8] + x_c) % 64;
                temp_y = (m_v_reg[(m_opcode & Y_MASK) >> 4] + y_c) % 32;

                if (buffer[(temp_y * 64) + temp_x] && new_pixel)
                    m_v_reg[15] = 1;

                buffer[(temp_y * 64) + temp_x] = buffer[(temp_y * 64) + temp_x] ^ new_pixel;
            }
        }

        should_draw = true;

        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xE09E)
    {
        if (key_buffer[m_v_reg[x]])
            m_pc += 2;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xE0A1)
    {
        if (!key_buffer[m_v_reg[x]])
            m_pc += 2;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF007)
    {
        m_v_reg[x] = m_dt;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF00A)
    {
        for (int i = 0; i < 16; i++)
        {
            if (key_buffer[i])
            {
                m_v_reg[x] = i;
                m_pc += 2;
                return;
            }
        }
    }
    else if ((m_opcode & 0xF0FF) == 0xF015)
    {
        m_dt = m_v_reg[x];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF018)
    {
        st = m_v_reg[x];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF01E)
    {
        m_i += m_v_reg[x];
        m_v_reg[15] = m_i > 0x0FFF;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF029)
    {
        m_i = m_v_reg[x] * 5;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF033)
    {
        m_memory[m_i] = m_v_reg[x] / 100;
        m_memory[m_i + 1] = (m_v_reg[x] / 10) % 10;
        m_memory[m_i + 2] = m_v_reg[x] % 10;
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF055)
    {
        for (char i = 0; i <= x; i++)
            m_memory[m_i + i] = m_v_reg[i];
        m_pc += 2;
    }
    else if ((m_opcode & 0xF0FF) == 0xF065)
    {
        for (char i = 0; i <= x; i++)
            m_v_reg[i] = m_memory[m_i + i];
        m_pc += 2;
    }

    if (m_dt > 0) m_dt--;

    if (st > 0) st--;
}

void Interpreter::load_rom(char* src, int length)
{
    for (int i = 0; i < length; i++)
        m_memory[512 + i] = src[i];
}