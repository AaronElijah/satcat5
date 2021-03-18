--------------------------------------------------------------------------
-- Copyright 2019 The Aerospace Corporation
--
-- This file is part of SatCat5.
--
-- SatCat5 is free software: you can redistribute it and/or modify it under
-- the terms of the GNU Lesser General Public License as published by the
-- Free Software Foundation, either version 3 of the License, or (at your
-- option) any later version.
--
-- SatCat5 is distributed in the hope that it will be useful, but WITHOUT
-- ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
-- FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
-- License for more details.
--
-- You should have received a copy of the GNU Lesser General Public License
-- along with SatCat5.  If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------
--
-- Port-type wrapper for "port_gmii_internal"
--
-- Xilinx IP-cores can only use simple std_logic and std_logic_vector types.
-- This shim provides that conversion.
--

library ieee;
use     ieee.std_logic_1164.all;
use     ieee.numeric_std.all;
use     work.common_functions.all;
use     work.switch_types.all;

entity wrap_port_gmii_internal is
    port (
    -- GMII interface.
    gmii_txc    : out std_logic;
    gmii_txd    : out std_logic_vector(7 downto 0);
    gmii_txen   : out std_logic;
    gmii_txerr  : out std_logic;
    gmii_rxc    : out std_logic;
    gmii_rxd    : in  std_logic_vector(7 downto 0);
    gmii_rxdv   : in  std_logic;
    gmii_rxerr  : in  std_logic;
    gmii_col    : out std_logic;
    gmii_crs    : out std_logic;

    -- Network port
    sw_rx_clk   : out std_logic;
    sw_rx_data  : out std_logic_vector(7 downto 0);
    sw_rx_last  : out std_logic;
    sw_rx_write : out std_logic;
    sw_rx_error : out std_logic;
    sw_rx_rate  : out std_logic_vector(15 downto 0);
    sw_rx_status: out std_logic_vector(7 downto 0);
    sw_rx_reset : out std_logic;
    sw_tx_clk   : out std_logic;
    sw_tx_data  : in  std_logic_vector(7 downto 0);
    sw_tx_last  : in  std_logic;
    sw_tx_valid : in  std_logic;
    sw_tx_ready : out std_logic;
    sw_tx_error : out std_logic;
    sw_tx_reset : out std_logic;

    -- Reference clock and reset.
    clk_125     : in  std_logic;    -- Main reference clock
    reset_p     : in  std_logic);   -- Reset / port shutdown
end wrap_port_gmii_internal;

architecture wrap_port_gmii_internal of wrap_port_gmii_internal is

signal rx_data  : port_rx_m2s;
signal tx_data  : port_tx_m2s;
signal tx_ctrl  : port_tx_s2m;

begin

-- Unused status signals.
gmii_col        <= '0';
gmii_crs        <= '0';

-- Forward reference clock.
gmii_rxc        <= clk_125;

-- Convert port signals.
sw_rx_clk       <= rx_data.clk;
sw_rx_data      <= rx_data.data;
sw_rx_last      <= rx_data.last;
sw_rx_write     <= rx_data.write;
sw_rx_error     <= rx_data.rxerr;
sw_rx_rate      <= rx_data.rate;
sw_rx_status    <= rx_data.status;
sw_rx_reset     <= rx_data.reset_p;
sw_tx_clk       <= tx_ctrl.clk;
sw_tx_ready     <= tx_ctrl.ready;
sw_tx_error     <= tx_ctrl.txerr;
sw_tx_reset     <= tx_ctrl.reset_p;
tx_data.data    <= sw_tx_data;
tx_data.last    <= sw_tx_last;
tx_data.valid   <= sw_tx_valid;

-- Unit being wrapped.
u_wrap : entity work.port_gmii_internal
    port map(
    gmii_txc    => gmii_txc,
    gmii_txd    => gmii_txd,
    gmii_txen   => gmii_txen,
    gmii_txerr  => gmii_txerr,
    gmii_rxc    => clk_125,
    gmii_rxd    => gmii_rxd,
    gmii_rxdv   => gmii_rxdv,
    gmii_rxerr  => gmii_rxerr,
    rx_data     => rx_data,
    tx_data     => tx_data,
    tx_ctrl     => tx_ctrl,
    clk_125     => clk_125,
    reset_p     => reset_p);

end wrap_port_gmii_internal;
