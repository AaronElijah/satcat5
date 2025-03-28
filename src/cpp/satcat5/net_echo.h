//////////////////////////////////////////////////////////////////////////
// Copyright 2021 The Aerospace Corporation.
// This file is a part of SatCat5, licensed under CERN-OHL-W v2 or later.
//////////////////////////////////////////////////////////////////////////
//! \file
//! Generic Echo service.
//!

#pragma once

#include <satcat5/ethernet.h>
#include <satcat5/udp_core.h>

namespace satcat5 {
    namespace net {
        //! The Echo protocol can be attached to any Dispatch object.  It copies
        //! each received frame back to the original sender.  Variants are
        //! provided for raw-Ethernet and UDP networking. Generic version
        //! requires a wrapper to be used.
        class ProtoEcho : public satcat5::net::Protocol {
        protected:
            //! Only children can safely access constructor/destructor.
            //! @{
            ProtoEcho(
                satcat5::net::Dispatch* iface,
                const satcat5::net::Type& type_req,
                const satcat5::net::Type& type_ack);
            ~ProtoEcho() SATCAT5_OPTIONAL_DTOR;
            //! @}

            //! Event handler to process incoming frames and respond.
            void frame_rcvd(satcat5::io::LimitedRead& src) override;

            satcat5::net::Dispatch* const m_iface;      //!< Registered iface.
            satcat5::net::Type const m_replytype;       //!< Type for replies.
        };
    }

    namespace eth {
        //! Thin wrapper for echo of ethernet frames through eth::Dispatch.
        //! Note: Always use different request/reply EtherTypes to
        //!       avoid the potential for infinite reply loops.
        //! \copydoc satcat5::net::ProtoEcho
        class ProtoEcho : public satcat5::net::ProtoEcho {
        public:
            ProtoEcho(
                satcat5::eth::Dispatch* iface,
                satcat5::eth::MacType type_req,
                satcat5::eth::MacType type_ack);
        };
    }

    namespace udp {
        //! Thin wrapper for echo of UDP/IP frames through udp::Dispatch.
        //! \copydoc satcat5::net::ProtoEcho
        class ProtoEcho : public satcat5::net::ProtoEcho {
        public:
            ProtoEcho(
                satcat5::udp::Dispatch* iface,
                satcat5::udp::Port port = satcat5::udp::PORT_ECHO);
        };
    }
}
