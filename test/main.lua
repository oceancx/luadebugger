dofile('d:\\Github\\luadebugger\\test\\hello.lua')

function main()
    luadbg_start(9527)

    local last_time = os.time()
    while true do 
        local now_time = os.time()
        if now_time - last_time >= 1 then
            luadbg_update()
            hello2()
        end
        last_time = now_time
    end
    luadbg_stop()
end

main()