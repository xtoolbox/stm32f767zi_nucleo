logEdit:clear()
function loadhex(filename)
    local r = {endAddress = 0, jumpAddress = 0, data={} }
    local lineCount = 1
    local baseAddr = 0
    for line in io.lines(filename) do
--        log(line)
        if line:find(":") == 1 then
--            log("OK")
            local count = tonumber("0x" .. line:sub(2,3))
            local addr = tonumber("0x" .. line:sub(4,7))
            local type = tonumber("0x" .. line:sub(8,9))
            local sum = 0
            local data = {}
            for i=1,count + 5 do
                local d = (tonumber("0x" .. line:sub(i*2,i*2+1)) or 0 )
                sum = sum + d
                if i > 4 then
                    data[#data + 1] = d
                end
            end
            data[#data] = nil      -- remove sum
            sum = bit_and(sum, 0xff)
            if sum ~= 0 then
                log("Sum error at line: " .. lineCount .. "  (" .. line .. ")")
                return nil
            else
                if type == 0 then
                    local romAddr = addr + baseAddr
                    if r.startAddress then
                        if r.startAddress > romAddr then
                            r.startAddress = romAddr
                        end
                    else
                        r.startAddress = romAddr
                    end
                    for i=1,#data do
                        r.data[ romAddr - r.startAddress + i ] = data[i]
                    end
                    if r.endAddress < (romAddr + #data - 1) then
                        r.endAddress = romAddr + #data - 1
                    end
                elseif type == 1 then
                    r.jumpAddress = baseAddr + addr
                elseif type == 2 then
                    log("Ignore Extended Segment Address Record")
                elseif type == 3 then
                    log("Start Segment Address Record")
                elseif type == 4 then
                    if #data ~= 2 then
                        log("Extended Linear Address Record error at line: " .. lineCount .. "  (" .. line .. ")")
                        return nil
                    else
                        local x = {0,0,data[2],data[1]}
                        baseAddr = QUtil.toUint32(x)
                    end
                elseif type == 5 then
                    --log("Ignore Start Linear Address Record")
                    r.appAddress = QUtil.toUint32(data, true)
                else
                    log("Unkonwn type at line: " .. lineCount .. "  (" .. line .. ")")
                end
            end
        else
            log("Error at line: " .. lineCount .. "  (" .. line .. ")")
            return nil
        end
        lineCount = lineCount + 1
    end
    for i=r.startAddress,r.endAddress do
        local p = i-r.startAddress+1
        if r.data[p] then
        else
            r.data[p] = 0xff
        end
    end
    return r
end

function needoutputhex(data)
    for i=1,#data do
        if bit_and(data[i],0xff) ~= 0xff then
            return true
        end
    end
    return false
end

function outputhex(file, type, data, address)
    local r = ""
    local d = {#data}
    local sum = 0
    d = QUtil.fromUint16(d, address, true)
    d = QUtil.fromUint8(d, type, true)
    for i=1,#data do
        d[#d+1] = data[i]
    end
    for i=1,#d do
        r = r .. string.format("%02X", bit_and(d[i],0xff))
        sum = sum + d[i]
    end
    r = r .. string.format("%02X",bit_and(0x10000-sum,0xff))
    file:write(":",r,"\n")
end

function gethexdata(data, start)
    local r = {}
    for i=start,#data do
        r[#r+1] = data[i]
        if #r == 16 then break end
    end
    return r
end

function savehex(hex, filename)
    if hex.data then
    local f = io.open(filename,"w+")
    local needHead = true
    local base = {0,0}
    if f then
        hex.startAddress = hex.startAddress or 0
        hex.jumpAddress = hex.jumpAddress or 0
        local i = 1
        while i<#hex.data do
            local d = gethexdata(hex.data, i)
            local addr = i + hex.startAddress - 1
            local x = QUtil.fromUint32(addr,true)
            if base[2] ~= x[2] or base[1] ~= x[1] then
                needHead = true
            end
            if needoutputhex(d) then
                if needHead then
                    outputhex(f, 4, {x[1],x[2]}, 0)
                    base[1],base[2] = x[1],x[2]
                    needHead = false
                end
                outputhex(f, 0, d, hex.startAddress + i - 1)
            else
                needHead = true
            end
            i = i + #d
        end
        if hex.appAddress then
            outputhex(f, 5, QUtil.fromUint32(hex.appAddress,true), 0)
        end
        outputhex(f, 1, {}, hex.jumpAddress)
        f:close()
    end
    end
end

function savebin(hex, filename)
    if hex.data then
        local f = QFile(filename)
        if f:open(8+1+2) then
		    f:write(hex.data)
		    f:close()
		end
	else
		log("Fail to open " .. filename)
	end
end
--[[
local x = loadhex([==[E:\work\SmartGreenHouse\src\Obj\control.hex]==])

log(string.format("Start address: 0x%08x" , x.startAddress))
log(string.format("End address: 0x%08x" , x.endAddress))
log(string.format("Jump address: 0x%08x" , x.jumpAddress))
log(string.format("App address: 0x%08x" , x.appAddress))

log(string.format("Total count: 0x%08x", #x.data))
x.data = {}
for i=1,0x20000 do
    x.data[i] = i
end
savehex(x, "123.hex")
--]]
