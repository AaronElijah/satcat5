//////////////////////////////////////////////////////////////////////////
// Copyright 2021-2023 The Aerospace Corporation.
// This file is a part of SatCat5, licensed under CERN-OHL-W v2 or later.
//////////////////////////////////////////////////////////////////////////
//! \file
//! ConfigBus core definitions
//!
//! \details
//! ConfigBus is a multipurpose memory-mapped interface used by many
//! SatCat5 gateware peripherals.  This file defines the software API
//! for accessing that interface, either locally or remotely.
//!
//! On bare-metal embedded systems, ConfigBus is directly memory-mapped
//! to a volatile pointer in the local address space.  This is, by far,
//! the simplest and most direct way to access ConfigBus and provides
//! native support for byte-at-a-time writes (e.g., for MailMap).  This
//! simplified interface is enabled by setting SATCAT5_CFGBUS_DIRECT = 1.
//!
//! If the "simple" flag is not set, we instead define an object-oriented
//! interface that overloads the array-index and assignment operators.
//!
//! In many cases, code written with this in mind should be compatible
//! with both options, e.g.:
//!      my_register[n] = writeval;
//!      readval = my_register[n];
//! The object-oriented interface allows hooks for unit tests or even
//! for remote commanding of an Ethernet-enabled ConfigBus host.

#pragma once

#include <satcat5/interrupts.h>
#include <satcat5/list.h>

//! By default, use the general-purpose interface (see above).
//! If your platform supports it, set this to 1 for better performance.
#ifndef SATCAT5_CFGBUS_DIRECT
#define SATCAT5_CFGBUS_DIRECT   0
#endif

namespace satcat5 {
    namespace cfg {
        //! Fixed ConfigBus gateware interface parameters.
        //!@{
        const unsigned DEVS_PER_CFGBUS  = 256;
        const unsigned REGS_PER_DEVICE  = 1024;
        const unsigned MAX_DEVICES      = 256;
        const unsigned MAX_TOTAL_REGS   = REGS_PER_DEVICE * MAX_DEVICES;
        //!@}

        //! Shortcut for don't-care register address.
        const unsigned REGADDR_ANY      = 0;

        //! Generic wrapper for a specific ConfigBus register.
        //!
        //! \link cfgbus_core.h ConfigBus API concepts. \endlink
        //!
        //! Note: Most devices should use the "Register" alias defined below,
        //!   which simplifies direct access if SATCAT5_CFGBUS_DIRECT is set.
        class WrappedRegister {
        public:
            WrappedRegister(ConfigBus* cfg, unsigned reg);
            operator u32();                             // Read from register
            void operator=(u32 wrval);                  // Write to register
            void write_repeat(unsigned count, const u32* data);
        protected:
            satcat5::cfg::ConfigBus* const m_cfg;       // Parent interface
            const unsigned m_reg;                       // Device + register index
        };

        //! Pointer-like wrapper for one or more ConfigBus registers.
        //!
        //! \link cfgbus_core.h ConfigBus API concepts. \endlink
        //!
        //! Note: Most devices should use the "Register" alias defined below,
        //!   which simplifies direct access if SATCAT5_CFGBUS_DIRECT is set.
        class WrappedRegisterPtr {
        public:
            WrappedRegisterPtr(ConfigBus* cfg, unsigned reg);
            bool operator!() const;                     // Valid register?
            satcat5::cfg::WrappedRegister operator*();  // Pointer dereference
            satcat5::cfg::WrappedRegister operator[](unsigned idx);
            satcat5::cfg::WrappedRegisterPtr operator+(unsigned idx);
        protected:
            satcat5::cfg::ConfigBus* const m_cfg;       // Parent interface
            const unsigned m_reg;                       // Device + register index
        };

        // Prefer the direct or indirect interface?
        #if SATCAT5_CFGBUS_DIRECT
            typedef volatile u32* Register;
            #define SATCAT5_NULL_REGISTER   0
        #else
            typedef satcat5::cfg::WrappedRegisterPtr Register;
            #define SATCAT5_NULL_REGISTER   WrappedRegisterPtr(0,0)
        #endif

        //! Status codes for ConfigBus read/write operations.
        //! \see satcat5::cfg::ConfigBus
        enum class IoStatus {
            OK = 0,         // Operation successful
            BUSERROR,       // ConfigBus error
            CMDERROR,       // Invalid command
            TIMEOUT,        // Network timeout
        };

        // Constants for legacy compatibility (deprecated).
        constexpr auto IOSTATUS_OK =
            satcat5::cfg::IoStatus::OK;
        constexpr auto IOSTATUS_BUSERROR =
            satcat5::cfg::IoStatus::BUSERROR;
        constexpr auto IOSTATUS_CMDERROR =
            satcat5::cfg::IoStatus::CMDERROR;
        constexpr auto IOSTATUS_TIMEOUT =
            satcat5::cfg::IoStatus::TIMEOUT;

        //! Generic ConfigBus API
        //!
        //! \link cfgbus_core.h ConfigBus API concepts. \endlink
        //!
        //! This class defines virtual methods for reading or writing a
        //! specific register.  The underlying implementation may access
        //! local memory directly, or it may send network packets to
        //! query a memory-mapped register on a remote device.
        class ConfigBus {
        public:
            //! Basic single-register read operation.
            virtual satcat5::cfg::IoStatus read(unsigned regaddr, u32& rdval) = 0;
            //! Basic single-register write operation.
            virtual satcat5::cfg::IoStatus write(unsigned regaddr, u32 wrval) = 0;

            //! Bulk-read with auto-increment (regaddr, regaddr+1, ...)
            //! Bulk read/write operations are more efficient when controlling
            //! ConfigBus peripherals over a network interface.
            virtual satcat5::cfg::IoStatus read_array(
                unsigned regaddr, unsigned count, u32* result);
            //! Bulk-read the same register N times.
            virtual satcat5::cfg::IoStatus read_repeat(
                unsigned regaddr, unsigned count, u32* result);
            //! Bulk-write with auto-increment (regaddr, regaddr+1, ...)
            virtual satcat5::cfg::IoStatus write_array(
                unsigned regaddr, unsigned count, const u32* data);
            //! Bulk-write the same register N times.
            virtual satcat5::cfg::IoStatus write_repeat(
                unsigned regaddr, unsigned count, const u32* data);

            //! Convert device + register to combined address.
            inline unsigned get_regaddr(unsigned dev, unsigned reg) const
                {return satcat5::cfg::REGS_PER_DEVICE * dev + reg;}

            //! Add an interrupt handler.
            //! Each ConfigBus interface has a single interrupt signal, which
            //! is the logical OR of interrupts from all attached peripherals.
            //! When any peripheral raises its interrupt signal, call irq_poll().
            void register_irq(satcat5::cfg::Interrupt* obj);
            //! Remove an interrupt handler.
            void unregister_irq(satcat5::cfg::Interrupt* obj);

            //! Count attached interrupt handlers.
            unsigned count_irq() const;

            //! Create register-map object for the given device address.
            //! (Or for a specific register, if the register address is specified.)
            satcat5::cfg::Register get_register(unsigned dev, unsigned reg = 0);

            //! Poll all registered ConfigBus interrupt handlers.
            //! Issue callbacks to any that may have triggered.
            //! (This method should not usually be called directly.)
            void irq_poll();

        protected:
            // Constructor should only be called by children.
            // (Only children should create or destroy base class.)
            explicit ConfigBus(void* base_ptr = 0);
            ~ConfigBus() {}

            // Direct-access pointer, if applicable.
            volatile u32* m_base_ptr;

            // Linked-list of interrupt handlers.
            satcat5::util::List<satcat5::cfg::Interrupt> m_irq_list;
        };

        //! Memory-mapped local ConfigBus
        //!
        //! \link cfgbus_core.h ConfigBus API concepts. \endlink
        //!
        //! The simplest and most direct implementation of the ConfigBus API.
        //! Reads and writes map directly to addresses in physical memory.
        //! This class also serves as a hardware interrupt handler for the
        //! shared ConfigBus interrupt.  (See interrupts.h)
        //! Some peripherals, such as port::MailMap, can only function when
        //! attached through a cfg::ConfigBusMmap interface.
        class ConfigBusMmap
            : public satcat5::cfg::ConfigBus
            , public satcat5::irq::Handler
        {
        public:
            //! Constructor accepts the base pointer for the memory-map interface,
            //! and the interrupt-index for the shared ConfigBus interrupt, if any.
            ConfigBusMmap(void* base_ptr, int irq);
            ~ConfigBusMmap() {}

            // Basic read and write operations.
            satcat5::cfg::IoStatus read(unsigned regaddr, u32& val) override;
            satcat5::cfg::IoStatus write(unsigned regaddr, u32 val) override;

            //! Get a raw pointer to the designated device-address.
            void* get_device_mmap(unsigned dev) const;

            //! Get a raw pointer to the designated combined-address.
            inline volatile u32* get_register_mmap(unsigned addr) const
                {return m_base_ptr + addr;}

        protected:
            void irq_event() override;
        };
    }
}
