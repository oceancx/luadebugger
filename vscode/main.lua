
old_print = print
-- old_print('log', get_default_cwd()..'dbg.log')
print = function(...)
    if is_stdio_mode() then
        local path = get_default_cwd()..'dbg.log'
        local file = io.open(path,'a+')
        local args = {...}
        for i,v in ipairs(args) do
            file:write(v..'\t')
        end
        file:write('\n')
        file:close()
    else
        old_print(...)
    end
end



local message_seq = 1
function vscode_final_send(js)
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    vscode_send_message(sent)
end

function vscode_send_response(req)
    local resp = {}
    resp.type = 'response'
    resp.command = req.command
    resp.request_seq = req.seq
    resp.success = true
    resp.seq = message_seq
    message_seq = message_seq + 1        
    if req.body then
        resp.body = req.body
    end

    vscode_final_send(cjson.encode(resp))
end

function vscode_send_event(ev)
    local event = {}
    event.type = 'event'
    event.event = ev
    event.seq = message_seq
    message_seq = message_seq + 1

    vscode_final_send(cjson.encode(event))
end


function runtime_final_send(js)
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    runtime_send_message(sent)
end

function dispatch_runtime_message(js)
    print(string.format("\nDA <= RT:\n%s\n", js))
    vscode_final_send(js)
end


function runtime_send_response(req)
    local resp = {}
    resp.type = 'response'
    resp.command = req.command
    resp.request_seq = req.seq
    resp.success = true
    resp.body = req.body
    resp.seq = message_seq
    message_seq = message_seq + 1        

    runtime_final_send(cjson.encode(resp))
end

function runtime_send_event(ev)
    local event = {}
    event.type = 'event'
    event.event = ev
    event.seq = message_seq
    message_seq = message_seq + 1

    runtime_final_send(cjson.encode(event))
end


function dispatch_vscode_message(js)
    print(string.format("\nDA <= VS:\n%s\n", js))
    
    local req = cjson.decode(js)
     if req.type == 'request' then
        local cmd = req.command
        if cmd == "initialize" then
            req.body = {
                supportsConfigurationDoneRequest = true,
                supportsEvaluateForHovers = true,
                supportsStepBack = true
            }
            vscode_send_response(req)
            vscode_send_event('initialized')
        elseif cmd == "launch" then
            local arguments = req.arguments
            local ip = arguments.ip
            local port = math.tointeger(arguments.port)
            local cmd = arguments.launchcmd

            vscode_on_launch_cmd('start '..cmd,ip,port)
            runtime_final_send(cjson.encode(req))
        elseif cmd == "attach" then
            local arguments = req.arguments
            local ip = arguments.ip
            local port = math.tointeger(arguments.port)
            -- local cwd = arguments.cwd
            vscode_on_attach_cmd(ip,port)
            runtime_final_send(cjson.encode(req))
        elseif cmd == "disconnect" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "restart" then
            vscode_send_response(req)
        elseif cmd == "setBreakpoints" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "setFunctionBreakpoints" then
            vscode_send_response(req)
        elseif cmd == "setExceptionBreakpoints" then
            vscode_send_response(req)
        elseif cmd == "configurationDone" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "continue" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "next" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepIn" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepOut" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepOver" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepBack" then
            
        elseif cmd == "reverseContinue" then
            
        elseif cmd == "restartFrame" then
            
        elseif cmd == "goto" then
            
        elseif cmd == "pause" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stackTrace" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "scopes" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "variables" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "setVariable" then

        elseif cmd == "source" then
            
        elseif cmd == "threads" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "evaluate" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepInTargets" then
        elseif cmd == "gotoTargets" then
        elseif cmd == "completions" then
        elseif cmd == "exceptionInfo" then
        elseif cmd == "loadedSources" then
            vscode_send_response(req)
        end
    end
end




local parsed_len = -1
local readstate = 1
function stdio_on_message(buf,netq, runtime_netq)
    while buf:readable_size() > 0 do
        local preview = buf:Preview(buf:readable_size())
        -- print('preview..' ..preview)
        if readstate == 1 then
            local s,e = preview:find("\n")
            if s then
                if not LINE_ENDING then
                    local s,e = preview:find("\r\n")
                    LINE_ENDING = s and "\r\n" or "\n"
                    set_line_ending_in_c(LINE_ENDING)
                end
                local line = buf:ReadAsString(e)
                local match = line:gmatch("Content%-Length: (%d*)")()
                if tonumber(match) then
                    parsed_len = tonumber(match)
                    -- dbg_trace("parsed_len "..tostring(parsed_len))
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
                local line = buf:ReadAsString(e)
                -- dbg_trace(line)
                readstate = readstate+1
            else
                break
            end
        elseif readstate == 3 then
            if buf:readable_size() >= parsed_len then
                local js  = buf:ReadAsString(parsed_len)
                -- dbg_trace('\n----stdio read-----\n'..js)
                readstate = 1
                -- netq:push_back(0,js)
                dispatch_vscode_message(js)
            else
                break
            end
        else
            break
        end
    end
end

function DA_in_break()
    local vs_netq = fetch_vscode_netq()
    while not vs_netq:empty(0) do
        print('DA_in_break vs_netq')
        local msg = vs_netq:front_as_string(0)
        vs_netq:pop_front(0)
        dispatch_vscode_message(msg)
    end
    local rt_netq = fetch_runtime_netq()
    while not rt_netq:empty(0) do
        print('DA_in_break rt_netq')
        local msg = rt_netq:front_as_string(0)
        rt_netq:pop_front(0)
        dispatch_runtime_message(msg)
    end
end

function main()
    luadbg_listen(9528)
    while is_debugger_adapter_run() do
        local vs_netq = fetch_vscode_netq()
        while not vs_netq:empty(0) do
            local msg = vs_netq:front_as_string(0)
            vs_netq:pop_front(0)
            dispatch_vscode_message(msg)
        end
        local rt_netq = fetch_runtime_netq()
        while not rt_netq:empty(0) do
            local msg = rt_netq:front_as_string(0)
            rt_netq:pop_front(0)
            dispatch_runtime_message(msg)
        end
    end
end

main()