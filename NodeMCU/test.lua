-- Wi-Fi credentials.
local apcfg = {
    ssid = "ssid",
    pwd = "password"
}

local stncfg = {
    ssid = "ssid",
    pwd = "password"
}

-- Print some useful information.
print("setting up Wi-Fi...")
print("station SSID: \"" .. stncfg.ssid .. "\" password: \"" .. stncfg.pwd .. "\"")
-- print("ap SSID: \"" .. apcfg.ssid .. "\" password: \"" .. apcfg.pwd .. "\"")

wifi.setmode(wifi.STATIONAP)
local mode = wifi.setmode(wifi.STATION)
local wifi_ready = false
print("Station mode is: " .. tostring(mode))

if mode == wifi.STATION then
    wifi.sta.config(stncfg)
    wifi.ap.config(apcfg)
else
    print("Station in unusable mode: " .. tostring(mode))
end

print("\ntype \"qw\" to cancel server startup.\n")

-- Start server after 5 sec.
tmr.alarm(1, 5000, 0, function()
    if wifi_ready == true then
        uart.on("data") -- stop listening UART.
        print "starting server..."
        require "server"
    else
        print("Wifi not ready, server start cancelled.")
    end
end)

-- Listen to UART data and stop the above timer in case of "qw" string was received.
uart.on("data", "w", function(d)
	if d == "qw" then
		tmr.stop(1) -- Stop timer. Server won't start.
		uart.on("data")
		print "cancelled."
	end
end, 0)

-- wifi event mon handlers
 wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, function(T)
 print("\n\tSTA - CONNECTED".."\n\tSSID: "..T.SSID.."\n\tBSSID: "..
 T.BSSID.."\n\tChannel: "..T.channel)
 end)

 wifi.eventmon.register(wifi.eventmon.STA_DISCONNECTED, function(T)
 print("\n\tSTA - DISCONNECTED".."\n\tSSID: "..T.SSID.."\n\tBSSID: "..
 T.BSSID.."\n\treason: "..T.reason)
 end)

 wifi.eventmon.register(wifi.eventmon.STA_AUTHMODE_CHANGE, function(T)
 print("\n\tSTA - AUTHMODE CHANGE".."\n\told_auth_mode: "..
 T.old_auth_mode.."\n\tnew_auth_mode: "..T.new_auth_mode)
 end)

 wifi.eventmon.register(wifi.eventmon.STA_GOT_IP, function(T)
 print("\n\tSTA - GOT IP".."\n\tStation IP: "..T.IP.."\n\tSubnet mask: "..
 T.netmask.."\n\tGateway IP: "..T.gateway)
 hostname = "hvacplu"
 print("Setting AP hostname = " .. hostname)
 -- wifi.ap.config(apcfg)
 wifi.sta.sethostname(hostname)
 end)

 wifi.eventmon.register(wifi.eventmon.STA_DHCP_TIMEOUT, function()
 print("\n\tSTA - DHCP TIMEOUT")
 end)

 wifi.eventmon.register(wifi.eventmon.AP_STACONNECTED, function(T)
 print("\n\tAP - STATION CONNECTED".."\n\tMAC: "..T.MAC.."\n\tAID: "..T.AID)
 end)

 wifi.eventmon.register(wifi.eventmon.AP_STADISCONNECTED, function(T)
 print("\n\tAP - STATION DISCONNECTED".."\n\tMAC: "..T.MAC.."\n\tAID: "..T.AID)
 end)

--[[
 wifi.eventmon.register(wifi.eventmon.AP_PROBEREQRECVED, function(T)
 print("\n\tAP - PROBE REQUEST RECEIVED".."\n\tMAC: ".. T.MAC.."\n\tRSSI: "..T.RSSI)
 end)
]]--

--[[
 wifi.eventmon.register(wifi.eventmon.WIFI_MODE_CHANGE, function(T)
 print("\n\tSTA - WIFI MODE CHANGED".."\n\told_mode: "..
 T.old_mode.."\n\tnew_mode: "..T.new_mode)
 end)]]--

-- station event handlers
wifi.sta.eventMonReg(wifi.STA_IDLE, function() print("STATION_IDLE") end)
wifi.sta.eventMonReg(wifi.STA_CONNECTING, function(previous_State)
    if(previous_State==wifi.STA_GOTIP) then
        print("Station lost connection with access point\n\tAttempting to reconnect...")
    else
        print("STATION_CONNECTING")
    end
end)
wifi.sta.eventMonReg(wifi.STA_WRONGPWD, function() print("STATION_WRONG_PASSWORD") end)
wifi.sta.eventMonReg(wifi.STA_APNOTFOUND, function() print("STATION_NO_AP_FOUND") end)
wifi.sta.eventMonReg(wifi.STA_FAIL, function() print("STATION_CONNECT_FAIL") end)
wifi.sta.eventMonReg(wifi.STA_GOTIP, function()
    print("STATION_GOT_IP")
    wifi_ready = true
end)
if (wifi.getmode() == wifi.STATION) then
    -- handlers invoked based on result, see station handlers below
    wifi.sta.eventMonStart()
end
