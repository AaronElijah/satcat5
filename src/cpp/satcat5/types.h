//////////////////////////////////////////////////////////////////////////
// Copyright 2021-2024 The Aerospace Corporation.
// This file is a part of SatCat5, licensed under CERN-OHL-W v2 or later.
//////////////////////////////////////////////////////////////////////////
// Basic type aliases and prototypes used throughout SatCat5

#pragma once

#include <cinttypes>

// Allow safe destruction of SatCat5 objects?
// Many embedded systems never need to delete anything, and disabling this
// feature can save several kilobytes of code-size.  Use with caution.
// For GCC/G++: compiler flag "-DSATCAT5_ALLOW_DELETION=0" to disable.
#ifndef SATCAT5_ALLOW_DELETION
#define SATCAT5_ALLOW_DELETION  1
#endif

#if SATCAT5_ALLOW_DELETION
#define SATCAT5_OPTIONAL_DTOR       // Full function defined elsewhere
#else
#define SATCAT5_OPTIONAL_DTOR {}    // Null inline placeholder
#endif

// Enable use of the QCBOR library?
// For GCC/G++: compiler flag "-DSATCAT5_QCBOR_ENABLE=1" to enable.
#ifndef SATCAT5_QCBOR_ENABLE
#define SATCAT5_QCBOR_ENABLE 0
#endif

// Default method for accessing the system clock.
// (See also: TimeRef class defined in "satcat5/timeref.h")
#ifndef SATCAT5_CLOCK
#define SATCAT5_CLOCK satcat5::poll::timekeeper.get_clock()
#endif

// Shortcuts for fixed-size integer types.
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

// Prototypes for widely-used interfaces and data-structures.
// (Comment indicates the file containing the full definition.)
namespace satcat5 {
    namespace aes {                 // Encryption using AES
        class Cipher;               // satcat5/aes_cipher.h
        class Gcm;                  // satcat5/aes_gcm.h
    }

    namespace ccsds_aos {           // CCSDS AOS Space Data Link Protocol
        struct Header;              // satcat5/ccsds_aos.h
        class Channel;              // satcat5/ccsds_aos.h
        class Dispatch;             // satcat5/ccsds_aos.h
    }

    namespace ccsds_spp {           // CCSDS Space Packet Protocol
        struct Header;              // satcat5/ccsds_spp.h
        class Address;              // satcat5/ccsds_spp.h
        class Dispatch;             // satcat5/ccsds_spp.h
        class Packetizer;           // satcat5/ccsds_spp.h
        class Protocol;             // satcat5/ccsds_spp.h
    }

    namespace cfg {                 // ConfigBus and peripherals
        struct TrafficStats;        // satcat5/cfgbus_stats.h
        class ConfigBus;            // satcat5/cfgbus_core.h
        class ConfigBusMmap;        // satcat5/cfgbus_core.h
        class ConfigBusRemote;      // satcat5/cfgbus_remote.h
        class GpiRegister;          // satcat5/cfgbus_gpio.h
        class GpoRegister;          // satcat5/cfgbus_gpio.h
        class I2c;                  // satcat5/cfgbus_i2c.h
        class I2cEventListener;     // satcat5/cfgbus_i2c.h
        class Interrupt;            // satcat5/cfgbus_interrupt.h
        class LedArray;             // satcat5/cfgbus_led.h
        class Mdio;                 // satcat5/cfgbus_mdio.h
        class MdioEventListener;    // satcat5/cfgbus_mdio.h
        class MdioLogger;           // satcat5/cfgbus_mdio.h
        class MdioWrapper;          // satcat5/cfgbus_mdio.h
        class MultiSerial;          // satcat5/cfgbus_multiserial.h
        class NetworkStats;         // satcat5/cfgbus_stats.h
        class PpsInput;             // satcat5/cfgbus_pps.h
        class PpsOutput;            // satcat5/cfgbus_pps.h
        class PtpRealtime;          // satcat5/cfgbus_ptpref.h
        class PtpReference;         // satcat5/cfgbus_ptpref.h
        class Spi;                  // satcat5/cfgbus_spi.h
        class SpiEventListener;     // satcat5/cfgbus_spi.h
        class Timer;                // satcat5/cfgbus_timer.h
        class Uart;                 // satcat5/cfgbus_uart.h
        class WrappedRegister;      // satcat5/cfgbus_core.h
        class WrappedRegisterPtr;   // satcat5/cfgbus_core.h
    }

    namespace coap {
        struct Code;                // satcat5/coap_constants.h
        class Connection;           // satcat5/coap_connection.h
        class Endpoint;             // satcat5/coap_endpoint.h
        class EndpointSpp;          // satcat5/coap_endpoint.h
        class EndpointSppFwd;       // satcat5/coap_endpoint.h
        class EndpointUdp;          // satcat5/coap_endpoint.h
        class EndpointUdpSimple;    // satcat5/coap_endpoint.h
        class Option;               // satcat5/coap_reader.h
        class Reader;               // satcat5/coap_reader.h
        class Writer;               // satcat5/coap_writer.h
    }

    namespace crc16 {               // 16-bit CRC calculation
        class KermitRx;             // satcat5/crc16_checksum.h
        class KermitTx;             // satcat5/crc16_checksum.h
        class XmodemRx;             // satcat5/crc16_checksum.h
        class XmodemTx;             // satcat5/crc16_checksum.h
    }

    namespace datetime {            // Human-readable date and time
        struct GpsTime;             // satcat5/datetime.h
        struct RtcTime;             // satcat5/datetime.h
        class Clock;                // satcat5/datetime.h
    }

    namespace eth {                 // Ethernet networking
        struct Header;              // satcat5/eth_header.h
        struct MacAddr;             // satcat5/eth_header.h
        struct MacType;             // satcat5/eth_header.h
        struct VlanRate;            // satcat5/switch_cfg.h
        struct VlanTag;             // satcat5/eth_header.h
        struct VtagPolicy;          // satcat5/switch_cfg.h
        struct VlanTag;             // satcat5/eth_header.h
        class Address;              // satcat5/eth_address.h
        class AeroFtpClient;        // satcat5/net_aeroftp.h
        class ArpListener;          // satcat5/eth_arp.h
        class ChecksumRx;           // satcat5/eth_checksum.h
        class ChecksumTx;           // satcat5/eth_checksum.h
        class ConfigBus;            // satcat5/cfgbus_remote.h
        class Dispatch;             // satcat5/eth_dispatch.h
        class MacSec;               // satcat5/eth_macsec.h
        class Protocol;             // satcat5/eth_protocol.h
        class ProtoArp;             // satcat5/eth_arp.h
        class ProtoConfig;          // satcat5/net_cfgbus.h
        class ProtoEcho;            // satcat5/net_echo.h
        class SlipCodec;            // satcat5/eth_checksum.h
        class SlipCodecInverse;     // satcat5/eth_checksum.h
        class Socket;               // satcat5/eth_socket.h
        class SocketCore;           // satcat5/eth_socket.h
        template <unsigned SIZE>
            class SwitchCache;      // satcat5/eth_sw_cache.h
        class SwitchConfig;         // satcat5/switch_cfg.h
        class SwitchCore;           // satcat5/eth_switch.h
        class SwitchPlugin;         // satcat5/eth_switch.h
        class SwitchPort;           // satcat5/eth_switch.h
        template <unsigned VMAX>
            class SwitchVlan;       // satcat5/eth_sw_vlan.h
        class Telemetry;            // satcat5/net_telemetry.h
    }

    namespace gui {                 // Graphical user interfaces
        struct Cursor;              // satcat5/gui_display.h
        struct Icon8x8;             // satcat5/gui_icons.h
        struct Icon16x16;           // satcat5/gui_icons.h
        struct LogColors;           // satcat5/gui_display.h
        template <class T>
            class Font;             // satcat5/gui_icons.h
        class Canvas;               // satcat5/gui_display.h
        class Display;              // satcat5/gui_display.h
        class DrawCmd;              // satcat5/gui_display.h
        class LogToDisplay;         // satcat5/gui_display.h
    }

    namespace io {                  // Input and output streams
        class ArrayRead;            // satcat5/io_readable.h
        class ArrayWrite;           // satcat5/io_writeable.h
        class BufferedCopy;         // satcat5/io_buffer.h
        class BufferedIO;           // satcat5/io_buffer.h
        class BufferedWriter;       // satcat5/io_buffer.h
        class EventListener;        // satcat5/io_readable.h
        class HdlcDecoder;          // satcat5/codec_hdlc.h
        class HdlcEncoder;          // satcat5/codec_hdlc.h
        class LimitedRead;          // satcat5/io_readable.h
        class LimitedWrite;         // satcat5/io_writeable.h
        class MultiBuffer;          // satcat5/multi_buffer.h
        class MultiReader;          // satcat5/multi_buffer.h
        class MultiReaderPriority;  // satcat5/multi_buffer.h
        class MultiReaderSimple;    // satcat5/multi_buffer.h
        class MultiWriter;          // satcat5/multi_buffer.h
        class MuxDown;              // satcat5/io_multiplexer.h
        class MuxUp;                // satcat5/io_multiplexer.h
        class NullRead;             // satcat5/io_readable.h
        class NullSink;             // satcat5/io_readable.h
        class NullWrite;            // satcat5/io_writeable.h
        class PacketBuffer;         // satcat5/pkt_buffer.h
        class Readable;             // satcat5/io_readable.h
        class ReadableRedirect;     // satcat5/io_readable.h
        class SlipCodec;            // satcat5/codec_slip.h
        class SlipCodecInverse;     // satcat5/codec_slip.h
        class SlipDecoder;          // satcat5/codec_slip.h
        class SlipEncoder;          // satcat5/codec_slip.h
        class Writeable;            // satcat5/io_writeable.h
        class WriteableRedirect;    // satcat5/io_writeable.h
    }

    namespace ip {                  // Internet Protocol v4
        struct Addr;                // satcat5/ip_core.h
        struct Header;              // satcat5/ip_core.h
        struct Mask;                // satcat5/ip_core.h
        struct Port;                // satcat5/ip_core.h
        struct Route;               // satcat5/ip_table.h
        struct Subnet;              // satcat5/ip_core.h
        class Address;              // satcat5/ip_address.h
        class DhcpClient;           // satcat5/ip_dhcp.h
        class DhcpServer;           // satcat5/ip_dhcp.h
        class Dispatch;             // satcat5/ip_dispatch.h
        class Ping;                 // satcat5/ip_ping.h
        class ProtoIcmp;            // satcat5/ip_icmp.h
        class Stack;                // satcat5/ip_stack.h
        class Table;                // satcat5/ip_table.h
    }

    namespace irq {                 // Interrupt handling
        class AtomicLock;           // satcat5/interrupts.h
        class Controller;           // satcat5/interrupts.h
        class Handler;              // satcat5/interrupts.h
        class Shared;               // satcat5/interrupts.h
        class VirtualTimer;         // satcat5/polling.h
    }

    namespace log {                 // Logging
        class EventHandler;         // satcat5/log.h
        class Log;                  // satcat5/log.h
        class LogBuffer;            // satcat5/log.h
        class ToWriteable;          // satcat5/log.h
    }

    namespace net {                 // Generic networking
        struct Type;                // satcat5/net_type.h
        class Address;              // satcat5/net_address.h
        class AeroFtpClient;        // satcat5/net_aeroftp.h
        class Dispatch;             // satcat5/net_dispatch.h
        class Protocol;             // satcat5/net_protocol.h
        class ProtoConfig;          // satcat5/net_cfgbus.h
        class SocketCore;           // satcat5/net_socket.h
        class TelemetryAggregator;  // satcat5/net_telemetry.h
        class TelemetrySource;      // satcat5/net_telemetry.h
        class TelemetryTier;        // satcat5/net_telemetry.h
    }

    namespace ntp {                 // Network time protocol (NTP)
        class Client;               // satcat5/ntp_client.h
        struct Header;              // satcat5/ntp_header.h
    }

    namespace poll {                // Queued-task servicing
        class Always;               // satcat5/polling.h
        class OnDemand;             // satcat5/polling.h
        class Timer;                // satcat5/polling.h
        class TimerAdapter;         // satcat5/polling.h
    }

    namespace port {                // Network ports
        class MailAdapter;          // satcat5/port_adapter.h
        class Mailbox;              // satcat5/port_mailbox.h
        class Mailmap;              // satcat5/port_mailmap.h
        class SerialGeneric;        // satcat5/port_serial.h
        class SerialAuto;           // satcat5/port_serial.h
        class SerialI2cController;  // satcat5/port_serial.h
        class SerialI2cPeripheral;  // satcat5/port_serial.h
        class SerialSpiController;  // satcat5/port_serial.h
        class SerialSpiPeripheral;  // satcat5/port_serial.h
        class SerialUart;           // satcat5/port_serial.h
        class SlipAdapter;          // satcat5/port_adapter.h
        class VlanAdapter;          // satcat5/port_adapter.h
    }

    namespace ptp {                 // Precision Time Protocol (PTP)
        class Callback;             // satcat5/ptp_source.h
        class Client;               // satcat5/ptp_client.h
        struct ClockInfo;           // satcat5/ptp_header.h
        struct CoeffLR;             // satcat5/ptp_filters.h
        struct CoeffPI;             // satcat5/ptp_filters.h
        struct CoeffPII;            // satcat5/ptp_filters.h
        template <unsigned>
            class ControllerLR;     // satcat5/ptp_filters.h
        class ControllerLR_Inner;   // satcat5/ptp_filters.h
        class ControllerPI;         // satcat5/ptp_filters.h
        class ControllerPII;        // satcat5/ptp_filters.h
        class Dispatch;             // satcat5/ptp_dispatch.h
        class DopplerTlv;           // satcat5/ptp_doppler.h
        class Filter;               // satcat5/ptp_filters.h
        struct Header;              // satcat5/ptp_header.h
        class Interface;            // satcat5/ptp_interface.h
        struct Measurement;         // satcat5/ptp_measurement.h
        class MeasurementCache;     // satcat5/ptp_measurement.h
        class Source;               // satcat5/ptp_source.h
        class SyncUnicastL2;        // satcat5/ptp_client.h
        class SyncUnicastL3;        // satcat5/ptp_client.h
        class Time;                 // satcat5/ptp_time.h
        class TlvHandler;           // satcat5/ptp_tlv.h
        struct TlvHeader;           // satcat5/ptp_tlv.h
        struct TlvType;             // satcat5/ptp_tlv.h
        class TrackingClock;        // satcat5/ptp_tracking.h
        class TrackingController;   // satcat5/ptp_tracking.h
    }

    namespace router2 {             // Software-defined IPv4 router
        class DeferFwd;             // satcat5/router2_deferfwd.h
        struct DeferPkt;            // satcat5/router2_deferfwd.h
        class Dispatch;             // satcat5/router2_dispatch.h
        class Offload;              // satcat5/router2_offload.h
        class StackCommon;          // satcat5/router2_stack.h
        template <unsigned>
            class StackGateware;    // satcat5/router2_stack.h
        template <unsigned>
            class StackSoftware;    // satcat5/router2_stack.h
        class Table;                // satcat5/router2_table.h
    }

    namespace test {                // Unit test framework
        class TftpClient;           // sim/cpp/test_udp_tftp.cc
        class TftpServer;           // sim/cpp/test_udp_tftp.cc
    }

    namespace udp {                 // UDP networking
        class Address;              // satcat5/udp_core.h
        class AeroFtpClient;        // satcat5/net_aeroftp.h
        class ConfigBus;            // satcat5/cfgbus_remote.h
        class Dispatch;             // satcat5/udp_dispatch.h
        class ProtoConfig;          // satcat5/net_cfgbus.h
        class ProtoEcho;            // satcat5/net_echo.h
        class Socket;               // satcat5/udp_socket.h
        class SocketCore;           // satcat5/udp_socket.h
        class Telemetry;            // satcat5/net_telemetry.h
        class TftpClient;           // satcat5/udp_tftp.h
        class TftpServerCore;       // satcat5/udp_tftp.h
        class TftpServerSimple;     // satcat5/udp_tftp.h
    }

    namespace util {                // Other utilities
        struct I2cAddr;             // satcat5/cfg_i2c.h
        struct TimeVal;             // satcat5/timeref.h
        class ListCore;             // satcat5/list.h
        template <class T>
            class List;             // satcat5/list.h
        template <class T>
            class LruCache;         // satcat5/lru_cache.h
        class Prng;                 // satcat5/utils.h
        class RunningMax;           // satcat5/utils.h
        class TimeRef;              // satcat5/timeref.h
        class TimeRegister;         // satcat5/timeref.h
        template <unsigned W>
            struct WideSigned;      // satcat5/wide_integer.h
        template <unsigned W>
            struct WideUnsigned;    // satcat5/wide_integer.h
    }
}
