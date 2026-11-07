-- SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
-- Copyright David Lewis
print("Starting...Remove init.lua to halt startup...")

function startup()
    dofile("test.lua")
    end

tmr.alarm(0,5000,0,startup)