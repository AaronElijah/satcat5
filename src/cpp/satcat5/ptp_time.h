//////////////////////////////////////////////////////////////////////////
// Copyright 2022-2024 The Aerospace Corporation.
// This file is a part of SatCat5, licensed under CERN-OHL-W v2 or later.
//////////////////////////////////////////////////////////////////////////
//!\file
//! High-precision "Time" object for use with PTP / IEEE1588
//!
//!\details
//! This file defines the `satcat5::ptp::Time` object, which is a high-precision
//! timestamp intended for use with the IEEE1588 Precision Time Protocol (PTP).
//!
//! For simpler but less precise timestamps, \see datetime.h.

#pragma once

#include <satcat5/types.h>
#include <satcat5/utils.h>

namespace satcat5 {
    namespace ptp {
        //! Define commonly used scaling factors.
        //!@{
        constexpr s64 NSEC_PER_SEC      = 1000000000LL;
        constexpr s64 NSEC_PER_MSEC     = 1000000LL;
        constexpr s64 NSEC_PER_USEC     = 1000LL;
        constexpr s64 USEC_PER_SEC      = 1000000LL;
        constexpr s64 MSEC_PER_SEC      = 1000LL;
        constexpr s64 SUBNS_PER_NSEC    = 65536LL;
        constexpr s64 SUBNS_PER_USEC    = SUBNS_PER_NSEC * NSEC_PER_USEC;
        constexpr s64 SUBNS_PER_MSEC    = SUBNS_PER_NSEC * NSEC_PER_MSEC;
        constexpr s64 SUBNS_PER_SEC     = SUBNS_PER_NSEC * NSEC_PER_SEC;
        //!@}

        //! High-precision timestamp for use with PTP / IEEE1588.
        //! The "Time" object can be used to represent a time-difference or
        //! an absolute time in the TAI epoch, matching the finest resolution
        //! supported by IEEE1588-2019.
        //!
        //! The internal representation is based on increments of 1/65536
        //! nanoseconds, which is referred to as a "subnanosecond" or "subns".
        class Time {
        public:
            //! Default constructor.
            constexpr Time()
                : m_secs(0), m_subns(0) {}

            //! Single argument constructor is scaled in subnanoseconds.
            //! This matches the format used for the PTP "correction" field.
            constexpr explicit Time(s64 subnanoseconds)
                : m_secs (satcat5::util::divide(subnanoseconds, SUBNS_PER_SEC))
                , m_subns(satcat5::util::modulo(subnanoseconds, SUBNS_PER_SEC)) {}

            //! Copy constructor.
            constexpr Time(const Time& other)
                : m_secs(other.m_secs), m_subns(other.m_subns) {}

            //! Multi-argument constructor accepts seconds, nanoseconds, and subnanosecods.
            //! This matches the format used for the PTP "timestamp" field.
            Time(u64 seconds, u32 nanoseconds, u16 subnanoseconds = 0);

            //! Read the "seconds" field without intermediate rounding.
            inline s64 field_secs() const
                {return m_secs;}
            //! Read the "nanoseconds" field, rounding down.
            //! Use this method in combination with \see correction.
            inline u32 field_nsec() const
                {return (u32)satcat5::util::div_floor(m_subns, SUBNS_PER_NSEC);}
            //! Read the raw "subnanoseconds" field.
            //! This is equal to 65536 * field_nsec() + correction()
            inline u64 field_subns() const
                {return (u64)m_subns;}

            //! Return "seconds" field after rounding to the nearest nanosecond.
            //! If you use `round_nsec`, then you should also use `round_sec`.
            inline s64 round_secs() const
                {return (*this + Time(SUBNS_PER_NSEC / 2)).field_secs();}
            //! Return "nanoseconds" field after rounding to the nearest nanosecond.
            inline u32 round_nsec() const
                {return (*this + Time(SUBNS_PER_NSEC / 2)).field_nsec();}

            //! Convert time-differences to the designated unit.
            //! The representable range is always at least +/- 24 hours.
            //! Times beyond the safe range will return INT64_MIN or INT64_MAX.
            //!@{
            s64 delta_subns() const;
            s64 delta_nsec() const;
            s64 delta_usec() const;
            s64 delta_msec() const;
            //!@}

            //! Read the standard 10-byte timestamp from a PTP message
            //! (e.g., originTimestamp: u48 seconds + u32 nanoseconds)
            //! Note: Add correctionField separately to maintain full precision.
            bool read_from(satcat5::io::Readable* src);

            //! Read or write the 10-byte timestamp from a PTP message.
            //! (e.g., originTimestamp: u48 seconds + u32 nanoseconds)
            //! Note: User must write `correction` to the PTP correctionField
            //!  to maintain full end-to-end precision.
            void write_to(satcat5::io::Writeable* dst) const;

            //! User-readable format for logging. \see satcat5::log::Log
            void log_to(satcat5::log::LogBuffer& wr) const;

            //! Get the correctionField value in subnanoseconds.
            //! For use with `field_sec`, `field_nsec`, or `write_to` methods.
            //! \returns Residual offset in subnanoseconds.
            inline u64 correction() const
                {return (u64)satcat5::util::modulo(m_subns, SUBNS_PER_NSEC);}

            //! Convert to SatCat5 date/time. \see datetime.h
            s64 to_datetime() const;

            //! Standard arithmetic operations.
            //!@{
            satcat5::ptp::Time abs() const;
            void operator+=(const satcat5::ptp::Time& other);
            void operator-=(const satcat5::ptp::Time& other);
            satcat5::ptp::Time operator=(const satcat5::ptp::Time& other);
            satcat5::ptp::Time operator+(const satcat5::ptp::Time& other) const;
            satcat5::ptp::Time operator-(const satcat5::ptp::Time& other) const;
            satcat5::ptp::Time operator-() const;
            //!@}

            //! Scalar multiply and divide are used for weighted averaging.
            //! Do not use scaling factors larger than ~10000 or it may overflow.
            //!@{
            void operator*=(unsigned scale);
            void operator/=(unsigned scale);
            satcat5::ptp::Time operator*(unsigned scale) const;
            satcat5::ptp::Time operator/(unsigned scale) const;
            //!@}

            //! Standard comparison operators.
            //!@{
            bool operator==(const satcat5::ptp::Time& other) const;
            bool operator<(const satcat5::ptp::Time& other) const;
            bool operator>(const satcat5::ptp::Time& other) const;
            inline bool operator!=(const satcat5::ptp::Time& other) const {return !operator==(other);}
            inline bool operator<=(const satcat5::ptp::Time& other) const {return !operator>(other);}
            inline bool operator>=(const satcat5::ptp::Time& other) const {return !operator<(other);}
            //!@}

        protected:
            void normalize();       // Reduce to canonical form
            s64 m_secs;             // Seconds since epoch (may be negative)
            s64 m_subns;            // Subnanoseconds, range [0 .. SUBNS_PER_SEC)
        };

        //! Convert from SatCat5 date/time to a ptp::Time object.
        satcat5::ptp::Time from_datetime(s64 gps_msec);

        //! Common time-related constants.
        //!@{
        constexpr satcat5::ptp::Time TIME_ZERO(0LL);
        constexpr satcat5::ptp::Time ONE_NANOSECOND(SUBNS_PER_NSEC);
        constexpr satcat5::ptp::Time ONE_MICROSECOND(SUBNS_PER_USEC);
        constexpr satcat5::ptp::Time ONE_MILLISECOND(SUBNS_PER_MSEC);
        constexpr satcat5::ptp::Time ONE_SECOND(SUBNS_PER_SEC);
        constexpr satcat5::ptp::Time ONE_MINUTE(SUBNS_PER_SEC * 60);
        constexpr satcat5::ptp::Time ONE_HOUR(SUBNS_PER_SEC * 3600);
        constexpr satcat5::ptp::Time ONE_DAY(SUBNS_PER_SEC * 3600 * 24);
        //!@}
    }
}
