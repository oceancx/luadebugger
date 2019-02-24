const char* debuggee_code = R"_(
--this file is only used debuggee thread
cjson = require 'cjson'

function create_on_message_parser()
    local parsed_len = -1
    local readstate = 1
    local LINE_ENDING = nil
    return function(conn,buf,netq)
        while buf:readablesize() > 0 do 
            local preview = buf:preview(buf:readablesize())
            if readstate == 1 then
                local s,e = preview:find("\n")
                if s then
                    if not LINE_ENDING then
                        local s,e = preview:find("\r\n")
                        LINE_ENDING = s and "\r\n" or "\n"
                        set_line_ending_in_c(LINE_ENDING)
                    end
                    local line = buf:readstring(e)
                    local match = line:gmatch("Content%-Length: (%d*)")()
                    if tonumber(match) then 
                        parsed_len = tonumber(match)
                        readstate = readstate + 1
                    else 
                        break
                    end 
                else 
                    break
                end
            elseif readstate == 2 then
                local s,e = preview:find("\n")
                if s then
                    local line = buf:readstring(e)
                    readstate = readstate+1
                else 
                    break
                end
            elseif readstate == 3 then
                if buf:readablesize() >= parsed_len then
                local js  = buf:readstring(parsed_len) 
                readstate = 1
                netq:push_back(0,js)
                else 
                    break
                end
            else
                break
            end 
        end
    end
end

function debuggee_on_connection(conn,netq)
    --print('[debuggee]' ..conn:tohostport().. ' ' ..(conn:connected() and 'connected' or 'disconnected'))
    netq:push_back(0,cjson.encode({ type='debuggee', event='connection_state', connected = conn:connected(), addr = conn:tohostport()  }))
end

local debuggee_on_message_parser = create_on_message_parser()
function debuggee_on_message(conn,buf,netq)
    debuggee_on_message_parser(conn,buf,netq)
end
)_";

const char* debugger_code = R"_(
local function utils_string_split(str, cut)
    str = str..cut 
    local pattern  = '(.-)'..cut
    local res = {}
    for w in string.gmatch(str, pattern) do
        table.insert(res,w)
        --print(w)
    end
    return res
end



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


local cjson = require 'cjson'
local MAIN_THREAD_ID = 1

function final_send(netq, js)
    print('-->finalsend')
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    print(sent)
    debugger_send_message(sent)
end

local message_seq = 1
function send_response(netq, req, success)
    if not req then return end
    local resp = {}
    resp.type = 'response'
    resp.command = req.command
    resp.request_seq = req.seq
    resp.success = success or true
    resp.body = req.body 
    resp.seq = message_seq
    message_seq = message_seq + 1
    
    final_send(netq,cjson.encode(resp))
end

function send_event(netq, ev, body)
    local event = {}
    event.type = 'event'
    event.event = ev
    event.body = body
    event.seq = message_seq
    message_seq = message_seq + 1

    final_send(netq,cjson.encode(event))
end          

local breakpoints = {}
local step_into = false
local step_over = false
local step_level = 0
local stack_level = 0
local HOOKMASK = 'lcr'

local function format_path(path)
    if string.find(path, '@') == 1 then
        path = string.sub(path, 2)
    end
    path = string.gsub(path,'\\','/')
    return path
end
        
local function set_breakpoint(file, line)

end

local function remove_breakpoint(file, line)

end

local function has_breakpoint(file, line)
    if breakpoints[file] then
        for i,bp in ipairs(breakpoints[file]) do
            if bp.line == line then
                
                return true
            end
        end
    else 
        return false
    end
end

local function fetch_breakpoint(file, line)
    for i,bp in ipairs(breakpoints[file]) do
        if bp.line == line then
            return bp
        end
    end
end

local function stack_depth(start_depth)
    for i = start_depth, 0, -1 do
        if debug.getinfo(i, "l") then return i+1 end
    end
    return start_depth
end

function debugger_verify_breakpoints()
    local id = 1
    for path, bps in pairs(breakpoints) do
        for i,bp in ipairs(bps) do
            bp.id = id 
            id = id + 1
            bp.verified = true
        end
    end
end

local function extract_file_name(path)
    local s,e =  path:find('.*/')
    if not s then return path end
    return path:sub( e+1 )
end

function debugger_fetch_stacks(start,lv)
    local stackFrames = {}
    for i = start,start+lv  do
        local info = debug.getinfo(i+3)
        if not info then break end
        -- print('fetch_stack',i)
        -- utils_dump_table(info)
        local frame = {}
        frame.column = 0
        frame.id = i
        frame.line = info.currentline
        frame.name = info.name or ''

        local src_path = info.source
        src_path = format_path(src_path)
        local name = extract_file_name(src_path) 
        if name ~= '__debugger__' then
            frame.source = {
                adapterData = info.what,
                path = src_path,
                name = name,
                sourceReference = 0
            }
            table.insert(stackFrames, frame)
        end
    end
    return stackFrames
end

function debugger_fetch_vars(frameId)
    frameId = math.tointeger(frameId)
    local function vars(f)
        local info = debug.getinfo(f)
        -- utils_dump_table(info)
        if not info then return end
        local func = info.func
        local i = 1
        local locals = {}
        -- get locals
        while true do
            local name, value = debug.getlocal(f, i)
            -- print('name',name,'value',value,'i',i,'f',f)
            if not name then break end
            if string.sub(name, 1, 1) ~= '(' then locals[name] = value end
            i = i + 1
        end
        -- get varargs (these use negative indices)
        i = 1
        while true do
            local name, value = debug.getlocal(f, -i)
            -- `not name` should be enough, but LuaJIT 2.0.0 incorrectly reports `(*temporary)` names here
            if not name or name ~= "(*vararg)" then break end
            locals[name:gsub("%)$"," "..i..")")] = value 
            i = i + 1
        end
        -- get upvalues
        i = 1
        local ups = {}
        while func do -- check for func as it may be nil for tail calls
            local name, value = debug.getupvalue(func, i)
            -- print('upname',name,'value',value,'i',i,'f',func)
            if not name then break end
            ups[name] = value
            i = i + 1
        end
        return locals, ups
    end


    local count = 0
    for i = 0 , 30 do 
        local source = debug.getinfo(i)
        if not source then break end
        local src = source.source
        src = format_path(src)

        local name = extract_file_name(src) 
        -- print('src', src, 'name', name)
        if name ~= '__debugger__' and source.what == 'Lua' then
            count = count + 1
            -- print('count', count , ' frameID', frameId)
            if count == frameId then
                return vars(i)
            end
        end
    end

    return {},{}
end


local ref_table = {}
local table2ref= {}
local currentId = 1

function encode_vars2ref(vars)
    if not vars or type(vars) ~= 'table' then return 0 end
    table2ref[vars] = currentId
    ref_table[currentId] = vars 
    root_id = currentId
    currentId = currentId + 1

    for name,value in pairs(vars) do 
        if type(value) == 'table' then
            table2ref[value] = currentId
            ref_table[currentId] = v
            currentId = currentId + 1
        end
    end    

    return root_id
end



local breaked_in_hook = false
local current_stack_frames = {}
local current_local_vars = {}
local current_up_vars = {}
function debugger_handle_message_new(msg)
    -- print('debugger_handle_message_new',msg)
    local req = cjson.decode(msg)
    if req.type ~= 'request' then return end
    local cmd = req.command 
    if cmd == "initialize" then        

    elseif cmd == "launch" then
        launch_req = req
    elseif cmd == "attach" then
        launch_req = req

    elseif cmd == "disconnect" then
        --{"command":"disconnect","arguments":{"restart":false},"type":"request","seq":93}
        send_response(netq,req)
        breakpoints = {}
        breaked_in_hook = false
    elseif cmd == "restart" then
        
    elseif cmd == "setBreakpoints" then
        SetBreakpoints(netq, req)
    elseif cmd == "configurationDone" then
        if launch_req then
            send_response(netq,launch_req)
            send_event(netq, 'stopped', { reason='entry', threadId = MAIN_THREAD_ID })
            send_response(netq, req)
            step_into = true        --stop on entry
        end
    elseif cmd == "continue" then
        -- assert(breaked_in_hook)
        send_response(netq,req)
        step_into = false
        step_over = false
        breaked_in_hook = false
    elseif cmd == 'stepIn' then
        -- {"request_seq":66,"command":"stepIn","success":true,"seq":74,"type":"response"}
        send_response(netq,req)
        step_into = true
        breaked_in_hook = false
    elseif cmd == 'next'  then
        ---{"type":"request","command":"next","arguments":{"threadId":1},"seq":50}
        -- assert(breaked_in_hook)
        send_response(netq,req)
        step_into = false
        step_over = true
        step_level = stack_level
        breaked_in_hook = false
    elseif cmd == "stepOut" then
        -- {"seq":272,"type":"request","arguments":{"threadId":1},"command":"stepOut"}
        send_response(netq, req)
        step_into = false
        step_over = true
        step_level = stack_level - 1 
        breaked_in_hook = false
    elseif cmd == "stepOver" then
        -- StepOver(netq, req)
    elseif cmd == "stepBack" then
        -- StepBack(netq, req)
    elseif cmd == "pause" then
        --{"arguments":{"threadId":1},"command":"pause","type":"request","seq":11}
        -- assert(not breaked_in_hook)
        send_response(netq,req)
        step_into = true
    elseif cmd == "stackTrace" then
        -- assert(breaked_in_hook)
        local args = req.arguments
        local startFrame = args.startFrame
        local threadId = args.threadId
        local levels  = args.levels
        local stackFrames = debugger_fetch_stacks(startFrame,levels)
        req.body = {}
        req.body.stackFrames = stackFrames
        req.body.totalFrames = #stackFrames 
        send_response(netq,req)
        current_stack_frames = stackFrames
    elseif cmd == "scopes" then
        -- print(msg)
        -- {"type":"request","arguments":{"frameId":1},"seq":290,"command":"scopes"}
        ref_table = {}
        currentId = 1
        table2ref = {}
        ref_table_name = {}
        local frameId = req.arguments.frameId           
        current_local_vars, current_up_vars = debugger_fetch_vars(frameId)
        -- print('locals')
        -- utils_dump_table(current_local_vars)
        -- print('ups')
        -- utils_dump_table(current_up_vars)
        local root_local = encode_vars2ref(current_local_vars)
        local root_up = encode_vars2ref(current_up_vars)
        req.body = {
            scopes = {
                {name='Locals', variablesReference=root_local, expensive = false},
                {name='Ups', variablesReference=root_up, expensive = true}
            }
        }
        send_response(netq,req)

    elseif cmd == "variables" then
        --{"command":"variables","arguments":{"variablesReference":1001},"type":"request","seq":46}
        local reference = req.arguments.variablesReference
        local vars = ref_table[reference] or {}
        local variables = {}
        for k,v in pairs(vars) do 
            local variable = {}
            print('k',k,'v',v,'val',val,'valstr',valstr)
            variable.name = k
            variable.type = type(v)
            variable.value = tostring(v)
            if variable.type == 'table' then
                variable.variablesReference = encode_vars2ref(v)
            else
                variable.variablesReference = 0
            end
            table.insert(variables,variable)
        end
        req.body = {
            variables = variables
        }
        send_response(netq,req)
    elseif cmd == "source" then
        
    elseif cmd == "threads" then
        -- assert(breaked_in_hook)
        req.body = {}
        req.body.threads = { 
            {id = MAIN_THREAD_ID, name='thread 1'}
        }
        send_response(netq,req)
    elseif cmd == "evaluate" then
        --{"command":"evaluate","arguments":{"expression":"t2343123","frameId":1,"context":"hover"},"type":"request","seq":76}
        local frameId = req.arguments.frameId 
        local expression = req.arguments.expression 
        local locals, ups =  debugger_fetch_vars(frameId)
        local display_value
        local value_ref = 0
        for k,v in pairs(locals) do 
            if k == expression then
                display_value = tostring(v)
                value_ref = type(v) == 'table' and encode_vars2ref(v) or 0
            end
        end

        for k,v in pairs(ups) do 
            if k == expression then
                display_value = tostring(v)
                value_ref = type(v) == 'table' and encode_vars2ref(v) or 0
            end
        end
        if display_value then
            req.body = {
                result  = display_value,
                variablesReference = value_ref,
            }
        end
        
        send_response(netq,req)

    elseif cmd == "loadedSources" then
        LoadedSources(netq, req)
    end
end

function SetBreakpoints(netq, req)
    local args = req.arguments
    local path = args.source.path
    local name = args.source.name 

    path = format_path(path)
    breakpoints[path] = {}  -- clear bps
    for i,bp in ipairs(args.breakpoints) do
        table.insert(breakpoints[path], { line = bp.line, verified = false, id = 0})
    end
    debugger_verify_breakpoints()
    req.body = {}
    req.body.breakpoints = breakpoints[path]
    send_response(netq, req)
end 

function debugger_hook_new(event, line)
    if event == 'call' then
        stack_level = stack_level + 1
    elseif event == 'return' or event  == 'tail return' then
        stack_level = stack_level - 1
    elseif event == 'line' then
        local msg = debugger_fetch_message()
        if msg ~= "" then
            debugger_handle_message_new(msg)
        end
        local info = debug.getinfo(2)   --;utils_dump_table(info)
        local file = info.source
        file = format_path(file)
        
        if step_into or (step_over and stack_level <= step_level) or has_breakpoint(file,line) then
            if step_into then
                -- print('step_into' ,'stack_level:'..stack_level)
                send_event(netq, 'stopped', { reason='step', threadId = MAIN_THREAD_ID })
            elseif (step_over and step_level <= stack_level) then
                -- print('step_over', 'step_level:'..step_level, 'stack_level:'..stack_level)
                send_event(netq, 'stopped', { reason='step', threadId = MAIN_THREAD_ID })
            else
                send_event(netq, 'stopped', { reason='breakpoint', threadId = MAIN_THREAD_ID })
            end
            step_into = false
            step_over = false
            breaked_in_hook = true

            while breaked_in_hook do
                if not debugger_is_connected() then breaked_in_hook=false;break end
                msg = debugger_fetch_message()
                if msg ~= "" then
                    debugger_handle_message_new(msg)
                else 
                    debugger_sleep(10)
                end     
            end
        end
    end
end


--每帧循环时调用
local debugger_connected_old = false
function debugger_update_session_new()
    local connected = debugger_is_connected()
    if connected~=debugger_connected_old then
        debugger_connected_old = connected
        if connected then
            debug.sethook(debugger_hook_new,HOOKMASK)
        else
            debug.sethook()
        end
    end
end
)_";