logEdit:clear()
function loads19(filename, range)
    if not QFile.exists(filename) then return nil end
    local r = {endAddress = 0, jumpAddress = 0, data={} }
    local lineCount = 1
    local baseAddr = 0
    for line in io.lines(filename) do
--        log(line)
        if line:find("S") == 1 then
--            log("OK")
            local type = tonumber("0x" .. line:sub(2,2))
            local count = tonumber("0x" .. line:sub(3,4))
            local addr = 0 --tonumber("0x" .. line:sub(4,7))
            local sum = 0
            local data = {}
            for i=1,count+1 do
                local d = (tonumber("0x" .. line:sub(i*2+1,i*2+2)) or 0 )
                sum = sum + d
                if i > 1 then
                    data[#data + 1] = d
                end
            end
            data[#data] = nil      -- remove sum
            sum = bit_and(sum, 0xff)
            if sum ~= 0xff then
                log("Sum error at line: " .. lineCount .. "  (" .. line .. ")", sum)
                return nil
            else
                if type == 0 then
                    --log("S19 start")
                    -- don't care
--[[
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
--]]
                elseif type == 1 then
                    log("Ignore 2 byte Address Record")
                elseif type == 2 then
                    log("Ignore 3 byte Address Record")
                elseif type == 3 then
                    -- 地址场由四字节说明
					addr = QUtil.toUint32(data, 1, true)
					local valid = true
					if range then
					    if addr >= range[1] and addr <=range[2] then
						    valid = true
						else
							log("data out of range")
							log(string.format("%x, (%x-%x)", addr, range[1], range[2]))
						    valid = false
						end
					end
					if valid then
						local romAddr = addr
						if r.startAddress then
							if r.startAddress > romAddr then
								r.startAddress = romAddr
							end
						else
							r.startAddress = romAddr
						end

						for i=5,#data do
							r.data[ romAddr - r.startAddress + i - 4] = data[i]
						end
						if r.endAddress < (romAddr + #data - 5) then
							r.endAddress = romAddr + #data - 5
						end
					end
                elseif type == 7 then
                    r.appAddress = QUtil.toUint32(data, true)
                    --log(string.format("app start at: %08x", r.appAddress))
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
    local newData = {}
    for i=r.startAddress,r.endAddress do
        local p = i-r.startAddress+1
        if r.data[p] then
        else
            r.data[p] = 0xff
        end
        newData[#newData + 1] = r.data[p]
    end
    r.data = newData
    return r
end

function needoutputs19(data)
    return true
end

function outputs19(file, type, data, address)
    local r = ""
    local len = #data + 1 + (address and 4 or 0)
    local d = {len}
    local sum = 0
    if address then
        d = QUtil.fromUint32(d, address, true)
    end
    for i=1,#data do
        d[#d+1] = data[i]
    end
    for i=1,#d do
        r = r .. string.format("%02X", bit_and(d[i],0xff))
        sum = sum + d[i]
    end
    r = r .. string.format("%02X",bit_and(0xffff-sum,0xff))
    file:write("S",type,r,"\n")
end

function gets19ata(data, start)
    local r = {}
    for i=start,#data do
        r[#r+1] = data[i]
        if #r == 32 then break end
    end
    return r
end

function saves19(hex, filename)
    if hex.data then
    local f = io.open(filename,"w+")
    local needHead = true
    local base = {0,0}
    if f then
        hex.startAddress = hex.startAddress or 0
        hex.jumpAddress = hex.jumpAddress or 0
        local i = 1
        while i<#hex.data do
            local d = gets19ata(hex.data, i)
            local addr = i + hex.startAddress - 1
            local x = QUtil.fromUint32(addr,true)
            if base[2] ~= x[2] or base[1] ~= x[1] then
                needHead = true
            end
            if needoutputs19(d) then
                if needHead then
                    outputs19(f, 0, {0,0}, nil)
                    base[1],base[2] = x[1],x[2]
                    needHead = false
                end
                outputs19(f, 3, d, hex.startAddress + i - 1)
            else
                needHead = true
            end
            i = i + #d
        end
        if hex.appAddress then
            outputs19(f, 7, {}, hex.appAddress)
        end
        f:close()
    end
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
