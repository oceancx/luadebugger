print('hello')
a = {}
function  what_the_f1()
    local a = 1
    local b = 2
    local c = a + b
    print('what_the_f1')
end

function  what_the_f2()
    print('what_the_f2')
    what_the_f1()
end
a.what_the_f1 = what_the_f1
a.what_the_f2 = what_the_f2
return a