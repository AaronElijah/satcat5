# ------------------------------------------------------------------------
# Copyright 2021 The Aerospace Corporation
#
# This file is part of SatCat5.
#
# SatCat5 is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# SatCat5 is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with SatCat5.  If not, see <https://www.gnu.org/licenses/>.
# ------------------------------------------------------------------------
#
# This script packages a Vivado IP core: satcat5.cfgbus_i2c_controller
#

# Create a basic IP-core project.
set ip_name "cfgbus_i2c_controller"
set ip_vers "1.0"
set ip_disp "SatCat5 ConfigBus I2C Controller"
set ip_desc "Controller for an external I2C interface."

set ip_root [file normalize [file dirname [info script]]]
source $ip_root/ipcore_shared.tcl

# Add all required source files:
#               Path                Filename/Part Family
ipcore_add_file $src_dir/common     cfgbus_common.vhd
ipcore_add_file $src_dir/common     cfgbus_fifo.vhd
ipcore_add_file $src_dir/common     cfgbus_i2c_controller.vhd
ipcore_add_file $src_dir/common     cfgbus_multiserial.vhd
ipcore_add_file $src_dir/common     common_primitives.vhd
ipcore_add_file $src_dir/common     common_functions.vhd
ipcore_add_file $src_dir/common     io_i2c_controller.vhd
ipcore_add_io   $src_dir/xilinx     $part_family
ipcore_add_sync $src_dir/xilinx     $part_family
ipcore_add_top  $ip_root            wrap_cfgbus_i2c_controller

# Connect I/O ports
ipcore_add_cfgbus Cfg cfg slave
ipcore_add_gpio i2c_sclk
ipcore_add_gpio i2c_sdata

# Set parameters
ipcore_add_param DEV_ADDR devaddr 0

# Package the IP-core.
ipcore_finished
