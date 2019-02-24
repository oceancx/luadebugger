debugger_start_session(9527)


ma = require("a")
local function utils_dump_table(t)

    if not t or type(t)~='table' then return end
   
    local count = 1 
    local next_layer = {}
    table.insert(next_layer, t)
    while true do 
        if #next_layer == 0 then break end
        local next_t = table.remove(next_layer,1)
        for k,v in pairs(next_t) do 
            if type(v) == 'table' then
                if count > 5 then break end
                count = count + 1
                table.insert(next_layer, v)
            else
                print(k,v)
            end
        end    
    end
end
function debugger_hook_new2( event, line)
    if event == 'line' then
        local info = debug.getinfo(2)  
        
        if info.what ~= 'main' then
            utils_dump_table(info)
        end
    end
end

-- debug.sethook(debugger_hook_new2,'lcr')
do
    local last_time = os.time()
    while true do 
        local now_time = os.time()
        if now_time - last_time >= 1 then
            debugger_update_session_new()
            
            ma.what_the_f2()
        end
        last_time = now_time
    end
    debugger_stop_session()
end