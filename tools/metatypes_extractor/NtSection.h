#pragma once
#include "BufferSpan.h"

bool extract_nt_section(BufferSpan image, BufferSpan nt_image, BufferSpan search_section_name, auto&& callback) {
    static constexpr auto nt_size = 20u;
    const auto num_sections = nt_image.seek(2).le_cast<uint16_t>();
    const auto optional_header_size = nt_image.seek(nt_size - 4).le_cast<uint16_t>();
    static constexpr auto section_header_size = 40u;
    const auto section_headers =
        nt_image.seek(nt_size + optional_header_size).truncate_to(num_sections * section_header_size);
    if (section_headers.count != num_sections * section_header_size) return false; // overflow
    for (auto i = 0u; i < num_sections; i++) {
        const auto section_header = section_headers.seek(i * section_header_size);
        const auto section_header_name = section_header.truncate_to(8u);
        if (!section_header_name.is_content_equal(search_section_name)) continue;
        const auto section_size = [&]() {
            const auto virtual_size = section_header.seek(8).le_cast<uint32_t>();
            if (0 != virtual_size) return virtual_size; // .obj files have not virtual size
            const auto size_of_raw_data = section_header.seek(16).le_cast<uint32_t>();
            return size_of_raw_data;
        }();
        const auto section_addr = section_header.seek(20).le_cast<uint32_t>();
        const auto section = image.seek(section_addr).truncate_to(section_size);
        if (section.count != section_size) return false; // overflow
        if (section.data < section_headers.end()) return false; // overlap with headers
        callback(section);
    }
    return true;
}

bool extract_pe_section(BufferSpan image, BufferSpan search_section_name, auto&& callback) {
    static constexpr auto dos_size = 64u;
    const auto dos_magic = image.le_cast<uint16_t>();
    if (dos_magic != 0x5A4D) return false; // MZ
    const auto nt_offset = image.seek(dos_size - 4).le_cast<uint32_t>();
    const auto nt_magic = image.seek(nt_offset).le_cast<uint32_t>();
    if (nt_magic != 0x4550) return false; // PE
    return extract_nt_section(image, image.seek(nt_offset + 4), search_section_name, callback);
}

bool extract_coff_section(BufferSpan image, BufferSpan search_section_name, auto&& callback) {
    return extract_nt_section(image, image, search_section_name, callback);
}
