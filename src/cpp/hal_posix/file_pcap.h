//////////////////////////////////////////////////////////////////////////
// Copyright 2024 The Aerospace Corporation.
// This file is a part of SatCat5, licensed under CERN-OHL-W v2 or later.
//////////////////////////////////////////////////////////////////////////
//
// File I/O for packet capture files (PCAP, PCAPNG)
//
// This file defines classes for reading and writing common packet-capture
// files as SatCat5 packet streams (i.e., io::Readable and io::Writeable).
//
// Supported file formats include:
//  * PCAP v2.4
//    https://datatracker.ietf.org/doc/id/draft-gharris-opsawg-pcap-00.html
//  * PCAPNG
//    https://www.ietf.org/archive/id/draft-tuexen-opsawg-pcapng-02.txt
//
// This implementation supports Ethernet packets only, using minimalist
// metadata where required. When writing files, packet timestamps are
// drawn from a provided datetime::Clock object.
//

#pragma once

#include <hal_posix/file_io.h>
#include <satcat5/datetime.h>

// Buffer size must be large enough for one full-size Ethernet frame.
#ifndef SATCAT5_PCAP_BUFFSIZE
#define SATCAT5_PCAP_BUFFSIZE   1600
#endif

namespace satcat5 {
    namespace io {
        // Define selected LinkType codes from the official registry:
        //  https://www.tcpdump.org/linktypes.html
        constexpr u16 LINKTYPE_ETHERNET     = 1;    // Ethernet (Default)
        constexpr u16 LINKTYPE_USER0        = 147;  // User-defined type #0
        constexpr u16 LINKTYPE_AOS          = 222;  // CCSDS-AOS Space Data Link
        constexpr u16 LINKTYPE_USER(u16 x)          // User-defined type #0-15
            { return LINKTYPE_USER0 + x; }          // Reserved for private use

        // Read packet stream from a file.
        class ReadPcap : public satcat5::io::ArrayRead {
        public:
            // Open the input file and autodetect format.
            explicit ReadPcap(const char* filename = 0);

            // Open the specified file.
            void open(const char* filename);
            inline void open(const std::string& filename)
                { open(filename.c_str()); }
            inline void close() {m_file.close();}

            // Override end-of-packet handling.
            void read_finalize() override;

        protected:
            // Read specific block formats.
            void pcap_hdr();    // Start-of-file header
            bool pcap_dat();    // Packet record
            bool pcapng_blk();  // Read next block (any type)
            void pcapng_shb();  // Section header block
            void pcapng_idb();  // Interface description block
            void pcapng_spb();  // Simple packet block
            void pcapng_epb();  // Enhanced packet block
            void pcapng_skip(); // Any unsupported block

            // Shortcuts for reading from the file.
            inline u16 file_rd16()
                { return m_mode_be ? m_file.read_u16() : m_file.read_u16l(); }
            inline u32 file_rd32()
                { return m_mode_be ? m_file.read_u32() : m_file.read_u32l(); }

            // Internal state and working buffer.
            satcat5::io::FileReader m_file;
            bool m_mode_be;         // Big-endian file?
            bool m_mode_ng;         // PCAPNG format?
            bool m_mode_pc;         // PCAPNG format?
            u32 m_trim;             // Ignore FCS bytes?
            u8 m_buff[SATCAT5_PCAP_BUFFSIZE];
        };

        // Store packet stream to a file.
        class WritePcap : public satcat5::io::ArrayWrite {
        public:
            // Create the capture object and set PCAP or PCAPNG mode.
            // Timestamps are drawn from the system time by default,
            // or from a user-provided timer for simulations.
            explicit WritePcap(bool pcapng = true);

            // Open the specified file, and optionally specify LinkType.
            void open(const char* filename, u16 type = LINKTYPE_ETHERNET);
            inline void open(const std::string& filename, u16 type = LINKTYPE_ETHERNET)
                { open(filename.c_str(), type); }
            inline void close()
                { m_file.close(); }

            // Passthrough mode carbon-copies each packet to another
            // Writeable object, using the internal working buffer.
            inline void set_passthrough(satcat5::io::Writeable* wr)
                { m_pass = wr; }

            // Override end-of-packet handling.
            bool write_finalize() override;

        protected:
            // Internal state and working buffer.
            satcat5::datetime::Clock m_clock;
            satcat5::io::FileWriter m_file;
            satcat5::io::Writeable* m_pass;
            const bool m_mode_ng;   // PCAPNG format?
            u8 m_buff[SATCAT5_PCAP_BUFFSIZE];
        };
    }
}
