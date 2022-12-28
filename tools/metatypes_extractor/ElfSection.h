#pragma once
#include "BufferSpan.h"

#include <cstdint>

bool extract_elf_section(BufferSpan image, BufferSpan search_section_name, auto&& callback) {
    const auto elf_magic = image.le_cast<uint32_t>();
    if (elf_magic != 0x464c457F) return false; // ELF
    const auto bit_class = image.seek(4u).le_cast<uint8_t>();
    if (bit_class != 1 && bit_class != 2) return false; // 32 or 64 bits
    const auto endianess = image.seek(5u).le_cast<uint8_t>();
    if (endianess != 1 && endianess != 2) return false; // little or big endian
    const auto version = image.seek(6u).le_cast<uint8_t>();
    if (version != 1) return false;
    if (bit_class == 1) { // 32 bits
        // TODO
        return true;
    }
    else { // 64 bits
        const auto sections_offset = image.seek(0x28).le_cast<uint64_t>();
        const auto section_header_size = image.seek(0x3A).le_cast<uint16_t>();
        const auto num_sections = image.seek(0x3C).le_cast<uint16_t>();
        const auto name_section_index = image.seek(0x3E).le_cast<uint16_t>();
        if (name_section_index >= num_sections) return false;
        const auto name_section_header = image.seek(sections_offset + section_header_size * name_section_index).truncate_to(section_header_size);

        const auto names_offset = name_section_header.seek(0x18).le_cast<uint64_t>();
        const auto names_size = name_section_header.seek(0x20).le_cast<uint64_t>();
        const auto names = image.seek(names_offset).truncate_to(names_size);

        for (auto i = 0u; i < num_sections; i++) {
            if (i == name_section_index) continue;
            const auto section_header = image.seek(sections_offset + section_header_size * i).truncate_to(section_header_size);

            const auto name_offset = section_header.le_cast<uint32_t>();
            const auto name = names.seek(name_offset).first_string().truncate_tail(search_section_name.count);
            if (!name.is_content_equal(search_section_name)) continue;
            const auto section_offset = section_header.seek(0x18).le_cast<uint64_t>();
            const auto section_size = section_header.seek(0x20).le_cast<uint64_t>();
            const auto section = image.seek(section_offset).truncate_to(section_size);
            if (section.count != section_size) return false; // overflow
            callback(section);
        }
        return true;
    }
}